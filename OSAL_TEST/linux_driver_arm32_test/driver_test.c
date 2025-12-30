
#include "d_os.h"
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>

#define CHRDEVBASE_MAJOR 200
#define CHRDEVBASE_NAME  "chrdev"

static char readbuf[100];
static char writebuf[100];
static char kerneldata[] = {"kernel data!"};

static int chrdevbase_open(struct inode *inode, struct file *filp)
{
  printk("chrdev open!\r\n");

  return 0;
}

static ssize_t chrdevbase_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
  int retvalue = 0;
  memcpy(readbuf, kerneldata, sizeof(kerneldata));
  retvalue = copy_to_user(buf, readbuf, cnt);
  if (retvalue == 0)
    printk("kernel senddata ok!\r\n");
  else
    printk("kernel senddata failed!\r\n");

  return 0;
}

static ssize_t chrdevbase_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
  int retvalue = 0;
  retvalue = copy_from_user(writebuf, buf, cnt);
  if (retvalue == 0)
    printk("kernel recevdata:%s\r\n", writebuf);
  else
    printk("kernel recevdata failed!\r\n");

  return 0;
}

static int chrdevbase_release(struct inode *inode, struct file *filp)
{
  return 0;
}

static struct file_operations chrdevbase_fops = {
    .owner = THIS_MODULE,
    .open = chrdevbase_open,
    .read = chrdevbase_read,
    .write = chrdevbase_write,
    .release = chrdevbase_release,
};

static int __init chrdevbase_init(void)
{
  int retvalue = 0;

  retvalue = register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);
  if (retvalue < 0)
    printk("chrdev driver register failed\r\n");

  DOS_Init();

  printk("chrdev init!\r\n");
  return 0;
}

static void __exit chrdevbase_exit(void)
{
  DOS_Exit();
  unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);

  printk("chrdev exit!\r\n");
}

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ALIENTEK");
MODULE_INFO(intree, "Y");