
// ==================================	port (kzalloc)
struct mxc_gpio_port {					// kzalloc
	struct list_head node;
	void __iomem *base;					// ioremap(&pdev->dev, IORESOURCE_MEM);
	int irq;							// = irq
	int irq_high;						// = irq_h
	struct irq_domain *domain;
	struct bgpio_chip bgc;
	u32 both_edges;
};

// =================================
struct irq_domain {
	struct list_head link;
	const char *name;
	const struct irq_domain_ops *ops;							// .xlate = irq_domain_xlate_onetwocell,
	void *host_data;
	unsigned int flags;

	/* Optional data */
	struct device_node *of_node;
	struct irq_domain_chip_generic *gc;
#ifdef	CONFIG_IRQ_DOMAIN_HIERARCHY
	struct irq_domain *parent;
#endif

	/* reverse map data. The linear map gets appended to the irq_domain */
	irq_hw_number_t hwirq_max;
	unsigned int revmap_direct_max_irq;
	unsigned int revmap_size;
	struct radix_tree_root revmap_tree;
	unsigned int linear_revmap[];
};

struct irq_chip_generic {
	raw_spinlock_t		lock;
	void __iomem		*reg_base;
	u32			(*reg_readl)(void __iomem *addr);
	void			(*reg_writel)(u32 val, void __iomem *addr);
	unsigned int		irq_base;
	unsigned int		irq_cnt;
	u32			mask_cache;
	u32			type_cache;
	u32			polarity_cache;
	u32			wake_enabled;
	u32			wake_active;
	unsigned int		num_ct;
	void			*private;
	unsigned long		installed;
	unsigned long		unused;
	struct irq_domain	*domain;
	struct list_head	list;
	struct irq_chip_type	chip_types[0];
};

struct irq_chip_type {
	struct irq_chip		chip;						// 
	struct irq_chip_regs	regs;
	irq_flow_handler_t	handler;
	u32			type;
	u32			mask_cache_priv;
	u32			*mask_cache;
};

// =================================  bgc
struct bgpio_chip {
	struct gpio_chip gc;

	unsigned long (*read_reg)(void __iomem *reg);				// bgpio_read8/bgpio_read16[be]/bgpio_read32[be]/bgpio_read64
	void (*write_reg)(void __iomem *reg, unsigned long data);	// bgpio_write8/bgpio_write16[be]/bgpio_write32[be]/bgpio_write64

	void __iomem *reg_dat;										// = dat
	void __iomem *reg_set;										// = set
	void __iomem *reg_clr;										// = clr
	void __iomem *reg_dir;										// = dirout / dirin

	int bits;													// 32 ???????????????????????????????
	unsigned long (*pin2mask)(struct bgpio_chip *bgc, unsigned int pin);  //bgpio_pin2mask_be : bgpio_pin2mask
	spinlock_t lock;
	unsigned long data;											// bgc->read_reg(bgc->reg_dat);
	unsigned long dir;											// bgc->read_reg(bgc->reg_dir);
};

// =================================  gc
struct gpio_chip {
	const char		*label;										// dev name
	struct device		*dev;									// pdev->dev
	struct module		*owner;
	struct list_head        list;

	int			(*request)(struct gpio_chip *chip,				// = bgpio_request
						unsigned offset);
	void			(*free)(struct gpio_chip *chip,
						unsigned offset);
	int			(*get_direction)(struct gpio_chip *chip,
						unsigned offset);
	int			(*direction_input)(struct gpio_chip *chip,		// bgpio_dir_in/bgpio_dir_in_inv/bgpio_simple_dir_in
						unsigned offset);
	int			(*direction_output)(struct gpio_chip *chip,		//bgpio_dir_out/bgpio_dir_out_inv/ bgpio_simple_dir_out
						unsigned offset, int value);
	int			(*get)(struct gpio_chip *chip,					// bgpio_get
						unsigned offset);
	void			(*set)(struct gpio_chip *chip,				// bgpio_set/bgpio_set_set/bgpio_set_with_clear
						unsigned offset, int value);
	void			(*set_multiple)(struct gpio_chip *chip,
						unsigned long *mask,
						unsigned long *bits);
	int			(*set_debounce)(struct gpio_chip *chip,
						unsigned offset,
						unsigned debounce);

	int			(*to_irq)(struct gpio_chip *chip,				// = mxc_gpio_to_irq
						unsigned offset);

	void			(*dbg_show)(struct seq_file *s,
						struct gpio_chip *chip);
	int			base;											// = -1 (初始化了 = gpiochip_find_base(chip->ngpio);)
	u16			ngpio;											// = bgc->bits;
	struct gpio_desc	*desc;
	const char		*const *names;
	bool			can_sleep;
	bool			irq_not_threaded;
	bool			exported;
#ifdef CONFIG_GPIOLIB_IRQCHIP
	struct irq_chip		*irqchip;
	struct irq_domain	*irqdomain;
	unsigned int		irq_base;
	irq_flow_handler_t	irq_handler;
	unsigned int		irq_default_type;
#endif

#if defined(CONFIG_OF_GPIO)
	struct device_node *of_node;								// = dev.of_node
	int of_gpio_n_cells;										// = 2
	int (*of_xlate)(struct gpio_chip *gc,						// = of_gpio_simple_xlate
			const struct of_phandle_args *gpiospec, u32 *flags);
#endif
#ifdef CONFIG_PINCTRL
	struct list_head pin_ranges;								// = pin_range->node
#endif
}

struct gpio_pin_range {
	struct list_head node;
	struct pinctrl_dev *pctldev;
	struct pinctrl_gpio_range range;
};

// ==============================================
struct irq_domain {
	struct list_head link;
	const char *name;
	const struct irq_domain_ops *ops;							// = irq_domain_simple_ops
	void *host_data;
	unsigned int flags;

	/* Optional data */
	struct device_node *of_node;
	struct irq_domain_chip_generic *gc;
#ifdef	CONFIG_IRQ_DOMAIN_HIERARCHY
	struct irq_domain *parent;
#endif

	/* reverse map data. The linear map gets appended to the irq_domain */
	irq_hw_number_t hwirq_max;
	unsigned int revmap_direct_max_irq;
	unsigned int revmap_size;
	struct radix_tree_root revmap_tree;
	unsigned int linear_revmap[];
};

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
