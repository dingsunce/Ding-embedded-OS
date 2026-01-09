
KERNELDIR := /home/ding/linux-5.15.24
CURRENT_PATH := $(shell pwd)

MODULE_NAME = LINUX_DRIVER_AEM32_TEST

ccflags-y += -I$(src)/../../../DING_OS
ccflags-y += -I$(src)/../../../OSAL/linux_kernel
ccflags-y += -I$(src)/../../../OSAL_TEST
ccflags-y += -std=gnu99
ccflags-y += -Wno-declaration-after-statement
ccflags-y += -Wno-implicit-fallthrough
ccflags-y += -DOS_DEBUG=1

driver_test_src :=	../driver_test.o

driver_test_src += \
		../../../DING_OS/d_mem.o \
		../../../DING_OS/d_os.o \
		../../../DING_OS/d_task.o \
		../../../DING_OS/double_list.o \
		../../../DING_OS/d_memb.o \
		../../../DING_OS/d_message.o \
		../../../DING_OS/d_process.o \
		../../../DING_OS/s_list.o \
		../../../DING_OS/d_list.o \
		../../../DING_OS/d_tick.o \

driver_test_src += \
		../../../OSAL/linux_kernel/osal.o

driver_test_src += \
		../../../OSAL_TEST/app_ipc_test.o \
		../../../OSAL_TEST/app_msg_test.o \

$(MODULE_NAME)-y += $(driver_test_src)

obj-m := $(MODULE_NAME).o



