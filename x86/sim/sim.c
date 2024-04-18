#include "sim.h"
#include "register.h"
#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG 1

static u16 read_reg(register_access ra, union RegState *reg_state) {
    if (ra.Count == 2) { // 16-bit access
        return reg_state->u16[ra.Index];
    } else { // 8-bit access
        return reg_state->u8[ra.Index][ra.Offset];
    }
}

static void write_reg(register_access ra, u16 val, union RegState *reg_state) {
    if (ra.Count == 2) {
        reg_state->u16[ra.Index] = val;
    } else {
        reg_state->u8[ra.Index][ra.Offset] = val;
    }
}

static u16 read_src(instruction_operand src, union RegState *reg_state) {
    if (src.Type == Operand_Immediate) {
        return src.Immediate.Value;
    } else if (src.Type == Operand_Register) {
        return read_reg(src.Register, reg_state);
    } else {
        print_err("ERROR [read_src]: Read from operand type not implemented\n");
    }

    return 0;
}

static void write_dst(instruction_operand dst, u16 val, union RegState *reg_state) {
    if (dst.Type == Operand_Register) {
#if DEBUG
        printf("%s <- %d\n",
               Sim86_RegisterNameFromOperand(&dst.Register), val);
#endif
        write_reg(dst.Register, val, reg_state);
    } else {
        print_err("ERROR [write_dst]: Write to operand type not implemented\n");
    }
}

void simulate_inst(instruction inst, union RegState *reg_state) {
    instruction_operand l_op = inst.Operands[0];
    instruction_operand r_op = inst.Operands[1];
    if (inst.Op == Op_mov) {
        u16 val = read_src(r_op, reg_state);
        write_dst(l_op, val, reg_state);
    } else {
        print_err("ERROR [simulate_inst]: Operation not implemented");
    }
}
