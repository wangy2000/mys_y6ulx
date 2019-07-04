
pwd_path=$(pwd)
ARCH=arm
# CROSS_COMPILE=${pwd_path}/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
CROSS_COMPILE=/datadisk/tools/gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
# CROSS_COMPILE=${pwd_path}/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
export ARCH CROSS_COMPILE

uboot_config=mys_imx6ull_14x14_emmc_defconfig
multi_thread=-j

function build_uboot()
{
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  O=${pwd_path}/output ${uboot_config}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  O=${pwd_path}/output ${multi_thread}$1
}

function clean_uboot()
{
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  O=${pwd_path}/output clean
}

function distclean_uboot()
{
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  O=${pwd_path}/output distclean
}

distclean_uboot
time build_uboot 6
read -p "please press enter to continue ..." time
distclean_uboot
time build_uboot 1