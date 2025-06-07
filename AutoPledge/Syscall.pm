#!/usr/bin/perl
#
# (c)2025 Ira Parsons
# AutoPledge::Syscall - detect direct linux syscalls from elf binary
#

package AutoPledge::Syscall;

use AutoPledge::Core ':promises';

# enumerate promises by syscall number
my @syscalls = (
                STDIO, # read = 0
                STDIO | WPATH, # write = 1
                RPATH | WPATH | CPATH,  # open = 2
                STDIO, # close = 3
                RPATH | WPATH, # stat = 4
                STDIO | WPATH | RPATH, # fstat = 5
                RPATH | TMPPATH | WPATH, # lstat = 6
                STDIO, # poll = 7
                STDIO, # lseek = 8
                STDIO, # mmap = 9
                STDIO, # mprotect = 10
                STDIO, # munmap = 11
                STDIO, # brk = 12
                STDIO, # rt_sigaction = 13
                STDIO, # rt_sigprocmask = 14
                STDIO, # rt_sigreturn = 15
                TTY | STDIO, # ioctl = 16
                STDIO,  # pread64 = 17
                STDIO, # pwrite64 = 18
                STDIO, # readv = 19
                STDIO, # writev = 20
                WPATH | RPATH, # access = 21
                STDIO, # pipe = 22
                STDIO, # select = 23
                0,     # sched_yield = 24
                0,     # mremap = 25
                STDIO, # msync = 26
                0,     # mincore = 27
                STDIO, # madvise = 28
                0,     # shmget = 29
                0,     # shmat = 30
                0,     # shmctl = 31
                STDIO, # dup = 32
                STDIO, # dup2 = 33
                0,     # pause = 34
                STDIO, # nanosleep = 35
                STDIO, # getitimer = 36
                0,     # alarm = 37
                STDIO, # setitimer = 38
                STDIO, # getpid = 39
                0,     # sendfile = 40
                DNS | INET | UNIX, # socket = 41
                DNS | INET | UNIX, # connect = 42
                INET | UNIX, # accept = 43
                DNS | INET | STDIO, # sendto = 44
                DNS | STDIO, # recvfrom = 45
                0,     # sendmsg = 46
                RECVFD,# recvmsg = 47
                STDIO, # shutdown = 48
                INET | UNIX, # bind = 49
                INET | UNIX, # listen = 50
                INET | UNIX, # getsockname = 51
                INET | UNIX, # getpeername = 52
                STDIO, # socketpair = 53
                INET | UNIX, # setsockopt = 54
                INET | UNIX, # getsockopt = 55
                THREAD,# clone = 56
                PROC,  # fork = 57
                PROC,  # vfork = 58
                EXEC,  # execve = 59
                0,     # exit = 60
                STDIO, # wait4 = 61
                PROC,  # kill = 62
                STDIO, # uname = 63
                0,     # semget = 64
                0,     # semop = 65
                0,     # semctl = 66
                0,     # shmdt = 67
                0,     # msgget = 68
                0,     # msgsnd = 69
                0,     # msgrcv = 70
                0,     # msgctl = 71
                FLOCK | STDIO, # fcntl = 72
                FLOCK, # flock = 73
                STDIO, # fsync = 74
                STDIO, # fdatasync = 75
                0,     # truncate = 76
                STDIO, # ftruncate = 77
                STDIO, # getdents = 78
                WPATH | RPATH, # getcwd = 79
                RPATH, # chdir = 80
                STDIO, # fchdir = 81
                CPATH, # rename = 82
                CPATH, # mkdir = 83
                CPATH, # rmdir = 84
                0,     # creat = 85
                CPATH, # link = 86
                TMPPATH | CPATH,# unlink = 87
                CPATH, # symlink = 88
                WPATH | RPATH, # readlink = 89
                FATTR | WPATH, # chmod = 90
                FATTR, # fchmod = 91
                CHOWN, # chown = 92
                CHOWN, # fchown = 93
                CHOWN, # lchown = 94
                STDIO, # umask = 95
                STDIO, # gettimeofday = 96
                STDIO, # getrlimit = 97
                0,     # getrusage = 98
                0,     # sysinfo = 99
                0,     # times = 100
                0,     # ptrace = 101
                STDIO, # getuid = 102
                0,     # syslog = 103
                STDIO, # getgid = 104
                ID,    # setuid = 105
                ID,    # setgid = 106
                STDIO, # geteuid = 107
                STDIO, # getegid = 108
                PROC,  # setpgid = 109
                STDIO, # getppid = 110
                STDIO, # getpgrp = 111
                PROC,  # setsid = 112
                ID,    # setreuid = 113
                ID,    # setregid = 114
                STDIO, # getgroups = 115
                ID,    # setgroups = 116
                ID,    # setresuid = 117
                STDIO, # getresuid = 118
                ID,    # setresgid = 119
                STDIO, # getresgid = 120
                STDIO, # getpgid = 121
                ID,    # setfsuid = 122
                ID,    # setfsgid = 123
                STDIO, # getsid = 124
                0,     # capget = 125
                0,     # capset = 126
                0,     # rt_sigpending = 127
                0,     # rt_sigtimedwait = 128
                0,     # rt_sigqueueinfo = 129
                STDIO, # rt_sigsuspend = 130
                STDIO, # sigaltstack = 131
                FATTR, # utime = 132
                DPATH, # mknod = 133
                0,     # uselib = 134
                0,     # personality = 135
                0,     # ustat = 136
                RPATH, # statfs = 137
                RPATH, # fstatfs = 138
                0,     # sysfs = 139
                ID | PROC, # getpriority = 140
                ID | PROC, # setpriority = 141
                PROC,  # sched_setparam = 142
                PROC,  # sched_getparam = 143
                PROC,  # sched_setscheduler = 144
                PROC,  # sched_getscheduler = 145
                PROC,  # sched_get_priority_max = 146
                PROC,  # sched_get_priority_min = 147
                0,     # sched_rr_get_interval = 148
                0,     # mlock = 149
                0,     # munlock = 150
                0,     # mlockall = 151
                0,     # munlockall = 152
                0,     # vhangup = 153
                0,     # modify_ldt = 154
                0,     # pivot_root = 155
                0,     # _sysctl = 156
                0,     # prctl = 157
                STDIO, # arch_prctl = 158
                0,     # adjtimex = 159
                ID,    # setrlimit = 160
                0,     # chroot = 161
                0,     # sync = 162
                0,     # acct = 163
                0,     # settimeofday = 164
                0,     # mount = 165
                0,     # umount2 = 166
                0,     # swapon = 167
                0,     # swapoff = 168
                0,     # reboot = 169
                0,     # sethostname = 170
                0,     # setdomainname = 171
                0,     # iopl = 172
                0,     # ioperm = 173
                0,     # create_module = 174
                0,     # init_module = 175
                0,     # delete_module = 176
                0,     # get_kernel_syms = 177
                0,     # query_module = 178
                0,     # quotactl = 179
                0,     # nfsservctl = 180
                0,     # getpmsg = 181
                0,     # putpmsg = 182
                0,     # afs_syscall = 183
                0,     # tuxcall = 184
                0,     # security = 185
                0,     # gettid = 186
                0,     # readahead = 187
                0,     # setxattr = 188
                0,     # lsetxattr = 189
                0,     # fsetxattr = 190
                0,     # getxattr = 191
                0,     # lgetxattr = 192
                0,     # fgetxattr = 193
                0,     # listxattr = 194
                0,     # llistxattr = 195
                0,     # flistxattr = 196
                0,     # removexattr = 197
                0,     # lremovexattr = 198
                0,     # fremovexattr = 199
                0,     # tkill = 200
                0,     # time = 201
                THREAD,# futex = 202
                0,     # sched_setaffinity = 203
                0,     # sched_getaffinity = 204
                0,     # set_thread_area = 205
                0,     # io_setup = 206
                0,     # io_destroy = 207
                0,     # io_getevents = 208
                0,     # io_submit = 209
                0,     # io_cancel = 210
                0,     # get_thread_area = 211
                0,     # lookup_dcookie = 212
                0,     # epoll_create = 213
                0,     # epoll_ctl_old = 214
                0,     # epoll_wait_old = 215
                0,     # remap_file_pages = 216
                0,     # getdents64 = 217
                STDIO, # set_tid_address = 218
                0,     # restart_syscall = 219
                0,     # semtimedop = 220
                0,     # fadvise64 = 221
                0,     # timer_create = 222
                0,     # timer_settime = 223
                0,     # timer_gettime = 224
                0,     # timer_getoverrun = 225
                0,     # timer_delete = 226
                0,     # clock_settime = 227
                STDIO, # clock_gettime = 228
                STDIO, # clock_getres = 229
                STDIO, # clock_nanosleep = 230
                STDIO, # exit_group = 231
                0,     # epoll_wait = 232
                0,     # epoll_ctl = 233
                0,     # tgkill = 234
                FATTR, # utimes = 235
                0,     # vserver = 236
                0,     # mbind = 237
                0,     # set_mempolicy = 238
                0,     # get_mempolicy = 239
                0,     # mq_open = 240
                0,     # mq_unlink = 241
                0,     # mq_timedsend = 242
                0,     # mq_timedreceive = 243
                0,     # mq_notify = 244
                0,     # mq_getsetattr = 245
                0,     # kexec_load = 246
                0,     # waitid = 247
                0,     # add_key = 248
                0,     # request_key = 249
                0,     # keyctl = 250
                0,     # ioprio_set = 251
                0,     # ioprio_get = 252
                0,     # inotify_init = 253
                0,     # inotify_add_watch = 254
                0,     # inotify_rm_watch = 255
                0,     # migrate_pages = 256
                CPATH | WPATH | RPATH, # openat = 257
                CPATH, # mkdirat = 258
                DPATH, # mknodat = 259
                CHOWN, # fchownat = 260
                0,     # futimesat = 261
                WPATH | RPATH, # newfstatat = 262
                TMPPATH | CPATH,# unlinkat = 263
                CPATH, # renameat = 264
                CPATH, # linkat = 265
                CPATH, # symlinkat = 266
                WPATH | RPATH, # readlinkat = 267
                FATTR | WPATH, # fchmodat = 268
                WPATH | RPATH, # faccessat = 269
                0,     # pselect6 = 270
                0,     # ppoll = 271
                0,     # unshare = 272
                0,     # set_robust_list = 273
                0,     # get_robust_list = 274
                0,     # splice = 275
                0,     # tee = 276
                0,     # sync_file_range = 277
                0,     # vmsplice = 278
                0,     # move_pages = 279
                FATTR, # utimensat = 280
                0,     # epoll_pwait = 281
                0,     # signalfd = 282
                0,     # timerfd_create = 283
                0,     # eventfd = 284
                0,     # fallocate = 285
                0,     # timerfd_settime = 286
                0,     # timerfd_gettime = 287
                INET | UNIX, # accept4 = 288
                0,     # signalfd4 = 289
                0,     # eventfd2 = 290
                0,     # epoll_create1 = 291
                STDIO, # dup3 = 292
                STDIO, # pipe2 = 293
                0,     # inotify_init1 = 294
                0,     # preadv = 295
                STDIO, # pwritev = 296
                0,     # rt_tgsigqueueinfo = 297
                0,     # perf_event_open = 298
                0,     # recvmmsg = 299
                0,     # fanotify_init = 300
                0,     # fanotify_mark = 301
                ID | PROC, # prlimit64 = 302
                0,     # name_to_handle_at = 303
                0,     # open_by_handle_at = 304
                0,     # clock_adjtime = 305
                0,     # syncfs = 306
                0,     # sendmmsg = 307
                0,     # setns = 308
                0,     # getcpu = 309
                0,     # process_vm_readv = 310
                0,     # process_vm_writev = 311
                0,     # kcmp = 312
                0,     # finit_module = 313
                0,     # sched_setattr = 314
                0,     # sched_getattr = 315
                CPATH, # renameat2 = 316
                0,     # seccomp = 317
                STDIO, # getrandom = 318
                0,     # memfd_create = 319
                0,     # kexec_file_load = 320
                0,     # bpf = 321
                EXEC,  # execveat = 322
                0,     # userfaultfd = 323
                0,     # membarrier = 324
                0,     # mlock2 = 325
                0,     # copy_file_range = 326
                0,     # preadv2 = 327
                0,     # pwritev2 = 328
                0,     # pkey_mprotect = 329
                0,     # pkey_alloc = 330
                0,     # pkey_free = 331
                0,     # statx = 332
                0,     # io_pgetevents = 333
                0,     # rseq = 334
                0,     # uretprobe = 335
                0,     # pidfd_send_signal = 424
                0,     # io_uring_setup = 425
                0,     # io_uring_enter = 426
                0,     # io_uring_register = 427
                0,     # open_tree = 428
                0,     # move_mount = 429
                0,     # fsopen = 430
                0,     # fsconfig = 431
                0,     # fsmount = 432
                0,     # fspick = 433
                0,     # pidfd_open = 434
                0,     # clone3 = 435
                0,     # close_range = 436
                0,     # openat2 = 437
                0,     # pidfd_getfd = 438
                0,     # faccessat2 = 439
                0,     # process_madvise = 440
                0,     # epoll_pwait2 = 441
                0,     # mount_setattr = 442
                0,     # quotactl_fd = 443
                0,     # landlock_create_ruleset = 444
                0,     # landlock_add_rule = 445
                0,     # landlock_restrict_self = 446
                0,     # memfd_secret = 447
                0,     # process_mrelease = 448
                0,     # futex_waitv = 449
                0,     # set_mempolicy_home_node = 450
                0,     # cachestat = 451
                0,     # fchmodat2 = 452
                0,     # map_shadow_stack = 453
                0,     # futex_wake = 454
                0,     # futex_wait = 455
                0,     # futex_requeue = 456
                0,     # statmount = 457
                0,     # listmount = 458
                0,     # lsm_get_self_attr = 459
                0,     # lsm_set_self_attr = 460
                0,     # lsm_list_modules = 461
                0,     # mseal = 462
                0,     # setxattrat = 463
                0,     # getxattrat = 464
                0,     # listxattrat = 465
                0      # removexattrat = 466
);

