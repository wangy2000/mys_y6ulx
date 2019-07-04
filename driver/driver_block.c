#include<linux/module.h>
#include<linux/spinlock_types.h>
#include<linux/slab.h>
#include<linux/blkdev.h>
#include<linux/kern_levels.h>


#define BD_NAME "robe_blk_dev"


struct blk_dev{
	char name[0x20];
	int major;
	int minor_first;
	int minor_cur;
	int minor_max;
	
	unsigned int sector;
	unsigned int header;
	unsigned int cylinder;
	unsigned int disk_cap;
	
	unsigned int max_hw_sectors;
	unsigned int logical_block_size;

	char *data;
	
	spinlock_t lock;

	struct request_queue *rq;
	struct gendisk *gd;
};

 struct blk_dev *robe_blk_dev;

static int robe_blk_dev_hw_init(struct blk_dev *bd)
{
	sprintf(bd->name,BD_NAME);
	bd->major = 0;
	bd->minor_first = 0;
	bd->minor_cur = 1;
	bd->minor_max = 16;

	bd->sector = 512;
	bd->header = 1;
	bd->cylinder = 0x800;
	bd->disk_cap = bd->sector * bd->header * bd->cylinder;
	
	bd->max_hw_sectors = bd->header * bd->cylinder;								// ?
	bd->logical_block_size = bd->disk_cap;

	bd->data = kzalloc(bd->disk_cap,GFP_KERNEL);
	if(bd->data == NULL)
		return -ENOMEM;

	spin_lock_init(&(bd->lock));
	
	printk(KERN_INFO"robe_blk_dev INFO:\n");
	printk(KERN_INFO"name[%s] - major[%d] - minor_first[%d] - minor_cur[%d] - minor_max[%d].\n"
		,bd->name,bd->major,bd->minor_first, bd->minor_cur,bd->minor_max );
	printk(KERN_INFO"sector[%ul] - header[%ul] - cylinder[%ul] - disk_cap[%d].\n"
		,bd->sector,bd->header,bd->cylinder,bd->disk_cap );
	printk(KERN_INFO"max_hw_sectors[%ul] - logical_block_size[%ul].\n"
		,bd->max_hw_sectors,bd->logical_block_size);
	printk(KERN_INFO"data[%p] - request_queue[%p] - gendisk[%p].\n",&(bd->data),&(bd->rq),&(bd->gd));

	return 0;
}

static int robe_bd_open(struct block_device *bd, fmode_t mode){
	return 0;
}

static void robe_bd_release(struct gendisk *bd, fmode_t mode){
	return;
}

static int robe_bd_ioctl (struct block_device *bd, fmode_t mode , unsigned arg1, unsigned long arg2){
	return 0;
}

static int robe_bd_getgeo(struct block_device *bd, struct hd_geometry *hd_gen){
	;
	return 0;
}

struct block_device_operations robe_blk_dev_ops = {
	.owner = THIS_MODULE,
	.open = robe_bd_open,
	.release = robe_bd_release,
	.ioctl = robe_bd_ioctl,
	.getgeo = robe_bd_getgeo,
};

void robe_blk_dev_request(struct request_queue *q){};							// ?

static int __init robe_blk_dev_init(void)
{
    int ret = 0;


	/* -------------------------------------------------------- block device alloc and init */
	robe_blk_dev = kzalloc(sizeof(struct blk_dev), GFP_KERNEL);
	if(robe_blk_dev == NULL){
		ret = -ENOMEM;
		goto error_kzalloc_dev;
	}

	ret = robe_blk_dev_hw_init(robe_blk_dev);
	if(ret == -ENOMEM){
		goto error_kzalloc_hw;
	}

	/* -------------------------------------------------------------  register block device */
	ret = register_blkdev(robe_blk_dev->major, robe_blk_dev->name);
	if(ret < 0){
		goto error_register_blkdev;
	}
	robe_blk_dev->major = ret;
	printk(KERN_INFO"robe_blk_dev->major[%d].\n",robe_blk_dev->major);

	robe_blk_dev->rq = blk_init_queue(robe_blk_dev_request,&(robe_blk_dev->lock));
	if(robe_blk_dev->rq == NULL ){
		ret = -EFAULT;
		goto error_blk_init_queue;
	}
	
//	blk_queue_max_hw_sectors(robe_blk_dev->rq, robe_blk_dev->max_hw_sectors);
//	blk_queue_logical_block_size(robe_blk_dev->rq, robe_blk_dev->logical_block_size);

	robe_blk_dev->gd = alloc_disk(robe_blk_dev->minor_cur);
	if(!robe_blk_dev->gd){
		ret = -EFAULT;
		goto error_alloc_disk;
	}
	robe_blk_dev->gd->major = robe_blk_dev->major;
	robe_blk_dev->gd->first_minor = robe_blk_dev->minor_first;
	robe_blk_dev->gd->fops = &robe_blk_dev_ops;
	robe_blk_dev->gd->queue = robe_blk_dev->rq;
	sprintf(robe_blk_dev->gd->disk_name,robe_blk_dev->name);
	
	set_capacity(robe_blk_dev->gd, robe_blk_dev->logical_block_size);
	printk(KERN_INFO"add_disk.\n");
	add_disk(robe_blk_dev->gd);
	printk(KERN_INFO"robe_blk_dev_init.\n");
	
	return 0;


	del_gendisk(robe_blk_dev->gd);
	error_alloc_disk:
	blk_cleanup_queue(robe_blk_dev->rq);
	error_blk_init_queue:
	unregister_blkdev(robe_blk_dev->major,robe_blk_dev->name);
	error_register_blkdev:
	kfree(robe_blk_dev->data);
	error_kzalloc_hw:
	kfree(robe_blk_dev);
	error_kzalloc_dev:
    return ret;
}

static void __exit robe_blk_dev_exit(void)
{
    del_gendisk(robe_blk_dev->gd);
	blk_cleanup_queue(robe_blk_dev->rq);
	unregister_blkdev(robe_blk_dev->major,robe_blk_dev->name);
	kfree(robe_blk_dev->data);
	kfree(robe_blk_dev);
	printk(KERN_INFO"robe_blk_dev_exit.\n");
}

module_init(robe_blk_dev_init);
module_exit(robe_blk_dev_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("robe zhang <82796620@qq.com>");
MODULE_DESCRIPTION("block_device_driver for test");

