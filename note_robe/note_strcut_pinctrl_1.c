

//================================================
struct resource {
	resource_size_t start;
	resource_size_t end;
	const char *name;
	unsigned long flags;
	struct resource *parent, *sibling, *child;
};

//================================================ imx6ul_pinctrl_of_match
struct of_device_id {
	char	name[32];
	char	type[32];
	char	compatible[128];
	const void *data;
};

//======================================================================================= other
//================================================ imx6ul_pinctrl_info / info = match->data
struct imx_pinctrl_soc_info {
	struct device *dev;						// = pdev->dev
	const struct pinctrl_pin_desc *pins; 	// pin 列表
	unsigned int npins;						// pin 数量
	struct imx_pin_reg *pin_regs;			// (devm_kzalloc) = -1
	struct imx_pin_group *groups;			// (devm_kzalloc)
	unsigned int ngroups;					// = pdev->dev.of_node 所有组数量
	struct imx_pmx_func *functions;			// (devm_kzalloc)
	unsigned int nfunctions;				// = pdev->dev.of_node 所有子结点数量
	unsigned int flags;
	u32 grp_index;
};

struct platform_device *pdev				// pdev->dev->driver_data = ipctl

//====================================================================================== input
struct imx_pin_reg {
	s16 mux_reg;							// = -1
	s16 conf_reg;							// = -1
};

//=================================================== func
struct imx_pmx_func {
	const char *name;									// 父节点名字
	const char **groups;								// 子节点名字
	unsigned num_groups;								// 子节点数量
};
//=================================================== grp
struct imx_pin_group {
	const char *name;
	unsigned npins;
	unsigned int *pin_ids;
	struct imx_pin *pins;
};

//==================================================
struct imx_pin {
	unsigned int pin;
	unsigned int mux_mode;
	u16 input_reg;
	unsigned int input_val;
	unsigned long config;
};

//================================================  ipctl(devm_kzalloc)
struct imx_pinctrl {
	struct device *dev;								// = info->dev
	struct pinctrl_dev *pctl;						// = pinctrl_register(imx_pinctrl_desc, &pdev->dev, ipctl);
	void __iomem *base;								// = pdev, IORESOURCE_MEM
	void __iomem *input_sel_base;					// = "fsl,input-sel"
	const struct imx_pinctrl_soc_info *info;		// = info
};

//================================================  imx_pinctrl_desc(devm_kzalloc)
struct pinctrl_desc {
	const char *name;								// = pdev.dev.init_name / pdev.dev.kobject.name
	struct pinctrl_pin_desc const *pins;			// = info.pin
	unsigned int npins;								// = info.npins
	const struct pinctrl_ops *pctlops;				// = imx_pctrl_ops
	const struct pinmux_ops *pmxops;				// = imx_pmx_ops
	const struct pinconf_ops *confops;				// = imx_pinconf_ops
	struct module *owner;							// = THIS_MODULE
#ifdef CONFIG_GENERIC_PINCONF
	unsigned int num_custom_params;
	const struct pinconf_generic_params *custom_params;
	const struct pin_config_item *custom_conf_items;
#endif
};

struct pinctrl_pin_desc {
	unsigned number;
	const char *name;
	void *drv_data;
};
//================================================  pctldev 
struct pinctrl_dev {
	struct list_head node;
	struct pinctrl_desc *desc;						// = imx_pinctrl_desc
	struct radix_tree_root pin_desc_tree;			// ok
	struct list_head gpio_ranges;					// ok
	struct device *dev;
	struct module *owner;							// = THIS_MODULE
	void *driver_data;								// = ipctl
	struct pinctrl *p;
	struct pinctrl_state *hog_default;
	struct pinctrl_state *hog_sleep;
	struct mutex mutex;								// init
#ifdef CONFIG_DEBUG_FS
	struct dentry *device_root;
#endif
};
struct pinctrl {
	struct list_head node;
	struct device *dev;
	struct list_head states;
	struct pinctrl_state *state;
	struct list_head dt_maps;
	struct kref users;
};
// ================================================== pindesc
struct pin_desc {
	struct pinctrl_dev *pctldev;						// ok/
	const char *name;
	bool dynamic_name;
	/* These fields only added when supporting pinmux drivers */
#ifdef CONFIG_PINMUX
	unsigned mux_usecount;
	const char *mux_owner;
	const struct pinctrl_setting_mux *mux_setting;
	const char *gpio_owner;
#endif
};

