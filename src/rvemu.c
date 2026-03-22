#include "machine.h"
#include "utils.h"

#include <assert.h>

int main(int argc, char **argv)
{
    if (argc != 2)
        fatalf("Usage: %s <program>", argv[0]);

    Machine machine = {0};
    machine_load_program(&machine, argv[1]);

    printf("entry address:   0x%016lx\n", TO_HOST(machine.mmu.entry));
    printf("machine address: 0x%016lx\n", (HostVAddr) &machine);

    while (1) {
        BreakCode brkcode = machine_step(&machine);
        assert(brkcode == ECALL);
    }

    return 0;
}
