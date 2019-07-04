// =======================================
static int at24_probe(struct i2c_client *client, const struct i2c_device_id *id)
{	
	struct at24_platform_data chip;
	struct at24_data *at24;
	
	chip = *(struct at24_platform_data *)client->dev.platform_data;
	chip.byte_len = BIT(magic & AT24_BITMASK(AT24_SIZE_BYTELEN));
	chip.flags = magic & AT24_BITMASK(AT24_SIZE_FLAGS);
	chip.page_size = 1;
	at24_get_ofdata(client, &chip);					// 设置 chip.page_size 和 chip.flags
	chip.setup = NULL;
	chip.context = NULL;
	
	at24 = devm_kzalloc(&client->dev, sizeof(struct at24_data) +
		num_addresses * sizeof(struct i2c_client *), GFP_KERNEL);
	at24->use_smbus = use_smbus;
	at24->use_smbus_write = use_smbus_write;
	at24->chip = chip;
	at24->num_addresses = num_addresses; 
	
	at24->bin.attr.name = "eeprom";
	at24->bin.attr.mode = chip.flags & AT24_FLAG_IRUGO ? S_IRUGO : S_IRUSR;
	at24->bin.read = at24_bin_read;					// ops
	at24->bin.size = chip.byte_len;
	at24->bin.write = at24_bin_write;				// ops
	at24->bin.attr.mode |= S_IWUSR;
			
	at24->macc.read = at24_macc_read;				// ops
	at24->macc.write = at24_macc_write;				// ops
	
	at24->write_max = write_max;
	at24->writebuf = devm_kzalloc(&client->dev,
				write_max + 2, GFP_KERNEL);
	
	at24->client[0] = client;
	
	i2c_set_clientdata(client, at24);				// client->dev->driver_data = at24
	
	chip.setup(&at24->macc, chip.context);
	
}
// =======================================
#define AT24_DEVICE_MAGIC(_len, _flags) 		\
	((1 << AT24_SIZE_FLAGS | (_flags)) 		\
	    << AT24_SIZE_BYTELEN | ilog2(_len))


#define AT24_SIZE_BYTELEN 5
#define AT24_SIZE_FLAGS 8
	
AT24_DEVICE_MAGIC(_len, _flags)
	= ((1 << AT24_SIZE_FLAGS | (_flags)) << AT24_SIZE_BYTELEN | ilog2(_len))
	= ((1 << 8 | (_flags)) << 5 | ilog2(_len))

static const struct i2c_device_id at24_ids[] = {
	/* needs 8 addresses as A0-A2 are ignored */
	{ "24c00", AT24_DEVICE_MAGIC(128 / 8, AT24_FLAG_TAKE8ADDR) },
	/* old variants can't be handled with this generic entry! */
	{ "24c01", AT24_DEVICE_MAGIC(1024 / 8, 0) },
	{ "24c02", AT24_DEVICE_MAGIC(2048 / 8, 0) },
	/* spd is a 24c02 in memory DIMMs */
	{ "spd", AT24_DEVICE_MAGIC(2048 / 8,
		AT24_FLAG_READONLY | AT24_FLAG_IRUGO) },
	{ "24c04", AT24_DEVICE_MAGIC(4096 / 8, 0) },
	/* 24rf08 quirk is handled at i2c-core */
	{ "24c08", AT24_DEVICE_MAGIC(8192 / 8, 0) },
	{ "24c16", AT24_DEVICE_MAGIC(16384 / 8, 0) },
	{ "24c32", AT24_DEVICE_MAGIC(32768 / 8, AT24_FLAG_ADDR16) },
	{ "24c64", AT24_DEVICE_MAGIC(65536 / 8, AT24_FLAG_ADDR16) },
	{ "24c128", AT24_DEVICE_MAGIC(131072 / 8, AT24_FLAG_ADDR16) },
	{ "24c256", AT24_DEVICE_MAGIC(262144 / 8, AT24_FLAG_ADDR16) },
	{ "24c512", AT24_DEVICE_MAGIC(524288 / 8, AT24_FLAG_ADDR16) },
	{ "24c1024", AT24_DEVICE_MAGIC(1048576 / 8, AT24_FLAG_ADDR16) },
	{ "at24", 0 },
	{ /* END OF LIST */ }
};
MODULE_DEVICE_TABLE(i2c, at24_ids);		// =======================================================

// ====================================
(Linux\drivers\misc\eeprom\at24.c)
 static struct i2c_driver at24_driver = {
	.driver = {
		.name = "at24",
		.owner = THIS_MODULE,
	},
	.probe = at24_probe,
	.remove = at24_remove,
	.id_table = at24_ids,
};

static int __init at24_init(void)
{
	if (!io_limit) {
		pr_err("at24: io_limit must not be 0!\n");
		return -EINVAL;
	}

	io_limit = rounddown_pow_of_two(io_limit);
	return i2c_add_driver(&at24_driver);
}
module_init(at24_init);

static void __exit at24_exit(void)
{
	i2c_del_driver(&at24_driver);
}
module_exit(at24_exit);

MODULE_DESCRIPTION("Driver for most I2C EEPROMs");
MODULE_AUTHOR("David Brownell and Wolfram Sang");
MODULE_LICENSE("GPL");


