u-boot.lds
(arch/arm/lib/vectors.S)
								.globl 	_start
	b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq
								.globl	_undefined_instruction
								.globl	_software_interrupt
								.globl	_prefetch_abort
								.globl	_data_abort
								.globl	_not_used
								.globl	_irq
								.globl	_fiq
								
								_undefined_instruction:	.word undefined_instruction
								_software_interrupt:	.word software_interrupt
								_prefetch_abort:	.word prefetch_abort
								_data_abort:		.word data_abort
								_not_used:		.word not_used
								_irq:			.word irq
								_fiq:			.word fiq
								
								.globl IRQ_STACK_START_IN
	IRQ_STACK_START_IN:
								.globl IRQ_STACK_START
	IRQ_STACK_START:
								.globl FIQ_STACK_START
	FIQ_STACK_START:
	
	中断处理函数：				寄存器压栈，处理结束，弹栈
		do_undefined_instruction
		do_software_interrupt
		do_prefetch_abort
		do_data_abort
		do_not_used
		do_irq
		do_fiq					(arch/arm/lib/interrupts.c)
	
(arch/arm/cpu/armv7/start.S)
	.reset
	.save_boot_params_ret
	reset:						
		save_boot_params_ret:	关闭FIQ/IRQ, 设置SVC32，或者进HYP模式
								设置向量表_start(定义CONFIG_SPL_BUILD)
		cpu_init_cp15:			关mmu，关caches，开i-cache
		cpu_init_crit:
			lowlevel_init：		设置sp指针=CONFIG_SYS_INIT_SP_ADDR，
								清零r9,=0(定义CONFIG_SPL_DM)
								设置r9,=gdata(定义CONFIG_SPL_BUILD)
								设置r9,放到 sp 空间上面
				 s_init:		设置PLL
		_main：
(arch/arm/lib/crt0.S:)_main
	ldr	sp, =(CONFIG_SYS_INIT_SP_ADDR):		设置sp
		#define CONFIG_SYS_INIT_SP_ADDR (CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)
		#define CONFIG_SYS_INIT_RAM_ADDR IRAM_BASE_ADDR
		#define IRAM_BASE_ADDR 0x00900000
		#define CONFIG_SYS_INIT_SP_OFFSET (CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
		#define CONFIG_SYS_INIT_RAM_SIZE IRAM_SIZE
		#define IRAM_SIZE 0x00020000
	board_init_f_alloc_reserve：
		(common/init/board_init.c:)：		保留1k空间
											设置r9
	board_init_f_init_reserve：
		(common/init/board_init.c:)：		设置gd->malloc_base
	board_init_f：
		(common/board_f.c:):				清零gd空间，设置gd->flags=0,gd->have_console=0
		initcall_run_list:
			init_sequence_f:
