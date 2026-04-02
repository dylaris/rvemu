#include "syscall.h"

#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>

const char *syscall_to_string(SyscallNr n)
{
#define X(name, number) case SYS_##name: return #name;
    switch (n) {
    SYSCALL_LIST(X)
    default:
        unreachable();
    }
#undef X
}

#define GET(name, reg) u64 name = cpu_get_gpr(&machine->state, reg)

static u64 sys_exit(Machine *machine)
{
    GET(code, GPR_A0);
    exit(code);
}

static u64 sys_exit_group(Machine *machine) { (void) machine; fatal("unimplemented sys_exit_group"); }
static u64 sys_getpid(Machine *machine) { (void) machine; fatal("unimplemented sys_getpid"); }
static u64 sys_kill(Machine *machine) { (void) machine; fatal("unimplemented sys_kill"); }
static u64 sys_tgkill(Machine *machine) { (void) machine; fatal("unimplemented sys_tgkill"); }

static u64 sys_read(Machine *machine)
{
    GET(fd, GPR_A0);
    GET(buf, GPR_A1);
    GET(count, GPR_A2);
    return read(fd, (void *) mmu_to_host(buf), count);
}

static u64 sys_write(Machine *machine)
{
    GET(fd, GPR_A0);
    GET(buf, GPR_A1);
    GET(len, GPR_A2);
    return write(fd, (void *) mmu_to_host(buf), len);
}

// the O_* macros is OS dependent.
// here is a workaround to convert newlib flags to the host.
#define NEWLIB_O_RDONLY   0x0
#define NEWLIB_O_WRONLY   0x1
#define NEWLIB_O_RDWR     0x2
#define NEWLIB_O_APPEND   0x8
#define NEWLIB_O_CREAT  0x200
#define NEWLIB_O_TRUNC  0x400
#define NEWLIB_O_EXCL   0x800
#define REWRITE_FLAG(flag) if (flags & NEWLIB_ ##flag) hostflags |= flag;

static int convert_flags(int flags)
{
    int hostflags = 0;
    REWRITE_FLAG(O_RDONLY);
    REWRITE_FLAG(O_WRONLY);
    REWRITE_FLAG(O_RDWR);
    REWRITE_FLAG(O_APPEND);
    REWRITE_FLAG(O_CREAT);
    REWRITE_FLAG(O_TRUNC);
    REWRITE_FLAG(O_EXCL);
    return hostflags;
}

static u64 sys_openat(Machine *machine)
{
    GET(dirfd, GPR_A0);
    GET(path,  GPR_A1);
    GET(flags, GPR_A2);
    GET(mode,  GPR_A3);
    return openat(dirfd, (const char *) mmu_to_host(path), convert_flags(flags), mode);
}

static u64 sys_close(Machine *machine)
{
    GET(fd, GPR_A0);
    if (fd > 2)
        return close(fd);
    return 0;
}

static u64 sys_lseek(Machine *machine)
{
    GET(fd,     GPR_A0);
    GET(offset, GPR_A1);
    GET(whence, GPR_A2);
    return lseek(fd, offset, whence);
}

static u64 sys_brk(Machine *machine)
{
    GET(addr, GPR_A0);
    if (addr == 0)
        addr = machine->mem.heap_brk;
    assert(addr >= machine->mem.heap_base);
    i64 increment = (i64) addr - machine->mem.heap_brk;
    mem_alloc(&machine->mem, increment);
    return addr;
}

static u64 sys_linkat(Machine *machine)
{
    GET(olddirfd, GPR_A0);
    GET(oldpath,  GPR_A1);
    GET(newdirfd, GPR_A2);
    GET(newpath,  GPR_A3);
    GET(flags,    GPR_A4);
    return linkat(olddirfd, (const char *) mmu_to_host(oldpath),
                  newdirfd, (const char *) mmu_to_host(newpath),
                  convert_flags(flags));
}

static u64 sys_unlinkat(Machine *machine)
{
    GET(dirfd, GPR_A0);
    GET(path,  GPR_A1);
    GET(flags, GPR_A2);
    return unlinkat(dirfd, (const char *) mmu_to_host(path), convert_flags(flags));
}

static u64 sys_mkdirat(Machine *machine) { (void) machine; fatal("unimplemented sys_mkdirat"); }
static u64 sys_renameat(Machine *machine) { (void) machine; fatal("unimplemented sys_renameat"); }
static u64 sys_chdir(Machine *machine) { (void) machine; fatal("unimplemented sys_chdir"); }
static u64 sys_getcwd(Machine *machine) { (void) machine; fatal("unimplemented sys_getcwd"); }

static u64 sys_fstat(Machine *machine)
{
    GET(fd, GPR_A0);
    GET(addr, GPR_A1);
    return fstat(fd, (struct stat *) mmu_to_host(addr));
}

