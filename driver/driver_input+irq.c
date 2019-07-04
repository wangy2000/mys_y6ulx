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


struct input_dev *dev ;

#define ROBE_GPIO IMX_GPIO_NR(5,0)
 
static irqreturn_t irq_fuction(int irq, void *dev_id)
{	
//	keydev = dev_id ; //上报键值
	input_report_key(dev, KEY_HOME,  \
				gpio_get_value(ROBE_GPIO));
	input_sync(dev); //上报一个同步事件	
	printk(KERN_NOTICE"irq:%d.\n",irq);
	return IRQ_HANDLED ;
}
	
static int __init robe_input_dev_init(void) 
{
	int err = 0 ;
	int irq_num1 = 0;
	int ret ;
	dev = input_allocate_device();
	if(IS_ERR_OR_NULL(dev)){
	    ret = -ENOMEM ;
		printk(KERN_NOTICE"input_allocate_device ERRPR.\n");
	    goto error_allocate;
	}

	dev->name = "robe_input_dev_name" ;
	dev->phys = "robe_input_dev_phys" ;
	dev->uniq = "robe_input_dev_uniq" ; 
	dev->id.bustype = BUS_HOST ; 
	dev->id.vendor = ID_PRODUCT ;
	dev->id.version = ID_VENDOR ;
	set_bit(EV_SYN,dev->evbit);	//设置为同步事件，这个宏可以在input.h中找到
	set_bit(EV_KEY,dev->evbit);	//因为是按键，所以要设置成按键事件
	set_bit(KEY_HOME,dev->keybit);	//设置这个按键表示为KEY_HOME这个键，到时用来上报
	ret = input_register_device(dev); //注册input设备
	if(IS_ERR_VALUE(ret)){
		printk(KERN_NOTICE"input_register_device ERRPR.\n");
		goto error_input_register ;;
	}
	    
	irq_num1 = gpio_to_irq(ROBE_GPIO);//申请中断号，并注册中断，下降沿触发，设备就是input设备
	printk("robe_input_dev_init: irq:%d.\n",irq_num1);
	err = request_irq(irq_num1,irq_fuction,0x83,"robe_input_dev_irq",dev);
	if(err != 0){
		printk(KERN_NOTICE"request_irq ERRPR.\n");
	     goto error_request_irq ;
		}
	printk(KERN_NOTICE"robe_input_dev_init OK.\n");
	return 0 ;
error_request_irq:
	input_unregister_device(dev);	
error_input_register:
	
	input_free_device(dev);
error_allocate:
	return ret ;
}
 
static void __exit robe_input_dev_exit(void) 
{	
	int irq_num1 ;
	irq_num1 = gpio_to_irq(ROBE_GPIO);
	free_irq(irq_num1,dev);
	input_unregister_device(dev);
	printk(KERN_NOTICE"robe_input_dev_exit OK.\n");
}
 
module_init(robe_input_dev_init);
module_exit(robe_input_dev_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("YYX");
MODULE_DESCRIPTION("Exynos4 KEY Driver");


