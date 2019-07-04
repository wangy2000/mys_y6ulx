

// ======================================
struct device_node {
	const char *name;
	const char *type;
	phandle phandle;
	const char *full_name;
	struct fwnode_handle fwnode;

	struct	property *properties;
	struct	property *deadprops;	/* removed properties */
	struct	device_node *parent;
	struct	device_node *child;
	struct	device_node *sibling;
	struct	kobject kobj;
	unsigned long _flags;
	void	*data;
#if defined(CONFIG_SPARC)
	const char *path_component_name;
	unsigned int unique_id;
	struct of_irq_controller *irq_trans;
#endif
};

struct of_device_id {					// = 
	char	name[32];
	char	type[32];
	char	compatible[128];
	const void *data;
};

// ================================= mxc_platform_info
struct spi_imx_master {					// = pdev->dev->platform_data
	int	*chipselect;
	int	num_chipselect;					// 读取 "fsl,spi-num-chipselects" 的值，或者 = mxc_platform_info->num_chipselect
};

// ================================= master
struct spi_master {
	struct device	dev;
	struct list_head list;
	s16			bus_num;					// pdev->id
	u16			num_chipselect;				// 读取 "fsl,spi-num-chipselects" 的值，或者 = mxc_platform_info->num_chipselect
	u16			dma_alignment;
	u16			mode_bits;					//  = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;
	u32			bits_per_word_mask;			// 0x1111 1111
#define SPI_BPW_MASK(bits) BIT((bits) - 1)
#define SPI_BIT_MASK(bits) (((bits) == 32) ? ~0U : (BIT(bits) - 1))
#define SPI_BPW_RANGE_MASK(min, max) (SPI_BIT_MASK(max) - SPI_BIT_MASK(min - 1))
	u32			min_speed_hz;
	u32			max_speed_hz;
	u16			flags;
#define SPI_MASTER_HALF_DUPLEX	BIT(0)		/* can't do full duplex */
#define SPI_MASTER_NO_RX	BIT(1)		/* can't do buffer read */
#define SPI_MASTER_NO_TX	BIT(2)		/* can't do buffer write */
#define SPI_MASTER_MUST_RX      BIT(3)		/* requires rx */
#define SPI_MASTER_MUST_TX      BIT(4)		/* requires tx */
	spinlock_t			bus_lock_spinlock;
	struct mutex		bus_lock_mutex;
	bool				bus_lock_flag;
	int			(*setup)(struct spi_device *spi);				// = spi_imx_setup; /  = spi_bitbang_setup;
	int			(*transfer)(struct spi_device *spi,
						struct spi_message *mesg);
	void		(*cleanup)(struct spi_device *spi);				// = spi_imx_cleanup; / = spi_bitbang_cleanup;
	bool		(*can_dma)(struct spi_master *master,
					   struct spi_device *spi,
					   struct spi_transfer *xfer);
	bool						queued;
	struct kthread_worker		kworker;
	struct task_struct			*kworker_task;
	struct kthread_work			pump_messages;
	spinlock_t				queue_lock;
	struct list_head		queue;
	struct spi_message		*cur_msg;
	bool				idling;
	bool				busy;
	bool				running;
	bool				rt;
	bool				auto_runtime_pm;
	bool                cur_msg_prepared;
	bool				cur_msg_mapped;
	struct completion   xfer_completion;
	size_t				max_dma_len;

	int (*prepare_transfer_hardware)(struct spi_master *master);		// = spi_bitbang_prepare_hardware;
	int (*transfer_one_message)(struct spi_master *master,				// = spi_bitbang_transfer_one;
				    struct spi_message *mesg);
	int (*unprepare_transfer_hardware)(struct spi_master *master);		//  = spi_bitbang_unprepare_hardware;
	int (*prepare_message)(struct spi_master *master,					//  = spi_imx_prepare_message;
			       struct spi_message *message);
	int (*unprepare_message)(struct spi_master *master,					//  = spi_imx_unprepare_message;
				 struct spi_message *message);
	void (*set_cs)(struct spi_device *spi, bool enable);
	int (*transfer_one)(struct spi_master *master, struct spi_device *spi,
			    struct spi_transfer *transfer);
	void (*handle_err)(struct spi_master *master,
			   struct spi_message *message);
	int			*cs_gpios;
	struct dma_chan		*dma_tx;
	struct dma_chan		*dma_rx;
	void			*dummy_rx;
	void			*dummy_tx;
};

