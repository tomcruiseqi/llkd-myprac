// Write my first driver.
// Remember this day Oct 9, 2023.
//
// Copyright (c) 2023
//
// Written by qizengtian <qizengtian@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <asm-generic/errno-base.h>
#include <linux/elf-em.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#include "../../convenient.h"
#include <linux/limits.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/version.h>

#define MYMODNAME "mymiscdrv"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("This is my first driver.");
MODULE_LICENSE("GPL-3");
MODULE_VERSION("1.0");

static int open_mymiscdev(struct inode *inode, struct file *filp) {
    char *buf = kzalloc(PATH_MAX, GFP_KERNEL);
    if (unlikely(!buf))
        return -ENOMEM;

    pr_info(" opening \"%s\" now; write open file: f_flags = 0x%x\n",
            file_path(filp, buf, PATH_MAX), filp->f_flags);
    kfree(buf);
    return 0;
}

static ssize_t read_mymiscdev(struct file *filp, char __user *ubuf,
                              size_t count, loff_t *off) {
    size_t bytes_to_read, dev_buf_length = 1024;
    char *dev_buf;
    dev_buf = kzalloc(dev_buf_length, GFP_KERNEL);
    if (unlikely(!dev_buf))
        return -ENOMEM;

    strncpy(dev_buf, "This is my fake misc device data.", dev_buf_length - 1);
    bytes_to_read = strnlen(dev_buf, dev_buf_length);

    if (count < bytes_to_read)
        bytes_to_read = count;

    if (copy_to_user(ubuf, dev_buf, bytes_to_read)) {
        pr_err("failed to copy data from my misc device to userspace.\n");
        goto err;
    }

    pr_info("to read %zd bytes\n", bytes_to_read);
    kfree(dev_buf);
    return bytes_to_read;

err:
    kfree(dev_buf);
    return -EIO;
}

static ssize_t write_mymiscdev(struct file *filp, const char __user *ubuf,
                               size_t count, loff_t *off) {
    pr_info("to write %zd bytes\n", count);
    return count;
}

static int close_mymiscdev(struct inode *inode, struct file *filp) {
    char *buf = kzalloc(PATH_MAX, GFP_KERNEL);
    if (unlikely(!buf))
        return -ENOMEM;
    pr_info("closing \"%s\"\n", file_path(filp, buf, PATH_MAX));
    kfree(buf);

    return 0;
}

static const struct file_operations mymiscdev_fops = {
    .open = open_mymiscdev,
    .read = read_mymiscdev,
    .write = write_mymiscdev,
    .release = close_mymiscdev,
    .llseek = no_llseek,
};

/* First, I need to create the device structure.
 */
static struct miscdevice mymiscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mymiscdev",
    .mode = 0666,
    .fops = &mymiscdev_fops,
};

/* Initializes my driver.
 */
static int __init mymiscdev_init(void) {
    int ret;
    struct device *dev;
    ret = misc_register(&mymiscdev);
    if (0 != ret) {
        pr_notice("misc device registration failed, aborting\n");
        return ret;
    }

    dev = mymiscdev.this_device;
    pr_info("mymiscdev driver (major # 10) registered, minor# = %d"
            " dev node is /dev/%s\n",
            mymiscdev.minor, mymiscdev.name);
    dev_info(dev, "sample dev_info(): minor# = %d\n", mymiscdev.minor);

    return 0;
}

static void __exit mymiscdev_exit(void) {
    misc_deregister(&mymiscdev);
    pr_info("my misc driver deregistered, bye\n");
}

module_init(mymiscdev_init);
module_exit(mymiscdev_exit);
