// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
// Test the OOM Killer.
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
#include <linux/version.h>

#define OURMODNAME "oom_killer_try"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("Test the OOM killer.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static int __init oom_killer_try_init(void)
{
	pr_info("%s: Inserted.\n", OURMODNAME);
	return 0;
}

static void __exit oom_killer_try_exit(void)
{
	pr_info("%s: Removed.\n", OURMODNAME);
}

module_init(oom_killer_try_init);
module_exit(oom_killer_try_exit);
