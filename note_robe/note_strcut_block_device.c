
// ====================================
struct block_device {
	dev_t			bd_dev;
	int			bd_openers;
	struct inode *		bd_inode;	/* will die */
	struct super_block *	bd_super;
	struct mutex		bd_mutex;	/* open/close mutex */
	struct list_head	bd_inodes;
	void *			bd_claiming;
	void *			bd_holder;
	int			bd_holders;
	bool			bd_write_holder;
#ifdef CONFIG_SYSFS
	struct list_head	bd_holder_disks;
#endif
	struct block_device *	bd_contains;
	unsigned		bd_block_size;
	struct hd_struct *	bd_part;
	unsigned		bd_part_count;
	int			bd_invalidated;
	struct gendisk *	bd_disk;
	struct request_queue *  bd_queue;
	struct list_head	bd_list;
	unsigned long		bd_private;
	int			bd_fsfreeze_count;
	struct mutex		bd_fsfreeze_mutex;
};

// ====================================
struct block_device_operations {
	int (*open) (struct block_device *, fmode_t);									//	
	void (*release) (struct gendisk *, fmode_t);									//
	int (*rw_page)(struct block_device *, sector_t, struct page *, int rw);
	int (*ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);			// 标准 ioctl 调用
	int (*compat_ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);	// 32位调用 ioctl
	long (*direct_access)(struct block_device *, sector_t,
					void **, unsigned long *pfn, long size);
	unsigned int (*check_events) (struct gendisk *disk,								// 
				      unsigned int clearing);
	int (*media_changed) (struct gendisk *);										//
	void (*unlock_native_capacity) (struct gendisk *);
	int (*revalidate_disk) (struct gendisk *);										//
	int (*getgeo)(struct block_device *, struct hd_geometry *);
	void (*swap_slot_free_notify) (struct block_device *, unsigned long);
	struct module *owner;															//
};

// ====================================
struct hd_geometry {
      unsigned char heads;
      unsigned char sectors;
      unsigned short cylinders;
      unsigned long start;
};

// ====================================
struct gendisk {
	int major;												// 初始化
	int first_minor;										// 初始化
	int minors; 											//
	char disk_name[DISK_NAME_LEN];							// 初始化	
	char *(*devnode)(struct gendisk *gd, umode_t *mode);
	unsigned int events;
	unsigned int async_events;
	struct disk_part_tbl __rcu *part_tbl;
	struct hd_struct part0;									// set_sector -  初始化

	const struct block_device_operations *fops;				// 初始化								
	struct request_queue *queue;							// 初始化	
	void *private_data;										// 初始化	

	int flags;												//
	struct device *driverfs_dev;
	struct kobject *slave_dir;

	struct timer_rand_state *random;
	atomic_t sync_io;
	struct disk_events *ev;
#ifdef  CONFIG_BLK_DEV_INTEGRITY
	struct blk_integrity *integrity;
#endif
	int node_id;
};

struct hd_struct {
	sector_t start_sect;
	sector_t nr_sects;							// set_capacity
	seqcount_t nr_sects_seq;
	sector_t alignment_offset;
	unsigned int discard_alignment;
	struct device __dev;
	struct kobject *holder_dir;
	int policy, partno;
	struct partition_meta_info *info;
#ifdef CONFIG_FAIL_MAKE_REQUEST
	int make_it_fail;
#endif
	unsigned long stamp;
	atomic_t in_flight[2];
#ifdef	CONFIG_SMP
	struct disk_stats __percpu *dkstats;
#else
	struct disk_stats dkstats;
#endif
	atomic_t ref;
	struct rcu_head rcu_head;
};

struct disk_part_tbl {							// gendisk->part_tbl-part[0] == gendisk->part0
	struct rcu_head rcu_head;
	int len;
	struct hd_struct __rcu *last_lookup;
	struct hd_struct __rcu *part[];
};

// ====================================
struct gendisk *alloc_disk(int minors);
void add_disk(struct gendisk *gd);
void del_gendisk(struct gendisk *gd);

static inline void set_capacity(struct gendisk *disk, sector_t size)
{	disk->part0.nr_sects = size;	}

struct request_queue *blk_init_queue(request_fn_proc *rfn, spinlock_t *lock)	// 初始化请求队列
void blk_cleanup_queue(struct request_queue *q)									// 清除请求队列

struct request *blk_peek_request(struct request_queue *q)						// 提取请求
void blk_start_request(struct request *req)										// 启动请求，从请求队列移除请求
//struct request *blk_fetch_request(struct request_queue *q)
__rq_for_each_bio(_bio, rq)														// 遍历请求的所有 bio
bio_for_each_segment(bvl, bio, iter)											// 遍历 bio 所有的 bio_vec
rq_for_each_segment(bvl, _rq, _iter)											// 遍历一个请求所有 bio 中的所有 segment
bio_cur_bytes(bio,iter);
blk_rq_pos(req)																	// 获取当前位置
blk_rq_sectors(req)																// 获取rq sectors 数量
void blk_end_request_all(struct request *rq, int error)							// 报告完成

