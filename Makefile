# Makefile for Scull Driver
# Author: Alex

# Point this variable to the Linux Kernel source tree
KERNEL_DIR=linux-src
obj-m := src/


all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD)
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) $@



