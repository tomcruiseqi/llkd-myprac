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


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define OUR_MODNAME    "modparams2"
MODULE_AUTHOR("Kaiwan N Billimoria");
MODULE_DESCRIPTION("LKP book:ch5/modparams/modparams2: module parameters");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");

/* Module parameters */
static int mp_debug_level;
module_param(mp_debug_level, int, 0660);
MODULE_PARM_DESC(mp_debug_level,
		 "Debug level [0-2]; 0 => no debug messages, 2 => high verbosity");

static char *mp_strparam = "My string param";
module_param(mp_strparam, charp, 0660);
MODULE_PARM_DESC(mp_strparam, "A demo string parameter");

static int control_freak;
module_param(control_freak, int, 0660);
MODULE_PARM_DESC(control_freak, "Set to the project's control level [1-5]. MANDATORY");

static int __init modparams2_init(void)
{
	pr_info("%s: inserted\n", OUR_MODNAME);
	if (mp_debug_level > 0)
		pr_info("module parameters passed: "
			"mp_debug_level=%d mp_strparam=%s\ncontrol_freak=%d\n",
			mp_debug_level, mp_strparam, control_freak);

	/* param 'control_freak': if it hasn't been passed (implicit guess), or is
	 * the same old value, or isn't within the right range,
	 * it's Unacceptable!  :-)
	 */
	if ((control_freak < 1) || (control_freak > 5)) {
		pr_warn("%s: Must pass along module parameter"
			" 'control_freak', value in the range [1-5]; aborting...\n",
			OUR_MODNAME);
		return -EINVAL;
	}
	return 0;		/* success */
}

static void __exit modparams2_exit(void)
{
	if (mp_debug_level > 0)
		pr_info("module parameters passed: mp_debug_level=%d mp_strparam=%s\n",
			mp_debug_level, mp_strparam);
	pr_info("%s: removed\n", OUR_MODNAME);
}

module_init(modparams2_init);
module_exit(modparams2_exit);
