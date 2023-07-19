// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
// Test the maxsize of slab api can give.
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>

#define OURMODNAME "slab_maxsize"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("Test the max alloc limit from k[m|z]alloc()");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static int stepsz = 20000;
module_param(stepsz, int, 0644);
MODULE_PARM_DESC(
    stepsz,
    "Amount to increase allocation by on each loop iteration (default=20000");

static int test_maxallocsz(void) {
    size_t size2alloc = 20, actual_alloced = 0;
    while (1) {
        void *p = kmalloc(size2alloc, GFP_KERNEL);
        if (!p) {
            pr_alert("kmalloc fail, size2alloc=%zu\n", size2alloc);
            return -ENOMEM;
        }
        //		pr_info("kmalloc(%7zu) = 0x%pK\n", size2alloc, p);
        actual_alloced = ksize(p);
        pr_info("kmalloc(%7zu) : %7zu : %7zu : %3zu%%\n", size2alloc,
                actual_alloced, (actual_alloced - size2alloc),
                (((actual_alloced - size2alloc) * 100) / size2alloc));
        kfree(p);
        size2alloc += stepsz;
    }
    return 0;
}

static int __init slab_maxsize_init(void) {
    pr_info("%s: inserted\n", OURMODNAME);
    return test_maxallocsz();
}

static void __exit slab_maxsize_exit(void) {
    pr_info("%s: removed\n", OURMODNAME);
}

module_init(slab_maxsize_init);
module_exit(slab_maxsize_exit);
