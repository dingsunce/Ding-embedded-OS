
KERNELDIR := /home/ding/linux-5.15.24
CURRENT_PATH := $(shell pwd)

MODULE_NAME = LINUX_DRIVER_AEM32_TEST

RELATIVE_DIR = ../../..

ccflags-y += -I$(src)/$(RELATIVE_DIR)/DING_OS
ccflags-y += -I$(src)/$(RELATIVE_DIR)/OSAL/linux_kernel
ccflags-y += -I$(src)/$(RELATIVE_DIR)/OSAL_TEST
ccflags-y += -std=gnu99
ccflags-y += -Wno-declaration-after-statement
ccflags-y += -Wno-implicit-fallthrough
ccflags-y += -DOS_DEBUG=1

driver_test_src :=	../driver_test.o

driver_test_src += \
		$(RELATIVE_DIR)/DING_OS/d_mem.o \
		$(RELATIVE_DIR)/DING_OS/d_os.o \
		$(RELATIVE_DIR)/DING_OS/d_task.o \
		$(RELATIVE_DIR)/DING_OS/double_list.o \
		$(RELATIVE_DIR)/DING_OS/d_memb.o \
		$(RELATIVE_DIR)/DING_OS/d_message.o \
		$(RELATIVE_DIR)/DING_OS/d_process.o \
		$(RELATIVE_DIR)/DING_OS/s_list.o \
		$(RELATIVE_DIR)/DING_OS/d_list.o \
		$(RELATIVE_DIR)/DING_OS/d_tick.o \

driver_test_src += \
		$(RELATIVE_DIR)/OSAL/linux_kernel/osal.o

driver_test_src += \
		$(RELATIVE_DIR)/OSAL_TEST/app_ipc_test.o \
		$(RELATIVE_DIR)/OSAL_TEST/app_msg_test.o \

$(MODULE_NAME)-y += $(driver_test_src)

obj-m := $(MODULE_NAME).o