sub detect {

    # get .text section of binary and disassemble
    my $raw = `objdump -Dj .text $_[0]`;

    # form lines into array
    my @lines = split("\n", $raw);

    # filter out lint
    splice(@lines, 0, 6);
    my $i = 0;
    foreach $string (@lines) {
        if($string =~ /^\s*$/) {
            splice(@lines, $i, 1);
        };
        if($string !~ /^\s/) {
	    splice(@lines, $i, 1);
        };
        $i++;
    }

    # format into address/instruction pairs
    my %mem;
    my $entry = -1;
    foreach $string (@lines) {
        $string =~ /^\s+(.*):.{23}(.*)$/;
        $mem{$1} = $2;
        if ($entry == -1) {
            $entry = $1;
        };
    }

    # search for direct syscalls
    # int 0x80 is deprecated and syscall numbers are inconsistent for code old enough to use it
    # sysenter throws illegal instruction on 64 bit
    my @dsys = grep { $mem{$_} =~ /syscall/} keys %mem;

    # determine syscall number for each syscall
    my @calls;
    foreach $string (@dsys) {
        my $addr = $string;
        do {
            $addr = sprintf("%x", hex($addr) - 1);
            ### Works for constant mov, xor zeroing ###
            if($mem{$addr} =~ /^mov\s*\$(.*),%.ax/) {
                push(@calls, hex($1));
                break;
            }
            elsif($mem{$addr} =~ /^xor\s*%.ax,%.ax/) {
                push(@calls, 0);
                break;
            }
            elsif($mem{$addr} =~ /,%.ax/) {
                # something else is happening to the register (poisoned)
                break;
            };
        } while(hex($addr) > hex($entry));
    };

    # assign promises by syscall number
    my $promises = 0;
    foreach $num (@calls) {
        if($num <= $#syscalls){
            $promises |= $syscalls[$num];
        };
    };

    return $promises;

}

1;
