// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
// Slab API practice.
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

#include "linux/overflow.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#define OURMODNAME "slab1"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("This is my practice of SLAB api.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static char *gptr;
struct myctx {
    u32 iarr[100];
    u64 uarr[100];
    char uname[128], passwd[16], config[16];
};

static struct myctx *ctx;

static int __init slab1_init(void) {
    gptr = kmalloc(1024, GFP_KERNEL);
    if (!gptr) {
        goto fail1;
    }

    pr_info("kmalloc() succeeds, (actual) KVA ret value = %px\n", gptr);
    print_hex_dump_bytes("gptr before memset: ", DUMP_PREFIX_OFFSET, gptr, 32);
    memset(gptr, 'm', 1024);
    print_hex_dump_bytes("gptr after memset: ", DUMP_PREFIX_OFFSET, gptr, 32);

    /* 2. Alloc memory for and initialize our 'context' structure. */
    pr_info("I think the size of struct myctx: 400+800+128+16+16=1360\n");
    pr_info("Alignment of [iarr]: %lu\n", offsetof(struct myctx, iarr));
    pr_info("Alignment of [uarr]: %lu\n", offsetof(struct myctx, uarr));
    pr_info("Alignment of [uname]: %lu\n", offsetof(struct myctx, uname));
    pr_info("Alignment of [passwd]: %lu\n", offsetof(struct myctx, passwd));
    pr_info("Alignment of [config]: %lu\n", offsetof(struct myctx, config));
    pr_info("sizeof struct myctx: %lu\n", sizeof(struct myctx));
    ctx = kzalloc(sizeof(struct myctx), GFP_KERNEL);
    if (!ctx)
        goto fail2;
    pr_info(
        "%s: context struct alloc'ed and initialized (actual KVA ret = %px)\n",
        OURMODNAME, ctx);
    print_hex_dump_bytes("ctx: ", DUMP_PREFIX_OFFSET, ctx, 32);

    return 0;

fail2:
    kfree(gptr);
fail1:
    return -ENOMEM;
}

static void __exit slab1_exit(void) {
    kfree(ctx);
    kfree(gptr);
}

module_init(slab1_init);
module_exit(slab1_exit);
