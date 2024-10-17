#ifndef RUNNING_H
#define RUNNING_H

#include <stdint.h>

extern uint32_t pc;
extern int32_t registers[32];

void run_cpu();

#endif