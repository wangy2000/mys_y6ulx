//  ====================================
(Linux\include\generated\mach-types.h)				// 自动生成
#define MACH_TYPE_EBSA110              0
#define MACH_TYPE_RISCPC               1
#define MACH_TYPE_EBSA285              4
#define MACH_TYPE_NETWINDER            5
#define MACH_TYPE_CATS                 6
#define MACH_TYPE_SHARK                15
#define MACH_TYPE_BRUTUS               16

(linux/arch/arm/tools/mach-types) 					// 源文件
# machine_is_xxx	CONFIG_xxxx		MACH_TYPE_xxx		number
#
ebsa110			ARCH_EBSA110		EBSA110			0
riscpc			ARCH_RPC			RISCPC			1
ebsa285			ARCH_EBSA285		EBSA285			4
netwinder		ARCH_NETWINDER		NETWINDER		5
cats			ARCH_CATS			CATS			6
shark			ARCH_SHARK			SHARK			15
brutus			SA1100_BRUTUS		BRUTUS			16

(https://www.arm.linux.org.uk/developer/machines/download.php)		// 网络更新
# machine_is_xxx	CONFIG_xxxx		MACH_TYPE_xxx		number
#
ebsa110			ARCH_EBSA110		EBSA110			0
riscpc			ARCH_RPC			RISCPC			1
nexuspci		ARCH_NEXUSPCI		NEXUSPCI		3
ebsa285			ARCH_EBSA285		EBSA285			4
netwinder		ARCH_NETWINDER		NETWINDER		5
cats			ARCH_CATS			CATS			6
tbox			ARCH_TBOX			TBOX			7
co285			ARCH_CO285			CO285			8
clps7110		ARCH_CLPS7110		CLPS7110		9
archimedes		ARCH_ARC			ARCHIMEDES		10
a5k				ARCH_A5K			A5K				11
etoile			ARCH_ETOILE			ETOILE			12
lacie_nas		ARCH_LACIE_NAS		LACIE_NAS		13
clps7500		ARCH_CLPS7500		CLPS7500		14
shark			ARCH_SHARK			SHARK			15
brutus			SA1100_BRUTUS		BRUTUS			16


__machine_arch_type 						// 全局变量

machine_arch_type							// 宏定义

//  ================================




