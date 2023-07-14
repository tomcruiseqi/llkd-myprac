/*
 * ch6/foreach/prcs_showall.c
 ***************************************************************
 * This program is part of the source code released for the book
 *  "Linux Kernel Programming"
 *  (c) Author: Kaiwan N Billimoria
 *  Publisher:  Packt
 *  GitHub repository:
 *  https://github.com/PacktPublishing/Linux-Kernel-Programming
 *
 * From: Ch 6 : Kernel and MM Internals - Essentials
 ****************************************************************
 * Brief Description:
 * This kernel module iterates over the task structures of all *processes*
 * currently alive on the box, printing out a few details for each of them.
 * We use the for_each_process() macro to do so here.
 *
 * For details, please refer the book, Ch 6.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 10, 0)
#include <linux/sched/signal.h>	/* for_each_xxx(), ... */
#endif
#include <linux/fs.h>		/* no_llseek() */
#include <linux/slab.h>
#include <linux/uaccess.h>	/* copy_to_user() */
#include <linux/kallsyms.h>

#define OURMODNAME "prcs_showall"

MODULE_AUTHOR("qizengtian");
MODULE_DESCRIPTION("learn how to get all processes and their information");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static int show_prcs_in_tasklist(void) {
    struct task_struct *p;
#define MAXLEN 128
    char tmp[MAXLEN];
    int numread = 0, n = 0, total = 0;
    char hdr[] = "      Name        |   TGID    |   PID     |   RUID    |   EUID";
    pr_info("%s\n", &hdr[0]);
    rcu_read_lock();
    for_each_process(p) {
        memset(tmp, 0, MAXLEN);
        n = snprintf(tmp, MAXLEN, "%-16s|%8d|%8d|%7u|%7u\n",
                     p->comm, p->tgid, p->pid, __kuid_val(p->cred->uid),
                     __kuid_val(p->cred->euid)
                     );
        numread += n;
        pr_info("%s", tmp);

        cond_resched();
        total++;
    } // for_each_process()
    rcu_read_unlock();

    return total;
}

static int __init prcs_showall_init(void) {
    int total;

    pr_info("%s: inserted\n", OURMODNAME);
    total = show_prcs_in_tasklist();
    pr_info("%s: total # of processes on system: %d\n", OURMODNAME, total);

    return 0;
}

static void __exit prcs_showall_exit(void) {
    pr_info("%s: removed\n", OURMODNAME);
}

module_init(prcs_showall_init);
module_exit(prcs_showall_exit);