#include "sim.h"
#include "register.h"
#include "ansi_colors.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void simulate_inst(instruction inst, union RegState *reg_state) {
    if (inst.Op != Op_mov) {
        printf("!!! Not a MOV\n");
        return;
    }

    instruction_operand l_op = inst.Operands[0];
    instruction_operand r_op = inst.Operands[1];
    if (l_op.Type == Operand_Register && r_op.Type == Operand_Immediate) {
        register_access ra = l_op.Register;
        u16 *reg = &reg_state->u16[ra.Index];
        *reg = r_op.Immediate.Value;
    } else {
        printf("TODO...\n");
    }
}
