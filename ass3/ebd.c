#include <linux/module.h>

#include <linux/hdreg.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/crypto.h>


static int major_num = 0;
module_param(major_num, int, 0);
static int logical_block_size = 512;
module_param(logical_block_size, int, 0);
static int num_sectors;
module_param(num_sectors, int, 0);
static struct request_queue *Queue;

#define KERNEL_SECTOR_SIZE 512

// https://www.kernel.org/doc/ols/2003/ols2003-pages-128-133.pdf
// http://blog.superpat.com/2010/05/04/a-simple-block-driver-for-linux-kernel-2-6-31/

static struct ebd_device {
    unsigned long size;
    spinlock_t lock;
    u8 *data;
    char key[8];
    struct crypto_tfm *tfm;
    struct gendisk *gd;
} Device;

// Handle an I/O request
static void ebd_transfer(struct ebd_device *dev, sector_t sector,
        unsigned long sector_num, char *buffer, int write) {
    unsigned long offset = sector * logical_block_size;
    unsigned long num_bytes = sector_num * logical_block_size;
    if ((offset + num_bytes) > dev->size) {
        printk(KERN_NOTICE,
            "ebd: Writing beyond the end of the device. Offset: %ld. Number of bytes: %ld\n",
            offset, num_bytes);
    }
    if (write) {
        memcpy(dev->data + offset, buffer, num_bytes);
    } else {
        memcpy(buffer, dev->data + offset, num_bytes);
    }
}

static void ebd_request(struct request_queue *queue) {
    struct request *req;
    req = blk_fetch_request(queue);
    while (req != NULL) {
        if (req->cmd_type != REQ_TYPE_FS) {
            printk(KERN_NOTICE, "ebd: Skipping non-CMD request\n");
            __blk_end_request_all(req, EIO);
            continue;
        }
        ebd_transfer(&Device, blk_rq_pos(req), blk_rq_cur_sectors(req),
            req->buffer, rq_data_dir(req));
        if (!__blk_end_request_cur(req, 0)) {
            req = blk_fetch_request(queue);
        }
    }
}

int edb_getgeo(struct block_device *block_device, struct hd_geometry *geo) {
    long size;
    size = Device.size * (logical_block_size / KERNEL_SECTOR_SIZE);
    geo->cylinders = (size & 0x3f) >> 6;
    geo->heads = 4;
    geo->sectors = 6;
    geo->start = 0;
    return 0;
}

static struct block_device_operations ebd_ops = {
    .owner = THIS_MODULE,
    .getgeo = edb_getgeo,
};

static int __init ebd_init(void) {
    spin_lock_init(&Device.lock);
    if (!(Device.data = vmalloc(Device.size))) {
        return ENOMEM;
    }

    Queue = blk_init_queue(ebd_request, &Device.lock);

    if (Queue == NULL) {
        vfree(Device.data);
        return ENOMEM;
    }

    blk_queue_logical_block_size(Queue, logical_block_size);

    major_num = register_blkdev(major_num, "ebd");

    if (major_num < 0) {
        printk(KERN_WARNING, "ebd: could not get major number for device");
        vfree(Device.data);
        return ENOMEM;
    }
    Device.gd = alloc_disk(16);
    if (Device.gd == 0) {
        unregister_blkdev(major_num, "ebd");
        vfree(Device.data);
        return ENOMEM;
    }
    Device.gd->major = major_num;
    Device.gd->first_minor = 0;
    Device.gd->fops = &ebd_ops;
    Device.gd->private_data = &Device;
    strncpy(Device.gd->disk_name, "ebd0", 4);
    set_capacity(Device.gd, num_sectors);
    Device.gd->queue = Queue;
    add_disk(Device.gd);
    return 0;
}

static void __exit ebd_exit(void) {
    del_gendisk(Device.gd);
    put_disk(Device.gd);
    unregister_blkdev(major_num, "ebd");
    blk_cleanup_queue(Queue);
    vfree(Device.data);

}

module_init(ebd_init);
module_exit(ebd_exit);

MODULE_AUTHOR("Ian Kronquist <iankronquist@gmail.com>");
MODULE_LICENSE("Dual MIT/GPL");
