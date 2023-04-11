
/*******************************************************************************
     Module stacking: user module.

     Copyright (c) 2023  qizengtian <qizengtian@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ******************************************************************************/

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define MODNAME     "user_lkm"
#if 1
MODULE_LICENSE("GPL");
#else
MODULE_LICENSE("MIT");
#endif

extern void llkd_sysinfo2(void);

extern long get_skey(int);

extern int exp_int;

/* Call some functions within the 'core' module */
static int __init

user_lkm_init(void) {
#define THE_ONE   0xfedface
    pr_info("inserted\n");
    u64 sk = get_skey(THE_ONE);

    pr_debug("Called get_skey(), ret = 0x%llx = %llu\n", sk, sk);
    pr_debug("exp_int = %d\n", exp_int);
    llkd_sysinfo2();

    return 0;
}

static void __exit

user_lkm_exit(void) {
    pr_info("bids you adieu\n");
}

module_init(user_lkm_init);
module_exit(user_lkm_exit);
