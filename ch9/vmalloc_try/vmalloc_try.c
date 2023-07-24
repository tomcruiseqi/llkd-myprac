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
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/vmalloc.h>

#define OURMODNAME "vmalloc_try"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("Test the vmalloc API.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static int kvnum = 5 * 1024 * 1024; // 5 MB
module_param(kvnum, int, 0644);
MODULE_PARM_DESC(kvnum, "number of bytes to allocate with kvmalloc();");

#define KVN_MIN_BYTES 16
#define DISP_BYTES 16

static void *vptr_rndm, *vptr_init, *kv, *kvarr;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
static void *vrx;
#endif

static int vmalloc_try(void) {
    /* 1. vmalloc(); mem contents are random. */
    vptr_rndm = vmalloc(10000);
    if (!vptr_rndm)
        goto err_out1;
    pr_info("1. vmalloc():  vptr_rndm = 0x%pKk (actual=0x%px)\n", vptr_rndm,
            vptr_rndm);
    print_hex_dump_bytes(" content: ", DUMP_PREFIX_NONE, vptr_rndm, DISP_BYTES);

    /* 2. vzalloc(); mem contents are set to zeroes */
    vptr_init = vzalloc(10000);
    if (!vptr_init)
        goto err_out2;
    pr_info("2. vzalloc(): vptr_init = 0x%pK (actual=0x%px)\n", vptr_init,
            vptr_init);
    print_hex_dump_bytes(" content: ", DUMP_PREFIX_NONE, vptr_init, DISP_BYTES);

    /* 3. kvmalloc(): allocate 'kvnum' bytes (5MB by default) with the
     * kvmalloc(); if kvnum is large (enough), this will become a vmalloc()
     * under the hood, else it falls back to a kmalloc()
     * */
    kv = kvmalloc(kvnum, GFP_KERNEL);
    if (!kv)
        goto err_out3;
    pr_info("3. kvmalloc():kv = 0x%pK (actual=0x%px)\n"
            "     (for %d bytes)\n",
            kv, kv, kvnum);
    print_hex_dump_bytes(" content: ", DUMP_PREFIX_NONE, kv, KVN_MIN_BYTES);

    /* 4. kcalloc(): allocate an array of 1000 64-bit quantities and zero out
     * the memory.
     */
    kvarr = kcalloc(1000, sizeof(u64), GFP_KERNEL);
    if (!kvarr)
        goto err_out4;
    pr_info("4. kcalloc(): kvarr = 0x%pK (actual=0x%px)\n", kvarr, kvarr);
    print_hex_dump_bytes(" content: ", DUMP_PREFIX_NONE, kvarr, DISP_BYTES);

/* 4. __vmalloc(): allocatee some 42 pages and set protections to RO. */
/* #undef WR2ROMEM_BUG */
#define WR2ROMEM_BUG
/* 'Normal' usage: keep this commented out, else
 * we will crash! Read the book, ch 9, for details;*/
/*
 * In 5.8.0, commit 88dca4c 'mm: remove the pgprot argument to __vmalloc'
 * has removed the pgprot arg from the __vmalloc(). So,, only attempt this
 * when we are on kernels < 5.8.0
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
    vrx = __vmalloc(42 * PAGE_SIZE, GFP_KERNEL, PAGE_KERNEL_RO);
    if (!vrx)
        goto err_out5;
    pr_info("5. __vmalloc():  vrx = 0x%pK (actual=0x%px)\n", vrx, vrx);
    /* Try reading the memory, should be fine. */
    print_hex_dump_bytes(" contents: ", DUMP_PREFIX_NONE, vrx, DISP_BYTES);
#ifdef WR2ROMEM_BUG
    /* Try writing to the RO memory! We should, of course, find that the kernel
     * crashes (emits an Oops!). */
    pr_info(
        " 6. attempting to now write into a kernel vmalloc-ed region that's "
        "RO!\n");
    *(u64 *)(vrx + 4) = 0xba;
#endif /* WR2ROMEM_BUG */
#else
    /* Logically, should now use the __vmalloc_node_range() BUT, whoops, it
     * isn't EXPORTED!
     */
    pr_info("4. >= 5.8.0: __vmalloc(): no page prot param; can use "
            "__vmalloc_node_range() but it's not exported");
    pr_cont(" so, simply skip this case.\n");
#endif

    return 0; /* success */

err_out5:
    kvfree(kvarr);
err_out4:
    kvfree(kv);
err_out3:
    vfree(vptr_init);
err_out2:
    vfree(vptr_rndm);
err_out1:
    return -ENOMEM;
}

static int __init vmalloc_try_init(void) {
    if (kvnum < KVN_MIN_BYTES) {
        pr_info("kvnum must be >= %d bytes (curr it's %d bytes)\n",
                KVN_MIN_BYTES, kvnum);
        return -EINVAL;
    }
    pr_info("%s: Inserted.\n", OURMODNAME);
    return vmalloc_try();
}

static void __exit vmalloc_exit_exit(void) {
    vfree(vptr_rndm);
    vfree(vptr_init);
    kvfree(kv);
    kvfree(kvarr);
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
    vfree(vrx);
#endif
    pr_info("%s: Removed.\n", OURMODNAME);
}

module_init(vmalloc_try_init);
module_exit(vmalloc_exit_exit);
