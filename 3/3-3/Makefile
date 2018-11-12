SRCS := ftrace_tracer.c
obj-m := process_tracer.o

SYSCALL_ADDRESS = 0x$(subst R sys_call_table,,$(shell grep sys_call_table /boot/System.map-$(shell uname -r)))
CFLAGS_process_tracer.o += -DSYSCALL_TABLE=$(SYSCALL_ADDRESS)

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

