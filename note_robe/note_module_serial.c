
// ========================================= probe
static int serial_imx_probe(struct platform_device *pdev)
	struct imx_port *sport;
	ret = serial_imx_probe_dt(sport, pdev);		// 从设备树获取 gpio 信息，并设置
												// 设置 sport.devdata = 
												// [IMX6Q_UART] = {
												//		.uts_reg = IMX21_UTS,
												//		.devtype = IMX6Q_UART,}
		
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(base))
		return PTR_ERR(base);

	rxirq = platform_get_irq(pdev, 0);
	txirq = platform_get_irq(pdev, 1);
	rtsirq = platform_get_irq(pdev, 2);

	sport->port.dev = &pdev->dev;
	sport->port.mapbase = res->start;
	sport->port.membase = base;
	sport->port.type = PORT_IMX,
	sport->port.iotype = UPIO_MEM;
	sport->port.irq = rxirq;
	sport->port.fifosize = 32;
	sport->port.ops = &imx_pops;
	sport->port.rs485_config = imx_rs485_config;
	sport->port.rs485.flags =
		SER_RS485_RTS_ON_SEND | SER_RS485_RX_DURING_TX;
	sport->port.flags = UPF_BOOT_AUTOCONF;
	init_timer(&sport->timer);
	sport->timer.function = imx_timeout;
	sport->timer.data     = (unsigned long)sport;

	sport->clk_ipg = devm_clk_get(&pdev->dev, "ipg");
	sport->clk_per = devm_clk_get(&pdev->dev, "per");
	
	sport->port.uartclk = clk_get_rate(sport->clk_per);
	
	devm_request_irq(&pdev->dev, rxirq, imx_rxint, 0,
				       dev_name(&pdev->dev), sport);
	devm_request_irq(&pdev->dev, txirq, imx_txint, 0,
				       dev_name(&pdev->dev), sport);				   
					   
	platform_set_drvdata(pdev, sport);				   
		
	uart_add_one_port(&imx_reg, &sport->port);
	


// ========================================= module_init
#define DRIVER_NAME "IMX-uart"
#define DEV_NAME		"ttymxc"
#define SERIAL_IMX_MAJOR	207
#define MINOR_START		16
#define IMX_CONSOLE	&imx_console

static struct uart_driver imx_reg = {
	.owner          = THIS_MODULE,
	.driver_name    = DRIVER_NAME,				// "IMX-uart"
	.dev_name       = DEV_NAME,					// "ttymxc"
	.major          = SERIAL_IMX_MAJOR,			// 207
	.minor          = MINOR_START,				// 16
	.nr             = ARRAY_SIZE(imx_ports),	
	.cons           = IMX_CONSOLE,				// &imx_console
};

static struct console imx_console = {
	.name		= DEV_NAME,						// "ttymxc"
	.write		= imx_console_write,
	.device		= uart_console_device,
	.setup		= imx_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &imx_reg,
};

static struct uart_ops imx_pops = {
	.tx_empty	= imx_tx_empty,
	.set_mctrl	= imx_set_mctrl,
	.get_mctrl	= imx_get_mctrl,
	.stop_tx	= imx_stop_tx,
	.start_tx	= imx_start_tx,
	.stop_rx	= imx_stop_rx,
	.enable_ms	= imx_enable_ms,
	.break_ctl	= imx_break_ctl,
	.startup	= imx_startup,
	.shutdown	= imx_shutdown,
	.flush_buffer	= imx_flush_buffer,
	.set_termios	= imx_set_termios,
	.type		= imx_type,
	.config_port	= imx_config_port,
	.verify_port	= imx_verify_port,
#if defined(CONFIG_CONSOLE_POLL)
	.poll_init      = imx_poll_init,
	.poll_get_char  = imx_poll_get_char,
	.poll_put_char  = imx_poll_put_char,
#endif
};

static struct imx_port *imx_ports[UART_NR];

static const struct tty_operations uart_ops = {
	.open		= uart_open,
	.close		= uart_close,
	.write		= uart_write,
	.put_char	= uart_put_char,
	.flush_chars	= uart_flush_chars,
	.write_room	= uart_write_room,
	.chars_in_buffer= uart_chars_in_buffer,
	.flush_buffer	= uart_flush_buffer,
	.ioctl		= uart_ioctl,
	.throttle	= uart_throttle,
	.unthrottle	= uart_unthrottle,
	.send_xchar	= uart_send_xchar,
	.set_termios	= uart_set_termios,
	.set_ldisc	= uart_set_ldisc,
	.stop		= uart_stop,
	.start		= uart_start,
	.hangup		= uart_hangup,
	.break_ctl	= uart_break_ctl,
	.wait_until_sent= uart_wait_until_sent,
#ifdef CONFIG_PROC_FS
	.proc_fops	= &uart_proc_fops,
#endif
	.tiocmget	= uart_tiocmget,
	.tiocmset	= uart_tiocmset,
	.get_icount	= uart_get_icount,
#ifdef CONFIG_CONSOLE_POLL
	.poll_init	= uart_poll_init,
	.poll_get_char	= uart_poll_get_char,
	.poll_put_char	= uart_poll_put_char,
#endif
};

