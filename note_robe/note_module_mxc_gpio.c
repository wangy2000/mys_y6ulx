
// ===========================================================	// irq 相关
(Linux\drivers\gpio\gpio-generic.c)
irq_alloc_descs(-1, 0, 32, numa_node_id());


	list_add(&domain->link, &irq_domain_list);


irq_domain_add_legacy(np, 32, irq_base, 0, &irq_domain_simple_ops, NULL);
	__irq_domain_add
		(kzalloc),												// 申请内存，并初始化
		list_add(&domain->link, &irq_domain_list);				// 添加到 irq_domain_list 全局便来给你中
	irq_domain_associate_many									// 设置添加 irq
	
mxc_gpio_init_gc(port, irq_base);								// 初始 irq 的操作方法



// ===========================================================
gpiochip_add(&port->bgc.gc);									// port->bgc.gc(struct gpio_chip)
	gpiochip_find_base(chip->ngpio);							// 初始化 gc.base 
	gpiochip_add_to_list(chip);									// 加入全局变量 gpio_chips 中
	chip->desc = descs = (kcalloc);								// 初始化 gc/chip->desc
	of_gpiochip_add(chip);
		chip->of_node = chip->dev->of_node;						// 设置 chip->of_node 成员	
		chip->of_gpio_n_cells = 2								// 设置 chip成员
		chip->of_xlate = of_gpio_simple_xlate;					// 设置 chip成员
		of_gpiochip_add_pin_range(chip);						// pinctrl 相关，填充chip->pin_ranges,填充结构体为(struct gpio_pin_range)
		of_gpiochip_scan_hogs(chip);
	acpi_gpiochip_add(chip);
	gpiochip_export(chip);
	
// ===========================================================
(Linux\drivers\gpio\gpio-generic.c)
bgpio_init
	bgpio_setup_io(bgc, dat, set, clr);							// 设置 bgc.reg_set|reg_clr, 和 gc 的set|get 方法
	bgpio_setup_accessors(dev, bgc, flags & BGPIOF_BIG_ENDIAN,
				    flags & BGPIOF_BIG_ENDIAN_BYTE_ORDER);		// 设置 bgc.read_reg|write_reg|pin2mask 方法

// ===========================================================
(Linux\drivers\gpio\gpio-mxc.c)
static int mxc_gpio_probe(struct platform_device *pdev){
	mxc_gpio_get_hw(pdev);										// 设置 mxc_gpio_hwtype(寄存器的地址和偏移值)
	port = devm_kzalloc(&pdev->dev, sizeof(*port), GFP_KERNEL);
	port->base = devm_ioremap_resource(&pdev->dev, iores);		//申请，映射 mem io
	
	port->irq_high = platform_get_irq(pdev, 1);
	port->irq = platform_get_irq(pdev, 0);						// 获取中断号
	writel(0, port->base + GPIO_IMR);
	writel(~0, port->base + GPIO_ISR);							// 清理中断寄存器
	irq_set_chained_handler(port->irq, mx3_gpio_irq_handler);	// 设置中断处理函数，开启中断
	irq_set_handler_data(port->irq, port);						// 设置中断数据
	
	bgpio_init();												// 以下 9 个参数
		&port->bgc, 				// struct ggpio_chip * bpc
		&pdev->dev, 				// strcut device * dev
		4,							// unsigned long sz
		port->base + GPIO_PSR, 		// void __iomem *dat
		port->base + GPIO_DR,		// void __iomem *set,
		NULL, 						// void __iomem *clr,
		port->base + GPIO_GDIR, 	// void __iomem *dirout
		NULL, 						// void __iomem *dirin,
		0							// unsigned long flags
																// 设置 mxc_gpio_port.bgc.gc
																// 设置 port->bgc.gc.to_irq = mxc_gpio_to_irq;\
																// 设置 port->bgc.gc.base
																
	gpiochip_add(&port->bgc.gc);								// 注册 gpiochip，
		gpiochip_add_to_list(chip);								// 加入全局变量 gpio_chips 中
		of_gpiochip_add(chip);									// 
		acpi_gpiochip_add(chip);
		gpiochip_export(chip);									// 创建 sysfs device
		
	irq_base = irq_alloc_descs(-1, 0, 32, numa_node_id());		// 获取 irq 值
	port->domain = irq_domain_add_legacy(np, 32, irq_base, 0,
					     &irq_domain_simple_ops, NULL);			// 添加 irq_domain_simple_ops
	mxc_gpio_init_gc(port, irq_base);							// 初始化 irq 相关
	list_add_tail(&port->node, &mxc_gpio_ports);				// 添加到 mxc_gpio_ports 链表
	
}

