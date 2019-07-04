void __init of_irq_init(const struct of_device_id *matches)
{
	struct device_node *np, *parent = NULL;
	struct intc_desc *desc, *temp_desc;
	struct list_head intc_desc_list, intc_parent_list;

	INIT_LIST_HEAD(&intc_desc_list);
	INIT_LIST_HEAD(&intc_parent_list);

	for_each_matching_node(np, matches) {
		if (!of_find_property(np, "interrupt-controller", NULL) ||
				!of_device_is_available(np))
			continue;
		/*
		 * Here, we allocate and populate an intc_desc with the node
		 * pointer, interrupt-parent device_node etc.
		 */
		desc = kzalloc(sizeof(*desc), GFP_KERNEL);
		if (WARN_ON(!desc))
			goto err;

		desc->dev = np;
		desc->interrupt_parent = of_irq_find_parent(np);
		if (desc->interrupt_parent == np)
			desc->interrupt_parent = NULL;
		list_add_tail(&desc->list, &intc_desc_list);
	}																// match 所有中断控制器，并分配内存，添加到 intc_desc_list 中

	/*
	 * The root irq controller is the one without an interrupt-parent.
	 * That one goes first, followed by the controllers that reference it,
	 * followed by the ones that reference the 2nd level controllers, etc.
	 */
	while (!list_empty(&intc_desc_list)) {
		/*
		 * Process all controllers with the current 'parent'.
		 * First pass will be looking for NULL as the parent.
		 * The assumption is that NULL parent means a root controller.
		 */
		list_for_each_entry_safe(desc, temp_desc, &intc_desc_list, list) {
			const struct of_device_id *match;
			int ret;
			of_irq_init_cb_t irq_init_cb;

			if (desc->interrupt_parent != parent)
				continue;

			list_del(&desc->list);
			match = of_match_node(matches, desc->dev);
			if (WARN(!match->data,
			    "of_irq_init: no init function for %s\n",
			    match->compatible)) {
				kfree(desc);
				continue;
			}

			pr_debug("of_irq_init: init %s @ %p, parent %p\n",
				 match->compatible,
				 desc->dev, desc->interrupt_parent);
			irq_init_cb = (of_irq_init_cb_t)match->data;
			ret = irq_init_cb(desc->dev, desc->interrupt_parent);
			if (ret) {
				kfree(desc);
				continue;
			}

			/*
			 * This one is now set up; add it to the parent list so
			 * its children can get processed in a subsequent pass.
			 */
			list_add_tail(&desc->list, &intc_parent_list);				// 控制器的根提取出来，执行 match->data(desc->dev, desc->interrupt_parent)
		}																// 从 intc_desc_list 删除，加入 intc_parent_list

		/* Get the next pending parent that might have children */
		desc = list_first_entry_or_null(&intc_parent_list,				// 如果空 = NULL，非空则 = 链表头
						typeof(*desc), list);
		if (!desc) {
			pr_err("of_irq_init: children remain, but no parents\n");
			break;
		}
		list_del(&desc->list);
		parent = desc->dev;
		kfree(desc);													// 删除根控制器，free回收内存，把自己标记为 parent (= desc->dev)
	}																	// 第二次循环开始，以parent为根的中断控制器全部初始化，并加入 intc_parent_list
																		// 一层一层全部初始化

	list_for_each_entry_safe(desc, temp_desc, &intc_parent_list, list) {	// 回收内存
		list_del(&desc->list);
		kfree(desc);						
	}
err:
	list_for_each_entry_safe(desc, temp_desc, &intc_desc_list, list) {		// 错误处理。出错就返回，内核就挂了
		list_del(&desc->list);
		kfree(desc);
	}
}
