//=======================================
(Linux\arch\arm\kernel\setup.c)
setup_arch(&command_line);
	setup_processor()							//
		list = lookup_processor_type(read_cpuid_id());	//查找 processor_type,汇编代码，位于(Linux\arch\arm\kernel\head-common.S)
		cpu_name = list->cpu_name;	
		__cpu_architecture = __get_cpu_architecture();		
		processor = *list->proc
		cpu_tlb = *list->tlb
		cpu_user = *list->user;
		cpu_cache = *list->cache;				// 设置变量
		
		pr_info("CPU: %s [%08x] revision %d (ARMv%s), cr=%08lx\n",cpu_name, read_cpuid_id(), read_cpuid_id() & 15,proc_arch[cpu_architecture()], get_cr());
		elf_hwcap = list->elf_hwcap				// 设置变量，硬件特性：HWCAP_IDIVT，HWCAP_IDIVT，HWCAP_IDIVA，HWCAP_LPAE。HWCAP_THUMB | HWCAP_IDIVT，
		cpuid_init_hwcaps()						
			elf_hwcap2 |= HWCAP2_AES;			// 设置变量，硬件特性 HWCAP2_PMULL，HWCAP2_AES，HWCAP2_SHA1，HWCAP2_SHA2，HWCAP2_CRC32
		init_default_cache_policy(list->__cpu_mm_mmu_flags);
			cache_policies[i].pmd == pmd		// 设置 cache 策略
		erratum_a15_798181_init();				// 设置 a15 的 erratum
			erratum_a15_798181_handler = erratum_a15_798181_broadcast;	
		elf_hwcap_fixup();						// 设置 elf_hwcap
		cacheid_init();							// 设置全局变量 cacheid
		
		cpu_init();
			set_my_cpu_offset(per_cpu_offset(cpu));
			cpu_proc_init();
				proc_mkdir("cpu", NULL)
					proc_mkdir_data(name, 0, parent, NULL);
						ent = __proc_create(&parent, name, S_IFDIR | mode, 2);
						ent->data = data;
						ent->proc_fops = &proc_dir_operations;
						ent->proc_iops = &proc_dir_inode_operations;
						proc_register(parent, ent)
							proc_alloc_inum(&dp->low_ino);
							pde_subdir_insert(dir, dp)	// proc 下创建 cpu 文件
			__asm__ (
			"msr	cpsr_c, %1\n\t"
			"add	r14, %0, %2\n\t"
			"mov	sp, r14\n\t"
			"msr	cpsr_c, %3\n\t"
			"add	r14, %0, %4\n\t"
			"mov	sp, r14\n\t"
			"msr	cpsr_c, %5\n\t"
			"add	r14, %0, %6\n\t"
			"mov	sp, r14\n\t"
			"msr	cpsr_c, %7\n\t"
			"add	r14, %0, %8\n\t"
			"mov	sp, r14\n\t"
			"msr	cpsr_c, %9"
			:
			: "r" (stk),
			  PLC (PSR_F_BIT | PSR_I_BIT | IRQ_MODE),
			  "I" (offsetof(struct stack, irq[0])),
			  PLC (PSR_F_BIT | PSR_I_BIT | ABT_MODE),
			  "I" (offsetof(struct stack, abt[0])),
			  PLC (PSR_F_BIT | PSR_I_BIT | UND_MODE),
			  "I" (offsetof(struct stack, und[0])),
			  PLC (PSR_F_BIT | PSR_I_BIT | FIQ_MODE),
			  "I" (offsetof(struct stack, fiq[0])),
			  PLC (PSR_F_BIT | PSR_I_BIT | SVC_MODE)
			: "r14");									// 嵌入汇编，设置各个模式栈指针
			
	mdesc = setup_machine_fdt(__atags_pointer);	
		early_init_dt_verify(phys_to_virt(dt_phys)
			initial_boot_params = params;									// 设置 initial_boot_params，指向 dtb 的虚拟地址
		mdesc = of_flat_dt_match_machine(mdesc_best, arch_get_next_mach); 	//获取 匹配的 mdesc 指针赋值给全局变量
			
		dt_root = of_get_flat_dt_root();
		prop = of_get_flat_dt_prop(dt_root, "compatible", &size);			// 找到适配设备树的 mesc
			
			
		early_init_dt_scan_nodes();	
			of_scan_flat_dt(early_init_dt_scan_chosen, boot_command_line);		// 从 dt 的 / 的 bootargs 获取 boot_command_line
			of_scan_flat_dt(early_init_dt_scan_root, NULL);						// dt_root_size_cells，dt_root_addr_cells
			of_scan_flat_dt(early_init_dt_scan_memory, NULL);					// 从 dt 获取 mem 并添加
	
		__machine_arch_type = mdesc->nr;
		
	machine_desc = mdesc;
	machine_name = mdesc->name;							// 获取全局变量
		
	init_mm.start_code = (unsigned long) _text;			
	init_mm.end_code   = (unsigned long) _etext;
	init_mm.end_data   = (unsigned long) _edata;
	init_mm.brk	   = (unsigned long) _end;				// 设置结构体全局变量 (struct mm_struct)init_mm(mm/init-mm.c)
		
	parse_early_param();								// 分析 boot_command_line 
		
	early_paging_init(mdesc, lookup_processor_type(read_cpuid_id()));		// 调用 mdesc->init_meminfo() 初始化 mem
		
	setup_dma_zone(mdesc);								// 从 mdesc->dma_zone_size 设置 dma 地址
		
	sanity_check_meminfo();
		
	arm_memblock_init(mdesc);							// initrd 内存保留，platform memblock 保留(运行mdesc->reserve()), initial_boot_params 保留，dma 保留，
		arm_memblock_steal_permitted = false;				// 锁定 memblock
		memblock_dump_all();								// 	dump 保留
		
	paging_init(mdesc);									// 创建页表，创建bootmem(放stext，init代码的)，创建 DMA 页表，创建 devicemap (cache flush regions，创建中断向量页表，创建内核只读页表)	
		build_mem_type_table();							
		prepare_page_table();
		map_lowmem();
		dma_contiguous_remap();
		devicemaps_init(mdesc);
		kmap_init();									// early_pte_alloc()，创建 tcm 页表
		tcm_init();
		top_pmd = pmd_off_k(0xffff0000);
		zero_page = early_alloc(PAGE_SIZE);				// 设置 0 页表，
		bootmem_init();
		empty_zero_page = virt_to_page(zero_page);
		__flush_dcache_page(NULL, empty_zero_page);
		
	request_standard_resources(mdesc);					// 申请 kernel_code, kernel_data, lp0, lp1, lp2 内存，并添加到 iomem_resource,
		struct screen_info screen_info = {				// 如果 mdesc->video_start 存在，申请并添加到
		 .orig_video_lines	= 30,
		 .orig_video_cols	= 80,
		 .orig_video_mode	= 0,
		 .orig_video_ega_bx	= 0,
		 .orig_video_isVGA	= 1,
		 .orig_video_points	= 8
		};												// 申请 屏幕内存
		
	arm_pm_restart = mdesc->restart;
		
	unflatten_device_tree();							// 创建 device tree blob  (initial_boot_params)
		__unflatten_device_tree(initial_boot_params, &of_root, early_init_dt_alloc_memory_arch)		
														// 解析设备树(Linux\drivers\of\fdt.c)
			fdt_check_header(blob)
			size = (unsigned long)unflatten_dt_node(blob, NULL, &start, NULL, NULL, 0, true);	// 计算 fdt 大小
				// blob = initial_boot_params
				// mem = NULL
				// poffset = &start (0)
				// dad = NULL
				// nodepp = NULL
				// fpsize = 0
				// dryrun = ture
				pathp = fdt_get_name(blob, *poffset, &l);					// 获取根名字给返回值，名字长度给 l
				np = unflatten_dt_alloc(&mem, sizeof(struct device_node) + allocl,__alignof__(struct device_node));
																			// 分配一个 device_node 和 name 的内存
				for (offset = fdt_first_property_offset(blob, *poffset);(offset >= 0);
									(offset = fdt_next_property_offset(blob, offset)))
					p = fdt_getprop_by_offset(blob, offset, &pname, &sz)	// 获取属性值返回，赋值名字和大小
					pp = unflatten_dt_alloc(&mem, sizeof(struct property),__alignof__(struct property));
																			// 给每一个属性分配内存
																			
(Linux\drivers\of\fdt.c)
//  ======================
fdt_first_property_offset(blob, *poffset)
fdt_next_property_offset(blob, offset)



				device_node
				fullname
				property
				
//  ======================				
				
				
				
				
				
			mem = dt_alloc(size + 4, __alignof__(struct device_node));		// 分配 fdt 空间				
			*(__be32 *)(mem + size) = cpu_to_be32(0xdeadbeef);				// 末尾写入标记				
			unflatten_dt_node(blob, mem, &start, NULL, mynodes, 0, false);	// 解析设备树
			be32_to_cpup(mem + size) != 0xdeadbeef)							// 再次检查末尾标记	
			
		of_alias_scan(early_init_dt_alloc_memory_arch);	// 设置全局变量
			of_aliases = 
			of_chosen =
			of_stdout = 
		
	arm_dt_init_cpu_maps();								// 读取 dt 初始化 cpu_maps
	
	psci_init();	
		
	smp_set_ops(mdesc->smp);							// 设置 smp_ops 全局变量 = psci_smp_ops / mdesc->smp
	smp_init_cpus();									// 运行smp_ops.smp_init_cpus()
	smp_build_mpidr_hash();								// 创建 smp_mpidr hash 表
		
	if (!is_smp())
		hyp_mode_check();	
		
	reserve_crashkernel();								// 设置全局变量 crashk_res，并添加到 iomem_resource 中
	
	handle_arch_irq = mdesc->handle_irq;				// 设置 handle_arch_irq 全局变量
	
	conswitchp = &dummy_con;							// 设置全局变量， dummy_con 位于 driver/video/console/dummycon.c 中
		const struct consw dummy_con = {
			.owner =		THIS_MODULE,
			.con_startup =	dummycon_startup,
			.con_init =		dummycon_init,
			.con_deinit =	DUMMY,
			.con_clear =	DUMMY,
			.con_putc =		DUMMY,
			.con_putcs =	DUMMY,
			.con_cursor =	DUMMY,
			.con_scroll =	DUMMY,
			.con_bmove =	DUMMY,
			.con_switch =	DUMMY,
			.con_blank =	DUMMY,
			.con_font_set =	DUMMY,
			.con_font_get =	DUMMY,
			.con_font_default =	DUMMY,
			.con_font_copy =	DUMMY,
			.con_set_palette =	DUMMY,
			.con_scrolldelta =	DUMMY,
		};
		struct consw {
			struct module *owner;
			const char *(*con_startup)(void);
			void	(*con_init)(struct vc_data *, int);
			void	(*con_deinit)(struct vc_data *);
			void	(*con_clear)(struct vc_data *, int, int, int, int);
			void	(*con_putc)(struct vc_data *, int, int, int);
			void	(*con_putcs)(struct vc_data *, const unsigned short *, int, int, int);
			void	(*con_cursor)(struct vc_data *, int);
			int	(*con_scroll)(struct vc_data *, int, int, int, int);
			void	(*con_bmove)(struct vc_data *, int, int, int, int, int, int);
			int	(*con_switch)(struct vc_data *);
			int	(*con_blank)(struct vc_data *, int, int);
			int	(*con_font_set)(struct vc_data *, struct console_font *, unsigned);
			int	(*con_font_get)(struct vc_data *, struct console_font *);
			int	(*con_font_default)(struct vc_data *, struct console_font *, char *);
			int	(*con_font_copy)(struct vc_data *, int);
			int     (*con_resize)(struct vc_data *, unsigned int, unsigned int,
						   unsigned int);
			int	(*con_set_palette)(struct vc_data *, unsigned char *);
			int	(*con_scrolldelta)(struct vc_data *, int);
			int	(*con_set_origin)(struct vc_data *);
			void	(*con_save_screen)(struct vc_data *);
			u8	(*con_build_attr)(struct vc_data *, u8, u8, u8, u8, u8, u8);
			void	(*con_invert_region)(struct vc_data *, u16 *, int);
			u16    *(*con_screen_pos)(struct vc_data *, int);
			unsigned long (*con_getxy)(struct vc_data *, unsigned long, int *, int *);
			/*
			 * Prepare the console for the debugger.  This includes, but is not
			 * limited to, unblanking the console, loading an appropriate
			 * palette, and allowing debugger generated output.
			 */
			int	(*con_debug_enter)(struct vc_data *);
			/*
			 * Restore the console to its pre-debug state as closely as possible.
			 */
			int	(*con_debug_leave)(struct vc_data *);
		};

	mdesc->init_early();								// 执行 mdesc->init_early()
	
	
	
	
	
	
// ======================================== 初始化 device	
	
start_kernel----rest_init----kernel_init----kernel_init_freeable----do_basic_setup----do_initcalls
	
