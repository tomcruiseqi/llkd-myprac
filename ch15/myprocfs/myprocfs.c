// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
//
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
#define pr_fmt(fmt) "%s:%s: " fmt, KBUILD_MODNAME, __func__

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("This is my procfs practice");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

struct mymiscdev_ctx {
    int debug_level;
#define DESC_MAX_LEN 128
    char desc[128];
};
static struct mymiscdev_ctx *gmydev_ctx;
static struct miscdevice *gmiscdev;

static int open_mymiscdev(struct inode *inode, struct file *fp) {
    dev_info(gmiscdev->this_device, "opening file.\n");
    return 0;
}

static ssize_t read_mymiscdev(struct file *fp, char __user *ubuf, size_t count,
                              loff_t *off) {
    int desc_len, ret;
    desc_len = strnlen(gmydev_ctx->desc, DESC_MAX_LEN);
    ret = desc_len > count ? count : desc_len;

    if (copy_to_user(ubuf, gmydev_ctx->desc, ret)) {
        dev_err(gmiscdev->this_device, "copy_to_user failed.\n");
        return -EIO;
    }

    return ret;
}

static ssize_t write_mymiscdev(struct file *fp, const char __user *ubuf,
                               size_t count, loff_t *off) {
    int ret = count > DESC_MAX_LEN ? DESC_MAX_LEN : count;
    if (copy_from_user(gmydev_ctx->desc, ubuf, ret)) {
        dev_err(gmiscdev->this_device, "copy_from_user failed.\n");
        return -EIO;
    }
    return ret;
}

static int close_mymiscdev(struct inode *inode, struct file *fp) {
    dev_info(gmiscdev->this_device, "mymisc device closed.\n");
    return 0;
}

static const struct file_operations mymisc_fops = {
    .open = open_mymiscdev,
    .read = read_mymiscdev,
    .write = write_mymiscdev,
    .release = close_mymiscdev,
    .llseek = no_llseek,
};

static struct miscdevice mymiscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mymiscdev",
    .mode = 0644,
    .fops = &mymisc_fops,
};

static int __init myprocfs_init(void) {
    /* First, I need to create a miscdevice. */
    if (misc_register(&mymiscdev)) {
        pr_err("create misc dev failed.\n");
        return -EIO;
    }
    gmiscdev = &mymiscdev;

    /* Create the device config context structure. */
    gmydev_ctx = devm_kzalloc(gmiscdev->this_device,
                              sizeof(struct mymiscdev_ctx), GFP_KERNEL);
    if (unlikely(!gmydev_ctx))
        goto delete_device_out;

    strscpy(gmydev_ctx->desc, "hello world.", DESC_MAX_LEN);
    gmydev_ctx->debug_level = 1;

    dev_info(gmiscdev->this_device, "initialized.");

    return 0;
delete_device_out:
    misc_deregister(gmiscdev);
    return -ENOMEM;
}

static void __exit myprocfs_exit(void) {
    devm_kfree(gmiscdev->this_device, gmydev_ctx);
    misc_deregister(gmiscdev);
    pr_info("removed.");
}

module_init(myprocfs_init);
module_exit(myprocfs_exit);
