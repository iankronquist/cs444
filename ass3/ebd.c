#include <linux/module.h>

#include <linux/hdreg.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/crypto.h>

/* This block device pretends to be a hard disk. It is actually backed by RAM.
   It encrypts its contents using AES.
   Resources consulted:
   * http://blog.superpat.com/2010/05/04/a-simple-block-driver-for-linux-kernel-2-6-31/
   * crypto/tcrypt.c
   * Documentation/crypto/api-intro.txt 
*/


// Define runtime configurable module parameters.
static int major_num = 0;
module_param(major_num, int, 0);
static int logical_block_size = 512;
module_param(logical_block_size, int, 0);
static int num_sectors = 1024;
module_param(num_sectors, int, 0);

// Create global queue for servicing requests.
static struct request_queue *Queue;

#define KERNEL_SECTOR_SIZE 512

// Key size in bytes
#define KEY_SIZE 32

// A global structure representing the ebd0 device.
static struct ebd_device {
    unsigned long size;                 // Size in bytes of the device.
    spinlock_t lock;                    // A spinlock to control access.
    u8 *data;                           // The device data.
    // I may remove this.
    u8 key[KEY_SIZE];                   // The AES key for encrypting the data.
    struct crypto_cipher *blkcipher; // Kernel CryptoAPI AES block cipher.
    struct scatterlist sg[2];           // Data structure used for encryption.
    struct gendisk *gd;                 // Generic disk structure.
} Device;

static void debug_print_bytes(char *a, unsigned long len) {
    unsigned long i;
    printk("edb0:");
    for (i = 0; i < len; ++i) {
        printk("%c", a[i]);
    }
    printk("\n");
}

// Handle I/O requests
static void ebd_transfer(struct ebd_device *dev, sector_t sector,
        unsigned long sector_num, char *buffer, int write) {
    unsigned long offset = sector * logical_block_size;
    unsigned long num_bytes = sector_num * logical_block_size;
    unsigned long block_size = crypto_cipher_blocksize(dev->blkcipher);
    unsigned long i;
    if ((offset + num_bytes) > dev->size) {
        printk("ebd: Writing beyond the end of the device. Offset: %ld. "
               "Number of bytes: %ld\n",
            offset, num_bytes);
    }
    if (write) {
        char *destination = dev->data + offset;
        for (i = 0; i < num_bytes; i += block_size) {
            crypto_cipher_encrypt_one(dev->blkcipher, &destination[i],
                &buffer[i]);
        }
    } else {
        char *source = dev->data + offset;
        for (i = 0; i < num_bytes; i += block_size) {
            crypto_cipher_decrypt_one(dev->blkcipher, &buffer[i], &source[i]);
        }
    }
}

static void ebd_request(struct request_queue *queue) {
    struct request *req;
    req = blk_fetch_request(queue);
    while (req != NULL) {
        if (req->cmd_type != REQ_TYPE_FS) {
            printk("ebd: Skipping non-CMD request\n");
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
    unsigned int err;
    // FIXME: Generate random key.
    u8 *test_key = "0123456789abcdef0123456789abcdef";

    // Initialize the device spin lock. Only one process should be able to
    // read/write to the device at time.
    spin_lock_init(&Device.lock);

    Device.size = num_sectors * logical_block_size;
    // Allocate memory to hold encrypted contents.
    if (!(Device.data = vmalloc(Device.size))) {
        printk("ebd: Failed to allocate memory\n");
        goto free_mem;
    }

    // Allocate block cipher machinery.
    Device.blkcipher = crypto_alloc_cipher("aes", 0, CRYPTO_ALG_ASYNC);
    if (IS_ERR(Device.blkcipher)) {
        printk("ebd: Failed to load aes cryptographic transform\n");
        goto free_mem;
    }

    // Initialize the scatter/gather table used to perform the encryption.
    //sg_init_table(Device.sg, );

    // Set the key
    err = crypto_cipher_setkey(Device.blkcipher, test_key, KEY_SIZE);
    if (err != 0) {
        printk("Failed to set key");
        goto free_mem;
    }

    // Create the queue of memory blocks.
    Queue = blk_init_queue(ebd_request, &Device.lock);
    if (Queue == NULL) {
        printk("ebd: Failed to initialize request queue\n");
        goto free_mem;
    }

    // Set the block size.
    blk_queue_logical_block_size(Queue, logical_block_size);


    // Register the driver major number, a unique numeric id.
    major_num = register_blkdev(major_num, "ebd");
    if (major_num < 0) {
        printk("ebd: could not get major number for device\n");
        goto free_mem;
    }

    // Allocate the VRAM disk
    Device.gd = alloc_disk(16);
    if (Device.gd == 0) {
        goto remove_dev;
    }

    // Add the queue to the device structure.
    Device.gd->queue = Queue;

    // Set up the disk properly.
    Device.gd->major = major_num;
    Device.gd->first_minor = 0;
    Device.gd->fops = &ebd_ops;
    Device.gd->private_data = &Device;
    strncpy(Device.gd->disk_name, "ebd0", 4);
    // Set the disk capacity for the disk.
    set_capacity(Device.gd, num_sectors);

    add_disk(Device.gd);

    // Exit successfully.
    return 0;

remove_dev:
    // Unregister the device.
    unregister_blkdev(major_num, "ebd");
free_mem:
    // Free storage.
    vfree(Device.data);
    // Apparently returning error numbers is kernel convention.
    return -ENOMEM;
}

static void __exit ebd_exit(void) {
    // Remove the disk structure
    del_gendisk(Device.gd);
    // Update disk reference count.
    put_disk(Device.gd);
    unregister_blkdev(major_num, "ebd");
    blk_cleanup_queue(Queue);
    vfree(Device.data);
}

module_init(ebd_init);
module_exit(ebd_exit);

MODULE_AUTHOR("Ian Kronquist <iankronquist@gmail.com>");
MODULE_LICENSE("Dual MIT/GPL");