// ================================= spi_imx
struct spi_imx_data {
	struct spi_bitbang bitbang;					// bitbang.master = master
												// bitbang.chipselect = spi_imx_chipselect;
												// bitbang.setup_transfer = spi_imx_setupxfer;
												// bitbang.txrx_bufs = spi_imx_transfer;
												// bitbang.master->setup = spi_imx_setup;
												// bitbang.master->cleanup = spi_imx_cleanup;
												// bitbang.master->prepare_message = spi_imx_prepare_message;
												// bitbang.master->unprepare_message = spi_imx_unprepare_message;
												// bitbang.master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;

	struct completion xfer_done;
	void __iomem *base;							// 设备树获取
	struct clk *clk_per;
	struct clk *clk_ipg;
	unsigned long spi_clk;

	unsigned int count;
	void (*tx)(struct spi_imx_data *);
	void (*rx)(struct spi_imx_data *);
	void *rx_buf;
	const void *tx_buf;
	unsigned int txfifo; /* number of words pushed in tx FIFO */

	/* DMA */
	unsigned int dma_is_inited;
	unsigned int dma_finished;
	bool usedma;
	u32 rx_wml;
	u32 tx_wml;
	u32 rxt_wml;
	struct completion dma_rx_completion;
	struct completion dma_tx_completion;
	struct dma_slave_config rx_config;
	struct dma_slave_config tx_config;

	const struct spi_imx_devtype_data *devtype_data;
	int chipselect[0];				//  读取 "fsl,spi-num-chipselects" 的值，或者 = mxc_platform_info->num_chipselect 
};

// =============================
struct spi_bitbang {
	spinlock_t		lock;
	u8			busy;
	u8			use_dma;
	u8			flags;		/* extra spi->mode support */

	struct spi_master	*master;

	/* setup_transfer() changes clock and/or wordsize to match settings
	 * for this transfer; zeroes restore defaults from spi_device.
	 */
	int	(*setup_transfer)(struct spi_device *spi,						// = spi_imx_setupxfer / spi_bitbang_setup_transfer;
			struct spi_transfer *t);

	void	(*chipselect)(struct spi_device *spi, int is_on);			// = spi_imx_chipselect;
#define	BITBANG_CS_ACTIVE	1	/* normally nCS, active low */
#define	BITBANG_CS_INACTIVE	0

	/* txrx_bufs() may handle dma mapping for transfers that don't
	 * already have one (transfer.{tx,rx}_dma is zero), or use PIO
	 */
	int	(*txrx_bufs)(struct spi_device *spi, struct spi_transfer *t);	//  = spi_imx_transfer / spi_bitbang_bufs

	/* txrx_word[SPI_MODE_*]() just looks like a shift register */
	u32	(*txrx_word[4])(struct spi_device *spi,
			unsigned nsecs,
			u32 word, u8 bits);
};

// =============================
struct spi_imx_devtype_data {
	void (*intctrl)(struct spi_imx_data *, int);
	int (*config)(struct spi_imx_data *, struct spi_imx_config *);
	void (*trigger)(struct spi_imx_data *);
	int (*rx_available)(struct spi_imx_data *);
	void (*reset)(struct spi_imx_data *);
	enum spi_imx_devtype devtype;
};

// =============================
struct resource {
	resource_size_t start;
	resource_size_t end;
	const char *name;
	unsigned long flags;
	struct resource *parent, *sibling, *child;
};
