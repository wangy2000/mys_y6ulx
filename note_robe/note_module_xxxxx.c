/* module_i2c_driver */
	(include/linux/i2c.h):
	#define module_i2c_driver(__i2c_driver) \
		module_driver(__i2c_driver, i2c_add_driver, \
				i2c_del_driver)
				
	module_i2c_driver(__i2c_driver)=
		module_driver(__i2c_driver, i2c_add_driver,i2c_del_driver)

	(include/linux/device.h):	
	#define module_driver(__driver, __register, __unregister, ...) \
	static int __init __driver##_init(void) \
	{ \
		return __register(&(__driver) , ##__VA_ARGS__); \
	} \
	module_init(__driver##_init); \
	static void __exit __driver##_exit(void) \
	{ \
		__unregister(&(__driver) , ##__VA_ARGS__); \
	} \
	module_exit(__driver##_exit);

	module_i2c_driver(__i2c_driver)=
		module_driver(__i2c_driver, i2c_add_driver,i2c_del_driver)
	module_i2c_driver(__i2c_driver)=
		static int __init __driver##_init(void) 
		{
			return __register(&(__driver) , ##__VA_ARGS__);
		}
		module_init(__driver##_init);
		static void __exit __driver##_exit(void) 
		{ 
			__unregister(&(__driver) , ##__VA_ARGS__);
		} 
		module_exit(__driver##_exit);

/* MODULE */
	(include/linux/device.h):	
	MODULE_LICENSE
	MODULE_AUTHOR
	MODULE_DESCRIPTION
	MODULE_ALIAS
	MODULE_SOFTDEP
	MODULE_FIRMWARE
	MODULE_DEVICE_TABLE
	MODULE_VERSION
		
/* MODULE_LICENSE */
	(include/linux/device.h):	
	#define MODULE_LICENSE(_license) MODULE_INFO(license, _license)
	static char license[128];
	MODULE_LICENSE("GPL v2")=
		MODULE_INFO(license, _license)
		
	#define MODULE_INFO(tag, info) __MODULE_INFO(tag, tag, info)
	MODULE_LICENSE("GPL v2")=
		MODULE_INFO(license, _license)
	MODULE_LICENSE("GPL v2")=
		__MODULE_INFO(tag, tag, info)
	
	(include/linux/moduleparam.h)
	#define __MODULE_INFO(tag, name, info)					  \
	static const char __UNIQUE_ID(name)[]					  \
		  __used __attribute__((section(".modinfo"), unused, aligned(1)))	  \
		  = __stringify(tag) "=" info
	MODULE_LICENSE("GPL v2")=
		MODULE_INFO(license, _license)
	MODULE_LICENSE("GPL v2")=
		__MODULE_INFO(tag, tag, info)	
	MODULE_LICENSE("GPL v2")=
		static const char __UNIQUE_ID(name)[] __used __attribute__((section(".modinfo"), unused, aligned(1)))
											= __stringify(tag) "=" info
	
	(include/linux/stringify.h)
	#define __stringify_1(x...)	#x
	#define __stringify(x...)	__stringify_1(x)
	MODULE_LICENSE("GPL v2")=
		license = "GPL v2"
	
/* MODULE_AUTHOR */
	(include/linux/device.h):
	#define MODULE_AUTHOR(_author) MODULE_INFO(author, _author)
	MODULE_AUTHOR("Benjamin Tissoires <benjamin.tissoires@gmail.com>")=
		MODULE_INFO(author, _author)
		author = "Benjamin Tissoires <benjamin.tissoires@gmail.com>"

/* MODULE_DESCRIPTION */		
	(include/linux/device.h):
	#define MODULE_DESCRIPTION(_description) MODULE_INFO(description, _description)
	MODULE_DESCRIPTION("Goodix touchscreen driver")=
		MODULE_INFO(description, _description)
		description = "Goodix touchscreen driver"

/* MODULE_ALIAS */
	(include/linux/device.h):
	#define MODULE_ALIAS(_alias) MODULE_INFO(alias, _alias)
	MODULE_ALIAS("abc")=
		MODULE_INFO(alias, _alias)
		alias = "abc"

/* MODULE_SOFTDEP */
	(include/linux/device.h):
	#define MODULE_SOFTDEP(_softdep) MODULE_INFO(softdep, _softdep)
	MODULE_SOFTDEP("abc")=
		MODULE_INFO(softdep, _softdep)
		softdep = "abc"
				
/* MODULE_FIRMWARE */
	(include/linux/device.h):
	#define MODULE_FIRMWARE(_firmware) MODULE_INFO(firmware, _firmware)
	MODULE_FIRMWARE("abc")=
		MODULE_INFO(firmware, _firmware)
		firmware = "abc"
		
/* MODULE_DEVICE_TABLE */
	(include/linux/device.h):
	#define MODULE_DEVICE_TABLE(type, name)					\
	extern const typeof(name) __mod_##type##__##name##_device_table		\
	  __attribute__ ((unused, alias(__stringify(name))))
	MODULE_DEVICE_TABLE(of, goodix_of_match)=
		extern const typeof(name) __mod_##type##__##name##_device_table		\
								__attribute__ ((unused, alias(__stringify(name))))
	MODULE_DEVICE_TABLE(of, goodix_of_match)=
		extern const typeof(goodix_of_match) __mod_of__goodix_of_match_device_table \
								__attribute__ ((unused, alias(__stringify(goodix_of_match))))
	
/* MODULE_VERSION */
	(include/linux/device.h):	
	#if defined(MODULE) || !defined(CONFIG_SYSFS)
	#define MODULE_VERSION(_version) MODULE_INFO(version, _version)
	#else
	#define MODULE_VERSION(_version)					\
		static struct module_version_attribute ___modver_attr = {	\
			.mattr	= {						\
				.attr	= {					\
					.name	= "version",			\
					.mode	= S_IRUGO,			\
				},						\
				.show	= __modver_version_show,		\
			},							\
			.module_name	= KBUILD_MODNAME,			\
			.version	= _version,				\
		};								\
		static const struct module_version_attribute			\
		__used __attribute__ ((__section__ ("__modver")))		\
		* __moduleparam_const __modver_attr = &___modver_attr
	#endif		
	MODULE_VERSION("abc") = 
		version = "abc"
	
	/* MODULE_VERSION2 */
	MODULE_VERSION("abc") = 
		static struct module_version_attribute ___modver_attr = {
				.mattr	= {	
						.attr	= {
							.name	= "version",
							.mode	= S_IRUGO,
							},
						.show	= __modver_version_show,
				},
				.module_name	= KBUILD_MODNAME,
				.version	= _version,
		};
		static const struct module_version_attribute __used __attribute__ ((__section__ ("__modver")))
				* __moduleparam_const __modver_attr = &___modver_attr
	
	(include/linux/moduleparam.h)
	#define __moduleparam_const const
	MODULE_VERSION("abc") = 
		static struct module_version_attribute ___modver_attr = {
				.mattr	= {	
						.attr	= {
							.name	= "version",
							.mode	= S_IRUGO,
							},
						.show	= __modver_version_show,
				},
				.module_name	= KBUILD_MODNAME,
				.version	= "abc",
		};
		static const struct module_version_attribute __used __attribute__ ((__section__ ("__modver")))
				* const __modver_attr = &___modver_attr
	
	
