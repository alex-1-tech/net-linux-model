obj-m += netmodule.o

BEAR := $(shell command -v bear 2>/dev/null)
BUILDDIR := /lib/modules/$(shell uname -r)/build

all:
ifdef BEAR
	@echo "Building with bear"
	bear -- make _build
else
	@echo "bear not installed, building without bear"
	make _build
endif

_build:
	make -C $(BUILDDIR) M=$(PWD) modules

clean:
	make -C $(BUILDDIR) M=$(PWD) clean

install:
	sudo insmod netmodule.ko

remove:
	sudo rmmod netmodule

info:
	modinfo netmodule.ko

test-setup:
	sudo ip link set netmodule0 up
	sudo ip addr add 10.0.0.1/24 dev netmodule0
	@echo "Interface configured:"
	ip addr show netmodule0

test-ping:
	ping -c 4 10.0.0.1

test-clean:
	sudo ip addr del 10.0.0.1/24 dev netmodule0 2>/dev/null || true
	sudo ip link set netmodule0 down 2>/dev/null || true

test-proc:
	@echo "Writing IP via /proc"
	echo "10.0.0.2" | sudo tee /proc/netmodule_ip
	@echo "Result:"
	ip addr show netmodule0

test-all: install test-setup test-proc test-ping

test-reset: test-clean remove