enum mxc_gpio_hwtype {
	IMX1_GPIO,	/* runs on i.mx1 */
	IMX21_GPIO,	/* runs on i.mx21 and i.mx27 */
	IMX31_GPIO,	/* runs on i.mx31 */
	IMX35_GPIO,	/* runs on all other i.mx */
};

static struct mxc_gpio_hwdata imx35_gpio_hwdata = {
	.dr_reg		= 0x00,
	.gdir_reg	= 0x04,
	.psr_reg	= 0x08,
	.icr1_reg	= 0x0c,
	.icr2_reg	= 0x10,
	.imr_reg	= 0x14,
	.isr_reg	= 0x18,
	.edge_sel_reg	= 0x1c,
	.low_level	= 0x00,
	.high_level	= 0x01,
	.rise_edge	= 0x02,
	.fall_edge	= 0x03,
};

static enum mxc_gpio_hwtype mxc_gpio_hwtype;		// = IMX35_GPIO
static struct mxc_gpio_hwdata *mxc_gpio_hwdata;		// = imx35_gpio_hwdata

#define GPIO_DR			(mxc_gpio_hwdata->dr_reg)
#define GPIO_GDIR		(mxc_gpio_hwdata->gdir_reg)
#define GPIO_PSR		(mxc_gpio_hwdata->psr_reg)
#define GPIO_ICR1		(mxc_gpio_hwdata->icr1_reg)
#define GPIO_ICR2		(mxc_gpio_hwdata->icr2_reg)
#define GPIO_IMR		(mxc_gpio_hwdata->imr_reg)
#define GPIO_ISR		(mxc_gpio_hwdata->isr_reg)
#define GPIO_EDGE_SEL		(mxc_gpio_hwdata->edge_sel_reg)

#define GPIO_INT_LOW_LEV	(mxc_gpio_hwdata->low_level)
#define GPIO_INT_HIGH_LEV	(mxc_gpio_hwdata->high_level)
#define GPIO_INT_RISE_EDGE	(mxc_gpio_hwdata->rise_edge)
#define GPIO_INT_FALL_EDGE	(mxc_gpio_hwdata->fall_edge)
#define GPIO_INT_BOTH_EDGES	0x4


struct of_device_id {
	char	name[32];
	char	type[32];
	char	compatible[128];
	const void *data;
};

static const struct of_device_id mxc_gpio_dt_ids[] = {
	{ .compatible = "fsl,imx1-gpio", .data = &mxc_gpio_devtype[IMX1_GPIO], },
	{ .compatible = "fsl,imx21-gpio", .data = &mxc_gpio_devtype[IMX21_GPIO], },
	{ .compatible = "fsl,imx31-gpio", .data = &mxc_gpio_devtype[IMX31_GPIO], },
	{ .compatible = "fsl,imx35-gpio", .data = &mxc_gpio_devtype[IMX35_GPIO], },
	{ /* sentinel */ }
};

struct platform_device_id {
	char name[PLATFORM_NAME_SIZE];
	kernel_ulong_t driver_data;
};

static struct platform_device_id mxc_gpio_devtype[] = {
	{
		.name = "imx1-gpio",
		.driver_data = IMX1_GPIO,
	}, {
		.name = "imx21-gpio",
		.driver_data = IMX21_GPIO,
	}, {
		.name = "imx31-gpio",
		.driver_data = IMX31_GPIO,
	}, {
		.name = "imx35-gpio",
		.driver_data = IMX35_GPIO,
	}, {
		/* sentinel */
	}
};

static struct platform_driver mxc_gpio_driver = {
	.driver		= {
		.name	= "gpio-mxc",
		.of_match_table = mxc_gpio_dt_ids,
	},
	.probe		= mxc_gpio_probe,
	.id_table	= mxc_gpio_devtype,
};

static int __init gpio_mxc_init(void)
{
	return platform_driver_register(&mxc_gpio_driver);
}
postcore_initcall(gpio_mxc_init);



