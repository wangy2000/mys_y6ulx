
// =============================== of_device_id / of_id
struct of_device_id {
	char	name[32];
	char	type[32];
	char	compatible[128];		// = "fsl,imx21-i2c"
	const void *data;				// = imx21_i2c_hwdata
};

// =============================== pdev->dev->platform_data / pdata
struct imxi2c_platform_data {
	u32 bitrate;
};


	
// ==============================	imx21_i2c_hwdata
static const struct imx_i2c_hwdata imx21_i2c_hwdata  = {	// OK
	.devtype		= IMX21_I2C,
	.regshift		= IMX_I2C_REGSHIFT,
	.clk_div		= imx_i2c_clk_div,
	.ndivs			= ARRAY_SIZE(imx_i2c_clk_div),
	.i2sr_clr_opcode	= I2SR_CLR_OPCODE_W0C,
	.i2cr_ien_opcode	= I2CR_IEN_OPCODE_1,
};

struct imx_i2c_hwdata {
	enum imx_i2c_type	devtype;
	unsigned		regshift;
	struct imx_i2c_clk_pair	*clk_div;
	unsigned		ndivs;
	unsigned		i2sr_clr_opcode;
	unsigned		i2cr_ien_opcode;
};	
// ==================================  驱动申请内存  i2c_imx
struct imx_i2c_struct {
	struct i2c_adapter	adapter;					// 下面赋值
	struct clk		*clk;							// = ok
	void __iomem		*base;						// = base
	wait_queue_head_t	queue;						// 初始化
	unsigned long		i2csr;
	unsigned int		disable_delay;
	int			stopped;
	unsigned int		ifdr; /* IMX_I2C_IFDR */
	unsigned int		cur_clk;
	unsigned int		bitrate;					// = ok
	const struct imx_i2c_hwdata	*hwdata;			// = imx21_i2c_hwdata

	struct imx_i2c_dma	*dma;
};

struct i2c_adapter {
	struct module *owner;							// = THIS_MODULE;
	unsigned int class;
	const struct i2c_algorithm *algo;				// = i2c_imx_algo
	void *algo_data;
	struct rt_mutex bus_lock;

	int timeout;									// = HZ
	int retries;
	struct device dev;								// parent = pdev->dev; 
													// of_node = &pdev->dev.of_node
													// dev.driver_data = i2c_imx
	int nr;
	char name[48];									// = pdev->name
	struct completion dev_released;

	struct mutex userspace_clients_lock;
	struct list_head userspace_clients;

	struct i2c_bus_recovery_info *bus_recovery_info;
	const struct i2c_adapter_quirks *quirks;
};

// =========================================
struct clk {
	struct clk_core	*core;
	const char *dev_id;
	const char *con_id;
	unsigned long min_rate;
	unsigned long max_rate;
	struct hlist_node clks_node;
};

struct clk_core {
	const char		*name;
	const struct clk_ops	*ops;
	struct clk_hw		*hw;
	struct module		*owner;
	struct clk_core		*parent;
	const char		**parent_names;
	struct clk_core		**parents;
	u8			num_parents;
	u8			new_parent_index;
	unsigned long		rate;
	unsigned long		req_rate;
	unsigned long		new_rate;
	struct clk_core		*new_parent;
	struct clk_core		*new_child;
	unsigned long		flags;
	unsigned int		enable_count;
	unsigned int		prepare_count;
	unsigned long		accuracy;
	int			phase;
	struct hlist_head	children;
	struct hlist_node	child_node;
	struct hlist_node	debug_node;
	struct hlist_head	clks;
	unsigned int		notifier_count;
#ifdef CONFIG_DEBUG_FS
	struct dentry		*dentry;
#endif
	struct kref		ref;
};

struct clk_ops {
	int		(*prepare)(struct clk_hw *hw);
	void		(*unprepare)(struct clk_hw *hw);
	int		(*is_prepared)(struct clk_hw *hw);
	void		(*unprepare_unused)(struct clk_hw *hw);
	int		(*enable)(struct clk_hw *hw);
	void		(*disable)(struct clk_hw *hw);
	int		(*is_enabled)(struct clk_hw *hw);
	void		(*disable_unused)(struct clk_hw *hw);
	unsigned long	(*recalc_rate)(struct clk_hw *hw,
					unsigned long parent_rate);
	long		(*round_rate)(struct clk_hw *hw, unsigned long rate,
					unsigned long *parent_rate);
	long		(*determine_rate)(struct clk_hw *hw,
					  unsigned long rate,
					  unsigned long min_rate,
					  unsigned long max_rate,
					  unsigned long *best_parent_rate,
					  struct clk_hw **best_parent_hw);
	int		(*set_parent)(struct clk_hw *hw, u8 index);
	u8		(*get_parent)(struct clk_hw *hw);
	int		(*set_rate)(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate);
	int		(*set_rate_and_parent)(struct clk_hw *hw,
				    unsigned long rate,
				    unsigned long parent_rate, u8 index);
	unsigned long	(*recalc_accuracy)(struct clk_hw *hw,
					   unsigned long parent_accuracy);
	int		(*get_phase)(struct clk_hw *hw);
	int		(*set_phase)(struct clk_hw *hw, int degrees);
	void		(*init)(struct clk_hw *hw);
	int		(*debug_init)(struct clk_hw *hw, struct dentry *dentry);
};

// =================================
struct resource {
	resource_size_t start;
	resource_size_t end;
	const char *name;
	unsigned long flags;
	struct resource *parent, *sibling, *child;
};

struct devres {
	struct devres_node		node;
	/* -- 3 pointers */
	unsigned long long		data[];	/* guarantee ull alignment */
};

// ================================
struct i2c_algorithm {
	/* If an adapter algorithm can't do I2C-level access, set master_xfer
	   to NULL. If an adapter algorithm can do SMBus access, set
	   smbus_xfer. If set to NULL, the SMBus protocol is simulated
	   using common I2C messages */
	/* master_xfer should return the number of messages successfully
	   processed, or a negative value on error */
	int (*master_xfer)(struct i2c_adapter *adap, struct i2c_msg *msgs,
			   int num);
	int (*smbus_xfer) (struct i2c_adapter *adap, u16 addr,
			   unsigned short flags, char read_write,
			   u8 command, int size, union i2c_smbus_data *data);

	/* To determine what the adapter supports */
	u32 (*functionality) (struct i2c_adapter *);

#if IS_ENABLED(CONFIG_I2C_SLAVE)
	int (*reg_slave)(struct i2c_client *client);
	int (*unreg_slave)(struct i2c_client *client);
#endif
};


// ==============================
#define dev_dbg(dev, format, arg...)		\
	dev_printk(KERN_DEBUG, dev, format, ##arg)
	
	
dev_dbg(dev, format, arg...)
=	dev_printk(KERN_DEBUG, dev, format, ##arg)	