(common/board_f.c)init_sequence_f:
	setup_mon_len:							设置gd->mon_len = (ulong)&__bss_end - (ulong)_start;
	initf_malloc：							设置gd->malloc_ptr = 0;设置gd->malloc_limit = CONFIG_SYS_MALLOC_F_LEN;
	initf_console_record：					空
	arch_cpu_init：							(arch/arm/cpu/armv7/mx6)
		init_aips:
		clear_mmdc_ch_mask:
		init_src:
	initf_dm：
		dm_init_and_scan:							
			dm_init:							gd->dm_root存在，返回。不存在初始化gd->uclass_root
				device_bind_by_name：
				device_probe：
			dm_scan_platdata：
			dm_scan_other：
	arch_cpu_init_dm：						空
	mark_bootstage:							添加record记录(static struct bootstage_record record[BOOTSTAGE_ID_COUNT] = { {1} };)
	board_early_init_f:						(board/myir/mys_imx6ull/mys_imx6ull.c)	//修改1
		setup_iomux_uart:					设置串口
	timer_init：							(arch/arm/imx-common/syscounter.c)
											初始化启动syscounter，清零gd->arch.tbl，gd->arch.tbu
	board_postclk_init：
		set_ldo_voltage：
	get_clocks：							设置gd->arch.sdhc_clk
	env_init：								设置gd->env_addr=default_environment[]地址，(include/env_default.h)
											设置gd->env_valid=1
	init_baud_rate:							设置gd->baudrate
	serial_init：							设置gd->flags |= GD_FLG_SERIAL_READY;
		get_current:
			default_serial_console：
				mxc_serial_drv：
					struct serial_device mxc_serial_drv={
						.name	= "mxc_serial",
						.start	= mxc_serial_init,
						.stop	= NULL,
						.setbrg	= mxc_serial_setbrg,
						.putc	= mxc_serial_putc,
						.puts	= default_serial_puts,
						.getc	= mxc_serial_getc,
						.tstc	= mxc_serial_tstc,
					}
		get_current()->start():				初始化串口，并使能。(drivers/serial/serial_mxc.c)
	console_init_f:							设置gd->have_console=1,初始化console
	display_options：						[打印]u-boot第一次开始输出信息，输出版本信息															
	display_text_info:
	print_cpuinfo：							[打印]cpu信息，reset cause：por 信息
	show_board_info：						[打印]board信息，(board/myir/mys_imx6ull/mys_imx6ull.c)
	announce_dram_init：					puts打印信息
	dram_init：
		get_ram_size：						获取实际ram size，赋值给gd->ram_size
	setup_dest_addr:						重新设置内存分配内存
											设置gd->relocaddr = gd->ram_top
	reserve_round_4k：						设置gd->relocaddr 4k对齐
	reserve_mmu：							设置gd->arch.tlb_size，gd->arch.tlb_addr
	reserve_trace：							空
	reserve_uboot：							预留gd->mon_len
	reserve_malloc：						预留TOTAL_MALLOC_LEN
	reserve_board：							预留gd->bd，设置gd->bd
	setup_machine：							设置gd->bd->bi_arch_number
	reserve_global_data：					预留gd->new_gd空间，设置gd->new_gd
	reserve_fdt：							预留gd->new_fdt，设置gd->new_fdt
	arch_reserve_stacks：					预留gd->irq_sp，设置gd->irq_sp
	dram_init_banksize：					设置gd->bd->bi_dram[0].start，
											设置gd->bd->bi_dram[0].size
	show_dram_config：						[打印]dram信息
	display_new_sp:							[打印]gd->start_addr_sp信息(已设置gd->start_addr_sp)
	setup_board_extra：						设置gd->bd->bi_s_version，
											设置gd->bd->bi_r_version，
											设置gd->bd->bi_procfreq = gd->cpu_clk
											设置gd->bd->bi_plb_busfreq = gd->bus_clk
	reloc_fdt：								复制memcpy(gd->new_fdt, gd->fdt_blob, gd->fdt_size);
											设置gd->fdt_blob = gd->new_fdt;
	setup_reloc：							复制memcpy(gd->new_gd, (char *)gd, sizeof(gd_t));
	
(arch/arm/lib/crt0.S:)_main	
	sp = gd->start_addr_sp
	r9 = new GD
	设置返回寄存器
	r0 = gd->relocaddr
	b relocate_code
(arch/arm/lib/relocate.S:)relocate_code	
	复制 __image_copy_start 和 __image_copy_end 之间代码
(arch/arm/lib/crt0.S:)_main		
	bl	relocate_vectors
(arch/arm/lib/relocate.S:)relocate_vectors
	r0 = gd->relocaddr
	复制代码
