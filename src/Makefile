# Makefile for Scull Driver
# Author: Alex

KERNEL_DIR=linux-src
obj-m := scull_mod.o
scull_mod-y := scull.o cbuffer.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD)


