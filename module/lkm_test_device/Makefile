obj-m += lkm_test.o

KERNEL := /usr/src/linux-headers-4.9.0-8-686/

all:
	make -C $(KERNEL) M=$(shell pwd) modules

clean:
	make -C $(KERNEL) M=$(shell pwd) clean

install:
	insmod lkm_test.ko

uninstall:
	rmmod lkm_test