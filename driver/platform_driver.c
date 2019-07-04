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


static int robe_driver_probe(struct platform_device *pdev){
	printk(KERN_NOTICE"robe_driver_probe.\n");
	return 0;
}
static int robe_driver_remove(struct platform_device *pdev){
	printk(KERN_NOTICE"robe_driver_probe.\n");
	return 0;
}

struct platform_driver robe_driver ={
	.probe = robe_driver_probe ,
	.remove = robe_driver_remove,
  	.driver = {
		.name = "robe_platform_dd",
		.owner = THIS_MODULE,
	},
};
module_platform_driver(robe_driver);



MODULE_LICENSE("Dual BSD/GPL");

