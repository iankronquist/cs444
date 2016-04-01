obj-m := ebd.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.o *.ko *.ko.unsigned modules.order Module.symvers ebd.mod.c .*.cmd
	rm -rf .tmp_versions

.PHONY: all
