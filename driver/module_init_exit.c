#include <linux/init.h>
#include <linux/err.h>
#include <linux/bug.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/compiler.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/log2.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/basic_mmio_gpio.h>


static int __init bgpio_pdev_probe(void)
{
	printk(KERN_INFO "insmod :bgpio_pdev_probe\n");
	return 0;
}

static void __exit bgpio_pdev_remove(void)
{
	printk(KERN_INFO "rmmod  :bgpio_pdev_remove\n");
}

module_init(bgpio_pdev_probe);
module_exit(bgpio_pdev_remove);

MODULE_DESCRIPTION("driver code by robe.");
MODULE_AUTHOR("robe zhang <82796620@qq.com>");
MODULE_LICENSE("GPL");
