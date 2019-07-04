
struct i2c_device_id {
	char name[I2C_NAME_SIZE];
	kernel_ulong_t driver_data;	/* Data private to the driver */
};

struct i2c_driver {
	unsigned int class;
	int (*attach_adapter)(struct i2c_adapter *) __deprecated;
	int (*probe)(struct i2c_client *, const struct i2c_device_id *);
	int (*remove)(struct i2c_client *);
	void (*shutdown)(struct i2c_client *);
	void (*alert)(struct i2c_client *, unsigned int data);
	int (*command)(struct i2c_client *client, unsigned int cmd, void *arg);
	struct device_driver driver;
	const struct i2c_device_id *id_table;
	int (*detect)(struct i2c_client *, struct i2c_board_info *);
	const unsigned short *address_list;
	struct list_head clients;
};
// ====================================== chip
struct at24_platform_data {
	u32		byte_len;		/* size (sum of all addr) */
	u16		page_size;		/* for writes */
	u8		flags;
#define AT24_FLAG_ADDR16	0x80	/* address pointer is 16 bit */
#define AT24_FLAG_READONLY	0x40	/* sysfs-entry will be read-only */
#define AT24_FLAG_IRUGO		0x20	/* sysfs-entry will be world-readable */
#define AT24_FLAG_TAKE8ADDR	0x10	/* take always 8 addresses (24c00) */
	void		(*setup)(struct memory_accessor *, void *context);
	void		*context;
};

// ====================================== at24
struct at24_data {
	struct at24_platform_data chip;				// OK
	struct memory_accessor macc;				// at24->macc.read = at24_macc_read;
												// at24->macc.write = at24_macc_write;
	int use_smbus;								// OK
	int use_smbus_write;						// OK

	/*
	 * Lock protects against activities from other Linux tasks,
	 * but not from changes by other I2C masters.
	 */
	struct mutex lock;
	struct bin_attribute bin;					// at24->bin.read = at24_bin_read;
												// at24->bin.size = chip.byte_len;
												// at24->bin.attr.mode = chip.flags & AT24_FLAG_IRUGO ? S_IRUGO : S_IRUSR;
												// at24->bin.attr.name = "eeprom";
	u8 *writebuf;								// at24->writebuf = devm_kzalloc(&client->dev,write_max + 2, GFP_KERNEL);
	unsigned write_max;							// at24->write_max = write_max;
	unsigned num_addresses;						// OK

	/*
	 * Some chips tie up multiple I2C addresses; dummy devices reserve
	 * them for us, and we'll use them with SMBus calls.
	 */
	struct i2c_client *client[];
};

struct bin_attribute {
	struct attribute	attr;
	size_t			size;
	void			*private;
	ssize_t (*read)(struct file *, struct kobject *, struct bin_attribute *,	// = at24_bin_read;
			char *, loff_t, size_t);
	ssize_t (*write)(struct file *, struct kobject *, struct bin_attribute *,
			 char *, loff_t, size_t);
	int (*mmap)(struct file *, struct kobject *, struct bin_attribute *attr,
		    struct vm_area_struct *vma);
};

struct memory_accessor {
	ssize_t (*read)(struct memory_accessor *, char *buf, off_t offset,
			size_t count);
	ssize_t (*write)(struct memory_accessor *, const char *buf,
			 off_t offset, size_t count);
};
// ====================================== client
struct i2c_client {
	unsigned short flags;		/* div., see below		*/
	unsigned short addr;		/* chip address - NOTE: 7bit	*/
					/* addresses are stored in the	*/
					/* _LOWER_ 7 bits		*/
	char name[I2C_NAME_SIZE];
	struct i2c_adapter *adapter;	/* the adapter we sit on	*/
	struct device dev;		/* the device structure		*/
	int irq;			/* irq issued by device		*/
	struct list_head detected;
#if IS_ENABLED(CONFIG_I2C_SLAVE)
	i2c_slave_cb_t slave_cb;	/* callback for slave mode	*/
#endif
};




