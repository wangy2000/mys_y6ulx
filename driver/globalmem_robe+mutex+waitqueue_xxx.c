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


#define GLOBALMEM_SIZE 0x20 

#define CMD_CLEAN 0
#define CMD_SET_R 1


dev_t devno;

static struct class *robe_class ;
static struct device *robe_device ;

struct globalmem_dev{
    struct cdev cdev;
	unsigned int current_len;
    unsigned char mem[GLOBALMEM_SIZE];
	struct mutex mutex;
	wait_queue_head_t r_wait;
	wait_queue_head_t w_wait;
};

struct globalmem_dev *globalmem_devp;

static int globalmem_open(struct inode *inode, struct file *filp)
{
	struct globalmem_dev *dev = container_of(inode->i_cdev, struct globalmem_dev, cdev);
    filp->private_data = dev;
	printk(KERN_INFO"globalmem_open:globalmem_dev[%p].\n",dev);
	return 0;
}

static int globalmem_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO"globalmem_release:globalmem_dev[%p].\n",filp->private_data);
	return 0;
}

int a;
static ssize_t globalmem_read(struct file *filp, char __user *buff, size_t size, loff_t *ppos)
{
    int ret = 0;
    unsigned long p = *ppos;
    unsigned int count = size;

	struct globalmem_dev *dev = filp->private_data;
	
	DECLARE_WAITQUEUE(wait,current);

	printk(KERN_INFO"globalmem_read:globalmem_dev[%p]. === a[%d]\n",dev,a++);
	
	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->r_wait,&wait);
	while(dev->current_len <=0){
		if(filp->f_flags & O_NONBLOCK){
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);
		schedule();
		if(signal_pending(current)){
			ret = -ERESTARTSYS;
			printk(KERN_INFO"globalmem_read:signal_pending.\n");
			goto out2;
		}
		mutex_lock(&dev->mutex);
	}

    if (count > GLOBALMEM_SIZE - p){
        count = GLOBALMEM_SIZE - p;
    	}
    if (copy_to_user(buff, dev->mem + p, count)){
        ret = -EFAULT;
		goto out;
    }else{
        memcpy(dev->mem,dev->mem + count, dev->current_len - count);
		dev->current_len -= count;
        ret = count;
        printk(KERN_INFO"read %u bytes from %d.\n", count, dev->current_len);
		wake_up_interruptible(&dev->w_wait);
    }
out:
//	mdelay(10000);
	mutex_unlock(&dev->mutex);
out2:
	remove_wait_queue(&dev->w_wait, &wait);
	set_current_state(TASK_RUNNING);	
	printk(KERN_INFO"globalmem_read EXIT.\n");
    return ret;
}


static ssize_t globalmem_write(struct file *filp, const char __user *buff, size_t size, loff_t *ppos)
{
	int ret=0;	
    unsigned long p = *ppos;
    unsigned int count = size;
    struct globalmem_dev *dev = filp->private_data;
	DECLARE_WAITQUEUE(wait,current);

	printk(KERN_INFO"globalmem_write:globalmem_dev[%p].\n",dev);
	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->w_wait,&wait);
	while(dev->current_len >= GLOBALMEM_SIZE){
		if(filp->f_flags & O_NONBLOCK){
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);
		schedule();
		if(signal_pending(current)){
			ret = -ERESTARTSYS;
			goto out2;
		}
		mutex_lock(&dev->mutex);
	}
    if (count > GLOBALMEM_SIZE - p){
        count = GLOBALMEM_SIZE - p;
    	}
    if (copy_from_user(dev->mem + p + dev->current_len , buff, count)){
        ret = -EFAULT;
		goto out;
    }else
    {
        dev->current_len += count;
        ret = count;
        printk(KERN_INFO"written %u bytes from %lu.\n", dev->current_len, p);
		wake_up_interruptible(&dev->r_wait);
    }
out:
	mutex_unlock(&dev->mutex);
out2:
	remove_wait_queue(&dev->w_wait,&wait);
	set_current_state(TASK_RUNNING);
	printk(KERN_INFO"globalmem_write EXIT.\n");
    return ret;
}

