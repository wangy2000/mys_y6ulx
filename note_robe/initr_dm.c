#define DM_ROOT_NON_CONST		(((gd_t *)gd)->dm_root)
#define DM_UCLASS_ROOT_NON_CONST	(((gd_t *)gd)->uclass_root)

struct udevice	*dm_root;

struct udevice {
	const struct driver *driver;	drv
	const char *name;				###
	void *platdata;					info->platdata
	void *parent_platdata;
	void *uclass_platdata;
	int of_offset;					###
	ulong driver_data;
	struct udevice *parent;			NULL
	void *priv;
	struct uclass *uclass;			uc
	void *uclass_priv;
	void *parent_priv;
//	struct list_head uclass_node;	###
	struct list_head child_head;	###
	struct list_head sibling_node;	###
	uint32_t flags;
	int req_seq;					-1
	int seq;						-1
#ifdef CONFIG_DEVRES
	struct list_head devres_head;
#endif
};

struct driver {
	char *name;
	enum uclass_id id;
	const struct udevice_id *of_match;
	int (*bind)(struct udevice *dev);
	int (*probe)(struct udevice *dev);
	int (*remove)(struct udevice *dev);
	int (*unbind)(struct udevice *dev);
	int (*ofdata_to_platdata)(struct udevice *dev);
	int (*child_post_bind)(struct udevice *dev);
	int (*child_pre_probe)(struct udevice *dev);
	int (*child_post_remove)(struct udevice *dev);
	int priv_auto_alloc_size;
	int platdata_auto_alloc_size;
	int per_child_auto_alloc_size;
	int per_child_platdata_auto_alloc_size;
	const void *ops;	/* driver-specific operations */
	uint32_t flags;
};

struct list_head uclass_root;

struct list_head {
	struct list_head *next, *prev;
};

struct driver_info {
	const char *name;
	const void *platdata;
};

static const struct driver_info root_info = {
	.name		= "root_driver",
};

struct uclass {
	void *priv;
	struct uclass_driver *uc_drv;
//	struct list_head dev_head;
	struct list_head sibling_node;
};

struct uclass_driver {
	const char *name;
	enum uclass_id id;
	int (*post_bind)(struct udevice *dev);
	int (*pre_unbind)(struct udevice *dev);
	int (*pre_probe)(struct udevice *dev);
	int (*post_probe)(struct udevice *dev);
	int (*pre_remove)(struct udevice *dev);
	int (*child_post_bind)(struct udevice *dev);
	int (*child_pre_probe)(struct udevice *dev);
	int (*init)(struct uclass *class);
	int (*destroy)(struct uclass *class);
	int priv_auto_alloc_size;
	int per_device_auto_alloc_size;
	int per_device_platdata_auto_alloc_size;
	int per_child_auto_alloc_size;
	int per_child_platdata_auto_alloc_size;
	const void *ops;
	uint32_t flags;
};

initr_dm
	dm_init_and_scan
		dm_init
			INIT_LIST_HEAD(&gd->uclass_root):											初始化
			
				static inline void INIT_LIST_HEAD(struct list_head *list)
				{
					list->next = list;
					list->prev = list;
				}
			device_bind_by_name(NULL, false, &root_info, &gd->dm_root);
				lists_driver_lookup_name(info->name);									返回driver入口地址 drv
				device_bind(parent/NULL, drv, info->name, (void *)info->platdata,-1, devp)
										uc
					udevice,uclass
					初始化udevice，udevice放入uclass里面(uclass_bind_device),执行bind
			device_probe(DM_ROOT_NON_CONST);
				执行probe
		dm_scan_platdata(false)
			执行bind
		dm_scan_other(false)
		
		
		
		
		
		
		
		