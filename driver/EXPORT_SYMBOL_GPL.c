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

static char *robe_char = "robe char parameter";
module_param(robe_char,charp,S_IRUGO);
static int robe_int = 34;
module_param(robe_int, int,S_IRUGO);

static int add_robe(int a_robe,int b_robe )
{
	return a_robe+b_robe;
}
EXPORT_SYMBOL_GPL(add_robe);

static int sub_robe(int a_robe,int b_robe )
{
	return a_robe-b_robe;
}
EXPORT_SYMBOL_GPL(sub_robe);


static int __init bgpio_pdev_probe(void)
{
	printk(KERN_INFO "insmod  :bgpio_pdev_remove.\n");
	printk(KERN_INFO "robe_char : %s.\n",robe_char);
	printk(KERN_INFO "robe_int  : %d.\n",robe_int);
	return 0;
}
module_init(bgpio_pdev_probe);

static void __exit bgpio_pdev_remove(void)
{
	printk(KERN_INFO "rmmod  :bgpio_pdev_remove\n");
}
module_exit(bgpio_pdev_remove);

MODULE_DESCRIPTION("driver code by robe.");
MODULE_AUTHOR("robe zhang <82796620@qq.com>");
MODULE_LICENSE("GPL");