static loff_t globalmem_llseek(struct file *filp, loff_t offset, int arg)
{
    loff_t ret = 0;
    switch(arg){
    case 0://begin from the start
        if (offset < 0)
        {
            ret = -EINVAL;
            break;
        }
        if ((unsigned int)offset > GLOBALMEM_SIZE){
            ret = -EINVAL;
            break;
        }
        filp->f_pos = offset;
        ret = filp->f_pos;
        break;
    case 1://begin from current position
        if ((unsigned int)offset + (unsigned int)filp->f_pos < 0)
        {
            ret = -EINVAL;
            break;
        }
        if ((unsigned int)offset + (unsigned int)filp->f_pos > GLOBALMEM_SIZE)
        {
            ret = -EINVAL;
            break;
        }
        filp->f_pos += offset;
        ret = filp->f_pos;
        break;
    default:
        ret = -EINVAL;
        break;
    }
	printk(KERN_INFO"globalmem llseek EXIT.\n");
    return ret;
}

static long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct globalmem_dev *dev = filp->private_data;
    switch(cmd){
		case CMD_CLEAN:
			mutex_lock(&dev->mutex);
            memset(dev->mem, 0, GLOBALMEM_SIZE);
			mutex_unlock(&dev->mutex);
            printk(KERN_INFO"globalmem is set to zero\n");
            break;
		case CMD_SET_R:
			mutex_lock(&dev->mutex);
            memset(dev->mem, 'R', GLOBALMEM_SIZE);
			mutex_unlock(&dev->mutex);
            printk(KERN_INFO"globalmem is set to 'R'.\n");
            break;
       default:
            return -EINVAL;
    }
	return 0;
}
static const struct file_operations globalmem_fops = {
    .owner = THIS_MODULE,
    .llseek = globalmem_llseek,
    .read = globalmem_read,
    .write = globalmem_write,
    .open = globalmem_open,
    .release = globalmem_release,
    .unlocked_ioctl = globalmem_ioctl,
};

static int __init globalmem_init(void){
    int ret;
/*    devno = MKDEV(230,0);
	  ret = register_chrdev_region(devno,1,"robe_register_chrdev_region");
*/	ret = alloc_chrdev_region(&devno,0,1,"robe_register_chrdev_region");
    if (ret < 0)return ret;
	printk(KERN_NOTICE"MAJOR(%d),MINOR(%d).\n",MAJOR(devno),MINOR(devno));
    globalmem_devp = kzalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
    if (!globalmem_devp)
    {
        ret = -ENOMEM;
        goto fail_malloc;
    }
	
	cdev_init(&globalmem_devp->cdev, &globalmem_fops);
	globalmem_devp->cdev.owner = THIS_MODULE;
	mutex_init(&globalmem_devp->mutex);
	init_waitqueue_head(&globalmem_devp->r_wait);
	init_waitqueue_head(&globalmem_devp->w_wait);
	globalmem_devp->current_len = 0;
	
	ret = cdev_add(&globalmem_devp->cdev, devno, 1);
	if (ret){
		goto fail_cdev_add;
        printk(KERN_NOTICE"init->cdev_add err. ERROR[%d].\n", ret);
		}
	printk(KERN_NOTICE" globalmem_init: globalmem_devp[%p].\n",globalmem_devp);

	robe_class = class_create(THIS_MODULE, "robe_class_create");
	if(IS_ERR(robe_class))
		goto fail_class_create;
	robe_device = device_create(robe_class,NULL,devno,NULL,"robe_device_create");
	if(IS_ERR(robe_device))
		goto fail_device_create;
	printk(KERN_NOTICE"globalmem_init EXIT.\n");
    return 0;
	
fail_device_create:
	class_destroy(robe_class);
fail_class_create:
	cdev_del(&((globalmem_devp)->cdev));
fail_cdev_add:
	kfree(globalmem_devp);
	printk(KERN_NOTICE"fail_cdev_add.\n");
fail_malloc:
    unregister_chrdev_region(devno, 1);
	printk(KERN_NOTICE"fail_malloc.\n");
    return ret;
}
module_init(globalmem_init);

static void __exit globalmem_exit(void)
{
	device_destroy(robe_class, devno);
	class_destroy(robe_class);
    cdev_del(&((globalmem_devp)->cdev));
	printk(KERN_NOTICE"globalmem_exit:cdev_del[%p].\n",&((globalmem_devp)->cdev));
    kfree(globalmem_devp);
    unregister_chrdev_region(devno, 1);
}
module_exit(globalmem_exit);
MODULE_LICENSE("Dual BSD/GPL");

