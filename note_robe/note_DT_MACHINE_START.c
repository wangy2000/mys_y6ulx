


DT_MACHINE_START(IMX6UL, "Freescale i.MX6 Ultralite (Device Tree)")
	.map_io		= imx6ul_map_io,
	.init_irq	= imx6ul_init_irq,
	.init_machine	= imx6ul_init_machine,
	.init_late	= imx6ul_init_late,
	.dt_compat	= imx6ul_dt_compat,
MACHINE_END

static const char *imx6ul_dt_compat[] __initconst = {
	"fsl,imx6ul",
	"fsl,imx6ull",
	NULL,
};

 * <mux_reg conf_reg input_reg mux_mode input_val>
#define MX6ULL_PAD_SNVS_TAMPER0__GPIO5_IO00                        0x0008 0x004C 0x0000 0x5 0x0


imx6ul_map_io
	imx6_pm_map_io					//pm_iotable初始化，mapping 到 IRAM
	imx_busfreq_map_io				//ddr频率变化代码，mapping 到 IRAM

imx6ul_init_irq
	imx_gpc_check_dt				//从 DTS 获取并 赋值 gpc_base 基地址						电压，频率控制
	imx_init_revision_from_anatop	//从 DTS 获取并 设置 __mxc_cpu_type，imx_soc_revision		cpu_type，soc_revision
	imx_src_init					//从 DTS 获取并 设备 src_base, imx_reset_controller			注册 reset control
	irqchip_init					//从 DTS 获取并 初始化 irqchip 
		of_irq_init(__irqchip_of_table);	//执行 match.data() 函数初始化 irqchip
		acpi_irq_init();

imx6ul_init_machine
	imx_soc_device_init				//soc device注册
	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL); // platform_bus_create 创建树
		//  匹配三种总线：simple-bus, simple-mfd, arm,amba-bus
	imx6ul_enet_init();				//网络初始化
	imx_anatop_init();				//电源初始化
	imx6ul_pm_init();				//DDR电源初始化，console_base 内存映射
	
imx6ul_init_late
	imx6ul_opp_init					// cpu 动态电压+频率调节 初始化
	platform_device_register_simple	// cpu 频率调节 设备注册 
	imx6ul_cpuidle_init				// cpu idle 驱动注册
	
imx6ul_dt_compat					//匹配列表
	static const char *imx6ul_dt_compat[] __initconst = {
	"fsl,imx6ul",
	"fsl,imx6ull",
	NULL,
};
	
	
	
// of_root 全局指针，strcut device_node* 类型的

// gpc_base 全集指针，指向 gpc 基地址

// __mxc_cpu_type 全局变量，unsigned int

// imx_soc_revision	局部指针

// src_base 局部指针
// imx_reset_controller 局部变量

// console_base



