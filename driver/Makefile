

#PWD_DIR=/datadisk/dev_mys_y6ulx/driver
PWD_DIR=$(shell pwd)
KERN_DIR=${PWD_DIR}/../MYiR-iMX-Linux

obj-m += robe_driver.o robe_device.o

all:
	make -C ${KERN_DIR} M=${PWD_DIR} modules

.PHONY: clean
clean:
	make -C ${KERN_DIR} M=${PWD_DIR} clean
