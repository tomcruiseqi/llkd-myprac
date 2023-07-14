
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

#define MODNAME   "core_lkm"
#define THE_ONE   0xfedface
MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("qizengtian/llkd: module stacking.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

int exp_int = 200;
EXPORT_SYMBOL_GPL(exp_int);

/* Functions to be called from other LKMs */

/* llkd_sysinfo2:
 * A more security-aware version of the earlier llkd_sysinfo() routine. We use
 * David Wheeler's flawfinder(1) tool to detect possible vulnerabilities;
 * Based on it's report, we change the strlen, and replace the strncat with
 * strlcat.
 */
void llkd_sysinfo2(void)
{
#define MSGLEN   128
	char msg[MSGLEN];

	memset(msg, 0, MSGLEN);
	snprintf(msg, 48, "%s(): minimal Platform Info:\nCPU: ", __func__);

	/* Strictly speaking, all this #if... is considered ugly and should be
	 * isolated as far as is possible
	 */
#ifdef CONFIG_X86
#if (BITS_PER_LONG == 32)
	strlcat(msg, "x86_32, ", MSGLEN);
#else
	strlcat(msg, "x86_64, ", MSGLEN);
#endif
#endif
#ifdef CONFIG_ARM
	strlcat(msg, "ARM-32, ", MSGLEN);
#endif
#ifdef CONFIG_ARM64
	strlcat(msg, "Aarch64, ", MSGLEN);
#endif
#ifdef CONFIG_MIPS
	strlcat(msg, "MIPS, ", MSGLEN);
#endif
#ifdef CONFIG_PPC
	strlcat(msg, "PowerPC, ", MSGLEN);
#endif
#ifdef CONFIG_S390
	strlcat(msg, "IBM S390, ", MSGLEN);
#endif

#ifdef __BIG_ENDIAN
	strlcat(msg, "big-endian; ", MSGLEN);
#else
	strlcat(msg, "little-endian; ", MSGLEN);
#endif

#if (BITS_PER_LONG == 32)
	strlcat(msg, "32-bit OS.\n", MSGLEN);
#elif(BITS_PER_LONG == 64)
	strlcat(msg, "64-bit OS.\n", MSGLEN);
#endif
	pr_info("%s", msg);
}
EXPORT_SYMBOL(llkd_sysinfo2);

#if (BITS_PER_LONG == 32)
u32 get_skey(int p)
#else				// 64-bit
u64 get_skey(int p)
#endif
{
#if (BITS_PER_LONG == 32)
	u32 secret = 0x567def;
#else				// 64-bit
	u64 secret = 0x123abc567def;
#endif
	pr_info("%s:%d: I've been called\n", __FILE__, __LINE__);
	if (p == THE_ONE)
		return secret;
	return 0;
}
EXPORT_SYMBOL(get_skey);

static int __init core_lkm_init(void)
{
	pr_info("inserted\n");
	return 0;	/* success */
}

static void __exit core_lkm_exit(void)
{
	pr_info("bids you adieu\n");
}

module_init(core_lkm_init);
module_exit(core_lkm_exit);
