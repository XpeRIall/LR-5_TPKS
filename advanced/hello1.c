#include "hello1.h"

#include <linux/errno.h>
#include <linux/ktime.h>
#include <linux/slab.h>

struct time_list {
	struct list_head header;
	ktime_t begin_time, end_time;
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

static int __init hello1_init(void)
{
	pr_info("Hello1 module loaded\n");
	return 0;
}
module_init(hello1_init);

int print_hello(uint n)
{
	struct time_list *next;
	uint i;

	if (n > 10) {
		pr_err("Error: the parameter cannot be greater than 10\n");
		return -EINVAL;
	}

	if (!n || (n >= 5 && n <= 10))
		pr_warn("Warning: the parameter is set to %u\n", n);

	for (i = 0; i < n; ++i) {
		next = kmalloc(sizeof(struct time_list), GFP_KERNEL);
		if (next) {
			list_add_tail(&next->header, &time_list_head);

			next->begin_time = ktime_get();

			pr_info("Hello, world\n");

			next->end_time = ktime_get();
		} else {
			list_free(&time_list_head);
			return -ENOMEM;
		}
	}

	return 0;
}
EXPORT_SYMBOL(print_hello);

void print_time(void)
{
	struct list_head *pos;

	list_for_each(pos, &time_list_head) {
		struct time_list *struct_buff = list_entry(pos,
			struct time_list, header);

		pr_info("%lld", ktime_to_ns(struct_buff->end_time)
			- ktime_to_ns(struct_buff->begin_time));
	}

	list_free(&time_list_head);
}
EXPORT_SYMBOL(print_time);

static void __exit hello1_exit(void)
{
	pr_info("Hello1 module unloaded\n");
}
module_exit(hello1_exit);
