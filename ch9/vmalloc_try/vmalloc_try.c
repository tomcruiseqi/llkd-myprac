// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
// Test the vmalloc API.
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/vmalloc.h>

#define OURMODNAME "vmalloc_try"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("Test the vmalloc API.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static int kvnum = 5 * 1024 * 1024; // 5 MB


static int __init vmalloc_try_init(void)
{
	pr_info("%s: Inserted.\n", OURMODNAME);
	return 0;
}

static void __exit vmalloc_exit_exit(void)
{
	pr_info("%s: Removed.\n", OURMODNAME);
}

module_init(vmalloc_try_init);
module_exit(vmalloc_exit_exit);
