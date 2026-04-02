#include "machine.h"
#include "syscall.h"

static void machine__load(Machine *machine, const char *prog)
{
    FILE *f = fopen(prog, "rb");
    if (!f)
        fatal(strerror(errno));

#ifdef TEST_TVM
    mem_load_bin(&machine->mem, f, 0x80000000);
#else
    mem_load_elf(&machine->mem, f);
#endif

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

static void do_syscall(Machine *machine)
{
    SyscallNr n = (SyscallNr) cpu_get_gpr(&machine->state, GPR_A7);
    SyscallFunc f = syscall_get(n);
    u64 ret = f(machine);
    cpu_set_gpr(&machine->state, GPR_A0, ret);
}

static void do_trap(Machine *machine)
{
    switch (machine->state.flow.ctl) {
    case FLOW_ECALL:
        do_syscall(machine);
        machine->state.pc = machine->state.flow.pc;
        break;

    case FLOW_ILLEGAL_INSTR:
        fatalf("illegal instruction '0x%08x' @ 0x%016lx", mem_read_u32(machine->state.pc), machine->state.pc);

    case FLOW_LOAD_FAULT:
    case FLOW_STORE_FAULT:
        fatalf("memory access fault @ 0x%016lx", machine->state.pc);

    case FLOW_LOAD_MISALIGN:
    case FLOW_STORE_MISALIGN:
        fatalf("memory align fault @ 0x%016lx", machine->state.pc);

    case FLOW_CRASH:
        fatalf("this emulator crashed: %s", strerror(errno));

    default:
        unreachable();
    }
}

BlockExec machine_dispatch(Machine *machine)
{
    (void) machine;
    return interp_block;
}

void machine_step(Machine *machine, BlockExec func)
{
    cpu_reset_flow(&machine->state);
    func(&machine->state);
}

bool machine_run(Machine *machine)
{
    while (true) {
        BlockExec func = machine_dispatch(machine);
        machine_step(machine, func);
        if (IS_TRAP(machine->state.flow.ctl))
            do_trap(machine);
        else
            machine->state.pc = machine->state.flow.pc;
    }
}

void machine_init(Machine *machine, const char *prog, int argc, char **argv)
{
    machine__load(machine, prog);
#ifdef TEST_TVM
    (void) argc;
    (void) argv;
    cpu_set_gpr(&machine->state, GPR_SP, 0x8010000);
#else
    machine__setup(machine, argc-1, argv+1);
#endif
}

void machine_fini(Machine *machine)
{
    (void) machine;
}
