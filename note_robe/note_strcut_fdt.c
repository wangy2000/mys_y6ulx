
// =======================================
struct fdt_header {
	uint32_t magic;			 /* magic word FDT_MAGIC */
	uint32_t totalsize;		 /* total size of DT block */
	uint32_t off_dt_struct;		 /* offset to structure */				// fdt_node_header
	uint32_t off_dt_strings;	 /* offset to strings */
	uint32_t off_mem_rsvmap;	 /* offset to memory reserve map */
	uint32_t version;		 /* format version */
	uint32_t last_comp_version;	 /* last compatible version */
	uint32_t boot_cpuid_phys;	 /* Which physical CPU id we're booting on */
	uint32_t size_dt_strings;	 /* size of the strings block */
	uint32_t size_dt_struct;	 /* size of the structure block */
};

struct fdt_reserve_entry {
	uint64_t address;
	uint64_t size;
};

struct fdt_node_header {												// fdt_node_header
	uint32_t tag;
	char name[0];
};

struct fdt_property {
	uint32_t tag;
	uint32_t len;
	uint32_t nameoff;
	char data[0];
};

// =======================================
struct device_node {
	const char *name;
	const char *type;
	phandle phandle;
	const char *full_name;
	struct fwnode_handle fwnode;					// fwnode->type = FWNODE_OF

	struct	property *properties;
	struct	property *deadprops;	/* removed properties */
	struct	device_node *parent;
	struct	device_node *child;
	struct	device_node *sibling;
	struct	kobject kobj;							// kobj->ktype = of_node_ktype
	unsigned long _flags;
	void	*data;
#if defined(CONFIG_SPARC)
	const char *path_component_name;
	unsigned int unique_id;
	struct of_irq_controller *irq_trans;
#endif
};

struct fwnode_handle {
	enum fwnode_type type;
	struct fwnode_handle *secondary;
};

struct kobject {
	const char		*name;
	struct list_head	entry;
	struct kobject		*parent;
	struct kset		*kset;
	struct kobj_type	*ktype;
	struct kernfs_node	*sd;
	struct kref		kref;
#ifdef CONFIG_DEBUG_KOBJECT_RELEASE
	struct delayed_work	release;
#endif
	unsigned int state_initialized:1;
	unsigned int state_in_sysfs:1;
	unsigned int state_add_uevent_sent:1;
	unsigned int state_remove_uevent_sent:1;
	unsigned int uevent_suppress:1;
};

struct kobj_type of_node_ktype = {
	.release = of_node_release,
};

struct property {
	char	*name;
	int	length;
	void	*value;
	struct property *next;
	unsigned long _flags;
	unsigned int unique_id;
	struct bin_attribute attr;
};
