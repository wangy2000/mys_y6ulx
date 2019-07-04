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



struct second_dev{
    struct cdev cdev;
	atomic_t counter;
	struct timer_list s_timer;
	struct class *second_class ;
	struct device *second_device ;
	dev_t devno;
};

struct second_dev *second_devp;

static ssize_t second_read (struct file *filp, char __user *buf, size_t count, loff_t *ppos){
	struct second_dev *dev = filp->private_data;
	int tmp_count =0 ;

	tmp_count = atomic_read(&dev->counter);
	if(copy_to_user(buf, &tmp_count,sizeof(int))){
		return -EFAULT;
	}
	else{
		return sizeof(int);
	}
}

static void second_callback(unsigned long arg){
	mod_timer(&second_devp->s_timer,jiffies + HZ);
	atomic_inc(&second_devp->counter);
	printk(KERN_INFO"robe_second_callback. robe_second[%d].\n",atomic_read(&second_devp->counter));
	return;
}

static int second_open (struct inode *inode, struct file *filp){
	filp->private_data = container_of(inode->i_cdev, struct second_dev, cdev);
	atomic_set(&second_devp->counter,0);
	init_timer(&second_devp->s_timer);
	second_devp->s_timer.function = &second_callback ;
	second_devp->s_timer.expires = jiffies + HZ;
	add_timer(&second_devp->s_timer);
	printk(KERN_INFO"robe_second_open.\n");
	return 0;
}

static int second_release (struct inode *inode, struct file *filp){
	filp->private_data = NULL;
	del_timer(&second_devp->s_timer);
	printk(KERN_INFO"robe_second_release.\n");
	return 0;
}

static const struct file_operations second_fops = {
    .owner = THIS_MODULE,
    .read = second_read,
    .open = second_open,
    .release = second_release,
};

static int __init second_init(void){
	
	int ret;

	second_devp = kzalloc(sizeof(struct second_dev), GFP_KERNEL);
    if (!second_devp)
    {
    	ret = -ENOMEM;
        goto fail_kzalloc;
    }
	ret = alloc_chrdev_region(&second_devp->devno,0,1,"robe_second_alloc_chrdev_region");
    if (ret < 0){
		goto fail_chrdev_region;
    }
	printk(KERN_NOTICE"MAJOR(%d),MINOR(%d).\n",MAJOR(second_devp->devno),MINOR(second_devp->devno));
    
	cdev_init(&second_devp->cdev, &second_fops);
	second_devp->cdev.owner = THIS_MODULE;
	
	ret = cdev_add(&second_devp->cdev, second_devp->devno, 1);
	if (ret){
		goto fail_cdev_add;
		}
	printk(KERN_NOTICE" second_init: second_devp[%p].\n",second_devp);

	second_devp->second_class = class_create(THIS_MODULE, "robe_second_class_create");
	if(IS_ERR(second_devp->second_class)){
		ret = -EBUSY;
		goto fail_class_create;
	}
	second_devp->second_device = device_create(second_devp->second_class,NULL,second_devp->devno,NULL,"robe_second_device_create");
	if(IS_ERR(second_devp->second_device)){
		ret = -EBUSY;
		goto fail_device_create;
	}
	printk(KERN_NOTICE"second_init EXIT.\n");
    return 0;
	
	fail_device_create:
		class_destroy(second_devp->second_class);

	fail_class_create:
		cdev_del(&((second_devp)->cdev));

	fail_cdev_add:
		unregister_chrdev_region(second_devp->devno, 1);

	fail_chrdev_region:
    
		kfree(second_devp);
	fail_kzalloc:
		return ret;
}
module_init(second_init);

static void __exit second_exit(void)
{
	
	device_destroy(second_devp->second_class,second_devp->devno);
	class_destroy(second_devp->second_class);
    cdev_del(&second_devp->cdev);
	printk(KERN_NOTICE"globalmem_exit:cdev_del[%p].\n",&((second_devp)->cdev));
    kfree(second_devp);
    unregister_chrdev_region(second_devp->devno, 1);
}
module_exit(second_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Robe zhang,<82796620@qq.com>");


