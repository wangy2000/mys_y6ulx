struct gpio_led_platform_data {
	int 		num_leds;
	const struct gpio_led *leds;

#define GPIO_LED_NO_BLINK_LOW	0	/* No blink GPIO state low */
#define GPIO_LED_NO_BLINK_HIGH	1	/* No blink GPIO state high */
#define GPIO_LED_BLINK		2	/* Please, blink */
	int		(*gpio_blink_set)(struct gpio_desc *desc, int state,
					unsigned long *delay_on,
					unsigned long *delay_off);
};


struct gpio_leds_priv {
	int num_leds;
	struct gpio_led_data leds[];
};

struct gpio_led_data {
	struct led_classdev cdev;
	struct gpio_desc *gpiod;
	struct work_struct work;
	u8 new_level;
	u8 can_sleep;
	u8 blinking;
	int (*platform_gpio_blink_set)(struct gpio_desc *desc, int state,
			unsigned long *delay_on, unsigned long *delay_off);
};

struct gpio_desc {
	unsigned	valid		: 1;
	unsigned	can_wakeup	: 1;
	unsigned	keypad_gpio	: 1;
	unsigned	dir_inverted	: 1;
	unsigned int	mask; /* bit mask in PWER or PKWR */
	unsigned int	mux_mask; /* bit mask of muxed gpio bits, 0 if no mux */
	unsigned long	config;
};



