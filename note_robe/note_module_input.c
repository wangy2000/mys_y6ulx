










static const struct of_device_id gpio_keys_of_match[] = {
	{ .compatible = "gpio-keys", },
	{ },
};
MODULE_DEVICE_TABLE(of, gpio_keys_of_match);

gpio_keys_probe
	gpio_keys_get_devtree_pdata(struct device)
		//读取device_node初始化gpio_keys_platform_data结构体和gpio_keys_button成员变量
		of_get_gpio_flags(pp, 0, &flags); 	//======================================
		irq_of_parse_and_map(pp, 0);		//======================================
	
	devm_input_allocate_device((struct device *)dev);
		//申请input_devres空间并init，申请inpput_dev空间并init。
		//input_dev->dev.parent 放到 dev 中， devres 放到 dev 中
		//input_dev->dev.parent = dev;  list_add_tail(devres->node->entry, &dev->devres_head);
		devres_alloc(devm_input_device_release,sizeof(struct input_devres), GFP_KERNEL);
			//input_dev->dev.parent = dev;  list_add_tail(devres->node->entry, &dev->devres_head);
			//kmalloc_track_caller(sizeof(struct devres) + sizeof(struct input_devres), gfp);
	
	platform_set_drvdata(pdev, ddata);	//pdev->dev->driver_data
	input_set_drvdata(input, ddata);	//input->dev->driver_data
		
	input->open = gpio_keys_open;
		//报告状态
		gpio_keys_report_state
			gpio_keys_gpio_report_event
			
	input->close = gpio_keys_close;
	
	
	gpio_keys_setup_key (pdev, input, (struct gpio_button_data *)bdata, (struct gpio_keys_button *)button);
		//一下二选一
		INIT_DELAYED_WORK(&bdata->work, gpio_keys_gpio_work_func);
		isr = gpio_keys_gpio_isr;
		//button->gpio 有效就初始化GPIO，设置防抖时间，设置irq，设置工作队列
		//1
		setup_timer(&bdata->release_timer,gpio_keys_irq_timer, (unsigned long)bdata);
		isr = gpio_keys_irq_isr;
		//irq有效才可以工作，设置防抖用作timer延时，设置irq
		//2
		devm_add_action(&pdev->dev, gpio_keys_quiesce_key, bdata);
		devm_request_any_context_irq(&pdev->dev, bdata->irq,isr, irqflags, desc, bdata);
	
	sysfs_create_group
	
	input_register_device
	//注册input设备
	
	device_init_wakeup
	
gpio_keys_remove

gpio_keys_suspend
gpio_keys_resume

static SIMPLE_DEV_PM_OPS(gpio_keys_pm_ops, gpio_keys_suspend, gpio_keys_resume);

static struct platform_driver gpio_keys_device_driver = {
	.probe		= gpio_keys_probe,
	.remove		= gpio_keys_remove,
	.driver		= {
		.name	= "gpio-keys",
		.pm	= &gpio_keys_pm_ops,
		.of_match_table = of_match_ptr(gpio_keys_of_match),
	}
};


gpio_keys_init
gpio_keys_exit

late_initcall(gpio_keys_init);
module_exit(gpio_keys_exit);

//=============================================================




