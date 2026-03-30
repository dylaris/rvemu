#include "syscall.h"

const char *syscall_to_string(SyscallNr n)
{
#define X(name, number) case SYS_##name: return #name;
    switch (n) {
    SYSCALLS(X)
    default:
        unreachable();
    }
#undef X
}

typedef u64 (*SyscallFunc)(Machine *);

static u64 sys_exit(Machine *mp) { (void) mp; fatal("unimplemented sys_exit"); }
static u64 sys_exit_group(Machine *mp) { (void) mp; fatal("unimplemented sys_exit_group"); }
static u64 sys_getpid(Machine *mp) { (void) mp; fatal("unimplemented sys_getpid"); }
static u64 sys_kill(Machine *mp) { (void) mp; fatal("unimplemented sys_kill"); }
static u64 sys_tgkill(Machine *mp) { (void) mp; fatal("unimplemented sys_tgkill"); }
static u64 sys_read(Machine *mp) { (void) mp; fatal("unimplemented sys_read"); }
static u64 sys_write(Machine *mp) { (void) mp; fatal("unimplemented sys_write"); }
static u64 sys_openat(Machine *mp) { (void) mp; fatal("unimplemented sys_openat"); }
static u64 sys_close(Machine *mp) { (void) mp; fatal("unimplemented sys_close"); }
static u64 sys_lseek(Machine *mp) { (void) mp; fatal("unimplemented sys_lseek"); }
static u64 sys_brk(Machine *mp) { (void) mp; fatal("unimplemented sys_brk"); }
static u64 sys_linkat(Machine *mp) { (void) mp; fatal("unimplemented sys_linkat"); }
static u64 sys_unlinkat(Machine *mp) { (void) mp; fatal("unimplemented sys_unlinkat"); }
static u64 sys_mkdirat(Machine *mp) { (void) mp; fatal("unimplemented sys_mkdirat"); }
static u64 sys_renameat(Machine *mp) { (void) mp; fatal("unimplemented sys_renameat"); }
static u64 sys_chdir(Machine *mp) { (void) mp; fatal("unimplemented sys_chdir"); }
static u64 sys_getcwd(Machine *mp) { (void) mp; fatal("unimplemented sys_getcwd"); }
static u64 sys_fstat(Machine *mp) { (void) mp; fatal("unimplemented sys_fstat"); }
static u64 sys_fstatat(Machine *mp) { (void) mp; fatal("unimplemented sys_fstatat"); }
static u64 sys_faccessat(Machine *mp) { (void) mp; fatal("unimplemented sys_faccessat"); }
static u64 sys_pread(Machine *mp) { (void) mp; fatal("unimplemented sys_pread"); }
static u64 sys_pwrite(Machine *mp) { (void) mp; fatal("unimplemented sys_pwrite"); }
static u64 sys_uname(Machine *mp) { (void) mp; fatal("unimplemented sys_uname"); }
static u64 sys_getuid(Machine *mp) { (void) mp; fatal("unimplemented sys_getuid"); }
static u64 sys_geteuid(Machine *mp) { (void) mp; fatal("unimplemented sys_geteuid"); }
static u64 sys_getgid(Machine *mp) { (void) mp; fatal("unimplemented sys_getgid"); }
static u64 sys_getegid(Machine *mp) { (void) mp; fatal("unimplemented sys_getegid"); }
static u64 sys_gettid(Machine *mp) { (void) mp; fatal("unimplemented sys_gettid"); }
static u64 sys_sysinfo(Machine *mp) { (void) mp; fatal("unimplemented sys_sysinfo"); }
static u64 sys_mmap(Machine *mp) { (void) mp; fatal("unimplemented sys_mmap"); }
static u64 sys_munmap(Machine *mp) { (void) mp; fatal("unimplemented sys_munmap"); }
static u64 sys_mremap(Machine *mp) { (void) mp; fatal("unimplemented sys_mremap"); }
static u64 sys_mprotect(Machine *mp) { (void) mp; fatal("unimplemented sys_mprotect"); }
static u64 sys_prlimit64(Machine *mp) { (void) mp; fatal("unimplemented sys_prlimit64"); }
static u64 sys_getmainvars(Machine *mp) { (void) mp; fatal("unimplemented sys_getmainvars"); }
static u64 sys_rt_sigaction(Machine *mp) { (void) mp; fatal("unimplemented sys_rt_sigaction"); }
static u64 sys_writev(Machine *mp) { (void) mp; fatal("unimplemented sys_writev"); }
static u64 sys_gettimeofday(Machine *mp) { (void) mp; fatal("unimplemented sys_gettimeofday"); }
static u64 sys_times(Machine *mp) { (void) mp; fatal("unimplemented sys_times"); }
static u64 sys_fcntl(Machine *mp) { (void) mp; fatal("unimplemented sys_fcntl"); }
static u64 sys_ftruncate(Machine *mp) { (void) mp; fatal("unimplemented sys_ftruncate"); }
static u64 sys_getdents(Machine *mp) { (void) mp; fatal("unimplemented sys_getdents"); }
static u64 sys_dup(Machine *mp) { (void) mp; fatal("unimplemented sys_dup"); }
static u64 sys_dup3(Machine *mp) { (void) mp; fatal("unimplemented sys_dup3"); }
static u64 sys_readlinkat(Machine *mp) { (void) mp; fatal("unimplemented sys_readlinkat"); }
static u64 sys_rt_sigprocmask(Machine *mp) { (void) mp; fatal("unimplemented sys_rt_sigprocmask"); }
static u64 sys_ioctl(Machine *mp) { (void) mp; fatal("unimplemented sys_ioctl"); }
static u64 sys_getrlimit(Machine *mp) { (void) mp; fatal("unimplemented sys_getrlimit"); }
static u64 sys_setrlimit(Machine *mp) { (void) mp; fatal("unimplemented sys_setrlimit"); }
static u64 sys_getrusage(Machine *mp) { (void) mp; fatal("unimplemented sys_getrusage"); }
static u64 sys_clock_gettime(Machine *mp) { (void) mp; fatal("unimplemented sys_clock_gettime"); }
static u64 sys_set_tid_address(Machine *mp) { (void) mp; fatal("unimplemented sys_set_tid_address"); }
static u64 sys_set_robust_list(Machine *mp) { (void) mp; fatal("unimplemented sys_set_robust_list"); }
static u64 sys_madvise(Machine *mp) { (void) mp; fatal("unimplemented sys_madvise"); }
static u64 sys_statx(Machine *mp) { (void) mp; fatal("unimplemented sys_statx"); }
static u64 sys_open(Machine *mp) { (void) mp; fatal("unimplemented sys_open"); }
static u64 sys_link(Machine *mp) { (void) mp; fatal("unimplemented sys_link"); }
static u64 sys_unlink(Machine *mp) { (void) mp; fatal("unimplemented sys_unlink"); }
static u64 sys_mkdir(Machine *mp) { (void) mp; fatal("unimplemented sys_mkdir"); }
static u64 sys_access(Machine *mp) { (void) mp; fatal("unimplemented sys_access"); }
static u64 sys_stat(Machine *mp) { (void) mp; fatal("unimplemented sys_stat"); }
static u64 sys_lstat(Machine *mp) { (void) mp; fatal("unimplemented sys_lstat"); }
static u64 sys_time(Machine *mp) { (void) mp; fatal("unimplemented sys_time"); }

#define X(name, number) [SYS_##name] = sys_##name,
static SyscallFunc syscall_table[] = {
    SYSCALLS(X)
};
#undef X

u64 do_syscall(Machine *mp, SyscallNr n)
{
    assert(n < ARRAY_SIZE(syscall_table));
    SyscallFunc func = syscall_table[n];
    if (!func)
        fatalf("unknown syscall number: %d", n);
    return func(mp);
}
