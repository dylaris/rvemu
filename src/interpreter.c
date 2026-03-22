#include "decoder.h"
#include "cpu.h"
#include "mmu.h"

#include <stdlib.h>

typedef void (Func)(CPU *, Inst *);

static Func *funcs[] = { };

void exec_block_interp(CPU *cpup)
{
    Inst inst = {0};
    while (1) {
        // Fetch
        u32 data = *(u32 *) TO_HOST(cpup->pc);

        // Decode
        inst_decode(&inst, data);

#if 0
        // Execute
        funcs[inst.kind](cpup, &inst);
        cpup->gp_regs[RI_ZERO] = 0; // Writes to x0 are ignored; x0 is hardwired to 0

        // Step
        if (inst.brk)
            break;
        else
            cpup->pc += inst.rvc ? 2 : 4;
#endif
    }
}
