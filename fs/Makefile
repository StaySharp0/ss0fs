
obj-m      := ss0fs.o
ss0fs-objs := super.o

ccflags-y  += -I$(src)/include
KLIB       := /lib/modules/$(shell uname -r)/build
PWD        := $(shell pwd)

all:
	$(MAKE) -C $(KLIB) M=$(PWD) modules

clean:
	$(MAKE) -C $(KLIB) M=$(PWD) clean

log:
	sudo dmesg

log-clear:
	sudo dmesg -C

test: all
	sudo insmod ss0fs.ko dyndbg="+p"; \
	sudo rmmod ss0fs.ko;  \
	sudo dmesg
