#include<linux/module.h>
#include<linux/spinlock_types.h>
#include<linux/slab.h>
#include<linux/blkdev.h>
#include<linux/kern_levels.h>
#include<asm/page.h>
#include<asm-generic/current.h>
#include <linux/thread_info.h>
#include <linux/kernel.h>
#include <uapi/linux/hdreg.h>
#include<linux/blk_types.h>



struct blk_dev{
	char name[0x20];				/* device info */
	int major;
	int minor_first;
	int minor_cur;
	int minor_max;
	
	unsigned int sector;				/* hw info */
	unsigned int header;
	unsigned int cylinder;
	unsigned int nr_sectors;
	unsigned int disk_cap;

	unsigned int page_size;					/* sys info */
	unsigned int max_hw_sectors;
	unsigned int logical_block_size;

	char *data;								/* gendisk data */
	
	spinlock_t lock;						/* request_queue lock */

	struct request_queue *rq;				/* request_queue */
	struct gendisk *gd;						/* gendisk */
};

 struct blk_dev *robe_blk_dev;

static int robe_blk_dev_hw_init(struct blk_dev *bd)
{
	sprintf(bd->name,"robe_blk_dev");
	bd->major = 0;
	bd->minor_first = 0;
	bd->minor_cur = bd->minor_first;bd->minor_cur++;
	bd->minor_max = 16;

	bd->sector = 0x200;
	bd->header = 0x4;
	bd->cylinder = 0x1000;
	bd->nr_sectors = bd->header * bd->cylinder;
	bd->disk_cap = bd->sector * bd->nr_sectors;

	bd->page_size = PAGE_SIZE;
	bd->max_hw_sectors = bd->page_size / bd->sector ;
	bd->logical_block_size = bd->disk_cap / bd->max_hw_sectors ;

	bd->data = kzalloc(bd->disk_cap,GFP_KERNEL);
	if(bd->data == NULL)
		return -ENOMEM;

	spin_lock_init(&(bd->lock));
	
	printk(KERN_INFO"robe_blk_dev INFO:\n");
	printk(KERN_INFO"name[%s] major[%d] minor_first[%d] minor_cur[%d] minor_max[%d].\n"
		,bd->name,bd->major,bd->minor_first, bd->minor_cur,bd->minor_max );
	printk(KERN_INFO"sector[%u] header[%u] cylinder[%u] nr_sectors[%u] disk_cap[%d].\n"
		,bd->sector,bd->header,bd->cylinder,bd->nr_sectors,bd->disk_cap );
	printk(KERN_INFO"page_size[%u] max_hw_sectors[%d] logical_block_size[%u].\n"
		,bd->page_size,bd->max_hw_sectors,bd->logical_block_size);
	printk(KERN_INFO"data[%p] request_queue[%p] gendisk[%p].\n",&(bd->data),&(bd->rq),&(bd->gd));

	return 0;
}
# if 0
static int robe_bd_open(struct block_device *bd, fmode_t mode){
/*	struct blk_dev *bdev ; = container_of(bd->bd_disk,struct blk_dev, gd);
	bd->bd_disk->private_data = bdev;
*/	
	return 0;
}

static void robe_bd_release(struct gendisk *bd, fmode_t mode){
	bd->private_data = NULL;
	
	return;
}

static int robe_bd_ioctl (struct block_device *bd, fmode_t mode , unsigned arg1, unsigned long arg2){
	return 0;
}
#endif

static int robe_bd_getgeo(struct block_device *bd, struct hd_geometry *hd_gen){
	struct blk_dev *bdev = bd->bd_disk->private_data;

	hd_gen->heads = bdev->header;
	hd_gen->sectors = bdev->sector;
	hd_gen->cylinders = bdev->cylinder;
	hd_gen->start = 0;

	return 0;
}

struct block_device_operations robe_blk_dev_ops = {
	.owner = THIS_MODULE,
/*	.open = robe_bd_open,
	.release = robe_bd_release,
	.ioctl = robe_bd_ioctl,
*/	.getgeo = robe_bd_getgeo,
};

static void ribe_blk_dev_transfer(struct blk_dev *dev,sector_t sector, unsigned long nsect, char * buffer, int write){
	unsigned long offset = sector * dev->sector;
	unsigned long nbytes = nsect * dev->sector;

	if((offset + nbytes ) > dev->disk_cap ){
		printk(KERN_INFO" out of disk capability.");
		return;
	}
	if(write){
		memcpy(dev->data + offset , buffer, nbytes);
	}else{
		memcpy( buffer,dev->data + offset , nbytes);;
	}
	return ;
}

