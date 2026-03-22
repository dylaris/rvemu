#ifndef API_H
#define API_H

#include <stdbool.h>

#include "cpu.h"
#include "decoder.h"
#include "elf64.h"
#include "interpreter.h"
#include "machine.h"
#include "mmu.h"
#include "utils.h"

bool api_decode(u32 data, Inst *outp);
bool api_lookup(u32 data, InstDef *outp);

#endif // API_H