static const struct tty_port_operations uart_port_ops = {
	.activate	= uart_port_activate,
	.shutdown	= uart_port_shutdown,
	.carrier_raised = uart_carrier_raised,
	.dtr_rts	= uart_dtr_rts,
};

uart_register_driver(&imx_reg);
	drv->state = kzalloc(sizeof(struct uart_state) * drv->nr, GFP_KERNEL);		
													// imx_reg->state 分配空间
	normal = alloc_tty_driver(drv->nr);	
	drv->tty_driver = normal;

	normal->driver_name	= drv->driver_name;
	normal->name		= drv->dev_name;
	normal->major		= drv->major;
	normal->minor_start	= drv->minor;
	normal->type		= TTY_DRIVER_TYPE_SERIAL;
	normal->subtype		= SERIAL_TYPE_NORMAL;
	normal->init_termios	= tty_std_termios;
	normal->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	normal->init_termios.c_ispeed = normal->init_termios.c_ospeed = 9600;
	normal->flags		= TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
	normal->driver_state    = drv;
	tty_set_operations(normal, &uart_ops);			// imx_reg->tty_driver 分配空间，初始化，赋值 ops = &uart_ops

	tty_port_init(port);
	port->ops = &uart_port_ops;						// imx_reg->state->port 初始化，并设置.port.ops = &uart_port_ops;
	
	tty_register_driver(normal);					// 注册 imx_reg->tty_driver (添加到全局链表头tty_drivers)
	
	
platform_driver_register(&serial_imx_driver);		// 注册 serial_imx_driver
	

// ========================================== of_match_table / (platform_device_id) imx_uart_devtype
static struct imx_uart_data imx_uart_devdata[] = {
	[IMX1_UART] = {
		.uts_reg = IMX1_UTS,
		.devtype = IMX1_UART,
	},
	[IMX21_UART] = {
		.uts_reg = IMX21_UTS,
		.devtype = IMX21_UART,
	},
	[IMX6Q_UART] = {
		.uts_reg = IMX21_UTS,
		.devtype = IMX6Q_UART,
	},
};

static struct platform_device_id imx_uart_devtype[] = {
	{
		.name = "imx1-uart",
		.driver_data = (kernel_ulong_t) &imx_uart_devdata[IMX1_UART],
	}, {
		.name = "imx21-uart",
		.driver_data = (kernel_ulong_t) &imx_uart_devdata[IMX21_UART],
	}, {
		.name = "imx6q-uart",
		.driver_data = (kernel_ulong_t) &imx_uart_devdata[IMX6Q_UART],
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(platform, imx_uart_devtype);

static const struct of_device_id imx_uart_dt_ids[] = {
	{ .compatible = "fsl,imx6q-uart", .data = &imx_uart_devdata[IMX6Q_UART], },
	{ .compatible = "fsl,imx1-uart", .data = &imx_uart_devdata[IMX1_UART], },
	{ .compatible = "fsl,imx21-uart", .data = &imx_uart_devdata[IMX21_UART], },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx_uart_dt_ids);

// ========================================== module
static struct platform_driver serial_imx_driver = {
	.probe		= serial_imx_probe,
	.remove		= serial_imx_remove,

	.suspend	= serial_imx_suspend,
	.resume		= serial_imx_resume,
	.id_table	= imx_uart_devtype,
	.driver		= {
		.name	= "imx-uart",
		.of_match_table = imx_uart_dt_ids,
	},
};

static int __init imx_serial_init(void)
{
	int ret = uart_register_driver(&imx_reg);

	if (ret)
		return ret;

	ret = platform_driver_register(&serial_imx_driver);
	if (ret != 0)
		uart_unregister_driver(&imx_reg);

	return ret;
}

static void __exit imx_serial_exit(void)
{
	platform_driver_unregister(&serial_imx_driver);
	uart_unregister_driver(&imx_reg);
}

module_init(imx_serial_init);
module_exit(imx_serial_exit);


