
gpio 操作相关函数：
	gpio 申请		gpio_request		gpio_request(gpio,"gpio_name")
	gpio 初始化		gpio_direction_out	gpio_direction_out(gpio,1)
	gpio 输出电平	gpio_set_value		gpio_set_value(gpio,1)
	gpio 释放:		gpio_free

头文件：linux/gpio.h
	
gpio 是如何编址的：

