#ifndef API_H
#define API_H

bool api_decode(u32 raw, Instr *out);
bool api_lookup(u32 raw, InstrInfo *out);

#endif // API_H
