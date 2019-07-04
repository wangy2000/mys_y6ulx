// ===================================================================
(Linux\drivers\i2c\busses\i2c-imx.c)



// ==================================================== probe  remove
static int i2c_imx_probe(struct platform_device *pdev)
{
	const struct of_device_id  of_id 
		= of_match_device(i2c_imx_dt_ids,&pdev->dev);		// 从 pdev 获取 match_table
	
	struct imxi2c_platform_data *pdata = dev_get_platdata(&pdev->dev);	// 从 pdev 获取 pdata
	
	irq = platform_get_irq(pdev, 0);									// 从 pdev 获取 irq
	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);				// 从 pdev 获取res
	base = devm_ioremap_resource(&pdev->dev, res);						// 从 pdev 得到 base
	
	phy_addr = (dma_addr_t)res->start;									// 从 pdev 得到 res->start
	
	i2c_imx = devm_kzalloc(&pdev->dev, sizeof(*i2c_imx), GFP_KERNEL);	// 分配 imx_i2c_struct 空间
	
	i2c_imx->hwdata = of_id->data;										// 给 i2c_imx->hwdata 赋值 imx21_i2c_hwdata (of_device_id->data)
	strlcpy(i2c_imx->adapter.name, pdev->name, sizeof(i2c_imx->adapter.name));
	i2c_imx->adapter.owner		= THIS_MODULE;
	i2c_imx->adapter.algo		= &i2c_imx_algo;						// i2c_imx_algo 方法
	i2c_imx->adapter.dev.parent	= &pdev->dev;
	i2c_imx->adapter.nr		= pdev->id;
	i2c_imx->adapter.dev.of_node	= pdev->dev.of_node;				// 给 i2c_imx->adapter 赋值
	i2c_imx->base			= base;										// 给 i2c_imx->base 赋值
	
	i2c_imx->clk = devm_clk_get(&pdev->dev, NULL);						// 从 pdev 设备树获取 clk 并添加到 devres
	
	clk_prepare_enable(i2c_imx->clk);
	
	devm_request_irq(&pdev->dev, irq, i2c_imx_isr,
			       IRQF_NO_SUSPEND, pdev->name, i2c_imx);				// 注册中断号中断函数 i2c_imx_isr
	
	init_waitqueue_head(&i2c_imx->queue);								// 初始化 i2c_imx->queue							
	
	i2c_set_adapdata(&i2c_imx->adapter, i2c_imx);						// 设置 i2c_imx->adapter 的 dev.driver_data
																		// (struct imx_i2c_struct *)i2c_imx
	i2c_imx->bitrate = IMX_I2C_BIT_RATE;(MACRO)
	of_property_read_u32(pdev->dev.of_node,
				   "clock-frequency", &i2c_imx->bitrate);
	i2c_imx->bitrate = pdata->bitrate;									// i2c_imx->bitrate 三选一
	
	/* Set up chip registers to defaults */
	imx_i2c_write_reg(i2c_imx->hwdata->i2cr_ien_opcode ^ I2CR_IEN,
			i2c_imx, IMX_I2C_I2CR);
	imx_i2c_write_reg(i2c_imx->hwdata->i2sr_clr_opcode, i2c_imx, IMX_I2C_I2SR); 		
																		// setup，读写外设的操作，具体看 soc 寄存器手册
	
	ret = i2c_add_numbered_adapter(&i2c_imx->adapter);					// adapter 添加到 i2c_adapter_idr 中，然后注册
	if (ret < 0) {														// 注册时候 赋值 adap->timeout = HZ
		dev_err(&pdev->dev, "registration failed\n");					// dev_set_name(&adap->dev, "i2c-%d", adap->nr);
		goto clk_disable;												// adap->dev.bus = &i2c_bus_type;
																		// adap->dev.type = &i2c_adapter_type;
	}

	/* Set up platform driver data */
	platform_set_drvdata(pdev, i2c_imx);								// pdev->dev->driver_data = i2c_imx
	clk_disable_unprepare(i2c_imx->clk);								// 时钟关掉
	
	i2c_imx_dma_request(i2c_imx, phy_addr);								// 收发 channel dma 支持
	
}

