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


#define GLOBALMEM_SIZE 0x20 

#define CMD_CLEAN 0
#define CMD_SET_R 1


dev_t devno;

static struct class *robe_class ;
static struct device *robe_device ;

struct globalmem_dev{
    struct cdev cdev;
    unsigned char mem[GLOBALMEM_SIZE];
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
	filp->private_data = NULL;
	return 0;
}


static ssize_t globalmem_read(struct file *filp, char __user *buff, size_t size, loff_t *ppos)
{
    int ret = 0;
    unsigned long p = *ppos;
    unsigned int count = size;

    struct globalmem_dev *dev = filp->private_data;
	printk(KERN_INFO"globalmem_read:globalmem_dev[%p].\n",dev);
    if (p >= GLOBALMEM_SIZE)
        return 0;
    if (count > GLOBALMEM_SIZE - p)
        count = GLOBALMEM_SIZE - p;
    if (copy_to_user(buff, dev->mem + p, count)){
        ret = -EFAULT;
    }else{
        *ppos += count;
        ret = count;
        printk(KERN_INFO"read %u bytes from %lu\n", count, p);
    }
	printk(KERN_INFO"globalmem_read OK.\n");
    return ret;
}


static ssize_t globalmem_write(struct file *filp, const char __user *buff, size_t size, loff_t *ppos)
{
	int ret=0;	
    unsigned long p = *ppos;
    unsigned int count = size;
    struct globalmem_dev *dev = filp->private_data;
	printk(KERN_INFO"globalmem_write:globalmem_dev[%p].\n",dev);
    if ( p >= GLOBALMEM_SIZE){
        return 0;
    	}
    if (count > GLOBALMEM_SIZE - p){
        count = GLOBALMEM_SIZE - p;
    	}
    if (copy_from_user(dev->mem + p, buff, count)){
        ret = -EFAULT;
    }else
    {
        *ppos += count;
        ret = count;
        printk(KERN_INFO"written %u bytes from %lu.\n", count, p);
    }
	printk(KERN_INFO"globalmem_write OK.\n");
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
	printk(KERN_INFO"globalmem llseek.\n");
    return ret;
}

static long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct globalmem_dev *dev = filp->private_data;
    switch(cmd){
		case CMD_CLEAN:
            memset(dev->mem, 0, GLOBALMEM_SIZE);
            printk(KERN_INFO"globalmem is set to zero\n");
            break;
		case CMD_SET_R:
            memset(dev->mem, 'R', GLOBALMEM_SIZE);
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
    devno = MKDEV(230,0);
	ret = register_chrdev_region(devno,1,"robe_register_chrdev_region");
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
	printk(KERN_NOTICE"globalmem_init OK.\n");
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

