#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");

static uint howmany = 1;
module_param(howmany, uint, S_IRUGO);
MODULE_PARM_DESC(howmany, "A parameter that defines a number of output strings");

struct time_list {
	struct list_head header;
	ktime_t time;
};

static LIST_HEAD(time_list_head);

void list_free(struct list_head *head)
{
	struct list_head *pos, *n;

	list_for_each_safe(pos, n, head) {
		kfree(pos);
	}

	INIT_LIST_HEAD(head);
}

static int __init hello_init(void)
{
	if (howmany > 10) {
		printk(KERN_ERR "Error: the parameter cannot be greater than 10\n");
		return -EINVAL;
	}

	if (!howmany || (howmany >= 5 && howmany <= 10))
		printk(KERN_WARNING "Warning: the parameter is set to %u\n", howmany);

	struct time_list *next;
	uint i;

	for (i = 0; i < howmany; ++i) {
		next = kmalloc(sizeof(struct time_list), GFP_KERNEL);
		if (next) {
			list_add_tail(&next->header, &time_list_head);

			next->time = ktime_get();

			printk(KERN_NOTICE "Hello, world\n");
		} else {
			list_free(&time_list_head);
			return -ENOMEM;
		}
	}

	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	struct list_head *pos;

	list_for_each(pos, &time_list_head) {
		printk(KERN_NOTICE "%lld", ktime_to_ns(list_entry(pos, struct time_list, header)->time));
	}

	list_free(&time_list_head);

	printk(KERN_NOTICE "Goodbye, cruel world\n");
}
module_exit(hello_exit);
