#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/platform_device.h>


static void robe_device_release(struct device *dev)
{
	printk(KERN_NOTICE"robe_device_release.\n");
	return ;
}


static struct platform_device robe_device = {
	.name = "robe_input_dd",
	.id = -1,
	.dev.release = robe_device_release,
};

static int robe_device_init(void){
	printk(KERN_NOTICE"robe_device_init.\n");
	return platform_device_register(&robe_device);
}

static void robe_device_exit(void){
	printk(KERN_NOTICE"robe_device_exit.\n");
	platform_device_unregister(&robe_device);
}

module_init(robe_device_init);
module_exit(robe_device_exit);



MODULE_LICENSE("Dual BSD/GPL");