static void robe_blk_dev_xfer_bio(struct blk_dev *dev, struct bio *bio ){
	struct bio_vec bvl;
	struct bvec_iter iter;
	sector_t sector = bio->bi_iter.bi_sector;

	bio_for_each_segment(bvl, bio, iter){
		char *buffer = __bio_kmap_atomic(bio, iter);
		ribe_blk_dev_transfer(dev,sector,bio_cur_bytes(bio) >> 9 ,buffer,bio_data_dir(bio) == WRITE);
		sector += bio_cur_bytes(bio) >> 9;
		__bio_kunmap_atomic(buffer);
	}
	return;
}

void robe_blk_dev_do_request(struct request_queue *q){
	struct request *req;
	struct bio *bio ;

	while((req = blk_peek_request(q)) != NULL){
		
		if(req->cmd_type != REQ_TYPE_FS){
			printk(KERN_INFO"skip req:not fs req.\n");
			blk_start_request(req);
			blk_end_request_all(req,-EIO);
			continue;
		}

		blk_start_request(req);
		__rq_for_each_bio(bio,req){
			printk(KERN_INFO"robe_blk_dev_do_request: dev->gd[%p].\n",req->rq_disk);
			robe_blk_dev_xfer_bio(req->rq_disk->private_data,bio);
		}
		blk_end_request_all(req,0);

	}
}

static int __init robe_blk_dev_init(void)
{
    int ret = 0;

	/* -------------------------------------------------------- blk_dev alloc and init */
	robe_blk_dev = kzalloc(sizeof(struct blk_dev), GFP_KERNEL);
	if(robe_blk_dev == NULL){
		ret = -ENOMEM;
		printk(KERN_INFO"ERROR:kzalloc.\n");
		goto error_kzalloc_dev;
	}

	ret = robe_blk_dev_hw_init(robe_blk_dev);
	if(ret == -ENOMEM){
		printk(KERN_INFO"ERROR:robe_blk_dev_hw_init.\n");
		goto error_kzalloc_hw;
	}

	/* -------------------------------------------------------------  register blk_dev */
	ret = register_blkdev(robe_blk_dev->major, robe_blk_dev->name);
	if(ret < 0){
		printk(KERN_INFO"ERROR:register_blkdev.\n");
		goto error_register_blkdev;
	}
	robe_blk_dev->major = ret;
	printk(KERN_INFO"robe_blk_dev->major[%d].\n",robe_blk_dev->major);

	/* -------------------------------------------------------------  request_queue init */
	robe_blk_dev->rq = blk_init_queue(robe_blk_dev_do_request,&(robe_blk_dev->lock));
	if(robe_blk_dev->rq == NULL ){
		ret = -EFAULT;
		printk(KERN_INFO"ERROR:blk_init_queue.\n");
		goto error_blk_init_queue;
	}
	blk_queue_max_hw_sectors(robe_blk_dev->rq, robe_blk_dev->max_hw_sectors);
	blk_queue_logical_block_size(robe_blk_dev->rq, robe_blk_dev->logical_block_size);

	/* ---------------------------------------------------------------      gendisk init */
	robe_blk_dev->gd = alloc_disk(robe_blk_dev->minor_cur);
	if(!robe_blk_dev->gd){
		ret = -EFAULT;
		printk(KERN_INFO"ERROR:alloc_disk.\n");
		goto error_alloc_disk;
	}
	sprintf(robe_blk_dev->gd->disk_name,robe_blk_dev->name);
	robe_blk_dev->gd->major = robe_blk_dev->major;
	robe_blk_dev->gd->first_minor = robe_blk_dev->minor_first;
	robe_blk_dev->gd->fops = &robe_blk_dev_ops;
	robe_blk_dev->gd->queue = robe_blk_dev->rq;
	set_capacity(robe_blk_dev->gd, robe_blk_dev->nr_sectors);
	add_disk(robe_blk_dev->gd);
	printk(KERN_INFO"robe_blk_dev_init OK. pid[%d].\n",current->pid);
	robe_blk_dev->gd->private_data = robe_blk_dev;
	printk(KERN_INFO"robe_blk_dev_init: robe_blk_dev->gd[%p].\n",robe_blk_dev->gd);
	
	return 0;

	del_gendisk(robe_blk_dev->gd);
	kfree(robe_blk_dev->gd);
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
	kfree(robe_blk_dev->gd);
	blk_cleanup_queue(robe_blk_dev->rq);
	unregister_blkdev(robe_blk_dev->major,robe_blk_dev->name);
	kfree(robe_blk_dev->data);
	kfree(robe_blk_dev);
	printk(KERN_INFO"robe_blk_dev_exit OK. pid[%d].\n",current->pid);
}

module_init(robe_blk_dev_init);
module_exit(robe_blk_dev_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("robe zhang <82796620@qq.com>");
MODULE_DESCRIPTION("block_device_driver for testing.");

