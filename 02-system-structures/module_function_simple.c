#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

// This function is called when the module is loaded.
int simple init(void)
{
	printk(KERN_INFO "Loading Module\n");
	return 0;
}

// This function is called when the module is removed.
void simple_exit(void)
{
	printk(KERN_INFO "Removing Module\n");
}

// Macros for reistering module entry and exit points.
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENCE("GPL");
MODULE_DESCRIPTION("SIMPLE MODULE");
MODULE_ARTHUR("JS");
