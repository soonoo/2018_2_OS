SRCS := ftracehooking.c iotracehooking.c
obj-m := ftracehooking.o iotracehooking.o

SYSCALL_ADDRESS = 0x$(subst R sys_call_table,,$(shell grep sys_call_table /boot/System.map-$(shell uname -r)))
CFLAGS_iotracehooking.o += -DSYSCALL_TABLE=$(SYSCALL_ADDRESS)
CFLAGS_ftracehooking.o += -DSYSCALL_TABLE=$(SYSCALL_ADDRESS)

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
