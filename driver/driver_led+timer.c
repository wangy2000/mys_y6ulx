#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/timer.h>  /*timer*/
#include <asm/uaccess.h>  /*jiffies*/
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/input.h>
#include <../arch/arm/mach-imx/hardware.h>
#include <linux/stddef.h>
#include <linux/cdev.h>



#define ROBE_LED IMX_GPIO_NR(5,2)		// cpu
//#define ROBE_LED2 IMX_GPIO_NR(5,2)		// user


struct robe_key_dev{
    struct cdev cdev;
	struct timer_list s_timer;
	int led;
    int led_status;
};

struct robe_key_dev *robe_key_devp;
 


void robe_key_callback_b(void){
	if(robe_key_devp->led_status == 1)
		robe_key_devp->led_status = 0;
	else
		robe_key_devp->led_status = 1;
	gpio_set_value(robe_key_devp->led,robe_key_devp->led_status);
	return;
}

static void robe_key_callback(unsigned long arg){
	mod_timer(&robe_key_devp->s_timer,jiffies + 2*HZ);
	robe_key_callback_b();
	return;
}


static const struct file_operations robe_key_ops = {
    .owner = THIS_MODULE,
};


static int __init robe_key_init(void) 
{
	int ret;
	
	dev_t devno;	
	ret = alloc_chrdev_region(&devno,0,1,"robe_key_region");
	if(IS_ERR_OR_NULL(&ret)){
	    ret = -ENOMEM ;
	    goto error_allocate;
	}

	robe_key_devp = kzalloc(sizeof(struct robe_key_dev), GFP_KERNEL);
	if(robe_key_devp==NULL){
		ret = -ENOMEM;
		goto error_kzalloc;
	}

	robe_key_devp->cdev.owner = THIS_MODULE;
	cdev_init(&robe_key_devp->cdev, &robe_key_ops);
	ret = cdev_add(&robe_key_devp->cdev, devno, 1);
	if(IS_ERR_VALUE(ret)){
		printk(KERN_NOTICE"cdev_add ERRPR.\n");
		goto error_cdev_add ;;
	}

	robe_key_devp->led = ROBE_LED;	
	ret = gpio_request(robe_key_devp->led,"robe_key");
	printk(KERN_NOTICE"led : %d . led_request: %d. \n",robe_key_devp->led, ret);
	ret = gpio_direction_output(robe_key_devp->led,1);
	printk(KERN_NOTICE"led : %d . led_direction_output: %d. \n",robe_key_devp->led, ret);

	init_timer(&robe_key_devp->s_timer);
	robe_key_devp->s_timer.function = &robe_key_callback ;
	robe_key_devp->s_timer.expires = jiffies + 2*HZ;
	add_timer(&robe_key_devp->s_timer);
	
	printk(KERN_NOTICE"robe_key_init OK.\n");
	return 0 ;

error_cdev_add:	
	kfree(robe_key_devp);
error_kzalloc:
	unregister_chrdev_region(devno,1);
error_allocate:
	return ret ;
}
 
static void __exit robe_key_exit(void) 
{	
	del_timer(&robe_key_devp->s_timer);
	gpio_free(robe_key_devp->led);
	cdev_del(&robe_key_devp->cdev);
	kfree(robe_key_devp);
	unregister_chrdev_region(robe_key_devp->cdev.dev,1);
	printk(KERN_NOTICE"robe_key_exit OK.\n");
}
 
module_init(robe_key_init);
module_exit(robe_key_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("robe.zhang<82796620@qq.com>");
MODULE_DESCRIPTION("robe_key module");


