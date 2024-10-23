#ifndef RUNNING_H
#define RUNNING_H

#include <stdint.h>

extern uint32_t pc;
extern int32_t registers[32];

void run_cpu();
inline void R_type(uint32_t instruction);
inline void I_type(uint32_t instruction);
inline void S_type(uint32_t instruction);
inline void B_type(uint32_t instruction);
inline void U_type(uint32_t instruction);
inline void J_type(uint32_t instruction);

#endif