// ==================================================
(arch/arm/kernel/head.S):
	#define KERNEL_RAM_VADDR	(PAGE_OFFSET + TEXT_OFFSET)
	#define PG_DIR_SIZE	0x4000
	#define PMD_ORDER	2
	.globl	swapper_pg_dir
	.equ	swapper_pg_dir, KERNEL_RAM_VADDR - PG_DIR_SIZE		// 全局变量

	.macro	pgtbl, rd, phys
	add	\rd, \phys, #TEXT_OFFSET
	sub	\rd, \rd, #PG_DIR_SIZE
	.endm														// 宏定义

	__HEAD
ENTRY(stext)
 ARM_BE8(setend	be )			@ ensure we are in BE8 mode

 THUMB(	adr	r9, BSYM(1f)	)	@ Kernel is always entered in ARM.
 THUMB(	bx	r9		)	@ If this is a Thumb-2 kernel,
 THUMB(	.thumb			)	@ switch to Thumb now.
 THUMB(1:			)											// BE8 mode, arm 指令

#ifdef CONFIG_ARM_VIRT_EXT
	bl	__hyp_stub_install
#endif
	@ ensure svc mode and all interrupts masked
	safe_svcmode_maskall r9										// 实模式

	mrc	p15, 0, r9, c0, c0		@ get processor id
	bl	__lookup_processor_type		@ r5=procinfo r9=cpuid		// 匹配 machine id processor id
	movs	r10, r5				@ invalid processor (r5=0)?
 THUMB( it	eq )		@ force fixup-able long branch encoding
	beq	__error_p			@ yes, error 'p'

#ifdef CONFIG_ARM_LPAE
	mrc	p15, 0, r3, c0, c1, 4		@ read ID_MMFR0
	and	r3, r3, #0xf			@ extract VMSA support
	cmp	r3, #5				@ long-descriptor translation table format?
 THUMB( it	lo )				@ force fixup-able long branch encoding
	blo	__error_lpae			@ only classic page table format
#endif

#ifndef CONFIG_XIP_KERNEL
	adr	r3, 2f
	ldmia	r3, {r4, r8}
	sub	r4, r3, r4			@ (PHYS_OFFSET - PAGE_OFFSET)
	add	r8, r8, r4			@ PHYS_OFFSET
#else
	ldr	r8, =PLAT_PHYS_OFFSET		@ always constant in this case
#endif

	/*
	 * r1 = machine no, r2 = atags or dtb,
	 * r8 = phys_offset, r9 = cpuid, r10 = procinfo
	 */
	bl	__vet_atags												// 中断向量表
#ifdef CONFIG_SMP_ON_UP
	bl	__fixup_smp
#endif
#ifdef CONFIG_ARM_PATCH_PHYS_VIRT
	bl	__fixup_pv_table
#endif
	bl	__create_page_tables									// 页表

	/*
	 * The following calls CPU specific code in a position independent
	 * manner.  See arch/arm/mm/proc-*.S for details.  r10 = base of
	 * xxx_proc_info structure selected by __lookup_processor_type
	 * above.  On return, the CPU will be ready for the MMU to be
	 * turned on, and r0 will hold the CPU control register value.
	 */
	ldr	r13, =__mmap_switched		@ address to jump to after			// 返回指针
						@ mmu has been enabled
	adr	lr, BSYM(1f)			@ return (PIC) address
	mov	r8, r4				@ set TTBR1 to swapper_pg_dir
	ldr	r12, [r10, #PROCINFO_INITFUNC]
	add	r12, r12, r10
	ret	r12
1:	b	__enable_mmu													// 开启 mmu

__enable_mmu:
#if defined(CONFIG_ALIGNMENT_TRAP) && __LINUX_ARM_ARCH__ < 6
	orr	r0, r0, #CR_A
#else
	bic	r0, r0, #CR_A
#endif
#ifdef CONFIG_CPU_DCACHE_DISABLE
	bic	r0, r0, #CR_C
#endif
#ifdef CONFIG_CPU_BPREDICT_DISABLE
	bic	r0, r0, #CR_Z
#endif
#ifdef CONFIG_CPU_ICACHE_DISABLE
	bic	r0, r0, #CR_I
#endif
#ifndef CONFIG_ARM_LPAE
	mov	r5, #(domain_val(DOMAIN_USER, DOMAIN_MANAGER) | \
		      domain_val(DOMAIN_KERNEL, DOMAIN_MANAGER) | \
		      domain_val(DOMAIN_TABLE, DOMAIN_MANAGER) | \
		      domain_val(DOMAIN_IO, DOMAIN_CLIENT))
	mcr	p15, 0, r5, c3, c0, 0		@ load domain access register
	mcr	p15, 0, r4, c2, c0, 0		@ load page table pointer
