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
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("This is my platform device with sysfs support practice.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

struct mymiscdev_ctx {
    int debug_level;
#define DESC_MAX_LEN 128
    char desc[128];
};
static struct mymiscdev_ctx *gmydev_ctx;
DEFINE_MUTEX(g_mymiscdev_ctx_lock);

/* Add support for sysfs. */
#define SYSFS_PLATFORM_DEV "my_platform_dev"
#define SYSFS_MYMISC_ATTR_DEBUG_LEVEL my_platform_debug_level
DEFINE_MUTEX(g_sysfs_debug_level);
static struct platform_device *gmyplatdev;
static ssize_t my_platform_debug_level_show(struct device *dev,
                                            struct device_attribute *attr,
                                            char *buf) {
    int bytes_read;

    if (mutex_lock_interruptible(&g_sysfs_debug_level))
        return -ERESTARTSYS;
    dev_info(dev, "read attribute:[%s].\n", attr->attr.name);
    bytes_read = snprintf(buf, sizeof(gmydev_ctx->debug_level), "%d\n",
                          gmydev_ctx->debug_level);
    mutex_unlock(&g_sysfs_debug_level);
    return bytes_read;
}

static ssize_t my_platform_debug_level_store(struct device *dev,
                                             struct device_attribute *attr,
                                             const char *buf, size_t count) {
    int ret;
    int user_debug_level;
    if (mutex_lock_interruptible(&g_sysfs_debug_level))
        return -ERESTARTSYS;

    if (kstrtoint(buf, 0, &user_debug_level)) {
        dev_err(dev, "failed to covert %s to int.\n", buf);
        ret = -EIO;
        goto out;
    }
    gmydev_ctx->debug_level = user_debug_level;
    ret = count;
out:
    mutex_unlock(&g_sysfs_debug_level);
    return ret;
}

static DEVICE_ATTR_RW(SYSFS_MYMISC_ATTR_DEBUG_LEVEL);

static int __init mydev_init(void) {
    int ret = 0;

    /* Create simple platform device. */
    gmyplatdev =
        platform_device_register_simple(SYSFS_PLATFORM_DEV, -1, NULL, 0);
    if (IS_ERR(gmyplatdev)) {
        ret = PTR_ERR(gmyplatdev);
        pr_err("create simple platform device failed.\n");
        goto out;
    }

    /* Create the device config context structure. */
    gmydev_ctx = devm_kzalloc(&gmyplatdev->dev, sizeof(struct mymiscdev_ctx),
                              GFP_KERNEL);
    if (unlikely(!gmydev_ctx)) {
        goto unregister_device;
        ret = -ENOMEM;
    }

    strscpy(gmydev_ctx->desc, "hello world.", DESC_MAX_LEN);
    gmydev_ctx->debug_level = 1;

    /* Create sysfs files. */
    if (device_create_file(&gmyplatdev->dev,
                           &dev_attr_SYSFS_MYMISC_ATTR_DEBUG_LEVEL)) {
        dev_err(&gmyplatdev->dev, "create sysfs failed.\n");
        ret = -EIO;
        goto delete_ctx;
    }
    dev_info(&gmyplatdev->dev, "create sysfs success.\n");

    return 0;
delete_ctx:
    devm_kfree(&gmyplatdev->dev, gmydev_ctx);
unregister_device:
    platform_device_unregister(gmyplatdev);
out:
    return ret;
}

static void __exit mydev_exit(void) {
    /* Remove the sysfs device attributes. */
    device_remove_file(&gmyplatdev->dev,
                       &dev_attr_SYSFS_MYMISC_ATTR_DEBUG_LEVEL);
    /* Delete the ctx structure. */
    devm_kfree(&gmyplatdev->dev, gmydev_ctx);
    /* Unregister the platform device. */
    platform_device_unregister(gmyplatdev);
    pr_info("removed.\n");
}

module_init(mydev_init);
module_exit(mydev_exit);
