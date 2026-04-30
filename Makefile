obj-m += netmodule.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo insmod netmodule.ko

remove:
	sudo rmmod netmodule

info:
	modinfo netmodule.ko