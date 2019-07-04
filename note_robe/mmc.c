
struct fsl_esdhc_cfg {
#ifdef CONFIG_FSL_LAYERSCAPE
	u64	esdhc_base;
#else
	u32	esdhc_base;
#endif
	u32	sdhc_clk;
	u8	max_bus_width;
	u8	wp_enable;
	struct mmc_config cfg;
};

struct mmc_config {
	const char *name;
	const struct mmc_ops *ops;
	uint host_caps;
	uint voltages;
	uint f_min;
	uint f_max;
	uint b_max;
	unsigned char part_type;
};

static const struct mmc_ops esdhc_ops = {
	.send_cmd	= esdhc_send_cmd,
	.set_ios	= esdhc_set_ios,
	.init		= esdhc_init,
	.getcd		= esdhc_getcd,
};
struct mmc {
	struct list_head link;
	const struct mmc_config *cfg;	/* provided configuration */
	uint version;
	void *priv;
	uint has_init;
	int high_capacity;
	uint bus_width;
	uint clock;
	uint card_caps;
	uint ocr;
	uint dsr;
	uint dsr_imp;
	uint scr[2];
	uint csd[4];
	uint cid[4];
	ushort rca;
	u8 part_support;
	u8 part_attr;
	u8 wr_rel_set;
	char part_config;
	uint tran_speed;
	uint read_bl_len;
	uint write_bl_len;
	uint erase_grp_size;	/* in 512-byte sectors */
	uint hc_wp_grp_size;	/* in 512-byte sectors */
	u64 capacity;
	u64 capacity_user;
	u64 capacity_boot;
	u64 capacity_rpmb;
	u64 capacity_gp[4];
	u64 enh_user_start;
	u64 enh_user_size;
	block_dev_desc_t block_dev;
	char op_cond_pending;	/* 1 if we are waiting on an op_cond command */
	char init_in_progress;	/* 1 if we have done mmc_start_init() */
	char preinit;		/* start init as early as possible */
	int ddr_mode;
};
struct block_dev_desc {
	int		if_type;	/* type of the interface */
	int		dev;		/* device number */
	unsigned char	part_type;	/* partition type */
	unsigned char	target;		/* target SCSI ID */
	unsigned char	lun;		/* target LUN */
	unsigned char	hwpart;		/* HW partition, e.g. for eMMC */
	unsigned char	type;		/* device type */
	unsigned char	removable;	/* removable device */
#ifdef CONFIG_LBA48
	unsigned char	lba48;		/* device can use 48bit addr (ATA/ATAPI v7) */
#endif
	lbaint_t	lba;		/* number of blocks */
	unsigned long	blksz;		/* block size */
	int		log2blksz;	/* for convenience: log2(blksz) */
	char		vendor [40+1];	/* IDE model, SCSI Vendor */
	char		product[20+1];	/* IDE Serial no, SCSI product */
	char		revision[8+1];	/* firmware revision */
	unsigned long	(*block_read)(block_dev_desc_t *block_dev,
				      lbaint_t start,
				      lbaint_t blkcnt,
				      void *buffer);
	unsigned long	(*block_write)(block_dev_desc_t *block_dev,
				       lbaint_t start,
				       lbaint_t blkcnt,
				       const void *buffer);
	unsigned long	(*block_erase)(block_dev_desc_t *block_dev,
				       lbaint_t start,
				       lbaint_t blkcnt);
	void		*priv;		/* driver private struct pointer */
};
struct mmc_cmd {
	ushort cmdidx;
	uint resp_type;
	uint cmdarg;
	uint response[4];
};