int register_blkdev(unsigned int major, const char *name)						// 注册块设备
void unregister_blkdev(unsigned int major, const char *name)					// 注销块设备


// --------
struct request_queue *blk_alloc_queue(gfp_t gfp_mask)							// 分配请求队列(不用调度器)
void blk_queue_make_request(struct request_queue *q, make_request_fn *mfn)		// 制造请求函数
void bio_endio(struct bio *bio, int error)


register_blkdev(sbull_major, "sbull");
初始化结构体
blk_init_queue(sbull_request, &dev->lock);		// sbull_request 申请队列函数

dev->gd = alloc_disk(SBULL_MINORS);
dev->gd->major = sbull_major;
dev->gd->first_minor = which*SBULL_MINORS;
dev->gd->fops = &sbull_ops;
dev->gd->queue = dev->queue;
dev->gd->private_data = dev;
snprintf (dev->gd->disk_name, 32, "sbull%c", which + 'a');
set_capacity(dev->gd, nsectors*(hardsect_size/KERNEL_SECTOR_SIZE));		// 设置容量
add_disk(dev->gd);

blk_queue_hardsect_size(dev->queue, hardsect_size);		// 设置 sector 大小


// ====================================
struct bio {
	struct bio		*bi_next;
	struct block_device	*bi_bdev;
	unsigned long		bi_flags;
	unsigned long		bi_rw;
	struct bvec_iter	bi_iter;											// 设备扇区索引
	unsigned int		bi_phys_segments;
	unsigned int		bi_seg_front_size;
	unsigned int		bi_seg_back_size;
	atomic_t		bi_remaining;
	bio_end_io_t		*bi_end_io;
	void			*bi_private;
#ifdef CONFIG_BLK_CGROUP
	struct io_context	*bi_ioc;
	struct cgroup_subsys_state *bi_css;
#endif
	union {
#if defined(CONFIG_BLK_DEV_INTEGRITY)
		struct bio_integrity_payload *bi_integrity; /* data integrity */
#endif
	};
	unsigned short		bi_vcnt;	/* how many bio_vec's */				// bio_vec 数量
	unsigned short		bi_max_vecs;	/* max bvl_vecs we can hold */
	atomic_t		bi_cnt;		/* pin count */
	struct bio_vec		*bi_io_vec;	/* the actual vec list */				// 请求对应的所有内存 (扇区数据对应的缓存段)
	struct bio_set		*bi_pool;
	struct bio_vec		bi_inline_vecs[0];
};

// =====================================
struct bvec_iter {
	sector_t      bi_sector;	/* device address in 512 byte sectors */
	unsigned int  bi_size;	/* residual I/O count */

	unsigned int  bi_idx;		/* current index into bvl_vec */

	unsigned int  bi_bvec_done;	/* number of bytes completed in current bvec */
};

// =====================================
struct bio_vec {
	struct page	*bv_page;
	unsigned int	bv_len;
	unsigned int	bv_offset;
};

// =====================================
struct request_queue {

	struct list_head	queue_head;
	struct request		*last_merge;
	struct elevator_queue	*elevator;				// 调度器
	int			nr_rqs[2];
	int			nr_rqs_elvpriv;

	struct request_list	root_rl;

	request_fn_proc		*request_fn;
	make_request_fn		*make_request_fn;
	prep_rq_fn		*prep_rq_fn;
	unprep_rq_fn		*unprep_rq_fn;
	merge_bvec_fn		*merge_bvec_fn;
	softirq_done_fn		*softirq_done_fn;
	rq_timed_out_fn		*rq_timed_out_fn;
	dma_drain_needed_fn	*dma_drain_needed;
	lld_busy_fn		*lld_busy_fn;

	struct blk_mq_ops	*mq_ops;

	unsigned int		*mq_map;

	struct blk_mq_ctx __percpu	*queue_ctx;
	unsigned int		nr_queues;

	struct blk_mq_hw_ctx	**queue_hw_ctx;
	unsigned int		nr_hw_queues;

	sector_t		end_sector;
	struct request		*boundary_rq;

	struct delayed_work	delay_work;

	struct backing_dev_info	backing_dev_info;

	void			*queuedata;								// 要处理的 request

	unsigned long		queue_flags;

	int			id;

	gfp_t			bounce_gfp;

	spinlock_t		__queue_lock;
	spinlock_t		*queue_lock;

	struct kobject kobj;

	struct kobject mq_kobj;

#ifdef CONFIG_PM
	struct device		*dev;
	int			rpm_status;
	unsigned int		nr_pending;
#endif

