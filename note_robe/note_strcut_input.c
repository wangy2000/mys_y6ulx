//  =======================================================  (pdata)
struct gpio_keys_platform_data {
	struct gpio_keys_button *buttons;//ok
	int nbuttons;		//ok
	unsigned int poll_interval;
	unsigned int rep:1;		//ok
	int (*enable)(struct device *dev);
	void (*disable)(struct device *dev);
	const char *name;
};
//                                                             (button)
struct gpio_keys_button {
	unsigned int code;		//ok
	int gpio;		//ok
	int active_low;		//ok
	const char *desc;		//ok
	unsigned int type;		//ok
	int wakeup;		//ok
	int debounce_interval;		//ok
	bool can_disable;		//ok
	int value;
	unsigned int irq;		//ok
	struct gpio_desc *gpiod;
};
//  ========================================================  (ddata)
struct gpio_keys_drvdata {
	const struct gpio_keys_platform_data *pdata;//OK
	struct input_dev *input;				//OK
	struct mutex disable_lock;				//OK
	struct gpio_button_data data[0];
};
//                                                             (bdata)
struct gpio_button_data {
	const struct gpio_keys_button *button;//ok
	struct input_dev *input;   //ok

	struct timer_list release_timer;  //ok 
	unsigned int release_delay;	//ok  /* in msecs, for IRQ-only buttons */

	struct delayed_work work;  //gpio_keys_gpio_work_func
	unsigned int software_debounce;	//ok /* in msecs, for GPIO-driven buttons */
 
	unsigned int irq;  //ok			gpio_keys_gpio_isr
	spinlock_t lock;    //ok
	bool disabled;
	bool key_pressed;
};
//========================================================
struct gpio_desc {
	struct gpio_chip	*chip;
	unsigned long		flags;
/* flag symbols are bit numbers */
#define FLAG_REQUESTED	0
#define FLAG_IS_OUT	1
#define FLAG_EXPORT	2	/* protected by sysfs_lock */
#define FLAG_SYSFS	3	/* exported via /sys/class/gpio/control */
#define FLAG_TRIG_FALL	4	/* trigger on falling edge */
#define FLAG_TRIG_RISE	5	/* trigger on rising edge */
#define FLAG_ACTIVE_LOW	6	/* value has active low */
#define FLAG_OPEN_DRAIN	7	/* Gpio is open drain type */
#define FLAG_OPEN_SOURCE 8	/* Gpio is open source type */
#define FLAG_USED_AS_IRQ 9	/* GPIO is connected to an IRQ */
#define FLAG_SYSFS_DIR	10	/* show sysfs direction attribute */
#define FLAG_IS_HOGGED	11	/* GPIO is hogged */

#define ID_SHIFT	16	/* add new flags before this one */

#define GPIO_FLAGS_MASK		((1 << ID_SHIFT) - 1)
#define GPIO_TRIGGER_MASK	(BIT(FLAG_TRIG_FALL) | BIT(FLAG_TRIG_RISE))

	const char		*label;
};

//  struct devres    ====================================================
struct devres {
	struct devres_node		node;
	/* -- 3 pointers */
	unsigned long long		data[];	/* guarantee ull alignment */
};

//  struct devres_node    ====================================================
struct devres_node {
	struct list_head		entry;
	dr_release_t			release;
#ifdef CONFIG_DEBUG_DEVRES
	const char			*name;
	size_t				size;
#endif
};

//  struct input_devres    ====================================================
struct input_devres {
	struct input_dev *input;
};


//========================================================
struct input_dev {
	const char *name;		//ok
	const char *phys;		//ok
	const char *uniq;
	struct input_id id;		//ok

	unsigned long propbit[BITS_TO_LONGS(INPUT_PROP_CNT)];

	unsigned long evbit[BITS_TO_LONGS(EV_CNT)];
	unsigned long keybit[BITS_TO_LONGS(KEY_CNT)];
	unsigned long relbit[BITS_TO_LONGS(REL_CNT)];
	unsigned long absbit[BITS_TO_LONGS(ABS_CNT)];
	unsigned long mscbit[BITS_TO_LONGS(MSC_CNT)];
	unsigned long ledbit[BITS_TO_LONGS(LED_CNT)];
	unsigned long sndbit[BITS_TO_LONGS(SND_CNT)];
	unsigned long ffbit[BITS_TO_LONGS(FF_CNT)];
	unsigned long swbit[BITS_TO_LONGS(SW_CNT)];

	unsigned int hint_events_per_packet;

	unsigned int keycodemax;
	unsigned int keycodesize;
	void *keycode;

	int (*setkeycode)(struct input_dev *dev,
			  const struct input_keymap_entry *ke,
			  unsigned int *old_keycode);
	int (*getkeycode)(struct input_dev *dev,
			  struct input_keymap_entry *ke);

	struct ff_device *ff;

	unsigned int repeat_key;
	struct timer_list timer;

	int rep[REP_CNT];

	struct input_mt *mt;

	struct input_absinfo *absinfo;

	unsigned long key[BITS_TO_LONGS(KEY_CNT)];
	unsigned long led[BITS_TO_LONGS(LED_CNT)];
	unsigned long snd[BITS_TO_LONGS(SND_CNT)];
	unsigned long sw[BITS_TO_LONGS(SW_CNT)];

	int (*open)(struct input_dev *dev);
	void (*close)(struct input_dev *dev);
	int (*flush)(struct input_dev *dev, struct file *file);
	int (*event)(struct input_dev *dev, unsigned int type, unsigned int code, int value);

	struct input_handle __rcu *grab;

	spinlock_t event_lock;
	struct mutex mutex;

	unsigned int users;
	bool going_away;

	struct device dev;		//dev.type, dev.class  

	struct list_head	h_list;
	struct list_head	node;

	unsigned int num_vals;
	unsigned int max_vals;
	struct input_value *vals;

	bool devres_managed;
};

//==================================================================
struct input_mt {
	int trkid;
	int num_slots;
	int slot;
	unsigned int flags;
	unsigned int frame;
	int *red;
	struct input_mt_slot slots[];
};

//  ================================================================
struct input_value {
	__u16 type;
	__u16 code;
	__s32 value;
};

//  ================================================================
struct timer_list {
	/*
	 * All fields that change during normal runtime grouped to the
	 * same cacheline
	 */
	struct list_head entry;
	unsigned long expires;
	struct tvec_base *base;

	void (*function)(unsigned long);
	unsigned long data;

	int slack;

#ifdef CONFIG_TIMER_STATS
	int start_pid;
	void *start_site;
	char start_comm[16];
#endif
#ifdef CONFIG_LOCKDEP
	struct lockdep_map lockdep_map;
#endif
};

// =======================================================================
struct input_keymap_entry {
#define INPUT_KEYMAP_BY_INDEX	(1 << 0)
	__u8  flags;
	__u8  len;
	__u16 index;
	__u32 keycode;
	__u8  scancode[32];
};








