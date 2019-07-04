
// struct kobject ========================================
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

// struct kernfs_node ========================================
struct kernfs_node {
	atomic_t		count;
	atomic_t		active;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map	dep_map;
#endif
	/*
	 * Use kernfs_get_parent() and kernfs_name/path() instead of
	 * accessing the following two fields directly.  If the node is
	 * never moved to a different parent, it is safe to access the
	 * parent directly.
	 */
	struct kernfs_node	*parent;
	const char		*name;

	struct rb_node		rb;

	const void		*ns;	/* namespace tag */
	unsigned int		hash;	/* ns + name hash */
	union {
		struct kernfs_elem_dir		dir;
		struct kernfs_elem_symlink	symlink;
		struct kernfs_elem_attr		attr;
	};

	void			*priv;

	unsigned short		flags;
	umode_t			mode;
	unsigned int		ino;
	struct kernfs_iattrs	*iattr;
}

// struct kset ========================================
struct kset {
	struct list_head list;
	spinlock_t list_lock;
	struct kobject kobj;
	const struct kset_uevent_ops *uevent_ops;
};

// struct kernfs_elem_dir ========================================
struct kernfs_elem_dir {
	unsigned long		subdirs;
	/* children rbtree starts here and goes through kn->rb */
	struct rb_root		children;

	/*
	 * The kernfs hierarchy this directory belongs to.  This fits
	 * better directly in kernfs_node but is here to save space.
	 */
	struct kernfs_root	*root;
};

// ===============================================================
struct kernfs_root {
	/* published fields */
	struct kernfs_node	*kn;
	unsigned int		flags;	/* KERNFS_ROOT_* flags */

	/* private fields, do not use outside kernfs proper */
	struct ida		ino_ida;
	struct kernfs_syscall_ops *syscall_ops;

	/* list of kernfs_super_info of this root, protected by kernfs_mutex */
	struct list_head	supers;

	wait_queue_head_t	deactivate_waitq;
};

// =================================================================
struct rb_root {
	struct rb_node *rb_node;
};

// =================================================================
struct rb_node {
	unsigned long  __rb_parent_color;
	struct rb_node *rb_right;
	struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));




