#include "machine.h"
#include "syscall.h"
#include "trap.h"

static void machine__load(Machine *machine, const char *prog)
{
    FILE *f = fopen(prog, "rb");
    if (!f)
        fatal(strerror(errno));

    mem_load_elf(&machine->mem, f);

    fclose(f);

    machine->state.pc = (u64) machine->mem.entry;
}

static void machine__setup(Machine *machine, int argc, char **argv)
{
    u64 stack_size = 32 * 1024 * 1024;
    GuestVAddr stack_base = mem_alloc(&machine->mem, stack_size);
    GuestVAddr stack_end  = stack_base + stack_size;

    cpu_set_gpr(&machine->state, GPR_SP, stack_end);

    stack_end -= 8; // auxv
    cpu_set_gpr(&machine->state, GPR_SP, stack_end);

    stack_end -= 8; // envp
    cpu_set_gpr(&machine->state, GPR_SP, stack_end);

    stack_end -= 8; // argv end
    cpu_set_gpr(&machine->state, GPR_SP, stack_end);

    for (int i = argc - 1; i >= 0; i--) {
        size_t len = strlen(argv[i]);
        GuestVAddr addr = mem_alloc(&machine->mem, len + 1);
        mem_write(addr, (void *) argv[i], len);

        stack_end -= 8; // argv[i]
        cpu_set_gpr(&machine->state, GPR_SP, stack_end);
        mem_write(stack_end, (void *) &addr, sizeof(addr));
    }

    stack_end -= 8; // argc
    cpu_set_gpr(&machine->state, GPR_SP, stack_end);
    mem_write(stack_end, (void *) &argc, sizeof(argc));
}

void machine_step(Machine *machine)
{
    while (1) {
        cpu_clean_trace(&machine->state);
        interp_block(&machine->state);
        machine->state.pc = machine->state.trace.target_pc;
    }
}

static void do_syscall(Machine *machine)
{
    SyscallNr n = (SyscallNr) cpu_get_gpr(&machine->state, GPR_A7);
    SyscallFunc f = syscall_get(n);
    u64 ret = f(machine);
    cpu_set_gpr(&machine->state, GPR_A0, ret);
}

static void do_trap(Machine *machine, Trap trap)
{
    switch (trap.kind) {
    case TRAP_ECALL_U:
        do_syscall(machine);
        machine->state.pc += 4;
        break;

    case TRAP_ILLEGAL_INSTR:
        fatalf("illegal instruction '' @ 0x%016lx", trap.fault_addr);

    case TRAP_LOAD_FAULT:
    case TRAP_STORE_FAULT:
        fatalf("memory access fault @ 0x%016lx", trap.fault_addr);

    case TRAP_LOAD_MISALIGN:
    case TRAP_STORE_MISALIGN:
        fatalf("memory align fault @ 0x%016lx", trap.fault_addr);

    case TRAP_CRASH:
        fatalf("this emulator crashed: %s", strerror(errno));

    default:
        unreachable();
    }
}

bool machine_run(Machine *machine)
{
    jmp_buf trap_buf;

    trap_init();

    while (1) {
        if (setjmp(trap_buf) == 0) {
            trap_enter(&trap_buf);
            machine_step(machine);
            trap_leave();
        } else {
            Trap trap = trap_get();
            do_trap(machine, trap);
        }
    }
}

void machine_init(Machine *machine, const char *prog, int argc, char **argv)
{
    machine__load(machine, prog);
    machine__setup(machine, argc-1, argv+1);
}

void machine_fini(Machine *machine)
{
    (void) machine;
}
