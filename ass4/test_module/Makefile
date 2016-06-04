obj-m := ff_slob_stability.o
KDIR := ../../linux-yocto-3.14

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


clean:
	rm -f *.o *.ko *.ko.unsigned modules.order Module.symvers ff_slob_stability.mod.c .*.cmd

.PHONY: all clean
