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
#include <asm/fpu/api.h>


MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("LLKD:ch5/fp_in_lkm: no performing FP "
                   "(floating point) arithmetic in kernel mode");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static double num = 22.0, den = 7.0, mypi;

static int __init fp_in_lkm_init(void)
{
    pr_info("%s: inserted\n");

    kernel_fpu_begin();
    mypi = num / den;
    kernel_fpu_end();
#if 1
    pr_info("PI = %.4f = %.4f\n", mypi, num / den);
#endif

    return 0;		/* success */
}

static void __exit fp_in_lkm_exit(void)
{
    kernel_fpu_begin();
    pr_info("mypi = %f\n", mypi);
    kernel_fpu_end();

    pr_info("removed\n");
}

module_init(fp_in_lkm_init);
module_exit(fp_in_lkm_exit);