	unsigned long		nr_requests;	/* Max # of requests */
	unsigned int		nr_congestion_on;
	unsigned int		nr_congestion_off;
	unsigned int		nr_batching;

	unsigned int		dma_drain_size;
	void			*dma_drain_buffer;
	unsigned int		dma_pad_mask;
	unsigned int		dma_alignment;

	struct blk_queue_tag	*queue_tags;
	struct list_head	tag_busy_list;

	unsigned int		nr_sorted;
	unsigned int		in_flight[2];
	
	unsigned int		request_fn_active;

	unsigned int		rq_timeout;
	struct timer_list	timeout;
	struct list_head	timeout_list;

	struct list_head	icq_list;
#ifdef CONFIG_BLK_CGROUP
	DECLARE_BITMAP		(blkcg_pols, BLKCG_MAX_POLS);
	struct blkcg_gq		*root_blkg;
	struct list_head	blkg_list;
#endif

	struct queue_limits	limits;

	unsigned int		sg_timeout;
	unsigned int		sg_reserved_size;
	int			node;
#ifdef CONFIG_BLK_DEV_IO_TRACE
	struct blk_trace	*blk_trace;
#endif

	unsigned int		flush_flags;
	unsigned int		flush_not_queueable:1;
	struct blk_flush_queue	*fq;

	struct list_head	requeue_list;
	spinlock_t		requeue_lock;
	struct work_struct	requeue_work;

	struct mutex		sysfs_lock;

	int			bypass_depth;
	int			mq_freeze_depth;

#if defined(CONFIG_BLK_DEV_BSG)
	bsg_job_fn		*bsg_job_fn;
	int			bsg_job_size;
	struct bsg_class_device bsg_dev;
#endif

#ifdef CONFIG_BLK_DEV_THROTTLING
	struct throtl_data *td;
#endif
	struct rcu_head		rcu_head;
	wait_queue_head_t	mq_freeze_wq;
	struct percpu_ref	mq_usage_counter;
	struct list_head	all_q_node;

	struct blk_mq_tag_set	*tag_set;
	struct list_head	tag_set_list;
};

// ====================================
struct request {
	struct list_head queuelist;
	union {
		struct call_single_data csd;
		unsigned long fifo_time;
	};

	struct request_queue *q;
	struct blk_mq_ctx *mq_ctx;

	u64 cmd_flags;
	enum rq_cmd_type_bits cmd_type;
	unsigned long atomic_flags;

	int cpu;

	unsigned int __data_len;
	sector_t __sector;

	struct bio *bio;								// bio 头
	struct bio *biotail;							// bio 尾

	union {
		struct hlist_node hash;
		struct list_head ipi_list;
	};

	union {
		struct rb_node rb_node;
		void *completion_data;
	};

	union {
		struct {
			struct io_cq		*icq;
			void			*priv[2];
		} elv;

		struct {
			unsigned int		seq;
			struct list_head	list;
			rq_end_io_fn		*saved_end_io;
		} flush;
	};

	struct gendisk *rq_disk;
	struct hd_struct *part;
	unsigned long start_time;
#ifdef CONFIG_BLK_CGROUP
	struct request_list *rl;
	unsigned long long start_time_ns;
	unsigned long long io_start_time_ns;
#endif

	unsigned short nr_phys_segments;
#if defined(CONFIG_BLK_DEV_INTEGRITY)
	unsigned short nr_integrity_segments;
#endif

	unsigned short ioprio;

	void *special;

	int tag;
	int errors;

	unsigned char __cmd[BLK_MAX_CDB];
	unsigned char *cmd;
	unsigned short cmd_len;

	unsigned int extra_len;
	unsigned int sense_len;
	unsigned int resid_len;
	void *sense;

	unsigned long deadline;
	struct list_head timeout_list;
	unsigned int timeout;
	int retries;

	rq_end_io_fn *end_io;
	void *end_io_data;

	struct request *next_rq;
};

// ===========================================
Disk /dev/sda: 465.8 GiB, 500107862016 bytes, 976773168 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 4096 bytes
I/O size (minimum/optimal): 4096 bytes / 4096 bytes
Disklabel type: gpt
Disk identifier: 2C01FF64-C353-4908-A40C-2ED939053C73

Device     Start       End   Sectors   Size Type
/dev/sda1   2048 976773134 976771087 465.8G Linux filesystem


Disk /dev/sdb: 111.8 GiB, 120034123776 bytes, 234441648 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: gpt
Disk identifier: 6726F6D3-D997-4102-B5D8-E80D85566FFC

Device         Start       End   Sectors   Size Type
/dev/sdb1       2048   1050623   1048576   512M EFI System
/dev/sdb2    1050624 232441855 231391232 110.3G Linux filesystem
/dev/sdb3  232441856 234440703   1998848   976M Linux swap
