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



#define ROBE_LED IMX_GPIO_NR(5,1)		// cpu
//#define ROBE_LED2 IMX_GPIO_NR(5,2)		// user
#define ROBE_KEY IMX_GPIO_NR(5,0)		// key

#define LED_interval 20
#define KEY_interval 1				// HZ=100


struct robe_key_dev{
    struct cdev cdev;
	struct timer_list s_timer;
	struct timer_list k_timer;
	int led;
    int led_status;
	int key;
	int key_status;
	int irq_key;
	int key_dev_id;
};

struct robe_key_dev *robe_key_devp;
static struct class *robe_class ;
static struct device *robe_device ;



void robe_led_callback_b(void){
	return;
}

static void robe_led_callback(unsigned long arg){
	mod_timer(&robe_key_devp->s_timer,jiffies + LED_interval);
	return;
}

static void robe_key_callback(unsigned long arg){
	mod_timer(&robe_key_devp->k_timer,jiffies + KEY_interval);
	if(gpio_get_value(robe_key_devp->key))
		gpio_set_value(robe_key_devp->led,0);
	else
		gpio_set_value(robe_key_devp->led,1);
	return;
}


static const struct file_operations robe_key_ops = {
    .owner = THIS_MODULE,
};

irqreturn_t robe_key_hander (int irq,void*dev_id)
{
	if(robe_key_devp->key_status == 0)
		robe_key_devp->key_status = 1;
	else
		robe_key_devp->key_status = 0;
	printk(KERN_NOTICE"key_irq: %d .\n",irq);
	return (IRQ_HANDLED);
}


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

	robe_class = class_create(THIS_MODULE, "robe_class_create");
	if(IS_ERR(robe_class))
	goto error_class_create;
	robe_device = device_create(robe_class,NULL,devno,NULL,"robe_led");
	if(IS_ERR(robe_device))
	goto error_device_create;

	robe_key_devp->led = ROBE_LED;	
	ret = gpio_request(robe_key_devp->led,"robe_key");
	printk(KERN_NOTICE"led : %d . led_request: %d. \n",robe_key_devp->led, ret);
	if(ret){
		goto error_gpio_request;
	}
	ret = gpio_direction_output(robe_key_devp->led,1);
	printk(KERN_NOTICE"led : %d . led_direction_output: %d. \n",robe_key_devp->led, ret);
	if(ret){
		goto error_gpio_direction_output;
	}

	robe_key_devp->key = ROBE_KEY;	
	ret = gpio_request(robe_key_devp->key,"robe_key");
	printk(KERN_NOTICE"key : %d . key_request: %d. \n",robe_key_devp->key, ret);
	if(ret){
		goto error_key_request;
	}
	
	ret = gpio_direction_input(robe_key_devp->key);
	printk(KERN_NOTICE"led : %d . led_direction_output: %d. \n",robe_key_devp->key, ret);
	if(ret){
		goto error_key_direction_input;
	}
	
	printk(KERN_NOTICE" HZ[%d]\n",HZ);
	
	init_timer(&robe_key_devp->s_timer);
	robe_key_devp->s_timer.function = &robe_led_callback ;
	robe_key_devp->s_timer.expires = jiffies + LED_interval;
	add_timer(&robe_key_devp->s_timer);

	init_timer(&robe_key_devp->k_timer);
	robe_key_devp->k_timer.function = &robe_key_callback ;
	robe_key_devp->k_timer.expires = jiffies + KEY_interval;
	add_timer(&robe_key_devp->k_timer);
	
	printk(KERN_NOTICE"robe_key_init OK.\n");
	return 0 ;

error_key_direction_input:

	gpio_free(robe_key_devp->key);
error_key_request:

error_gpio_direction_output:
	gpio_free(robe_key_devp->led);
error_gpio_request:
	device_destroy(robe_class, robe_key_devp->cdev.dev);
error_device_create:
	class_destroy(robe_class);
error_class_create:
	cdev_del(&robe_key_devp->cdev);
error_cdev_add:	
	kfree(robe_key_devp);
error_kzalloc:
	unregister_chrdev_region(devno,1);
error_allocate:
	printk(KERN_NOTICE"robe_key_init ERROR.\n");
	return ret ;
}
 
static void __exit robe_key_exit(void) 
{	
	
	del_timer(&robe_key_devp->s_timer);
	del_timer(&robe_key_devp->k_timer);
	gpio_free(robe_key_devp->led);
	gpio_free(robe_key_devp->key);
	device_destroy(robe_class, robe_key_devp->cdev.dev);
	class_destroy(robe_class);
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