(arch/arm/lib/crt0.S:)_main
	bl	c_runtime_cpu_setup：关闭I-cache
	清空bss
	bl coloured_LED_init
	bl red_led_on
	mov     r0, r9					/* gd_t */
	ldr	r1, [r9, #GD_RELOCADDR]		/* dest_addr */
	ldr	pc, =board_init_r
(common/baord_r.c:)board_init_r	
	initcall_run_list:
		init_sequence_r
init_sequence_r:
	initr_reloc:							设置gd->flags
		gd->flags |= GD_FLG_RELOC | GD_FLG_FULL_MALLOC_INIT;
	initr_caches:							使能D-cache
	initr_reloc_global_data:				设置monitor_flash_len
		monitor_flash_len = _end - __image_copy_start;
	initr_barrier：							空
	initr_malloc：							设置malloc_start，并初始化
		malloc_start = gd->relocaddr - TOTAL_MALLOC_LEN;
	initr_console_record：					空
	bootstage_relocate：					复制record
	initr_dm：
		gd->dm_root_f = gd->dm_root;
		gd->dm_root = NULL;
	dm_init：
		device_bind_by_name：				初始化gd->dm_root，(udevice)
	initr_bootstage：						空
	board_init：										//修改2
		gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;	设置变量
		imx_iomux_v3_setup_multiple_pads				初始化pads(gpio5:7/8/10/11) SNVS_TAMPER7/8 和 bootmode
		iox74lv_init									检查boot mode
		setup_i2c										设置pad，设置复用，使能clk
		setup_fec
		setup_usb
		board_qspi_init
		setup_gpmi_nand
	stdio_init_tables:						初始化struct stdio_dev的list变量
	initr_serial,
		serial_initialize					初始化所有串口
	initr_announce,							打印信息
	power_init_board：						空
	initr_nand								空
	initr_onenand							空
	initr_mmc
		mmc_initialize
			INIT_LIST_HEAD (&mmc_devices);	初始化mmc_devices链表头
			mmc_probe						probe/初始化mmc结构体
			do_preinit						初始化mmc卡/按照sd2.0标准 初始化后设置标志		
	initr_dataflash							空
	initr_env
		env_relocate						设置env，从emmc分配空间
		load_addr = getenv_ulong("loadaddr", 16, load_addr);		设置load_addr
	initr_malloc_bootparams					空
	initr_secondary_cpu						空
	mac_read_from_eeprom					空
	stdio_add_devices,
		drv_video_init
			board_video_skip				找到displays，配置lcdif pad.
			video_init
				mxs_lcd_init				设置lcdc
			stdio_register(&console_dev)	注册console_dev设备
		drv_system_init						注册"serial"设备
		serial_stdio_init					注册serial_device设备
		drv_jtag_console_init				注册"jtag"设备
	initr_jumptable,						给gd->jt分配堆空间
	console_init_r
		console_setfile						设置stdio_devices[file] = dev;初始化gd->jf->getc/putc
		setenv(stdio_names[i], stdio_devices[i]->name)			设置stdio_devices名字
	interrupt_init							中断初始化,设置堆栈指针 (中断处理实现在arch/arm/lib/interrupts.c)
		IRQ_STACK_START = gd->irq_sp - 4;
		IRQ_STACK_START_IN = gd->irq_sp + 8;
		FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;
	initr_enable_interrupts					开中断
	initr_ethaddr							获取环境变量ip地址，验证有效性
	board_late_init							增加sd/qspi启动模式，//修改3
		setenv("board_name", "MYS6ULL");	设置环境变量
		setenv("board_rev", "14X14");
		board_late_mmc_env_init				检测mmc是否启动设备
			mmc_get_env_dev					获取mmc启动设备
			getenv("mmcautodetect");		获取mmcautodetect环境变量，并判断是否等于yes，决定是否mmc启动
			strcmp(autodetect_str, "yes")
											设置环境变量mmcdev/mmcroot
			run_command(cmd, 0);			执行cmd启动
				sprintf(cmd, "mmc dev %d", dev_no);
		set_wdog_reset						使能看门狗
	initr_net								初始化，或者reset eth控制器							
	initr_check_fastboot					是否需要从 fastboot 启动	
	run_main_loop							主循环
		main_loop							陷入死循环执行 main_loop 中
(common/main.c:)main_loop
	cli_init								cli环境初始化
	run_preboot_environment_command			获取preboot 环境变量，并执行
	s = bootdelay_process();
		bootdelay_process
			s = getenv("bootdelay");		获取bootdelay环境变量，保存到stored_bootdelsy
			stored_bootdelsy = s			
			retrun s;s = getenv("bootcmd");	返回bootcmd指令
	if (cli_process_fdt(&s))				查找设备树是否有bootcmd，有的话覆盖环境变量bootcmd，并执行bootcmd，然后hang
		cli_secure_boot_cmd(s);				(如果fdt设置了secureboot,就从fdt，bootcmd启动。)
	autoboot_command(s);					如果在bootdelay时间内按了按键，不执行bootcmd，不然直接启动
		run_command_list(s, -1, 0);			启动前后关闭恢复disable_ctrlc 功能
	cli_loop();
(common/cli.c:)run_command_list(s, -1, 0)
	parse_string_outer(buff, FLAG_PARSE_SEMICOLON);
		=parse_string_outer("bootcmd", (1<<1));
(common/cli_hush.c:)parse_string_outer
	parse_stream_outer
		run_list
			run_list_real
				run_pipe_real
					cmd_process
						cmd_call
							(cmdtp->cmd)(cmdtp, flag, argc, argv);
(cmd/bootm.c)do_bootz
	bootz_start
	do_bootm_states
		bootm_start
		bootm_find_os
		bootm_find_other
		bootm_disable_interrupts
		bootm_load_os
		lmb_reserve
		bootm_os_get_boot_func
		boot_selected_os
			arch_preboot_os
			boot_fn
				do_bootm_linux
					boot_prep_linux
					boot_jump_linux
						kernel_entry(0, machid, r2);
						
/* kernel_entry */
void (*kernel_entry)(int zero, int arch, uint params);

/* cmd_tbl_s / cmd_tbl_t 结构体 */
struct cmd_tbl_s {
	char		*name;		/* Command Name			*/
	int		maxargs;	/* maximum number of arguments	*/
	int		repeatable;	/* autorepeat allowed?		*/
					/* Implementation function	*/
	int		(*cmd)(struct cmd_tbl_s *, int, int, char * const []);
	char		*usage;		/* Usage message	(short)	*/
#ifdef	CONFIG_SYS_LONGHELP
	char		*help;		/* Help  message	(long)	*/
#endif
#ifdef CONFIG_AUTO_COMPLETE
	/* do auto completion on the arguments */
	int		(*complete)(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]);
#endif
};

