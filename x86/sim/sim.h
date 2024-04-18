#ifndef SIM_H
#define SIM_H

#include "register.h"

#include "sim86_shared.h"

void simulate_inst(instruction inst, union RegState *reg_state);

#endif