static u64 sys_fstatat(Machine *machine) { (void) machine; fatal("unimplemented sys_fstatat"); }
static u64 sys_faccessat(Machine *machine) { (void) machine; fatal("unimplemented sys_faccessat"); }
static u64 sys_pread(Machine *machine) { (void) machine; fatal("unimplemented sys_pread"); }
static u64 sys_pwrite(Machine *machine) { (void) machine; fatal("unimplemented sys_pwrite"); }
static u64 sys_uname(Machine *machine) { (void) machine; fatal("unimplemented sys_uname"); }
static u64 sys_getuid(Machine *machine) { (void) machine; fatal("unimplemented sys_getuid"); }
static u64 sys_geteuid(Machine *machine) { (void) machine; fatal("unimplemented sys_geteuid"); }
static u64 sys_getgid(Machine *machine) { (void) machine; fatal("unimplemented sys_getgid"); }
static u64 sys_getegid(Machine *machine) { (void) machine; fatal("unimplemented sys_getegid"); }
static u64 sys_gettid(Machine *machine) { (void) machine; fatal("unimplemented sys_gettid"); }
static u64 sys_sysinfo(Machine *machine) { (void) machine; fatal("unimplemented sys_sysinfo"); }
static u64 sys_mmap(Machine *machine) { (void) machine; fatal("unimplemented sys_mmap"); }
static u64 sys_munmap(Machine *machine) { (void) machine; fatal("unimplemented sys_munmap"); }
static u64 sys_mremap(Machine *machine) { (void) machine; fatal("unimplemented sys_mremap"); }
static u64 sys_mprotect(Machine *machine) { (void) machine; fatal("unimplemented sys_mprotect"); }
static u64 sys_prlimit64(Machine *machine) { (void) machine; fatal("unimplemented sys_prlimit64"); }
static u64 sys_getmainvars(Machine *machine) { (void) machine; fatal("unimplemented sys_getmainvars"); }
static u64 sys_rt_sigaction(Machine *machine) { (void) machine; fatal("unimplemented sys_rt_sigaction"); }
static u64 sys_writev(Machine *machine) { (void) machine; fatal("unimplemented sys_writev"); }

static u64 sys_gettimeofday(Machine *machine)
{
    GET(tv, GPR_A0);
    GET(tz, GPR_A1);
    return gettimeofday((struct timeval *) mmu_to_host(tv),
                        tz != 0 ? (struct timezone *) mmu_to_host(tz) : NULL);
}

static u64 sys_times(Machine *machine) { (void) machine; fatal("unimplemented sys_times"); }
static u64 sys_fcntl(Machine *machine) { (void) machine; fatal("unimplemented sys_fcntl"); }
static u64 sys_ftruncate(Machine *machine) { (void) machine; fatal("unimplemented sys_ftruncate"); }
static u64 sys_getdents(Machine *machine) { (void) machine; fatal("unimplemented sys_getdents"); }
static u64 sys_dup(Machine *machine) { (void) machine; fatal("unimplemented sys_dup"); }
static u64 sys_dup3(Machine *machine) { (void) machine; fatal("unimplemented sys_dup3"); }
static u64 sys_readlinkat(Machine *machine) { (void) machine; fatal("unimplemented sys_readlinkat"); }
static u64 sys_rt_sigprocmask(Machine *machine) { (void) machine; fatal("unimplemented sys_rt_sigprocmask"); }
static u64 sys_ioctl(Machine *machine) { (void) machine; fatal("unimplemented sys_ioctl"); }
static u64 sys_getrlimit(Machine *machine) { (void) machine; fatal("unimplemented sys_getrlimit"); }
static u64 sys_setrlimit(Machine *machine) { (void) machine; fatal("unimplemented sys_setrlimit"); }
static u64 sys_getrusage(Machine *machine) { (void) machine; fatal("unimplemented sys_getrusage"); }
static u64 sys_clock_gettime(Machine *machine) { (void) machine; fatal("unimplemented sys_clock_gettime"); }
static u64 sys_set_tid_address(Machine *machine) { (void) machine; fatal("unimplemented sys_set_tid_address"); }
static u64 sys_set_robust_list(Machine *machine) { (void) machine; fatal("unimplemented sys_set_robust_list"); }
static u64 sys_madvise(Machine *machine) { (void) machine; fatal("unimplemented sys_madvise"); }
static u64 sys_statx(Machine *machine) { (void) machine; fatal("unimplemented sys_statx"); }

static u64 sys_open(Machine *machine)
{
    GET(path,  GPR_A0);
    GET(flags, GPR_A1);
    GET(mode,  GPR_A2);
    return open((const char *) mmu_to_host(path), convert_flags(flags), mode);
}

static u64 sys_link(Machine *machine)
{
    GET(oldpath, GPR_A0);
    GET(newpath, GPR_A0);
    return link((const char *) mmu_to_host(oldpath), (const char *) mmu_to_host(newpath));
}


static u64 sys_unlink(Machine *machine)
{
    GET(path, GPR_A0);
    return unlink((const char *) mmu_to_host(path));
}

static u64 sys_mkdir(Machine *machine) { (void) machine; fatal("unimplemented sys_mkdir"); }
static u64 sys_access(Machine *machine) { (void) machine; fatal("unimplemented sys_access"); }
static u64 sys_stat(Machine *machine) { (void) machine; fatal("unimplemented sys_stat"); }
static u64 sys_lstat(Machine *machine) { (void) machine; fatal("unimplemented sys_lstat"); }
static u64 sys_time(Machine *machine) { (void) machine; fatal("unimplemented sys_time"); }

#define X(name, number) [SYS_##name] = sys_##name,
static SyscallFunc syscall_table[] = {
    SYSCALL_LIST(X)
};
#undef X

SyscallFunc syscall_get(SyscallNr n)
{
    assert(n < ARRAY_SIZE(syscall_table));
    SyscallFunc func = syscall_table[n];
    if (!func)
        fatalf("unknown syscall number: %d", n);
    return func;
}
