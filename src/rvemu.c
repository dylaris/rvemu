#include "machine.h"

int main(int argc, char **argv)
{
    if (argc < 2)
        fatalf("Usage: %s <program>", argv[0]);

    Machine machine = {0};
    machine_init(&machine, argv[1], argc, argv);

#if 1
    printf("entry address:   0x%016lx\n", machine.mem.entry);
    printf("machine address: 0x%016lx\n", (HostVAddr) &machine);
#endif

    machine_run(&machine);

    machine_fini(&machine);

    return 0;
}
