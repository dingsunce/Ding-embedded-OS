
KERNELDIR := /home/ding/linux-5.15.24
CURRENT_PATH := $(shell pwd)

MODULE_NAME = LINUX_DRIVER_AEM32_TEST

ccflags-y += -I$(src)/../../../DING_OS
ccflags-y += -I$(src)/../../../OSAL/linux_kernel
ccflags-y += -std=gnu99
ccflags-y += -Wno-declaration-after-statement

driver_test_src :=	../driver_test.o
driver_test_src += \
		../../../DING_OS/d_mem.o \
		../../../DING_OS/d_os.o \
		../../../DING_OS/d_task.o \
		../../../DING_OS/DList.o \
		../../../DING_OS/memb.o \
		../../../DING_OS/message.o \
		../../../DING_OS/process.o \
		../../../DING_OS/s_list.o \
		../../../DING_OS/SList.o \
		../../../DING_OS/SysTick.o \

driver_test_src += \
		../../../OSAL/linux_kernel/osal.o


$(MODULE_NAME)-y += $(driver_test_src)

obj-m := $(MODULE_NAME).o



