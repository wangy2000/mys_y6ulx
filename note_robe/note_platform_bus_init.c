
// 内核 platform_bus_init 
kernel
driver_init
	platform_bus_init
		early_platform_cleanup		//清除 early_platform_device_list
		device_register(&platform_bus);
/* 			调用 device_add($platform_bus),初始化device.device_private
			struct device platform_bus = {
				.init_name	= "platform",
			}; */
		bus_register(&platform_bus_type);
		of_platform_register_reconfig_notifier()
		
		
		
		
device_add(struct device *dev)
	get_device(dev);
	device_private_init		//初始化device.device_private成员
	device_create_file
	device_add_class_symlinks
	device_add_attrs
	bus_add_device
	dpm_sysfs_add
	device_pm_add
	device_create_file
	device_create_sys_dev_entry
	devtmpfs_create_node
	blocking_notifier_call_chain
	kobject_uevent
	bus_probe_device




