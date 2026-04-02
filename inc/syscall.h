#ifndef SYSCALL_H
#define SYSCALL_H

#include "machine.h"

#define SYSCALL_LIST(X)      \
    X(exit,            93)   \
    X(exit_group,      94)   \
    X(getpid,          172)  \
    X(kill,            129)  \
    X(tgkill,          131)  \
    X(read,            63)   \
    X(write,           64)   \
    X(openat,          56)   \
    X(close,           57)   \
    X(lseek,           62)   \
    X(brk,             214)  \
    X(linkat,          37)   \
    X(unlinkat,        35)   \
    X(mkdirat,         34)   \
    X(renameat,        38)   \
    X(chdir,           49)   \
    X(getcwd,          17)   \
    X(fstat,           80)   \
    X(fstatat,         79)   \
    X(faccessat,       48)   \
    X(pread,           67)   \
    X(pwrite,          68)   \
    X(uname,           160)  \
    X(getuid,          174)  \
    X(geteuid,         175)  \
    X(getgid,          176)  \
    X(getegid,         177)  \
    X(gettid,          178)  \
    X(sysinfo,         179)  \
    X(mmap,            222)  \
    X(munmap,          215)  \
    X(mremap,          216)  \
    X(mprotect,        226)  \
    X(prlimit64,       261)  \
    X(getmainvars,     2011) \
    X(rt_sigaction,    134)  \
    X(writev,          66)   \
    X(gettimeofday,    169)  \
    X(times,           153)  \
    X(fcntl,           25)   \
    X(ftruncate,       46)   \
    X(getdents,        61)   \
    X(dup,             23)   \
    X(dup3,            24)   \
    X(readlinkat,      78)   \
    X(rt_sigprocmask,  135)  \
    X(ioctl,           29)   \
    X(getrlimit,       163)  \
    X(setrlimit,       164)  \
    X(getrusage,       165)  \
    X(clock_gettime,   113)  \
    X(set_tid_address, 96)   \
    X(set_robust_list, 99)   \
    X(madvise,         233)  \
    X(statx,           291)  \
    X(open,            1024) \
    X(link,            1025) \
    X(unlink,          1026) \
    X(mkdir,           1030) \
    X(access,          1033) \
    X(stat,            1038) \
    X(lstat,           1039) \
    X(time,            1062)

#define OLD_SYSCALL_THRESHOLD 1024

#define X(name, number) SYS_##name = number,
typedef enum {
    SYSCALL_LIST(X)
} SyscallNr;
#undef X

typedef u64 (*SyscallFunc)(Machine *);

const char *syscall_to_string(SyscallNr n);
SyscallFunc syscall_get(SyscallNr n);

#endif // SYSCALL_H
