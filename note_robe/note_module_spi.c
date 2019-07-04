
// ============================================== probe
static int spi_imx_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;				// 从 pdev 获取 of_node(就可以从 device tree 获取信息)
	const struct of_device_id *of_id =
			of_match_device(spi_imx_dt_ids, &pdev->dev);	// 从 pdev 匹配到 match_table(struct of_device_id)
	struct spi_imx_master *mxc_platform_info =
			dev_get_platdata(&pdev->dev);					//  从 pdev 获取 pdev->dev->platform_data
	struct spi_master *master;
	struct spi_imx_data *spi_imx;
	struct resource *res;
	int i, ret, num_cs, irq;	
	
	of_property_read_u32(np, "fsl,spi-num-chipselects", &num_cs);
	if (mxc_platform_info)
		num_cs = mxc_platform_info->num_chipselect;			// 获取 num_chipselect
	
	master = spi_alloc_master(&pdev->dev,					// 分配空间，设置 master 的 driver_data
			sizeof(struct spi_imx_data) + sizeof(int) * num_cs);
	
	platform_set_drvdata(pdev, master);						// 设置 pdev->dev->driver_data = master
	
	master->bits_per_word_mask = SPI_BPW_RANGE_MASK(1, 32);	// 0x1111 1111
	master->bus_num = pdev->id;
	master->num_chipselect = num_cs;

	spi_imx = spi_master_get_devdata(master);				
	spi_imx->bitbang.master = master;						// 填充 spi_imx->bitbang.master
	
	for (i = 0; i < master->num_chipselect; i++) {
		int cs_gpio = of_get_named_gpio(np, "cs-gpios", i);
		if (!gpio_is_valid(cs_gpio) && mxc_platform_info)
			cs_gpio = mxc_platform_info->chipselect[i];

		spi_imx->chipselect[i] = cs_gpio;
		if (!gpio_is_valid(cs_gpio))
			continue;

		ret = devm_gpio_request(&pdev->dev, spi_imx->chipselect[i],
					DRIVER_NAME);
		if (ret) {
			dev_err(&pdev->dev, "can't get cs gpios\n");
			goto out_master_put;
		}
	}														// 设置 cs / spi_imx->chipselect[i] = cs_gpio;
	
	spi_imx->bitbang.chipselect = spi_imx_chipselect;
	spi_imx->bitbang.setup_transfer = spi_imx_setupxfer;
	spi_imx->bitbang.txrx_bufs = spi_imx_transfer;
	spi_imx->bitbang.master->setup = spi_imx_setup;
	spi_imx->bitbang.master->cleanup = spi_imx_cleanup;
	spi_imx->bitbang.master->prepare_message = spi_imx_prepare_message;
	spi_imx->bitbang.master->unprepare_message = spi_imx_unprepare_message;
	spi_imx->bitbang.master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;	// 初始化一些方法和模式

	init_completion(&spi_imx->xfer_done);					// 初始化 spi_imx->xfer_done

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	spi_imx->base = devm_ioremap_resource(&pdev->dev, res);	// 获取并remap 赋值给 base

	irq = platform_get_irq(pdev, 0);
	ret = devm_request_irq(&pdev->dev, irq, spi_imx_isr, 0,
			       dev_name(&pdev->dev), spi_imx);			// 获取并等级 riq 
	
	spi_imx->clk_ipg = devm_clk_get(&pdev->dev, "ipg");
	spi_imx->clk_per = devm_clk_get(&pdev->dev, "per");		// 设置两个时钟
	
	ret = clk_prepare_enable(spi_imx->clk_per);
	ret = clk_prepare_enable(spi_imx->clk_ipg);				// enable 两个时钟
	
	spi_imx->spi_clk = clk_get_rate(spi_imx->clk_per);		// 得到 spi 时钟
	
	if ((spi_imx->devtype_data == &imx51_ecspi_devtype_data
	    || spi_imx->devtype_data == &imx6ul_ecspi_devtype_data)
	    && spi_imx_sdma_init(&pdev->dev, spi_imx, master, res))	// 初始化 dma
	
	spi_imx->devtype_data->reset(spi_imx);
	spi_imx->devtype_data->intctrl(spi_imx, 0);					// match table 的 .data 成员实际上是一些方法，复位控制的方法
	
	master->dev.of_node = pdev->dev.of_node;					// master 执行父节点 pdev
	
	ret = spi_bitbang_start(&spi_imx->bitbang);					// 启动 spi 
	// ------------------------------------------
		if (!master->mode_bits)
			master->mode_bits = SPI_CPOL | SPI_CPHA | bitbang->flags;

		if (master->transfer || master->transfer_one_message)
			return -EINVAL;

		master->prepare_transfer_hardware = spi_bitbang_prepare_hardware;
		master->unprepare_transfer_hardware = spi_bitbang_unprepare_hardware;
		master->transfer_one_message = spi_bitbang_transfer_one;
		
		spi_register_master(spi_master_get(master));			// 注册
		
	// ------------------------------------------				// 这主要是用 bitbang 覆盖以前的方法，然后注册 spi master
	clk_disable_unprepare(spi_imx->clk_ipg);
	clk_disable_unprepare(spi_imx->clk_per);					// 关闭两个时钟
}

