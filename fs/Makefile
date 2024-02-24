MOD := ss0fs
obj-m += $(MOD).o
ss0fs-objs := super.o fs_context.o

KLIB :=/lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)

all:
	$(MAKE) -C $(KLIB) M=$(PWD) modules

clean:
	$(MAKE) -C $(KLIB) M=$(PWD) clean

insmod: all
	sudo rmmod $(MOD).ko; true
	sudo insmod $(MOD).ko

log:
	sudo dmesg

log-clear:
	sudo dmesg -C

test: insmod log-clear
	sudo mkdir -p /mnt/test; \
	sudo mount -t $(MOD) test /mnt/test; \
	sudo dmesg
