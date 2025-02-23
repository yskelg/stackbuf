# stackbuf

In certain situations, inserting code and observing the results can be the fastest way to understand what's happening—especially when you need to see which function call stack is being traversed. I thought this method could be useful, particularly when too many events are occurring, and you want to store as many logs as possible in global memory area or whatelse. :)

Especially if KDump isn’t working properly or you can only use a RAM dump or `/proc/kcore` debugging, I think this method could be useful for leaving relevant information and could be applied in various debugging environments.

## Get stack tracing from global stated buffer

```
# insmod stackbuf.ko
```
Live debugging with `/proc/kcore`
```

# ~/crash/crash /usr/lib/debug/boot/vmlinux-5.15.0-133-generic

crash 8.0.6++
Copyright (C) 2002-2025  Red Hat, Inc.
Copyright (C) 2004, 2005, 2006, 2010  IBM Corporation
Copyright (C) 1999-2006  Hewlett-Packard Co
Copyright (C) 2005, 2006, 2011, 2012  Fujitsu Limited
Copyright (C) 2006, 2007  VA Linux Systems Japan K.K.
Copyright (C) 2005, 2011, 2020-2024  NEC Corporation
Copyright (C) 1999, 2002, 2007  Silicon Graphics, Inc.
Copyright (C) 1999, 2000, 2001, 2002  Mission Critical Linux, Inc.
Copyright (C) 2015, 2021  VMware, Inc.
This program is free software, covered by the GNU General Public License,
and you are welcome to change it and/or distribute copies of it under
certain conditions.  Enter "help copying" to see the conditions.
This program has absolutely no warranty.  Enter "help warranty" for details.

GNU gdb (GDB) 10.2
Copyright (C) 2021 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-pc-linux-gnu".
Type "show configuration" for configuration details.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...

      KERNEL: /usr/lib/debug/boot/vmlinux-5.15.0-133-generic  [TAINTED]
    DUMPFILE: /proc/kcore
        CPUS: 128
        DATE: Sun Feb 23 07:40:55 UTC 2025
      UPTIME: 00:26:49
LOAD AVERAGE: 0.09, 0.06, 0.09
       TASKS: 1732
    NODENAME: tenstorrent
     RELEASE: 5.15.0-133-generic
     VERSION: #144-Ubuntu SMP Fri Feb 7 20:47:38 UTC 2025
     MACHINE: x86_64  (3095 Mhz)
      MEMORY: 511.7 GB
         PID: 14247
     COMMAND: "crash"
        TASK: ff413a25af22a000  [THREAD_INFO: ff413a25af22a000]
         CPU: 114
       STATE: TASK_RUNNING (ACTIVE)

crash> mod -s stackbuf stackbuf.ko

crash> printf "%s", (char*)&stackbuf[0]
stackbuf_init+0x1a/0xeda [stackbuf]
do_one_initcall+0x46/0x1e0
do_init_module+0x52/0x260
load_module+0xb45/0xbe0
__do_sys_finit_module+0xbf/0x120
__x64_sys_finit_module+0x18/0x20
x64_sys_call+0x1ac3/0x1fa0
do_syscall_64+0x56/0xb0
entry_SYSCALL_64_after_hwframe+0x6c/0xd6
crash> printf "%s", (char*)&stackbuf[0]
stackbuf_init+0x1a/0xeda [stackbuf]
do_one_initcall+0x46/0x1e0
do_init_module+0x52/0x260
load_module+0xb45/0xbe0
__do_sys_finit_module+0xbf/0x120
__x64_sys_finit_module+0x18/0x20
x64_sys_call+0x1ac3/0x1fa0
do_syscall_64+0x56/0xb0
entry_SYSCALL_64_after_hwframe+0x6c/0xd6

crash> printf "%s", (char*)&stackbuf[0] > global.log

$ ~/linux-next/scripts/decode_stacktrace.sh /usr/lib/debug/boot/vmlinux-5.15.0-133-generic stackbuf.ko `pwd` < global.log > decoded-global.lo

$ cat  decoded-global.log
stackbuf_init (/home/tester/stackbuf/stackbuf.c:12) stackbuf
do_one_initcall (/build/linux-mJOtZy/linux-5.15.0/init/main.c:1302)
do_init_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:3740)
load_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:4148)
__do_sys_finit_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:4239)
__x64_sys_finit_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:4216)
x64_sys_call (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/syscall_64.c:33)
do_syscall_64 (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/common.c:51 /build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/common.c:81)
entry_SYSCALL_64_after_hwframe (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/entry_64.S:119)
```

## Get stack tracing like `dump_stack()`
Here, we used print statements.
```
# insmod stackbuf.ko
# rmmod stackbuf.ko
# dmesg | tail -50
```

```
$ cat stackbuf.log

[  618.059960] INIT stackbuf.
[  618.059978] Stack trace(271):
               stackbuf_init+0x1a/0xeda [stackbuf]
               do_one_initcall+0x46/0x1e0
               do_init_module+0x52/0x260
               load_module+0xb45/0xbe0
               __do_sys_finit_module+0xbf/0x120
               __x64_sys_finit_module+0x18/0x20
               x64_sys_call+0x1ac3/0x1fa0
               do_syscall_64+0x56/0xb0
               entry_SYSCALL_64_after_hwframe+0x6c/0xd6
[  626.558990] Stack trace(207):
               stackbuf_exit+0xe/0x36 [stackbuf]
               __do_sys_delete_module.constprop.0+0x184/0x290
               __x64_sys_delete_module+0x12/0x20
               x64_sys_call+0x16cf/0x1fa0
               do_syscall_64+0x56/0xb0
               entry_SYSCALL_64_after_hwframe+0x6c/0xd6
[  626.558993] EXIT stackbuf.

$ ~/linux-next/scripts/decode_stacktrace.sh /usr/lib/debug/boot/vmlinux-5.15.0-130-generic stacktrace.ko  `pwd` < stacktrace.log > decoded-stacktrace.log

$ cat decoded-stackbuf.log

[  618.059978] Stack trace(271):
stackbuf_init (/home/tester/stackbuf/stackbuf.c:12) stackbuf
do_one_initcall (/build/linux-mJOtZy/linux-5.15.0/init/main.c:1302)
do_init_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:3740)
load_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:4148)
__do_sys_finit_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:4239)
__x64_sys_finit_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:4216)
x64_sys_call (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/syscall_64.c:33)
do_syscall_64 (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/common.c:51 /build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/common.c:81)
entry_SYSCALL_64_after_hwframe (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/entry_64.S:119)
[  626.558990] Stack trace(207):
stackbuf_exit (/home/tester/stackbuf/stackbuf.c:26 /home/tester/stackbuf/stackbuf.c:36) stackbuf
__do_sys_delete_module.constprop.0 (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:969)
__x64_sys_delete_module (/build/linux-mJOtZy/linux-5.15.0/kernel/module.c:912)
x64_sys_call (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/syscall_64.c:33)
do_syscall_64 (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/common.c:51 /build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/common.c:81)
entry_SYSCALL_64_after_hwframe (/build/linux-mJOtZy/linux-5.15.0/arch/x86/entry/entry_64.S:119)
[  626.558993] EXIT stackbuf.
```