static int i2c_imx_remove(struct platform_device *pdev)
{
	struct imx_i2c_struct *i2c_imx = platform_get_drvdata(pdev);

	/* remove adapter */
	dev_dbg(&i2c_imx->adapter.dev, "adapter removed\n");
	i2c_del_adapter(&i2c_imx->adapter);									// 删除 i2c_imx->adapter

	if (i2c_imx->dma)
		i2c_imx_dma_free(i2c_imx);

	/* setup chip registers to defaults */
	imx_i2c_write_reg(0, i2c_imx, IMX_I2C_IADR);
	imx_i2c_write_reg(0, i2c_imx, IMX_I2C_IFDR);
	imx_i2c_write_reg(0, i2c_imx, IMX_I2C_I2CR);
	imx_i2c_write_reg(0, i2c_imx, IMX_I2C_I2SR);

	return 0;
}


static struct i2c_algorithm i2c_imx_algo = {
	.master_xfer	= i2c_imx_xfer,
	.functionality	= i2c_imx_func,
};

static u32 i2c_imx_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL
		| I2C_FUNC_SMBUS_READ_BLOCK_DATA;
}




// ================================================= PM
static int i2c_imx_suspend(struct device *dev)
{
	pinctrl_pm_select_sleep_state(dev);
	return 0;
}

static int i2c_imx_resume(struct device *dev)
{
	pinctrl_pm_select_default_state(dev);
	return 0;
}

static SIMPLE_DEV_PM_OPS(imx_i2c_pm, i2c_imx_suspend, i2c_imx_resume);
#define IMX_I2C_PM	(&imx_i2c_pm)

// =============================================   of_match_table     driver_data
static const struct imx_i2c_hwdata imx1_i2c_hwdata  = {			//  driver_data
	.devtype		= IMX1_I2C,
	.regshift		= IMX_I2C_REGSHIFT,
	.clk_div		= imx_i2c_clk_div,
	.ndivs			= ARRAY_SIZE(imx_i2c_clk_div),
	.i2sr_clr_opcode	= I2SR_CLR_OPCODE_W0C,
	.i2cr_ien_opcode	= I2CR_IEN_OPCODE_1,

};

static const struct imx_i2c_hwdata imx21_i2c_hwdata  = {		// OK
	.devtype		= IMX21_I2C,
	.regshift		= IMX_I2C_REGSHIFT,
	.clk_div		= imx_i2c_clk_div,
	.ndivs			= ARRAY_SIZE(imx_i2c_clk_div),
	.i2sr_clr_opcode	= I2SR_CLR_OPCODE_W0C,
	.i2cr_ien_opcode	= I2CR_IEN_OPCODE_1,

};

static struct imx_i2c_hwdata vf610_i2c_hwdata = {
	.devtype		= VF610_I2C,
	.regshift		= VF610_I2C_REGSHIFT,
	.clk_div		= vf610_i2c_clk_div,
	.ndivs			= ARRAY_SIZE(vf610_i2c_clk_div),
	.i2sr_clr_opcode	= I2SR_CLR_OPCODE_W1C,
	.i2cr_ien_opcode	= I2CR_IEN_OPCODE_0,

};

static struct platform_device_id imx_i2c_devtype[] = {			// id_table
	{
		.name = "imx1-i2c",
		.driver_data = (kernel_ulong_t)&imx1_i2c_hwdata,
	}, {
		.name = "imx21-i2c",
		.driver_data = (kernel_ulong_t)&imx21_i2c_hwdata,
	}, {
		/* sentinel */
	}
};

static const struct of_device_id i2c_imx_dt_ids[] = {			// of_match_table
	{ .compatible = "fsl,imx1-i2c", .data = &imx1_i2c_hwdata, },
	{ .compatible = "fsl,imx21-i2c", .data = &imx21_i2c_hwdata, },
	{ .compatible = "fsl,vf610-i2c", .data = &vf610_i2c_hwdata, },
	{ /* sentinel */ }
};
// ============================================	driver
static struct platform_driver i2c_imx_driver = {
	.probe = i2c_imx_probe,
	.remove = i2c_imx_remove,
	.driver	= {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = i2c_imx_dt_ids,
		.pm = IMX_I2C_PM,
	},
	.id_table	= imx_i2c_devtype,
};

static int __init i2c_adap_imx_init(void)
{
	return platform_driver_register(&i2c_imx_driver);
}
subsys_initcall(i2c_adap_imx_init);

static void __exit i2c_adap_imx_exit(void)
{
	platform_driver_unregister(&i2c_imx_driver);
}
module_exit(i2c_adap_imx_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Darius Augulis");
MODULE_DESCRIPTION("I2C adapter driver for IMX I2C bus");
MODULE_ALIAS("platform:" DRIVER_NAME);