// ============================================== remove
static int spi_imx_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct spi_imx_data *spi_imx = spi_master_get_devdata(master);

	spi_bitbang_stop(&spi_imx->bitbang);

	writel(0, spi_imx->base + MXC_CSPICTRL);
	clk_unprepare(spi_imx->clk_ipg);
	clk_unprepare(spi_imx->clk_per);
	spi_imx_sdma_exit(spi_imx);
	spi_master_put(master);

	return 0;
}

// ============================================== id_table / (platform_device_id)
static struct platform_device_id spi_imx_devtype[] = {
	{
		.name = "imx1-cspi",
		.driver_data = (kernel_ulong_t) &imx1_cspi_devtype_data,
	}, {
		.name = "imx21-cspi",
		.driver_data = (kernel_ulong_t) &imx21_cspi_devtype_data,
	}, {
		.name = "imx27-cspi",
		.driver_data = (kernel_ulong_t) &imx27_cspi_devtype_data,
	}, {
		.name = "imx31-cspi",
		.driver_data = (kernel_ulong_t) &imx31_cspi_devtype_data,
	}, {
		.name = "imx35-cspi",
		.driver_data = (kernel_ulong_t) &imx35_cspi_devtype_data,
	}, {
		.name = "imx51-ecspi",
		.driver_data = (kernel_ulong_t) &imx51_ecspi_devtype_data,
	}, {
		.name = "imx6ul-ecspi",
		.driver_data = (kernel_ulong_t) &imx6ul_ecspi_devtype_data,
	}, {
		/* sentinel */
	}
};													// driver_data 和 match table.data 一样样的，多此一举是为什么？？？？？？

static struct spi_imx_devtype_data imx6ul_ecspi_devtype_data = {
	.intctrl = mx51_ecspi_intctrl,
	.config = mx51_ecspi_config,
	.trigger = mx51_ecspi_trigger,
	.rx_available = mx51_ecspi_rx_available,
	.reset = mx51_ecspi_reset,
	.devtype = IMX6UL_ECSPI,
};

// ============================================== driver.of_match_table / (of_device_id)
static const struct of_device_id spi_imx_dt_ids[] = {
	{ .compatible = "fsl,imx1-cspi", .data = &imx1_cspi_devtype_data, },
	{ .compatible = "fsl,imx21-cspi", .data = &imx21_cspi_devtype_data, },
	{ .compatible = "fsl,imx27-cspi", .data = &imx27_cspi_devtype_data, },
	{ .compatible = "fsl,imx31-cspi", .data = &imx31_cspi_devtype_data, },
	{ .compatible = "fsl,imx35-cspi", .data = &imx35_cspi_devtype_data, },
	{ .compatible = "fsl,imx51-ecspi", .data = &imx51_ecspi_devtype_data, },
	{ .compatible = "fsl,imx6ul-ecspi", .data = &imx6ul_ecspi_devtype_data, },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, spi_imx_dt_ids);

// ============================================== pm
static int spi_imx_suspend(struct device *dev)
{
	pinctrl_pm_select_sleep_state(dev);
	return 0;
}

static int spi_imx_resume(struct device *dev)
{
	pinctrl_pm_select_default_state(dev);
	return 0;
}

static SIMPLE_DEV_PM_OPS(imx_spi_pm, spi_imx_suspend, spi_imx_resume);
#define IMX_SPI_PM       (&imx_spi_pm)

// =============================================== module
static struct platform_driver spi_imx_driver = {
	.driver = {
		   .name = DRIVER_NAME,
		   .of_match_table = spi_imx_dt_ids,
		   .pm = IMX_SPI_PM,
	},
	.id_table = spi_imx_devtype,
	.probe = spi_imx_probe,
	.remove = spi_imx_remove,
};
module_platform_driver(spi_imx_driver);

MODULE_DESCRIPTION("SPI Master Controller driver");
MODULE_AUTHOR("Sascha Hauer, Pengutronix");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRIVER_NAME);
