#include <linux/module.h>
#include <linux/random.h>
#include <linux/slab.h>

static int __init ff_slob_stability_init(void) {
	unsigned int times, buf_size, i;
	void *prev_alloc = NULL, *this_alloc;
	get_random_bytes(&times, sizeof(times));
	times %= 10000;
	printk("Making %u allocations.\n", times);
	for (i = 0; i < times; ++i) {
		get_random_bytes(&buf_size, sizeof(buf_size));
		buf_size %= 100000;
		this_alloc = kmalloc(buf_size, GFP_KERNEL);
		kfree(this_alloc);
	}
	printk("Done.\n");
	return 0;
}

static void __exit ff_slob_stability_exit(void) {
}

module_init(ff_slob_stability_init);
module_exit(ff_slob_stability_exit);

MODULE_AUTHOR("Ian Kronquist <iankronquist@gmail.com>");
MODULE_LICENSE("Dual MIT/GPL");
