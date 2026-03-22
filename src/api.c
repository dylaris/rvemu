#include "api.h"

#include "decoder.c"

bool api_decode(u32 data, Inst *outp)
{
    inst_decode(outp, data);
    return true;
}

bool api_lookup(u32 data, InstDef *outp)
{
    *outp = inst_lookup(data);
    return true;
}
