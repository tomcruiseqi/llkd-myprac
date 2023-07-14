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

#include "linux/gfp.h"
#include "linux/mm.h"
#include "linux/printk.h"
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include "klib_llkd.h"

#define OURMODNAME "ch8_myprac"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("this is the place for me to try something i want.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static const void * gptr;
static int bsa_alloc_order = 10;
static unsigned long align_offset = 300;

static int __init myprac_init(void) {
  unsigned long new_ptr, aligned_ptr;
  pr_info("MAX_ORDER: %d\n", MAX_ORDER);

  gptr = (void *)__get_free_pages(GFP_KERNEL, bsa_alloc_order);

  if (!gptr) {
    goto out1;
  }
  show_phy_pages(gptr, PAGE_SIZE * powerof(2, bsa_alloc_order), 1);
  // now, i need to test the PAGE_ALIGN micro.
  new_ptr = (unsigned long)gptr + align_offset;
  aligned_ptr = (unsigned long)gptr + PAGE_ALIGN(align_offset);
  pr_info("oldaddr: 0x%pK newaddr: 0x%pK aligned: 0x%pK\n",\
      (void *)gptr,\
      (void *)new_ptr,\
      (void *)aligned_ptr);

  return 0;
out1:
  return -ENOMEM;
}

static void __exit myprac_exit(void) {
  free_pages((unsigned long)gptr, bsa_alloc_order);
}

module_init(myprac_init);
module_exit(myprac_exit);
