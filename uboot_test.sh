#!/bin/bash

# var
pwd_path=$(pwd)
uboot_dir=${pwd_path}/u-boot-2019.04
uboot_config=mx6ull_14x14_evk_defconfig
uboot_file=${uboot_dir}/u-boot-dtb.imx
# uboot_file=${uboot_dir}/u-boot.imx

output=output
verbose=2

robe_uboot_config=robe_mys_y6ulx_defconfig

robe_output=
robe_verbose=2

sd_dev=/dev/sdb

# env
ARCH=arm
# CROSS_COMPILE=${pwd_path}/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
CROSS_COMPILE=${pwd_path}/gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
# CROSS_COMPILE=${pwd_path}/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
export ARCH CROSS_COMPILE

# setting
multi_thread=-j6

# BSP function
# notice
function echo_g()
{
	echo -ne "\e[32m"
	echo -n $*
	echo -e "\e[0m"
}

function echo_r()
{
	echo -ne "\e[31m"
	echo -n $*
	echo -e "\e[0m"
}

function echo_w()
{
	echo -ne "\e[0m"
	echo -n $*
	echo -e "\e[0m"
}

function echo_g_n()
{
	echo -ne "\e[32m"
	echo -n $*
	echo -ne "\e[0m"
}

function echo_r_n()
{
	echo -ne "\e[31m"
	echo -n $*
	echo -ne "\e[0m"
}

function echo_w_n()
{
	echo -ne "\e[0m"
	echo -n $*
	echo -ne "\e[0m"
}

function notice_pause_g()
{
	echo_g_n $*
	echo_g_n " press ENTER to continue!"
	read
}

function notice_pause_r()
{
	echo_r_n $*
	echo_r_n " press ENTER to continue!"
	read
}

function notice_pause_w()
{
	echo_w_n $*
	echo_w_n " press ENTER to continue!"
	read
}

# uboot build
function uboot_distclean()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} distclean
	echo_g "uboot distclean OK!"
	popd
}

function uboot_build()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  ${uboot_config}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  ${multi_thread}
	echo_g "build uboot OK!"
	popd
}

# uboot test build
function uboot_distclean_output_verbose()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} distclean O=${output} V=${verbose}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper O=${output} V=${verbose}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} distclean
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
	echo_g "uboot distclean OK!"
	popd
}

function uboot_build_output_verbose()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  ${uboot_config} O=${output} V=${verbose}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  O=${output} V=${verbose} ${multi_thread} 
	echo_g "build uboot OK!"
	popd
}

# robe porting build
function robe_uboot_config_output_verbose()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  ${robe_uboot_config}  O=${robe_output} V=${robe_verbose} && \
	echo_g "robe uboot config OK!"
	popd
}

function robe_uboot_build_output_verbose()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}  O=${robe_output} V=${robe_verbose} ${multi_thread}  && \
	echo_g "robe uboot build OK!"
	popd
}

function robe_uboot_clean_output_verbose()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} clean O=${robe_output} V=${robe_verbose}
	echo_g "robe uboot clean OK!"
	popd
}

function robe_uboot_mrproper_output_verbose()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper O=${robe_output} V=${robe_verbose}
	echo_g "robe uboot mrproper OK!"
	popd
}

function robe_uboot_distclean_output_verbose()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} distclean O=${robe_output} V=${robe_verbose}
	echo_g "robe uboot distclean OK!"
	popd
}

function robe_uboot_menuconfig_output_verbose()
{
	pushd ${uboot_dir}
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} menuconfig O=${robe_output} V=${robe_verbose}
	echo_g "robe uboot menuconfig OK!"
	popd
}

function sdcard_burn_uboot()	
{
	sudo dd if=${uboot_file} of=${sd_dev}  bs=1k seek=1
}


# ================
# uboot_distclean
# uboot_build
# uboot_test_distclean
# uboot_test_build
# robe_uboot_config_output_verbose
# robe_uboot_build_output_verbose
# robe_uboot_clean_output_verbose
# robe_uboot_mrproper_output_verbose
# robe_uboot_distclean_output_verbose
# robe_uboot_menuconfig_output_verbose
# sdcard_burn_uboot

# =================================================USER INTERFACES
# onekey build menu. CLI UI
function menu()
{
	echo_w [1] uboot_distclean
	echo_w [2] uboot_build
	echo_w [3] uboot_distclean_output_verbose
	echo_w [4] uboot_build_output_verbose
	echo_w [5] robe_uboot_config_output_verbose
	echo_w [6] robe_uboot_build_output_verbose
	echo_w [7] robe_uboot_clean_output_verbose
	echo_w [8] robe_uboot_mrproper_output_verbose
	echo_w [9] robe_uboot_distclean_output_verbose
	echo_w [a] robe_uboot_menuconfig_output_verbose
	echo_w [b] sdcard_burn_uboot
}

function do_something()
{
	menu
	read -p "please select: " ANSWER
	if [[ $ANSWER == "1" ]]; then uboot_distclean;fi
	if [[ $ANSWER == "2" ]]; then uboot_build;fi
	if [[ $ANSWER == "3" ]]; then uboot_distclean_output_verbose;fi
	if [[ $ANSWER == "4" ]]; then uboot_build_output_verbose;fi
	if [[ $ANSWER == "5" ]]; then robe_uboot_config_output_verbose;fi
	if [[ $ANSWER == "6" ]]; then robe_uboot_build_output_verbose;fi
	if [[ $ANSWER == "7" ]]; then robe_uboot_clean_output_verbose;fi
	if [[ $ANSWER == "8" ]]; then robe_uboot_mrproper_output_verbose;fi
	if [[ $ANSWER == "9" ]]; then robe_uboot_distclean_output_verbose;fi
	if [[ $ANSWER == "a" ]]; then robe_uboot_menuconfig_output_verbose;fi
	if [[ $ANSWER == "b" ]]; then sdcard_burn_uboot;fi
}

do_something

