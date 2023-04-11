// SPDX-License-Identifier: GPL-3.0
//
// <description...>
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define OURMODNAME   "lkm_template"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("a simple LKM template; do refer to the (better) Makefile as well");
MODULE_LICENSE("GPL-3.0");	// or whatever
MODULE_VERSION("0.1");

static int __init lkm_template_init(void)
{
    pr_info("inserted\n");
    return 0;		/* success */
}

static void __exit lkm_template_exit(void)
{
    pr_info("removed\n");
}

module_init(lkm_template_init);
module_exit(lkm_template_exit);
