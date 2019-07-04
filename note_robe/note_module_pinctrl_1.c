// =============================================================
(Linux\drivers\pinctrl\freescale\pinctrl-imx.c)
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

const struct pinctrl_ops *pctlops;				// = imx_pctrl_ops

static const struct pinctrl_ops imx_pctrl_ops = {
	.get_groups_count = imx_get_groups_count,		//从 pinctrl_dev 的 info 获取 group 数量
	.get_group_name = imx_get_group_name,			//从 pinctrl_dev 的 info 获取 group 名字
	.get_group_pins = imx_get_group_pins,			//从 pinctrl_dev 的 info 获取 group 的数量和 pin_id
	.pin_dbg_show = imx_pin_dbg_show,
	.dt_node_to_map = imx_dt_node_to_map,			//从 pinctrl_dev 的 info 获取 dt node 的 pinctrl_map 
	.dt_free_map = imx_dt_free_map,					// kfree(map)

};

const struct pinmux_ops *pmxops;				// = imx_pmx_ops

static const struct pinmux_ops imx_pmx_ops = {
	.get_functions_count = imx_pmx_get_funcs_count,	//从 pinctrl_dev 的 info 获取 function 数量
	.get_function_name = imx_pmx_get_func_name,		//从 pinctrl_dev 的 info 获取 function 名字
	.get_function_groups = imx_pmx_get_groups,		//从 pinctrl_dev 的 info 获取 group 和编号
	.set_mux = imx_pmx_set,							// 设置 mux_reg,input_reg
	.gpio_request_enable = imx_pmx_gpio_request_enable,		// 设置 mux_reg
	.gpio_set_direction = imx_pmx_gpio_set_direction,		// 设置 mux_reg
};

const struct pinconf_ops *confops;				// = imx_pinconf_ops

static const struct pinconf_ops imx_pinconf_ops = {
	.pin_config_get = imx_pinconf_get,			// 读 conf_reg
	.pin_config_set = imx_pinconf_set,			// 写 conf_reg
	.pin_config_dbg_show = imx_pinconf_dbg_show, // 打印 conf_reg
	.pin_config_group_dbg_show = imx_pinconf_group_dbg_show,	//打印 group 的 conf_reg
};


//  =============================================================
(Linux\drivers\pinctrl\core.c)
pinctrl_register
	pinctrl_register_pins(pctldev, pctldesc->pins, pctldesc->npins);	//注册后，返回 pintrl_dev

//  ==============================================================
imx_pinctrl_probe_dt(pdev, info);
	imx_pinctrl_parse_functions(child, info, i++);		//填充 info 中的 function 结构
		imx_pinctrl_parse_groups						//填充 info 中的 group 结构，pin 的 6个值

//  ==============================================================
//(Linux\drivers\pinctrl\freescale\pinctrl-imx.c)
int imx_pinctrl_probe(struct platform_device *pdev,struct imx_pinctrl_soc_info *info)
{				// arg[0]: pdev, arg[1]:imx6ul_pinctrl_info / imx6ull_snvs_pinctrl_info
	
	struct pinctrl_desc *imx_pinctrl_desc;
	struct imx_pinctrl *ipctl;
	
	
	// ------------------
	info->dev = &pdev->dev;

	info->pin_regs = devm_kmalloc(&pdev->dev, sizeof(*info->pin_regs) *
				      info->npins, GFP_KERNEL);
					  
	imx_pinctrl_probe_dt(pdev, info);		//初始化所有funciotn 和group 所有pin 6个值
					  
	// ------------------
	imx_pinctrl_desc = devm_kzalloc(&pdev->dev, sizeof(*imx_pinctrl_desc),
					GFP_KERNEL);

	imx_pinctrl_desc->name = dev_name(&pdev->dev);
	imx_pinctrl_desc->pins = info->pins;
	imx_pinctrl_desc->npins = info->npins;
	imx_pinctrl_desc->pctlops = &imx_pctrl_ops;
	imx_pinctrl_desc->pmxops = &imx_pmx_ops;
	imx_pinctrl_desc->confops = &imx_pinconf_ops;
	imx_pinctrl_desc->owner = THIS_MODULE;	
	
	// ------------------
	ipctl = devm_kzalloc(&pdev->dev, sizeof(*ipctl), GFP_KERNEL);
	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ipctl->base = devm_ioremap_resource(&pdev->dev, res);
	
	ipctl->input_sel_base = of_iomap(np, 0);
	
	ipctl->info = info;
	ipctl->dev = info->dev;
	platform_set_drvdata(pdev, ipctl);
	ipctl->pctl = pinctrl_register(imx_pinctrl_desc, &pdev->dev, ipctl);	//注册 pinctrl_dev
	
}


//  ============================================================== match->data
//(Linux\drivers\pinctrl\freescale\pinctrl-imx6ul.c)

static const struct pinctrl_pin_desc imx6ul_pinctrl_pads[] = {
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE0),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE1),
	,
}

static struct pinctrl_pin_desc imx6ull_snvs_pinctrl_pads[] = {
	IMX_PINCTRL_PIN(MX6ULL_PAD_BOOT_MODE0),
	IMX_PINCTRL_PIN(MX6ULL_PAD_BOOT_MODE1),
	,
};

static struct imx_pinctrl_soc_info imx6ul_pinctrl_info = {
	.pins = imx6ul_pinctrl_pads,
	.npins = ARRAY_SIZE(imx6ul_pinctrl_pads),
};

static struct imx_pinctrl_soc_info imx6ull_snvs_pinctrl_info = {
	.pins = imx6ull_snvs_pinctrl_pads,
	.npins = ARRAY_SIZE(imx6ull_snvs_pinctrl_pads),
	.flags = ZERO_OFFSET_VALID,
};

// ==================================================================
static struct of_device_id imx6ul_pinctrl_of_match[] = {
	{ .compatible = "fsl,imx6ul-iomuxc", .data = &imx6ul_pinctrl_info, },
	{ .compatible = "fsl,imx6ull-iomuxc-snvs", .data = &imx6ull_snvs_pinctrl_info, },
	{ /* sentinel */ }
};

static int imx6ul_pinctrl_probe(struct platform_device *pdev)
{
	match = of_match_device(imx6ul_pinctrl_of_match, &pdev->dev);		//获取匹配的 match_table
	pinctrl_info = (struct imx_pinctrl_soc_info *) match->data;			//获取 match->data
	imx_pinctrl_probe(pdev, pinctrl_info);								//调用 imx_pinctrl_probe	
		//(Linux\drivers\pinctrl\freescale\pinctrl-imx.c)
}

static struct platform_driver imx6ul_pinctrl_driver = {
	.driver = {
		.name = "imx6ul-pinctrl",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(imx6ul_pinctrl_of_match),
	},
	.probe = imx6ul_pinctrl_probe,
	.remove = imx_pinctrl_remove,
};

static int __init imx6ul_pinctrl_init(void)
{
	return platform_driver_register(&imx6ul_pinctrl_driver);
}
arch_initcall(imx6ul_pinctrl_init);

static void __exit imx6ul_pinctrl_exit(void)
{
	platform_driver_unregister(&imx6ul_pinctrl_driver);
}
module_exit(imx6ul_pinctrl_exit);

MODULE_AUTHOR("Anson Huang <Anson.Huang@freescale.com>");
MODULE_DESCRIPTION("Freescale imx6ul pinctrl driver");
MODULE_LICENSE("GPL v2");





