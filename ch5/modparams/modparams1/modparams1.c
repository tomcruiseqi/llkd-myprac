// SPDX-License-Identifier: GPL-3.0
//
// Test cases for module parameters.
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
#include <linux/kernel.h>
#include <stat.h>

#define OUR_MODNAME    "modparams1"
MODULE_AUTHOR("Kaiwan N Billimoria and qizengtian");
MODULE_DESCRIPTION("module parameters demo LKM #1");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

/* Module parameters */
static int mp_debug_level;
module_param(mp_debug_level, int, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
MODULE_PARM_DESC(mp_debug_level,
"Debug level [0-2]; 0 => no debug messages, 2 => high verbosity");

static char *mp_strparam = "My string param";
module_param(mp_strparam, charp, 0660);
MODULE_PARM_DESC(mp_strparam, "A demo string parameter");

static int __init modparams1_init(void)
{
	pr_info("%s: inserted\n", OUR_MODNAME);
	pr_info("module parameters passed: mp_debug_level=%d mp_strparam=%s\n",
		mp_debug_level, mp_strparam);
	return 0;		/* success */
}

static void __exit modparams1_exit(void)
{
	pr_info("module parameters passed: mp_debug_level=%d mp_strparam=%s\n",
		mp_debug_level, mp_strparam);
	pr_info("%s: removed\n", OUR_MODNAME);
}

module_init(modparams1_init);
module_exit(modparams1_exit);
