pwd_path=$(shell pwd)
GCC=${pwd_path}/../gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
GCC1=gcc

arm:robe_app_arm

.PHONY: robe_app_arm
robe_app_arm:
	rm -rf $@
	${GCC} -g -o $@ robe_app.c

x86:x86_64

x86_64:robe_app_86_64

.PHONY: robe_app_86_64
robe_app_86_64:
	rm -rf $@
	${GCC1} -o $@ robe_app.c -O0



