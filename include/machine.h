#ifndef MACHINE_H
#define MACHINE_H

#include "cpu.h"
#include "memory.h"

typedef struct {
    CPUState state;
    Memory mem;
} Machine;

void machine_step(Machine *machine);
bool machine_run(Machine *machine);
void machine_init(Machine *machine, const char *prog, int argc, char **argv);
void machine_fini(Machine *machine);

#endif // MACHINE_H
