// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
// Test the slab debug.
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

#include "linux/printk.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#define OURMODNAME "slub_debug"
#define OURCACHENAME "our_cache"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("Test the SLUB debug skills.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

struct myctx {
    u32 iarr[10];
    u64 uarr[10];
    char uame[128], passwd[16], config[64];
};

static struct kmem_cache *gctx_cachep;
struct myctx *obj = NULL;

static int use_our_cache(void) {
    obj = kmem_cache_alloc(gctx_cachep, GFP_KERNEL);
    if (!obj)
        return -ENOMEM;

    pr_info("our cache object (@ %pK), actual=%pk, size:%u bytes;"
            " actual ksize=%zu\n",
            obj, obj, kmem_cache_size(gctx_cachep), ksize(obj));
    print_hex_dump_bytes("obj: ", DUMP_PREFIX_OFFSET, obj,
                         sizeof(struct myctx));

    return 0;
}

static int create_our_cache(void) {
    int ret = 0;

    gctx_cachep = kmem_cache_create(
        OURCACHENAME, sizeof(struct myctx), sizeof(long),
        SLAB_POISON | SLAB_RED_ZONE | SLAB_HWCACHE_ALIGN, NULL);
    if (!gctx_cachep) {
        if (IS_ERR(gctx_cachep))
            ret = PTR_ERR(gctx_cachep);
    }
    return ret;
}

static void use_the_object(void *s, u8 c, size_t n) {
    memset(s, c, n);
    pr_info("%s: after memset s, '%c', %zu\n", OURMODNAME, c, n);
    print_hex_dump_bytes("obj: ", DUMP_PREFIX_ADDRESS, s, sizeof(struct myctx));
}

static int __init slub_debug_init(void) {
    pr_info("%s: inserted.\n", OURMODNAME);
    create_our_cache();
    return use_our_cache();
}

static void __exit slub_debug_exit(void) {
    kmem_cache_free(gctx_cachep, obj);
    use_the_object(obj, '!', 10); /*The UAF bug! */
    pr_info("%s: removed.\n", OURMODNAME);
    kmem_cache_destroy(gctx_cachep);
}

module_init(slub_debug_init);
module_exit(slub_debug_exit);