/* U_BOOT_CMD 宏定义解析 */
U_BOOT_CMD(_name, _maxargs, _rep, _cmd, _usage, _help)
U_BOOT_CMD_COMPLETE(_name, _maxargs, _rep, _cmd, _usage, _help, NULL)
ll_entry_declare(cmd_tbl_t, _name, cmd) =
	U_BOOT_CMD_MKENT_COMPLETE(_name, _maxargs, _rep, _cmd,_usage, _help, _comp);
ll_entry_declare(cmd_tbl_t, _name, cmd) =
	{ #_name, _maxargs, _rep, _cmd, _usage,_CMD_HELP(_help) _CMD_COMPLETE(_comp) }
//U_BOOT_CMD 宏解析结果，两种情况
ll_entry_declare(cmd_tbl_t, _name, cmd) =
	{ #_name, _maxargs, _rep, _cmd, _usage,_help,_comp}
ll_entry_declare(cmd_tbl_t, _name, cmd) =
	{ #_name, _maxargs, _rep, _cmd, _usage, }

/* env */ 	
baudrate=115200
board_name=MYS6ULL
board_rev=14X14
boot_fdt=try
bootargs=console=ttymxc0,115200 root=/dev/nfs ip=dhcp nfsroot=:,v3,tcp
bootcmd=run findfdt;mmc dev ${mmcdev};mmc dev ${mmcdev}; if mmc rescan; then if run loadbootscript; then run bootscript; else if run loadimage; then run mmcboot; else run netboot; fi; fi; else run netboot; fi
bootcmd_mfg=run mfgtool_args;bootz ${loadaddr} ${initrd_addr} ${fdt_addr};
bootdelay=3
bootscript=echo Running bootscript from mmc ...; source
console=ttymxc0
ethact=FEC0
ethprime=FEC
fdt_addr=0x83000000
fdt_file=mys-imx6ull-14x14.dtb
fdt_high=0xffffffff
findfdt=if test $fdt_file = undefined; then if test $board_name = EVK && test $board_rev = 9X9; then setenv fdt_file imx6ull-9x9-evk.dtb; fi; if test $board_name = MYS6ULL && test $board_rev = 14X14; then setenv fdt_file mys-imx6ull-14x14.dtb; fi; if test $fdt_file = undefined; then echo WARNING: Could not determine dtb to use; fi; fi;
get_cmd=dhcp
image=zImage
initrd_addr=0x83800000
initrd_high=0xffffffff
ip_dyn=yes
loadaddr=0x80800000
loadbootscript=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};
loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}
loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}
mfgtool_args=setenv bootargs console=${console},${baudrate} rdinit=/linuxrc g_mass_storage.stall=0 g_mass_storage.removable=1 g_mass_storage.file=/fat g_mass_storage.ro=1 g_mass_storage.idVendor=0x066F g_mass_storage.idProduct=0x37FF g_mass_storage.iSerialNumber="" clk_ignore_unused 
mmcargs=setenv bootargs console=${console},${baudrate} root=${mmcroot}
mmcautodetect=yes
mmcboot=echo Booting from mmc ...; run mmcargs; if test ${boot_fdt} = yes || test ${boot_fdt} = try; then if run loadfdt; then bootz ${loadaddr} - ${fdt_addr}; else if test ${boot_fdt} = try; then bootz; else echo WARN: Cannot load the DT; fi; fi; else bootz; fi;
mmcdev=0
mmcpart=1
mmcroot=/dev/mmcblk0p2 rootwait rw
netargs=setenv bootargs console=${console},${baudrate} root=/dev/nfs ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp
netboot=echo Booting from net ...; run netargs; if test ${ip_dyn} = yes; then setenv get_cmd dhcp; else setenv get_cmd tftp; fi; ${get_cmd} ${image}; if test ${boot_fdt} = yes || test ${boot_fdt} = try; then if ${get_cmd} ${fdt_addr} ${fdt_file}; then bootz ${loadaddr} - ${fdt_addr}; else if test ${boot_fdt} = try; then bootz; else echo WARN: Cannot load the DT; fi; fi; else bootz; fi;
panel=LCD050_800x480_Robe
script=boot.scr
	
/* bootcmd */		/* 先从bootscript启动，再mmc启动(加载zImage fdt，bootz启动)，其次net启动 */
bootcmd=
run findfdt;
mmc dev ${mmcdev};
mmc dev ${mmcdev};
if mmc rescan; then	
	if run loadbootscript; then	run bootscript;
	else if run loadimage; then	run mmcboot; 
		else run netboot;
		fi;
	fi; 
else run netboot;
fi;

/* findfdt */ 
findfdt=
if test $fdt_file = undefined; then 
	if test $board_name = EVK && test $board_rev = 9X9; then setenv fdt_file imx6ull-9x9-evk.dtb; 
	fi; 
	if test $board_name = MYS6ULL && test $board_rev = 14X14; then setenv fdt_file mys-imx6ull-14x14.dtb; 
	fi; 
	if test $fdt_file = undefined; then echo WARNING: Could not determine dtb to use; 
	fi; 
fi;	
	
	
	
	
	
	
	
	