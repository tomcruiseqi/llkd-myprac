// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
// This my place to figure out some of my questions.
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

#include "klib_llkd.h"
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/overflow.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/version.h>

#define OURMODNAME "ch8_myprac"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("this is the place for me to try something i want.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static const void *gptr;
static int bsa_alloc_order = 0;
static unsigned long align_offset = 300;

struct mystruc {
	size_t count;
	struct page pages[0];
};

struct mystruc2 {
	size_t count;
	struct page pages[1];
};

struct mystruc3 {
	size_t count;
	struct page pages[];
};

static int __init myprac_init(void)
{
	unsigned long new_ptr, aligned_ptr;
	unsigned long count = 1, bytes_count;
	struct mystruc *instances;
	struct mystruc2 *instances2;
	struct mystruc3 *instances3;

	pr_info("MAX_ORDER: %d\n", MAX_ORDER);
	gptr = (void *)__get_free_pages(GFP_KERNEL, bsa_alloc_order);
	if (!gptr) {
		goto out1;
	}
	show_phy_pages(gptr, PAGE_SIZE * powerof(2, bsa_alloc_order), 1);
	// now, i need to test the PAGE_ALIGN micro.
	new_ptr = (unsigned long)gptr + align_offset;
	aligned_ptr = (unsigned long)gptr + PAGE_ALIGN(align_offset);
	pr_info("oldaddr: 0x%pK newaddr: 0x%pK aligned: 0x%pK\n", (void *)gptr,
		(void *)new_ptr, (void *)aligned_ptr);

	// Test the sizeof dynamically structures.
	bytes_count = struct_size(instances, pages, count);
	instances = kmalloc(bytes_count, GFP_KERNEL);
	instances->count = count;
	pr_info("struct_size of mystruc: %lu\n"
		"sizeof of struct mystruc: %lu\n"
		"sizeof size_t: %lu\n"
		"sizeof struct page: %lu\n"
		"sizeof struct page[0]: %lu\n",
		bytes_count, sizeof(struct mystruc), sizeof(size_t),
		sizeof(struct page), sizeof(instances->pages));
	kfree(instances);

	// Test the size of count 1 trailling array.
	bytes_count = struct_size(instances2, pages, count);
	instances2 = kmalloc(bytes_count, GFP_KERNEL);
	instances2->count = count;
	pr_info("struct_size of mystruc2: %lu\n"
		"sizeof struct mystruc2: %lu\n"
		"sizeof size_t: %lu\n"
		"sizeof struct page: %lu\n"
		"sizeof struct pages[1]: %lu\n",
		bytes_count, sizeof(struct mystruc2), sizeof(size_t),
		sizeof(struct page), sizeof(instances2->pages));
	kfree(instances2);

	// Test the flexible trailling structure.
	bytes_count = struct_size(instances3, pages, count);
	instances3 = kmalloc(bytes_count, GFP_KERNEL);
	instances3->count = count;
	pr_info("struct_size of mystruc3: %lu\n"
		"sizeof struct mystruc3: %lu\n"
		"sizeof size_t: %lu\n"
		"sizeof struct page: %lu\n"
		"sizeof struct page[]: sizeof failed for incomplete structure.\n",
		bytes_count, sizeof(struct mystruc3), sizeof(size_t),
		sizeof(struct page));
	kfree(instances3);

	return 0;
out1:
	return -ENOMEM;
}

static void __exit myprac_exit(void)
{
	free_pages((unsigned long)gptr, bsa_alloc_order);
}

module_init(myprac_init);
module_exit(myprac_exit);
