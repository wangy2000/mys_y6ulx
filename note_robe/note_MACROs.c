
// ======================================= EXPORT_SYMBOL(sym)

#define EXPORT_SYMBOL(sym)	__EXPORT_SYMBOL(sym, "")

#define __EXPORT_SYMBOL(sym, sec)				\
	extern typeof(sym) sym;					\
	__CRC_SYMBOL(sym, sec)	static const char   __kstrtab_##sym[]    __attribute__((section("__ksymtab_strings"), aligned(1))) \
																= VMLINUX_SYMBOL_STR(sym);		   \
	extern const struct 	kernel_symbol     	__ksymtab_##sym;  \
	__visible   const   struct kernel_symbol    __ksymtab_##sym	    __used	  __attribute__((section("___ksymtab" sec "+" #sym), unused))	\
																= { (unsigned long)&sym, __kstrtab_##sym }

	extern typeof(sym) sym;					\
	__CRC_SYMBOL(sym, sec)	static const char   __kstrtab_##sym[]    __attribute__((section("__ksymtab_strings"), aligned(1))) \
																= VMLINUX_SYMBOL_STR(sym);		   \
	extern const struct 	kernel_symbol     	__ksymtab_##sym;  \
	__visible   const   struct kernel_symbol    __ksymtab_##sym	    __used	  __attribute__((section("___ksymtab" sec "+" #sym), unused))	\
																= { (unsigned long)&sym, __kstrtab_##sym }



struct kernel_symbol
{
	unsigned long value;
	const char *name;
};

#define VMLINUX_SYMBOL_STR(x) __VMLINUX_SYMBOL_STR(x)

#define __VMLINUX_SYMBOL_STR(x) #x

	extern typeof(sym) sym;					\
	__CRC_SYMBOL(sym, sec)	static const char   __kstrtab_##sym[]    __attribute__((section("__ksymtab_strings"), aligned(1))) \
																= sym;		   \
	extern const struct 	kernel_symbol     	__ksymtab_##sym;  \
	__visible   const   struct kernel_symbol    __ksymtab_##sym	    __used	  __attribute__((section("___ksymtab" sec "+" #sym), unused))	\
																= { (unsigned long)&sym, __kstrtab_##sym }



// ======================================= DT_MACHINE_START

#define DT_MACHINE_START(_name, _namestr)		\
static const struct machine_desc __mach_desc_##_name	\
 __used							\
 __attribute__((__section__(".arch.info.init"))) = {	\
	.nr		= ~0,				\
	.name		= _namestr,

#endif

#define MACHINE_END				\
};

#define MACHINE_START(_type,_name)			\
static const struct machine_desc __mach_desc_##_type	\
 __used							\
 __attribute__((__section__(".arch.info.init"))) = {	\
	.nr		= MACH_TYPE_##_type,		\
	.name		= _name,

struct machine_desc {
	unsigned int		nr;		/* architecture number	*/
	const char		*name;		/* architecture name	*/
	unsigned long		atag_offset;	/* tagged list (relative) */
	const char *const 	*dt_compat;	/* array of device tree
						 * 'compatible' strings	*/

	unsigned int		nr_irqs;	/* number of IRQs */

#ifdef CONFIG_ZONE_DMA
	phys_addr_t		dma_zone_size;	/* size of DMA-able area */
#endif

	unsigned int		video_start;	/* start of video RAM	*/
	unsigned int		video_end;	/* end of video RAM	*/

	unsigned char		reserve_lp0 :1;	/* never has lp0	*/
	unsigned char		reserve_lp1 :1;	/* never has lp1	*/
	unsigned char		reserve_lp2 :1;	/* never has lp2	*/
	enum reboot_mode	reboot_mode;	/* default restart mode	*/
	unsigned		l2c_aux_val;	/* L2 cache aux value	*/
	unsigned		l2c_aux_mask;	/* L2 cache aux mask	*/
	void			(*l2c_write_sec)(unsigned long, unsigned);
	struct smp_operations	*smp;		/* SMP operations	*/
	bool			(*smp_init)(void);
	void			(*fixup)(struct tag *, char **);
	void			(*dt_fixup)(void);
	void			(*init_meminfo)(void);
	void			(*reserve)(void);/* reserve mem blocks	*/
	void			(*map_io)(void);/* IO mapping function	*/
	void			(*init_early)(void);
	void			(*init_irq)(void);
	void			(*init_time)(void);
	void			(*init_machine)(void);
	void			(*init_late)(void);
#ifdef CONFIG_MULTI_IRQ_HANDLER
	void			(*handle_irq)(struct pt_regs *);
#endif
	void			(*restart)(enum reboot_mode, const char *);
};


DT_MACHINE_START(IMX6UL, "Freescale i.MX6 Ultralite (Device Tree)")
	.map_io		= imx6ul_map_io,
	.init_irq	= imx6ul_init_irq,
	.init_machine	= imx6ul_init_machine,
	.init_late	= imx6ul_init_late,
	.dt_compat	= imx6ul_dt_compat,
MACHINE_END

static const struct machine_desc __mach_desc_##_name	\
 __used							\
 __attribute__((__section__(".arch.info.init"))) = {	\
	.nr		= ~0,				\
	.name		= _namestr,
// -----------------
	.map_io		= imx6ul_map_io,
	.init_irq	= imx6ul_init_irq,
	.init_machine	= imx6ul_init_machine,
	.init_late	= imx6ul_init_late,
	.dt_compat	= imx6ul_dt_compat,
// -----------------
};

static const struct machine_desc __mach_desc_IMX6UL	   __used		 __attribute__((__section__(".arch.info.init")))
 = {	\
	.nr		= ~0,				\
	.name		= "Freescale i.MX6 Ultralite (Device Tree)",
// -----------------
	.map_io		= imx6ul_map_io,
	.init_irq	= imx6ul_init_irq,
	.init_machine	= imx6ul_init_machine,
	.init_late	= imx6ul_init_late,
	.dt_compat	= imx6ul_dt_compat,
// -----------------
};

MACHINE_START(IMX6UL, "Freescale i.MX6 Ultralite (Device Tree)")
	.map_io		= imx6ul_map_io,
	.init_irq	= imx6ul_init_irq,
	.init_machine	= imx6ul_init_machine,
	.init_late	= imx6ul_init_late,
	.dt_compat	= imx6ul_dt_compat,
MACHINE_END

static const struct machine_desc __mach_desc_IMX6UL	   __used		 __attribute__((__section__(".arch.info.init")))
 = {	\
	.nr		= MACH_TYPE_IMX6UL,				\
	.name		= "Freescale i.MX6 Ultralite (Device Tree)",
// -----------------
	.map_io		= imx6ul_map_io,
	.init_irq	= imx6ul_init_irq,
	.init_machine	= imx6ul_init_machine,
	.init_late	= imx6ul_init_late,
	.dt_compat	= imx6ul_dt_compat,
// -----------------
};

// ===============================================




