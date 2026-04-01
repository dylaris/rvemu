#include "trap.h"

#include <signal.h>

static jmp_buf *trap_jmp_buf = NULL;
static Trap trap;

static void trap__signal_handler(int sig)
{
    switch (sig) {
    case SIGSEGV:
        trap.kind = TRAP_LOAD_FAULT;
        break;
    case SIGILL:
        trap.kind = TRAP_ILLEGAL_INSTR;
        break;
    case SIGBUS:
        trap.kind = TRAP_LOAD_MISALIGN;
        break;
    case SIGFPE:
        trap.kind = TRAP_LOAD_FAULT;
        break;
    default:
        trap.kind = TRAP_CRASH;
        break;
    }

    trap.fault_addr = 0;

    if (trap_jmp_buf) {
        trap_leave();
        longjmp(*trap_jmp_buf, 1);
    }
}

void trap_init(void)
{
    signal(SIGSEGV, trap__signal_handler);
    signal(SIGILL,  trap__signal_handler);
    signal(SIGBUS,  trap__signal_handler);
    signal(SIGFPE,  trap__signal_handler);
}

void trap_enter(jmp_buf *buf)
{
    trap_jmp_buf = buf;
}

void trap_leave(void)
{
    trap_jmp_buf = NULL;
}

void trap_throw(TrapKind kind, GuestVAddr fault_addr)
{
    trap.kind = kind;
    trap.fault_addr = fault_addr;

    if (trap_jmp_buf) {
        jmp_buf *saved = trap_jmp_buf;
        trap_leave();
        longjmp(*saved, 1);
    }

    fatal("uncaught trap");
}

Trap trap_get(void)
{
    return trap;
}