#endif
	b	__turn_mmu_on
ENDPROC(__enable_mmu)

ENTRY(__turn_mmu_on)
	mov	r0, r0
	instr_sync
	mcr	p15, 0, r0, c1, c0, 0		@ write control reg
	mrc	p15, 0, r3, c0, c0, 0		@ read id reg
	instr_sync
	mov	r3, r3
	mov	r3, r13
	ret	r3																// 返回 __mmap_switched
__turn_mmu_on_end:
ENDPROC(__turn_mmu_on)

// ============================================
(Linux\arch\arm\kernel\head-common.S)
__mmap_switched:
	adr	r3, __mmap_switched_data

	ldmia	r3!, {r4, r5, r6, r7}
	cmp	r4, r5				@ Copy data segment if needed
1:	cmpne	r5, r6
	ldrne	fp, [r4], #4
	strne	fp, [r5], #4
	bne	1b

	mov	fp, #0				@ Clear BSS (and zero fp)
1:	cmp	r6, r7
	strcc	fp, [r6],#4
	bcc	1b

 ARM(	ldmia	r3, {r4, r5, r6, r7, sp})
 THUMB(	ldmia	r3, {r4, r5, r6, r7}	)
 THUMB(	ldr	sp, [r3, #16]		)
	str	r9, [r4]			@ Save processor ID
	str	r1, [r5]			@ Save machine type
	str	r2, [r6]			@ Save atags pointer
	cmp	r7, #0
	strne	r0, [r7]			@ Save control register values
	b	start_kernel													// 跳转
ENDPROC(__mmap_switched)

// ============================================
(Linux\init\main.c)
start_kernel()	
	lockdep_init()								//初始化 classhash_table(4k个) chainhash_table(32k个)，设置标记 lockdep_initialized=1
	set_task_stack_end_magic(&init_task);		//设置init_task 栈幻数
	smp_setup_processor_id();					//开启多核
		// 打印 Booting Linux on physical CPU 0x0
	debug_objects_early_init					//debug初始化 obj_hash[i].lock (16k个)
	boot_init_stack_canary						//栈溢出保护。产生current->stack_canary,并保存
	cgroup_init_early
	local_irq_disable();						// asm 操作 cpsr
	early_boot_irqs_disabled					//全局变量，设置为true
	boot_cpu_init();
	page_address_init();
	pr_notice("%s", linux_banner);				// 打印 linux_banner：
		// Linux version 4.1.15+ (root@7060) (gcc version 6.3.1 20170109 (Linaro GCC 6.3-2017.02) ) #1 SMP PREEMPT Sun Mar 31 20:51:03 CST 2019
	setup_arch(&command_line);					// 主要是 mdesc ，单独解析：note_kernel_setup_arch-command_line.c 文件
												// =====================
	mm_init_cpumask(&init_mm);					// 清除 mm->cpu_vm_mask_var 的位
	setup_command_line(command_line);			// bootmem 保存 command_line
	setup_nr_cpu_ids();							// 设置宏 nr_cpu_ids
	setup_per_cpu_areas();	
	smp_prepare_boot_cpu();	
	build_all_zonelists(NULL, NULL);
	page_alloc_init();	
	
	pr_notice("Kernel command line: %s\n", boot_command_line);		// 打印
		// Kernel command line: console=ttymxc0,115200 root=/dev/nfs rw nfsroot=192.168.50.21:/datadisk/dev_mys_y6ulx/nfs_rootfs ip=192.168.50.100:192.168.50.21:192.168.50.1:255.255.255.0::eth0:off
	
	parse_early_param();						// 执行 __setup_start, __setup_end 段之间的代码，处理 param
	
	after_dashes = parse_args("Booting kernel",
				  static_command_line, __start___param,
				  __stop___param - __start___param,
				  -1, -1, &unknown_bootoption);
	if (!IS_ERR_OR_NULL(after_dashes))
		parse_args("Setting init args", after_dashes, NULL, 0, -1, -1,
			   set_init_arg);					// 分析 args
	
	jump_label_init();							// jump_label 初始化，
	
	setup_log_buf(0);							// 分配 log_buf 空间，并设置全局变量 log_buf , log_buf_len
	
	pidhash_init();								// 给全局变量 pid_hash 分配列表
	
	vfs_caches_init_early();
		dcache_init_early()						// 给 dentry_hashtable 分配空间
		inode_init_early();						// 给 inode_hashtable 分配空间比初始化
	
	sort_main_extable();						// 异常表排序
	
	trap_init();								// 空
	
	mm_init();									//
		page_ext_init_flatmem();				// 分配 page_ext 页，并初始化
		mem_init();								// free bootmem 打印 信息
			// Memory: 246880K/262144K available (8516K kernel code, 439K rwdata, 2944K rodata, 432K init, 450K bss, 15264K reserved, 0K cma-reserved, 0K highmem)
		kmem_cache_init();						// 设置全局 kmem_cache 变量，并设置状态
			kmem_cache = &kmem_cache_boot;
			slab_state = UP;
		percpu_init_late();
		pgtable_init();
			ptlock_cache_init();
			pgtable_cache_init();
		vmalloc_init();
		ioremap_huge_init();
	
	sched_init();
	
	preempt_disable();							// 关闭抢占
	
	idr_init_cache();							// 创建，并赋值给 idr_layer_cache 
	
	rcu_init();									// rcu 初始化，开启 RCU 的 softirq		
												//注册 notifer
	trace_init();

	context_tracking_init();
	radix_tree_init();
	
	early_irq_init();							// 获取 irq (16)数量，每个分配 irq_desc 并添加到 irq_desc_tree 中
	
	init_IRQ();									// 初始化 irq，调用的 machine_desc->init_irq(); 
	
	tick_init();								// tick 初始化
	
	rcu_init_nohz();
		
	init_timers();								// 初始化 timer，注册 timer notifer, 开启 softirq
	hrtimers_init();							// hight resolution 时钟

	softirq_init();	

	timekeeping_init();
	
	time_init();								// 
		machine_desc->init_time();				// 空
		of_clk_init(NULL);						// 从 __clk_of_table 初始化时钟
		clocksource_of_init();					// 从 __clksrc_of_table 获取节点，并运行 match->data(np) 函数初始化
			// 打印 sched_clock: %u bits at %lu%cHz, resolution %lluns, wraps every %lluns\n",bits, r, r_unit, res, wrap
			
	sched_clock_postinit();
	perf_event_init();
	profile_init();
	
	call_function_init();
	
	WARN(!irqs_disabled(), "Interrupts were enabled early\n");
	early_boot_irqs_disabled = false;
	local_irq_enable();	
	
	kmem_cache_init_late();
	
	console_init();
		tty_ldisc_begin();
			(void) tty_register_ldisc(N_TTY, &tty_ldisc_N_TTY);
				tty_ldiscs[disc] = new_ldisc;
				new_ldisc->num = disc;
				new_ldisc->refcount = 0;		// 注册tty_ldiscs
					//================================
					struct tty_ldisc_ops tty_ldisc_N_TTY = {
						.magic           = TTY_LDISC_MAGIC,
						.name            = "n_tty",
						.open            = n_tty_open,
						.close           = n_tty_close,
						.flush_buffer    = n_tty_flush_buffer,
						.chars_in_buffer = n_tty_chars_in_buffer,
						.read            = n_tty_read,
						.write           = n_tty_write,
						.ioctl           = n_tty_ioctl,
						.set_termios     = n_tty_set_termios,
						.poll            = n_tty_poll,
						.receive_buf     = n_tty_receive_buf,
						.write_wakeup    = n_tty_write_wakeup,
						.fasync		 = n_tty_fasync,
						.receive_buf2	 = n_tty_receive_buf2,
					};
					//================================
					
		__con_initcall_start()					// 全局程序表  开始
		.......
		__con_initcall_end()					// 全局程序表  结束，全部执行一遍
	
	if (panic_later)
		panic("Too many boot %s vars at `%s'", panic_later,
		      panic_param);

	lockdep_info();								//
	
	locking_selftest();							//

#ifdef CONFIG_BLK_DEV_INITRD
	if (initrd_start && !initrd_below_start_ok &&
	    page_to_pfn(virt_to_page((void *)initrd_start)) < min_low_pfn) {
		pr_crit("initrd overwritten (0x%08lx < 0x%08lx) - disabling it.\n",
		    page_to_pfn(virt_to_page((void *)initrd_start)),
		    min_low_pfn);
		initrd_start = 0;
	}
#endif
	page_ext_init();
	debug_objects_mem_init();
	kmemleak_init();							// early log
	setup_per_cpu_pageset();
	numa_policy_init();
	if (late_time_init)
		late_time_init();
	sched_clock_init();
	calibrate_delay();							// 打印信息
		// Calibrating delay loop (skipped), value calculated using timer frequency.. 48.00
		// BogoMIPS (lpj=240000)

	pidmap_init();								// pid map
	anon_vma_init();
	acpi_early_init();
#ifdef CONFIG_X86
	if (efi_enabled(EFI_RUNTIME_SERVICES))
		efi_enter_virtual_mode();
#endif
#ifdef CONFIG_X86_ESPFIX64
	/* Should be run before the first non-init thread is created */
	init_espfix_bsp();
#endif
	thread_info_cache_init();					// 赋值全局变量 thread_info_cache(struct kmem_cache)
	cred_init();								// 赋值全局变量 cred_jar (struct kmem_cache)
	fork_init();								// 赋值全局变量 task_struct_cachep, max_threads
		init_task.signal->rlim[RLIMIT_NPROC].rlim_cur = max_threads/2;
		init_task.signal->rlim[RLIMIT_NPROC].rlim_max = max_threads/2;
		init_task.signal->rlim[RLIMIT_SIGPENDING] = init_task.signal->rlim[RLIMIT_NPROC];
		
	proc_caches_init();							// 赋值全局变量 sighand_cachep，signal_cachep，files_cachep，fs_cachep，mm_cachep，vm_area_cachep，nsproxy_cachep
	buffer_init();								// 赋值全局变量 bh_cachep, max_buffer_heads
	key_init();									// 赋值全局变量 key_jar, root_key_user，key_user_tree
	security_init();							// 赋值全局变量 security_ops，
												// __security_initcall_start[], __security_initcall_end[]; 之间所有函数执行一遍
	dbg_late_init();
	vfs_caches_init(totalram_pages);
		// 设置全局变量 names_cachep
		dcache_init();							// 初始化
		inode_init();							// inode_cachep, inode_hashtable 赋值
		files_init(mempages);					// filp_cachep 赋值
		mnt_init();								// mnt_cache,mount_hashtable,mountpoint_hashtable
			kernfs_init();						// kernfs_node_cache 赋值
			sysfs_init();						// sysfs_root sysfs_root_kn 赋值，注册 sysfs 文件系统，有 mount kill_sb 方法
			init_rootfs();						// 注册 rootfs 文件系统，有 mount 方法， 初始化 ramfs 系统，初始化 shmem 系统
			init_mount_tree();					// 赋值添加 fs_kobj	, 添加 mnt 名字空间		
			
		bdev_cache_init();						// bdev_cachep 赋值，注册 bdev 文件系统
		chrdev_init();							// chrdev probe
			request_module()					// 
	signals_init();								// sigqueue_cachep 赋值
	/* rootfs populating might need page-writeback */
	page_writeback_init();
	proc_root_init();							// 注册 proc 文件系统，创建 mounts 的 syslinks，创建 sysvipc | fs | dirver | fs/nfsd 目录
	nsfs_init();								// 设置全局变量 nsfs_mnt
	cpuset_init();								
	cgroup_init();
	taskstats_init_early();						// init_task.delays 赋值
	delayacct_init();

	check_bugs();

	acpi_subsystem_init();						// 高级配置，电源子系统初始化 ， 赋值 has_full_constraints
	sfi_init_late();

	if (efi_enabled(EFI_RUNTIME_SERVICES)) {
		efi_late_init();
		efi_free_boot_services();
	}

	ftrace_init();								// 设置 ftrace_module_notify_exit 通知

	/* Do the rest non-__init'ed, we're now alive */
	rest_init();
	
// ===================================================
rest_init(void)
	rcu_scheduler_starting
		rcu_scheduler_active = 1
	smpboot_thread_init	
		register_cpu_notifier(&smpboot_thread_notifier);

	kernel_thread(kernel_init, NULL, CLONE_FS);						// do_fork 后执行 kernel_init 进程，加载用户
	numa_default_policy();
	pid = kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
	rcu_read_lock();
	kthreadd_task = find_task_by_pid_ns(pid, &init_pid_ns);
	rcu_read_unlock();
	complete(&kthreadd_done);

	init_idle_bootup_task(current);
	schedule_preempt_disabled();

	cpu_startup_entry(CPUHP_ONLINE);




// ===================================================
kernel_init






// ===================================================
kthreadd























