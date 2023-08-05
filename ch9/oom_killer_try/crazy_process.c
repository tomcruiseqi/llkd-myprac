
// SPDX-License-Identifier: GPL-3.0
//
// Brief Description:
// Userspace program to run out the memory and invoke the OOM Killer.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BLK (getpagesize() * 2)

static int force_page_fault = 0;

int main(int argc, char **argv)
{
	char *p;
	int i = 0, j = 1, stepval = 5000, verbose = 0;

	if (argc < 3) {
		fprintf(stderr,
			"Usage: %s alloc-loop-count force-page-fault[0|1] [verbose_flag[0|1]]\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	if (atoi(argv[2]) == 1)
		force_page_fault = 1;

	if (argc >= 4) {
		if (atoi(argv[3]) == 1)
			verbose = 1;
	}

	printf("%s: PID %d (verbose mode: %s). size to alloc every time: %d\n",
	       argv[0], getpid(), (verbose == 1 ? "on" : "off"), BLK);
	do {
		p = (char *)malloc(BLK);
		if (!p) {
			fprintf(stderr, "%s: loop #%d: malloc failure.\n",
				argv[0], i);
			break;
		}

		/* *IMPORTANT* Demand Paging :
		 * Force the MMU to raise the page fault exception by writing into the
		 * page; writing a single byte, any byte, will do the trick! This is as
		 * the virtual address referenced will have no PTE entry, causing the
		 * MMU to raise the page fault!
		 * The fault handler, being intelligent, figures out it's a "good fault"
		 * (a minor fault) and allocates a page frame via the page allocator!
		 * Only now do we have physical memory!
		 */
		if (force_page_fault) {
			p[1103] &=
				0x0b; // write something into a byte of the 1st page
			p[5227] |=
				0xaa; // write something into a byte of the 2nd page
		}
		if (!(i % stepval)) { // every 'stepval' iterations..
			if (!verbose) {
				if (!(j % 5))
					printf(". (%dM)\t",
					       (j * 8 * 5000) >> 10);
				else
					printf(".");
				fflush(stdout);
				j++;
			} else {
				printf("%06d\taddr p = %p   break = %p\n", i,
				       (void *)p, (void *)sbrk(0));
			}
		}
		i++;
	} while (p && (i < atoi(argv[1])));

	exit(EXIT_SUCCESS);
}
