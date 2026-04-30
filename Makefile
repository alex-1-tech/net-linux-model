obj-m += netmodule.o

BEAR := $(shell command -v bear 2>/dev/null)

all:
ifdef BEAR
	@echo "Building with bear"
	bear -- make _build
else
	@echo "bear not installed, building without bear"
	make _build
endif

_build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo insmod netmodule.ko

remove:
	sudo rmmod netmodule

info:
	modinfo netmodule.ko