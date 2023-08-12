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

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("This is my misc device with procfs support.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

/* Procfs dir and file entry. */
#define PROCFS_ROOT_DIR "mymiscdev"
#define PROCFS_ROOT_DIR_PERMS 0644

#define PROCFS_DEBUG_LEVEL "debug_level"
#define PROCFS_DEBUG_LEVEL_PERMS 0644
DEFINE_MUTEX(g_debug_level_lock);

struct proc_dir_entry *gprocfs_root_dir;

struct mymiscdev_ctx {
    int debug_level;
#define DESC_MAX_LEN 128
    char desc[128];
};
static struct mymiscdev_ctx *gmymiscdev_ctx;
static struct miscdevice *gmiscdev;
DEFINE_MUTEX(g_mymiscdev_ctx_lock);

static int open_mymiscdev(struct inode *inode, struct file *fp) {
    dev_info(gmiscdev->this_device, "opening file.\n");
    return 0;
}

static ssize_t read_mymiscdev(struct file *fp, char __user *ubuf, size_t count,
                              loff_t *off) {
    int desc_len, ret = 0, bytes_read;

    mutex_lock_interruptible(&g_mymiscdev_ctx_lock);
    desc_len = strnlen(gmymiscdev_ctx->desc, DESC_MAX_LEN);
    bytes_read = desc_len > count ? count : desc_len;

    if (copy_to_user(ubuf, gmymiscdev_ctx->desc, bytes_read)) {
        dev_err(gmiscdev->this_device, "copy_to_user failed.\n");
        ret = -EIO;
    }
    ret = bytes_read;

    mutex_unlock(&g_mymiscdev_ctx_lock);
    return ret;
}

static ssize_t write_mymiscdev(struct file *fp, const char __user *ubuf,
                               size_t count, loff_t *off) {
    int ret = 0, bytes_write;
    bytes_write = count > DESC_MAX_LEN ? DESC_MAX_LEN : count;
    if (copy_from_user(gmymiscdev_ctx->desc, ubuf, bytes_write)) {
        dev_err(gmiscdev->this_device, "copy_from_user failed.\n");
        ret = -EIO;
    }
    ret = bytes_write;

    mutex_unlock(&g_mymiscdev_ctx_lock);
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

/* Now, we add the procfs for this device. */
static int mymisc_procfs_show_debug_level(struct seq_file *seq, void *v) {
    if (mutex_lock_interruptible(&g_debug_level_lock))
        return -ERESTARTSYS;
    dev_info(gmiscdev->this_device, "procfs %s open.\n", PROCFS_DEBUG_LEVEL);
    seq_printf(seq, "%d\n", gmymiscdev_ctx->debug_level);
    mutex_unlock(&g_debug_level_lock);
    return 0;
}
static int mymisc_procfs_open_debug_level(struct inode *inode,
                                          struct file *fp) {
    return single_open(fp, mymisc_procfs_show_debug_level, NULL);
}

static ssize_t mymisc_procfs_write_debug_level(struct file *fp,
                                               const char __user *ubuf,
                                               size_t count, loff_t *off) {
    ssize_t ret = count;
    int user_debug_level;
    char *kbuf;

    if (mutex_lock_interruptible(&g_debug_level_lock))
        return -ERESTARTSYS;
    dev_info(gmiscdev->this_device, "procfs %s write.\n", PROCFS_DEBUG_LEVEL);
    /* Convert from string to int. */
    kbuf = devm_kzalloc(gmiscdev->this_device, sizeof(char), count);
    if (unlikely(!kbuf)) {
        ret = -ENOMEM;
        goto out;
    }

    if (copy_from_user(kbuf, ubuf, count)) {
        dev_err(gmiscdev->this_device, "copy_from_user failed.\n");
        ret = -EIO;
        goto out;
    }

    if (kstrtoint(kbuf, 0, &user_debug_level)) {
        dev_err(gmiscdev->this_device, "kstrtoint failed.\n");
        ret = -EIO;
        goto kstrtointfailed;
    }

    gmymiscdev_ctx->debug_level = user_debug_level;
    ret = count;
    goto out;

kstrtointfailed:
    devm_kfree(gmiscdev->this_device, kbuf);
out:
    mutex_unlock(&g_debug_level_lock);
    return ret;
}

static const struct file_operations mymisc_procfs_fops = {
    .owner = THIS_MODULE,
    .open = mymisc_procfs_open_debug_level,
    .write = mymisc_procfs_write_debug_level,
    .release = single_release,
    .read = seq_read,
    .llseek = seq_lseek,
};

static int __init mymiscdev_init(void) {
    int ret = 0;
    /* First, I need to create a miscdevice. */
    if (misc_register(&mymiscdev)) {
        pr_err("create misc dev failed.\n");
        ret = -EIO;
        goto out;
    }
    gmiscdev = &mymiscdev;

    /* Create the device config context structure. */
    gmymiscdev_ctx = devm_kzalloc(gmiscdev->this_device,
                              sizeof(struct mymiscdev_ctx), GFP_KERNEL);
    if (unlikely(!gmymiscdev_ctx))
        goto unregister_device;

    strscpy(gmymiscdev_ctx->desc, "hello world.", DESC_MAX_LEN);
    gmymiscdev_ctx->debug_level = 1;

    dev_info(gmiscdev->this_device, "device initialized.");

    /* Add procfs for this device. */
    gprocfs_root_dir = proc_mkdir(PROCFS_ROOT_DIR, NULL);
    if (unlikely(!gprocfs_root_dir)) {
        dev_err(gmiscdev->this_device, "create procfs root dir failed.\n");
        ret = -EIO;
        goto delete_ctx;
    }

    /* Add files. */
    if (unlikely(!proc_create(PROCFS_DEBUG_LEVEL, PROCFS_DEBUG_LEVEL_PERMS,
                              gprocfs_root_dir, &mymisc_procfs_fops))) {
        dev_err(gmiscdev->this_device, "create procfs sub-file failed!\n");
        ret = -EIO;
        goto delete_procfs_root;
    }

    dev_info(gmiscdev->this_device, "create procfs success.\n");

    return 0;
delete_procfs_root:
    remove_proc_subtree(PROCFS_ROOT_DIR, NULL);
delete_ctx:
    devm_kfree(gmiscdev->this_device, gmymiscdev_ctx);
unregister_device:
    ret = -ENOMEM;
    misc_deregister(gmiscdev);
out:
    return ret;
}

static void __exit mymiscdev_exit(void) {
    /* Remove the procfs. */
    remove_proc_subtree(PROCFS_ROOT_DIR, NULL);
    devm_kfree(gmiscdev->this_device, gmymiscdev_ctx);
    misc_deregister(gmiscdev);
    pr_info("removed.\n");
}

module_init(mymiscdev_init);
module_exit(mymiscdev_exit);
