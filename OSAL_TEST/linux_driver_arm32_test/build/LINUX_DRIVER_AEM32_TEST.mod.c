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
	{ 0x2b68bd2f, "del_timer" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0xf1ceac2b, "__register_chrdev" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x7e423ba3, "mutex_unlock" },
	{ 0xc1cc3aa8, "kthread_create_on_node" },
	{ 0xda02d67, "jiffies" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0x5f754e5a, "memset" },
	{ 0x37befc70, "jiffies_to_msecs" },
	{ 0x8a01a639, "__mutex_init" },
	{ 0x69fe1cb, "kthread_stop" },
	{ 0xe8371cdf, "mutex_lock" },
	{ 0xcea899ea, "down" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x92997ed8, "_printk" },
	{ 0x3b49451e, "wake_up_process" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0x364b3fff, "up" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0x1e9dd9fb, "down_timeout" },
};

MODULE_INFO(depends, "");

