																			/* arch/arm/kernel/vmlinux.lds 
																			 * Automatically generated file; DO NOT EDIT.
																			 * Linux/arm 4.1.15 Kernel Configuration
																			*/
OUTPUT_ARCH(arm)
ENTRY(stext)
jiffies = jiffies_64;
SECTIONS
{
																			/*
																			 * XXX: The linker does not define how output sections are
																			 * assigned to input sections when there are multiple statements
																			 * matching the same input section name.  There is no documented
																			 * order of matching.
																			 *
																			 * unwind exit sections must be discarded before the rest of the
																			 * unwind sections get included.
																			 */
  /DISCARD/ : {
												*(.ARM.exidx.exit.text)
												*(.ARM.extab.exit.text)
												*(.exitcall.exit)
												*(.discard)
												*(.discard.*)
  }
  . = 0x80000000 + 0x00008000;
  .head.text : {
						_text = .;				*(.head.text)
  }
  .text : { 																/* Real text segment		*/
						_stext = .; 										/* Text and read-only data	*/
		. = ALIGN(8);	__idmap_text_start = .; *(.idmap.text) 		
						__idmap_text_end = .; 
		. = ALIGN((1 << 12));
						__hyp_idmap_text_start = .;	
												*(.hyp.idmap.text)	
						__hyp_idmap_text_end = .;
						__exception_text_start = .;	
												*(.exception.text)	
						__exception_text_end = .; 
		. = ALIGN(8); 							*(.text.hot) 
												*(.text .text.fixup) 
												*(.ref.text) 
												*(.text.unlikely)
		. = ALIGN(8); 	__sched_text_start = .; 	
												*(.sched.text)
						__sched_text_end = .;
		. = ALIGN(8); 	__lock_text_start = .; 	*(.spinlock.text)
						__lock_text_end = .;
		. = ALIGN(8); 	__kprobes_text_start = .; 
												*(.kprobes.text)
						__kprobes_text_end = .;
												*(.gnu.warning)
												*(.glue_7)
												*(.glue_7t)
		. = ALIGN(4);							*(.got) 					/* Global offset table		*/
		. = ALIGN(4); 	__proc_info_begin = .; 	*(.proc.info.init) 	
						__proc_info_end = .;
  }
		. = ALIGN(((1 << 12))); 
  .rodata : AT(ADDR(.rodata) - 0) { 
						__start_rodata = .; 	*(.rodata) 					/* start of rodata section */
												*(.rodata.*) 
												*(__vermagic) 
		. = ALIGN(8); 	__start___tracepoints_ptrs = .;
												*(__tracepoints_ptrs) 
						__stop___tracepoints_ptrs = .; 
												*(__tracepoints_strings) 
  } 
  .rodata1 : AT(ADDR(.rodata1) - 0) { 			*(.rodata1) 
  } 
		. = ALIGN(8); 	
  __bug_table : AT(ADDR(__bug_table) - 0) { 
						__start___bug_table = .; 
												*(__bug_table) 
						__stop___bug_table = .;
  } 
  .pci_fixup : AT(ADDR(.pci_fixup) - 0) { 
						__start_pci_fixups_early = .; 
												*(.pci_fixup_early)
						__end_pci_fixups_early = .; 
						__start_pci_fixups_header = .; 
												*(.pci_fixup_header)
						__end_pci_fixups_header = .; 
						__start_pci_fixups_final = .; 
												*(.pci_fixup_final) 
						__end_pci_fixups_final = .; 
						__start_pci_fixups_enable = .; 
												*(.pci_fixup_enable) 
						__end_pci_fixups_enable = .;
						__start_pci_fixups_resume = .; 
												*(.pci_fixup_resume) 
						__end_pci_fixups_resume = .; 
						__start_pci_fixups_resume_early = .; 
												*(.pci_fixup_resume_early) 
						__end_pci_fixups_resume_early = .; 
						__start_pci_fixups_suspend = .; 
												*(.pci_fixup_suspend) 
						__end_pci_fixups_suspend = .; 
						__start_pci_fixups_suspend_late = .; 
												*(.pci_fixup_suspend_late) 
						__end_pci_fixups_suspend_late = .; 
  } 
  .builtin_fw : AT(ADDR(.builtin_fw) - 0) { 
						__start_builtin_fw = .; *(.builtin_fw)
						__end_builtin_fw = .; 
  } 
  __ksymtab : AT(ADDR(__ksymtab) - 0) { 
						__start___ksymtab = .; 	*(SORT(___ksymtab+*)) 	
						__stop___ksymtab = .; 
  }
  __ksymtab_gpl : AT(ADDR(__ksymtab_gpl) - 0) { 
 						__start___ksymtab_gpl = .;
												*(SORT(___ksymtab_gpl+*)) 	
						__stop___ksymtab_gpl = .; 
  } 
  __ksymtab_unused : AT(ADDR(__ksymtab_unused) - 0) { 
						__start___ksymtab_unused = .; 	
												*(SORT(___ksymtab_unused+*)) 
						__stop___ksymtab_unused = .; 
  }
  __ksymtab_unused_gpl : AT(ADDR(__ksymtab_unused_gpl) - 0) { 
						__start___ksymtab_unused_gpl = .; 
												*(SORT(___ksymtab_unused_gpl+*)) 
						__stop___ksymtab_unused_gpl = .; 
  }
  __ksymtab_gpl_future : AT(ADDR(__ksymtab_gpl_future) - 0) { 
						__start___ksymtab_gpl_future = .; 
												*(SORT(___ksymtab_gpl_future+*)) 
						__stop___ksymtab_gpl_future = .; 
  }
  __kcrctab : AT(ADDR(__kcrctab) - 0) { 
						__start___kcrctab = .; 	*(SORT(___kcrctab+*)) 				
						__stop___kcrctab = .; 
  } 
  __kcrctab_gpl : AT(ADDR(__kcrctab_gpl) - 0) { 
						__start___kcrctab_gpl = .;
												*(SORT(___kcrctab_gpl+*)) 			
						__stop___kcrctab_gpl = .; 
  } 
  __kcrctab_unused : AT(ADDR(__kcrctab_unused) - 0) { 
 						__start___kcrctab_unused = .; 	
												*(SORT(___kcrctab_unused+*)) 		
						__stop___kcrctab_unused = .;
  } 
  __kcrctab_unused_gpl : AT(ADDR(__kcrctab_unused_gpl) - 0) { 
						__start___kcrctab_unused_gpl = .; 	
												*(SORT(___kcrctab_unused_gpl+*)) 	
						__stop___kcrctab_unused_gpl = .; 
  } 
  __kcrctab_gpl_future : AT(ADDR(__kcrctab_gpl_future) - 0) { 
						__start___kcrctab_gpl_future = .; 	
												*(SORT(___kcrctab_gpl_future+*)) 	
						__stop___kcrctab_gpl_future = .;
  }
  __ksymtab_strings : AT(ADDR(__ksymtab_strings) - 0) { 
												*(__ksymtab_strings) 
  } 
  __init_rodata : AT(ADDR(__init_rodata) - 0) {
												*(.ref.rodata)
  } 
  __param : AT(ADDR(__param) - 0) { 
						__start___param = .; 				
												*(__param) 
						__stop___param = .; 
  } 
  __modver : AT(ADDR(__modver) - 0) { 
						__start___modver = .; 	*(__modver) 					
						__stop___modver = .; 
		. = ALIGN(((1 << 12))); 
						__end_rodata = .; /* End of rodata section */
  } 
		. = ALIGN(((1 << 12)));
		. = ALIGN(4);
  __ex_table : AT(ADDR(__ex_table) - 0) {
						__start___ex_table = .;	*(__ex_table)
						__stop___ex_table = .;
  }
	/*
	 * Stack unwinding tables
	 */
		. = ALIGN(8);
  .ARM.unwind_idx : {  	__start_unwind_idx = .;	*(.ARM.exidx*)			
						__stop_unwind_idx = .;
  }
  .ARM.unwind_tab : {	__start_unwind_tab = .;	*(.ARM.extab*)			
						__stop_unwind_tab = .;
  }
  .notes : AT(ADDR(.notes) - 0) { 
						__start_notes = .; 		*(.note.*)
						__stop_notes = .; 
  }
						_etext = .;											/* End of text and rodata section */
		. = ALIGN((1 << 12));
						__init_begin = .;
																			/*
																			 * The vectors and stubs are relocatable code, and the
																			 * only thing that matters is their relative offsets
																			 */
						__vectors_start = .;
  .vectors 0 : AT(__vectors_start) {
												*(.vectors)
  }
  . = __vectors_start + SIZEOF(.vectors);
						__vectors_end = .;
						__stubs_start = .;
  .stubs 0x1000 : AT(__stubs_start) {
												*(.stubs)
  }
  . = __stubs_start + SIZEOF(.stubs);
						__stubs_end = .;
		. = ALIGN(8); 
  .init.text : AT(ADDR(.init.text) - 0) { 
						_sinittext = .; 		*(.init.text) 
												*(.meminit.text) 
						_einittext = .;
  }
  .exit.text : {
												*(.exit.text) 
												*(.memexit.text)
  }
  .init.proc.info : {
 
  }
  .init.arch.info : {
						__arch_info_begin = .;
												*(.arch.info.init)
						__arch_info_end = .;
  }
  .init.tagtable : {
						__tagtable_begin = .;
												*(.taglist.init)
						__tagtable_end = .;
  }
  .init.smpalt : {
						__smpalt_begin = .;
												*(.alt.smp.init)
						__smpalt_end = .;
  }
  .init.pv_table : {
						__pv_table_begin = .;
												*(.pv_table)
						__pv_table_end = .;
  }
  .init.data : {								*(.init.data) 
												*(.meminit.data) 
												*(.init.rodata) 
												*(.meminit.rodata) 
		. = ALIGN(8);	__clk_of_table = .;		*(__clk_of_table) 
												*(__clk_of_table_end) 
		. = ALIGN(8);	__reservedmem_of_table = .; 
												*(__reservedmem_of_table) 
												*(__reservedmem_of_table_end) 
		. = ALIGN(8);	__clksrc_of_table = .; 	*(__clksrc_of_table) 
												*(__clksrc_of_table_end) 
		. = ALIGN(8);	__cpu_method_of_table = .; 
												*(__cpu_method_of_table) 
												*(__cpu_method_of_table_end) 
		. = ALIGN(8);	__cpuidle_method_of_table = .; 
												*(__cpuidle_method_of_table)
												*(__cpuidle_method_of_table_end) 
		. = ALIGN(32); 	__dtb_start = .; 		*(.dtb.init.rodata)		__dtb_end = .; 
		. = ALIGN(8); 	__irqchip_of_table = .; *(__irqchip_of_table) 
												*(__irqchip_of_table_end)
		. = ALIGN(16); 	__setup_start = .; 		*(.init.setup)			__setup_end = .;
						__initcall_start = .;	*(.initcallearly.init) 	__initcall0_start = .; 
												*(.initcall0.init) 
												*(.initcall0s.init) 
						__initcall1_start = .; 	*(.initcall1.init) 
												*(.initcall1s.init) 
						__initcall2_start = .; 	*(.initcall2.init) 
												*(.initcall2s.init) 
						__initcall3_start = .; 	*(.initcall3.init) 
												*(.initcall3s.init) 
						__initcall4_start = .; 	*(.initcall4.init) 
												*(.initcall4s.init) 
						__initcall5_start = .; 	*(.initcall5.init) 
												*(.initcall5s.init) 
						__initcallrootfs_start = .; 
												*(.initcallrootfs.init) 
												*(.initcallrootfss.init) 
						__initcall6_start = .; 	*(.initcall6.init) 
												*(.initcall6s.init) 
						__initcall7_start = .; 	*(.initcall7.init)
												*(.initcall7s.init) 
						__initcall_end = .;
						__con_initcall_start = .;
												*(.con_initcall.init) 
						__con_initcall_end = .;
						__security_initcall_start = .; 
												*(.security_initcall.init)
						__security_initcall_end = .;
		. = ALIGN(4); 	__initramfs_start = .; 	*(.init.ramfs) 
		. = ALIGN(8); 							*(.init.ramfs.info)
  }
  .exit.data : {
												*(.exit.data) 
												*(.memexit.data) 
												*(.memexit.rodata)
  }
		. = ALIGN((1 << 12)); 
  .data..percpu : AT(ADDR(.data..percpu) - 0) { 
						__per_cpu_load = .; 
						__per_cpu_start = .; 	*(.data..percpu..first) 
		. = ALIGN((1 << 12)); 					*(.data..percpu..page_aligned) 
		. = ALIGN((1 << 6)); 					*(.data..percpu..read_mostly) 
		. = ALIGN((1 << 6)); 					*(.data..percpu) 
												*(.data..percpu..shared_aligned) 
						__per_cpu_end = .; 
  }
		. = ALIGN(((1 << 12) << 1));
						__init_end = .;
						__data_loc = .;
  .data : AT(__data_loc) {
						_data = .; 											/* address in memory */
						_sdata = .;
		/*
		 * first, the init task union, aligned
		 * to an 8192 byte boundary.
		 */
		. = ALIGN(((1 << 12) << 1)); 			*(.data..init_task)
		. = ALIGN((1 << 12)); 
						__nosave_begin = .; 
												*(.data..nosave) 
		. = ALIGN((1 << 12)); 
						__nosave_end = .;
		. = ALIGN((1 << 6)); 					*(.data..cacheline_aligned)
		. = ALIGN((1 << 6)); 					*(.data..read_mostly) 
		. = ALIGN((1 << 6));
																			/*
																			 * and the usual data section
																			 */
												*(.data) 
												*(.ref.data) 
												*(.data..shared_aligned) 
												*(.data.unlikely) 
		. = ALIGN(32); 							*(__tracepoints) 
		. = ALIGN(8); 	__start___jump_table = .; 
												*(__jump_table) 
						__stop___jump_table = .; 
		. = ALIGN(8); 	__start___verbose = .; 	*(__verbose) 
						__stop___verbose = .;
   CONSTRUCTORS
						_edata = .;
  }
						_edata_loc = __data_loc + SIZEOF(.data);
		. = ALIGN(0); 	__bss_start = .; 
		. = ALIGN(0); 
  .sbss : AT(ADDR(.sbss) - 0) { 				*(.sbss) 
												*(.scommon) 
  } 
		. = ALIGN(0); 
  .bss : AT(ADDR(.bss) - 0) { 					*(.bss..page_aligned) 
												*(.dynbss) 
												*(.bss) 
												*(COMMON) 
  } 
		. = ALIGN(0); 	__bss_stop = .;
						_end = .;
  .stab 0 : { 									*(.stab) 
  } 
  .stabstr 0 : { 								*(.stabstr) 
  } 
  .stab.excl 0 : { 								*(.stab.excl) 
  } 
  .stab.exclstr 0 : { 							*(.stab.exclstr) 
  } 
  .stab.index 0 : { 							*(.stab.index) 
  } 
  .stab.indexstr 0 : { 							*(.stab.indexstr) 
  } 
  .comment 0 : { 								*(.comment) 
  }
}
																			/*
																			 * These must never be empty
																			 * If you have to comment these two assert statements out, your
																			 * binutils is too old (for other reasons as well)
																			 */
ASSERT((__proc_info_end - __proc_info_begin), "missing CPU support")
ASSERT((__arch_info_end - __arch_info_begin), "no machine record defined")
																			/*
																			 * The HYP init code can't be more than a page long,
																			 * and should not cross a page boundary.
																			 * The above comment applies as well.
																			 */
ASSERT(__hyp_idmap_text_end - (__hyp_idmap_text_start & (~((1 << 12) - 1))) <= (1 << 12),
 "HYP init code too big or misaligned")
