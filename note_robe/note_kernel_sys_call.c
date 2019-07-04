// ==================================================
添加系统调用
(添加系统调用号)
Linux\arch\arm\include\asm\unistd.h
Linux\arch\arm\include\uapi\asm\unistd.h
#define __NR_OABI_SYSCALL_BASE	0x900000
#define __NR_SYSCALL_BASE	__NR_OABI_SYSCALL_BASE

(添加调用函数名)
Linux\arch\arm\kernel\calls.S

(添加调用函数声明)
Linux\arch\arm\kernel\calls.S

(添加调用函数实现部分)
SYSCALL_DEFINE0(fork)
{
#ifdef CONFIG_MMU
	return do_fork(SIGCHLD, 0, 0, NULL, NULL);
#else
	/* can not support in nommu mode */
	return -EINVAL;
#endif
}


// ==================================================
(Linux\include\linux\syscalls.h)
#define SYSCALL_DEFINE0(sname)					\
	SYSCALL_METADATA(_##sname, 0);				\
	asmlinkage long sys_##sname(void)

SYSCALL_DEFINE0(sname)
=	SYSCALL_METADATA(_##sname, 0);
	asmlinkage long sys_##sname(void)
	
#define SYSCALL_METADATA(sname, nb, ...)			\
	static const char *types_##sname[] = {			\
		__MAP(nb,__SC_STR_TDECL,__VA_ARGS__)		\
	};							\
	static const char *args_##sname[] = {			\
		__MAP(nb,__SC_STR_ADECL,__VA_ARGS__)		\
	};							\
	SYSCALL_TRACE_ENTER_EVENT(sname);			\
	SYSCALL_TRACE_EXIT_EVENT(sname);			\
	static struct syscall_metadata __used			\
	  __syscall_meta_##sname = {				\
		.name 		= "sys"#sname,			\
		.syscall_nr	= -1,	/* Filled in at boot */	\
		.nb_args 	= nb,				\
		.types		= nb ? types_##sname : NULL,	\
		.args		= nb ? args_##sname : NULL,	\
		.enter_event	= &event_enter_##sname,		\
		.exit_event	= &event_exit_##sname,		\
		.enter_fields	= LIST_HEAD_INIT(__syscall_meta_##sname.enter_fields), \
	};							\
	static struct syscall_metadata __used			\
	  __attribute__((section("__syscalls_metadata")))	\
	 *__p_syscall_meta_##sname = &__syscall_meta_##sname;
	
SYSCALL_METADATA(sname, nb, ...)
=	static const char *types_##sname[] = {
		__MAP(nb,__SC_STR_TDECL,__VA_ARGS__)
	};							
	static const char *args_##sname[] = {
		__MAP(nb,__SC_STR_ADECL,__VA_ARGS__)
	};							
	SYSCALL_TRACE_ENTER_EVENT(sname);
	SYSCALL_TRACE_EXIT_EVENT(sname);
	static struct syscall_metadata 	__used	  
	__syscall_meta_##sname = {
		.name 		= "sys"#sname,
		.syscall_nr	= -1,							/* Filled in at boot */	\
		.nb_args 	= nb,
		.types		= nb ? types_##sname : NULL,
		.args		= nb ? args_##sname : NULL,
		.enter_event	= &event_enter_##sname,
		.exit_event	= &event_exit_##sname,
		.enter_fields	= LIST_HEAD_INIT(__syscall_meta_##sname.enter_fields),
	};	
	static struct syscall_metadata __used	  __attribute__((section("__syscalls_metadata")))
	 *__p_syscall_meta_##sname = &__syscall_meta_##sname;		

#define SYSCALL_TRACE_ENTER_EVENT(sname)				\
	static struct syscall_metadata __syscall_meta_##sname;		\
	static struct ftrace_event_call __used				\
	  event_enter_##sname = {					\
		.class			= &event_class_syscall_enter,	\
		{							\
			.name                   = "sys_enter"#sname,	\
		},							\
		.event.funcs            = &enter_syscall_print_funcs,	\
		.data			= (void *)&__syscall_meta_##sname,\
		.flags                  = TRACE_EVENT_FL_CAP_ANY,	\
	};								\
	static struct ftrace_event_call __used				\
	  __attribute__((section("_ftrace_events")))			\
	 *__event_enter_##sname = &event_enter_##sname;

SYSCALL_TRACE_ENTER_EVENT(sname)
=	static struct syscall_metadata __syscall_meta_##sname;
	static struct ftrace_event_call __used
	  event_enter_##sname = {
		.class			= &event_class_syscall_enter,
		{						
			.name                   = "sys_enter"#sname,
		},							\
		.event.funcs            = &enter_syscall_print_funcs,
		.data			= (void *)&__syscall_meta_##sname,
		.flags                  = TRACE_EVENT_FL_CAP_ANY,
	};	
	static struct ftrace_event_call __used	 __attribute__((section("_ftrace_events")))
	 *__event_enter_##sname = &event_enter_##sname;


#define SYSCALL_TRACE_EXIT_EVENT(sname)					\
	static struct syscall_metadata __syscall_meta_##sname;		\
	static struct ftrace_event_call __used				\
	  event_exit_##sname = {					\
		.class			= &event_class_syscall_exit,	\
		{							\
			.name                   = "sys_exit"#sname,	\
		},							\
		.event.funcs		= &exit_syscall_print_funcs,	\
		.data			= (void *)&__syscall_meta_##sname,\
		.flags                  = TRACE_EVENT_FL_CAP_ANY,	\
	};								\
	static struct ftrace_event_call __used				\
	  __attribute__((section("_ftrace_events")))			\
	*__event_exit_##sname = &event_exit_##sname;


// =====================================
SYSCALL_DEFINE0(fork)
{
#ifdef CONFIG_MMU
	return do_fork(SIGCHLD, 0, 0, NULL, NULL);
#else
	/* can not support in nommu mode */
	return -EINVAL;
#endif
}


// =====================================
struct syscall_metadata {
	const char	*name;
	int		syscall_nr;
	int		nb_args;
	const char	**types;
	const char	**args;
	struct list_head enter_fields;

	struct ftrace_event_call *enter_event;
	struct ftrace_event_call *exit_event;
};

struct ftrace_event_call {
	struct list_head	list;
	struct ftrace_event_class *class;
	union {
		char			*name;
		/* Set TRACE_EVENT_FL_TRACEPOINT flag when using "tp" */
		struct tracepoint	*tp;
	};
	struct trace_event	event;
	char			*print_fmt;
	struct event_filter	*filter;
	void			*mod;
	void			*data;
	/*
	 *   bit 0:		filter_active
	 *   bit 1:		allow trace by non root (cap any)
	 *   bit 2:		failed to apply filter
	 *   bit 3:		ftrace internal event (do not enable)
	 *   bit 4:		Event was enabled by module
	 *   bit 5:		use call filter rather than file filter
	 *   bit 6:		Event is a tracepoint
	 */
	int			flags; /* static flags of different events */

#ifdef CONFIG_PERF_EVENTS
	int				perf_refcount;
	struct hlist_head __percpu	*perf_events;
	struct bpf_prog			*prog;

	int	(*perf_perm)(struct ftrace_event_call *,
			     struct perf_event *);
#endif
};