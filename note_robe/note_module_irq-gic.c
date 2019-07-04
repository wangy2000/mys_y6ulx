// ========================================
(Linux\drivers\irqchip\irq-gic.c)

#define IRQCHIP_DECLARE(name, compat, fn) OF_DECLARE_2(irqchip, name, compat, fn)

#define OF_DECLARE_2(table, name, compat, fn) \
		_OF_DECLARE(table, name, compat, fn, of_init_fn_2)
		
#define _OF_DECLARE(table, name, compat, fn, fn_type)			\
	static const struct of_device_id __of_table_##name		\
		__used __section(__##table##_of_table)			\
		 = { .compatible = compat,				\
		     .data = (fn == (fn_type)NULL) ? fn : fn  }
		

IRQCHIP_DECLARE(cortex_a7_gic, "arm,cortex-a7-gic", gic_of_init);
=OF_DECLARE_2(irqchip, cortex_a7_gic, "arm,cortex-a7-gic", gic_of_init);
=_OF_DECLARE(irqchip, cortex_a7_gic, "arm,cortex-a7-gic", gic_of_init, of_init_fn_2)
=static const struct of_device_id __of_table_##name __used __section(__##table##_of_table)
					 = { .compatible = compat,
						.data = (fn == (fn_type)NULL) ? fn : fn  }

=static const struct of_device_id __of_table_cortex_a7_gic __used __section(__irqchip_of_table)
					 = { .compatible = "arm,cortex-a7-gic",
						.data = (gic_of_init == (of_init_fn_2)NULL) ? gic_of_init : gic_of_init  }
						
=static const struct of_device_id  		
					__of_table_cortex_a7_gic 		__used __section(__irqchip_of_table)
					 = { .compatible = "arm,cortex-a7-gic",
						.data = (gic_of_init == (of_init_fn_2)NULL) ? gic_of_init : gic_of_init  }
						
= struct of_device_id  __of_table_cortex_a7_gic = = { .compatible = "arm,cortex-a7-gic",
													.data = gic_of_init,
}

// =======================================
内核 init_IRQ() 调用
	machine_desc->init_irq();				// 有线调用这个初始化，不存在就调用下面这个初始化。imx6ull 这个板子调用这个初始化
	(imx6ul_init_irq())
	irqchip_init();							// 其他板子初始化
		of_irq_init(__irqchip_of_table);
		acpi_irq_init();

// =======================================	imx6ull irq 初始化
(Linux\arch\arm\mach-imx\mach-imx6ul.c)
machine_desc->init_irq();
imx6ul_init_irq()
	imx_gpc_check_dt				//从 DTS 获取并 赋值 gpc_base 基地址						电压，频率控制
	imx_init_revision_from_anatop	//从 DTS 获取并 设置 __mxc_cpu_type，imx_soc_revision		cpu_type，soc_revision
	imx_src_init					//从 DTS 获取并 设备 src_base, imx_reset_controller			注册 reset control
	irqchip_init					//从 DTS 获取并 初始化 irqchip 



// =======================================  其他板子直接初始化，imx6ull 初始化增加了 gpc，src 初始化部分
(Linux\drivers\irqchip\irqchip.c)
irqchip_init();							
	of_irq_init(__irqchip_of_table);		// Scan and init matching interrupt controllers in DT
											// match 所有中断控制器，并分配内存，添加到 intc_desc_list 中
											// 控制器的根提取出来，执行 match->data(desc->dev, desc->interrupt_parent)
											// 从 intc_desc_list 删除，加入 intc_parent_list
											// 如果空 = NULL，非空则 = 链表头
											// 删除根控制器，free回收内存，把自己标记为 parent (= desc->dev)
											// 第二次循环开始，以parent为根的中断控制器全部初始化，并加入 intc_parent_list
											// 第三次开始以 intc_parent_list 逐个为根，取出回收内存，并从 intc_desc_list 中找到所有子节点初始化
											// 把所有 中断控制器 全部初始化完，删除 intc_parent_list 中所有节点，回收内存
											// 总结：以 gic 为根，初始化一个中断控制器树
		gic_of_init()						// 初始化的方法
			gic_init_bases(gic_cnt, -1, dist_base, cpu_base, percpu_offset, node);
											// 从 dt 获取并设置 gic 的 dist_base, cpu_base 和 gic_get_common_base 方法
											// 从 gic 硬件获取支持的 riq 数量，并设置 gic 的 gic_irqs
											// 初始化 irq_domain 添加到 irq_domain_list 链表中
			irq_of_parse_and_map(node, 0);	// Parse and map an interrupt into linux virq space
			gic_cascade_irq(gic_cnt, irq);	// 设置 irq handler data ，添加链表，清零 irq 并启动 irq 

	acpi_irq_init();





































