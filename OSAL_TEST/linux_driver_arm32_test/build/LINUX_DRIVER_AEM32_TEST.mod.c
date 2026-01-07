#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x723dfb7b, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xf9a482f9, "msleep" },
	{ 0xc0025372, "hrtimer_forward" },
	{ 0xbe1b7efd, "hrtimer_cancel" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0xf1ceac2b, "__register_chrdev" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x7e423ba3, "mutex_unlock" },
	{ 0xc1cc3aa8, "kthread_create_on_node" },
	{ 0xda02d67, "jiffies" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0xd49bd803, "wait_for_completion" },
	{ 0x5b94bd36, "hrtimer_start_range_ns" },
	{ 0x5f754e5a, "memset" },
	{ 0x37befc70, "jiffies_to_msecs" },
	{ 0x8a01a639, "__mutex_init" },
	{ 0xe8371cdf, "mutex_lock" },
	{ 0xcea899ea, "down" },
	{ 0x3ea1b6e4, "__stack_chk_fail" },
	{ 0x92997ed8, "_printk" },
	{ 0xa0cc755f, "sched_setattr_nocheck" },
	{ 0x3b49451e, "wake_up_process" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0x4848926, "hrtimer_init" },
	{ 0x364b3fff, "up" },
	{ 0xf9dd9922, "__init_swait_queue_head" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x9cbcaf26, "complete" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0x1e9dd9fb, "down_timeout" },
};

MODULE_INFO(depends, "");

