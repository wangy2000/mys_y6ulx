
// ==============================================
struct irq_desc {
	struct irq_data		irq_data;
	unsigned int __percpu	*kstat_irqs;
	irq_flow_handler_t	handle_irq;
#ifdef CONFIG_IRQ_PREFLOW_FASTEOI
	irq_preflow_handler_t	preflow_handler;
#endif
	struct irqaction	*action;	/* IRQ action list */
	unsigned int		status_use_accessors;
	unsigned int		core_internal_state__do_not_mess_with_it;
	unsigned int		depth;		/* nested irq disables */
	unsigned int		wake_depth;	/* nested wake enables */
	unsigned int		irq_count;	/* For detecting broken IRQs */
	unsigned long		last_unhandled;	/* Aging timer for unhandled count */
	unsigned int		irqs_unhandled;
	atomic_t		threads_handled;
	int			threads_handled_last;
	raw_spinlock_t		lock;
	struct cpumask		*percpu_enabled;
#ifdef CONFIG_SMP
	const struct cpumask	*affinity_hint;
	struct irq_affinity_notify *affinity_notify;
#ifdef CONFIG_GENERIC_PENDING_IRQ
	cpumask_var_t		pending_mask;
#endif
#endif
	unsigned long		threads_oneshot;
	atomic_t		threads_active;
	wait_queue_head_t       wait_for_threads;
#ifdef CONFIG_PM_SLEEP
	unsigned int		nr_actions;
	unsigned int		no_suspend_depth;
	unsigned int		cond_suspend_depth;
	unsigned int		force_resume_depth;
#endif
#ifdef CONFIG_PROC_FS
	struct proc_dir_entry	*dir;
#endif
	int			parent_irq;
	struct module		*owner;
	const char		*name;
}____cacheline_internodealigned_in_smp;

// ==============================================
struct intc_desc {
	struct list_head	list;
	struct device_node	*dev;						// = np
	struct device_node	*interrupt_parent;
};

// ==============================================   gic
struct gic_chip_data {
	union gic_base dist_base;						// gic 地址
	union gic_base cpu_base;						// cpu 偏移
#ifdef CONFIG_CPU_PM
	u32 saved_spi_enable[DIV_ROUND_UP(1020, 32)];
	u32 saved_spi_conf[DIV_ROUND_UP(1020, 16)];
	u32 saved_spi_target[DIV_ROUND_UP(1020, 4)];
	u32 __percpu *saved_ppi_enable;
	u32 __percpu *saved_ppi_conf;
#endif
	struct irq_domain *domain;						// 申请 irq_domain, 赋值 ops后，添加到 irq_domain_list 链表中
	unsigned int gic_irqs;							// 支持的 irq 总数
#ifdef CONFIG_GIC_NON_BANKED
	void __iomem *(*get_base)(union gic_base *);	// = gic_get_common_base
#endif
};

union gic_base {
	void __iomem *common_base;
	void __percpu * __iomem *percpu_base;
};
//==============================================
struct irq_domain {
	struct list_head link;
	const char *name;
	const struct irq_domain_ops *ops;				// = gic_irq_domain_hierarchy_ops
	void *host_data;								// = gic (struct gic_chip_data )
	unsigned int flags;

	/* Optional data */
	struct device_node *of_node;					// = gic 的 node
	struct irq_domain_chip_generic *gc;
#ifdef	CONFIG_IRQ_DOMAIN_HIERARCHY
	struct irq_domain *parent;
#endif

	/* reverse map data. The linear map gets appended to the irq_domain */
	irq_hw_number_t hwirq_max;						// = irq 支持的最大数量，从 gic 读取的
	unsigned int revmap_direct_max_irq;·			// = 0
	unsigned int revmap_size;						// = irq 支持的最大数量，从 gic 读取的
	struct radix_tree_root revmap_tree;				
	unsigned int linear_revmap[];
};

//==============================================  
struct irq_domain_ops {
	int (*match)(struct irq_domain *d, struct device_node *node);
	int (*map)(struct irq_domain *d, unsigned int virq, irq_hw_number_t hw);
	void (*unmap)(struct irq_domain *d, unsigned int virq);
	int (*xlate)(struct irq_domain *d, struct device_node *node,
		     const u32 *intspec, unsigned int intsize,
		     unsigned long *out_hwirq, unsigned int *out_type);

#ifdef	CONFIG_IRQ_DOMAIN_HIERARCHY
	/* extended V2 interfaces to support hierarchy irq_domains */
	int (*alloc)(struct irq_domain *d, unsigned int virq,
		     unsigned int nr_irqs, void *arg);
	void (*free)(struct irq_domain *d, unsigned int virq,
		     unsigned int nr_irqs);
	void (*activate)(struct irq_domain *d, struct irq_data *irq_data);
	void (*deactivate)(struct irq_domain *d, struct irq_data *irq_data);
#endif
};

static const struct irq_domain_ops gic_irq_domain_hierarchy_ops = {  // irq_domain_ops 操作方法
	.xlate = gic_irq_domain_xlate,
	.alloc = gic_irq_domain_alloc,
	.free = irq_domain_free_irqs_top,
};

//=============================================
struct of_phandle_args {
	struct device_node *np;
	int args_count;
	uint32_t args[MAX_PHANDLE_ARGS];
};