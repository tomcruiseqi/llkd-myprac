// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
// Test the slab cache.
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h> /* curret */
#include <linux/slab.h>
#include <linux/version.h>

#define OURMODAME "slab_custom"
#define OURCACHENAME "our_ctor"

static int use_ctor = 1;
module_param(use_ctor, uint, 0);
MODULE_PARM_DESC(
    use_ctor,
    "If set to 1 (default), our custom ctor routine."
    " will initialize slabmem; when 0, no custom constrctor will run.");

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("Test the creation of a custom slab cache.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

struct myctx {
    u32 iarr[10];                            // 40 bytes.
    u64 uarr[10];                            // 80 bytes.
    char uname[128], passwd[16], config[64]; // 208 bytes.
};

static struct kmem_cache *gctx_cachep;

static int use_our_cache(void) {
    struct myctx *obj = NULL;
    obj = kmem_cache_alloc(gctx_cachep, GFP_KERNEL);
    if (!obj)
        return -ENOMEM;

    pr_info("Our cache object (@ %pK), actual=%px) size is %u bytes;"
            "actual ksize=%zu\n",
            obj, obj, kmem_cache_size(gctx_cachep), ksize(obj));
    print_hex_dump_bytes("obj: ", DUMP_PREFIX_OFFSET, obj,
                         sizeof(struct myctx));

    /* free it*/
    kmem_cache_free(gctx_cachep, obj);
    return 0;
}

/* The parameter is the pointer to the just allocated memory 'object' from
 * our custom slab cache; here, this is our 'constructor' routine; so, we
 * initialize our just allocated memory object.
 */
static void our_ctor(void *new) {
    struct myctx *ctx = new;
    struct task_struct *p = current;

    /* TIP: to see how exactly we got here, insert this call:
     * dump_stack();
     * (read it bottom-up ignoring call frames that begin with '?')
     */
    // dump_stack();

    pr_info("in ctor: just alloced mem object is @0x%px\n", ctx);
    memset(ctx, 0, sizeof(struct myctx));

    /* As a demo, we init the 'config' field of our structure to some
     * (arbitrary) 'accounting' values from our task_struct.
     */
    snprintf(ctx->config, 6 * sizeof(u64) + 5, "%d.%d,%ld.%ld,%ld,%ld", p->tgid,
             p->pid, p->nvcsw, p->nivcsw, p->min_flt, p->maj_flt);
}

static int create_our_cache(void) {
    int ret = 0;
    void *ctor_fn = NULL;

    if (use_ctor == 1)
        ctor_fn = our_ctor;
    pr_info("sizeof our ctx structure is %zu bytes\n"
            " using custom constructor routine? %s\n",
            sizeof(struct myctx), use_ctor == 1 ? "yes" : "no");

    /* Create a new slab cache:
     * kmem_cache_create(const char *name, unsiged int size, unsigned int align,
     * slab_flags_t flags, void (*ctor)(void *));
     */
    gctx_cachep = kmem_cache_create(
        OURCACHENAME,           // Name of our cache.
        sizeof(struct myctx),   // (Min) size of each object
        sizeof(long),           // Alignment.
        SLAB_POISON |           // Set the cache full of with a5.
            SLAB_RED_ZONE |     // Catching buffer under|over-flow bugs.
            SLAB_HWCACHE_ALIGN, // Good for performance.
        ctor_fn);               // constrctor, on by default.

    if (!gctx_cachep) {
        pr_warn("kmem_cache_create()0 failed.\n");
        if (IS_ERR(gctx_cachep))
            ret = PTR_ERR(gctx_cachep);
    }
    return ret;
}

static int __init slab_custom_init(void) {
    pr_info("inserted\n");
    create_our_cache();
    return use_our_cache();
}

static void __exit slab_custom_exit(void) {
    pr_info("custom cache destroyed; removed.\n");
    kmem_cache_destroy(gctx_cachep);
}

module_init(slab_custom_init);
module_exit(slab_custom_exit);
