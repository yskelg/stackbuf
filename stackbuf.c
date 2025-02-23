// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2025 Yunseong Kim <yskelg@gmail.com>
 * https://www.linkedin.com/in/yunseong-kim-linux-kernel/
 */

#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/stacktrace.h>

char stackbuf[512];

static size_t set_stack_trace(char *buf, size_t buf_size) {
    unsigned long entries[32];
    unsigned int i, nr_entries;
    size_t pos = 0;

    nr_entries = stack_trace_save(entries, sizeof(entries), 1);

    for (i = 0; i < nr_entries && pos < buf_size - 1; i++) {
        char sym[KSYM_NAME_LEN];
        sprint_symbol(sym, entries[i]);
        pos += snprintf(buf + pos, buf_size - pos, "%s\n", sym);
        if (pos >= buf_size) break;
    }

    return pos;
}

static void test_function(void) {
    size_t len = set_stack_trace(stackbuf, sizeof(stackbuf));
    pr_info("Stack trace(%ld):\n%s", len, stackbuf);
}

static int stackbuf_init(void) {
    pr_info("INIT stackbuf.\n");
    test_function();
    return 0;
}

static void stackbuf_exit(void) {
    test_function();
    pr_info("EXIT stackbuf.\n");
}

module_init(stackbuf_init);
module_exit(stackbuf_exit);
MODULE_DESCRIPTION("Yunseong Kim <yskelg@gmail.com>");
MODULE_LICENSE("GPL v2");
