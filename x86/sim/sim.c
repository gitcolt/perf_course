#include "sim.h"
#include "register.h"
#include "debug.h"
#include "flags.h"
#include "ansi_colors.h"

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

static u16 write_reg(register_access ra, u16 val, union RegState *reg_state) {
    if (ra.Count == 2) {
        return reg_state->u16[ra.Index] = val;
    } else {
        return reg_state->u8[ra.Index][ra.Offset] = val;
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

static u16 write_dst(instruction_operand dst, u16 val, union RegState *reg_state) {
    if (dst.Type == Operand_Register) {
#if DEBUG
        printf("%s <- %d (0x%x)\n",
               Sim86_RegisterNameFromOperand(&dst.Register), val, val);
#endif
        return write_reg(dst.Register, val, reg_state);
    } else {
        print_err("ERROR [write_dst]: Write to operand type not implemented\n");
    }

    return 0;
}

void simulate_inst(instruction inst, union RegState *reg_state) {
    instruction_operand l_op = inst.Operands[0];
    instruction_operand r_op = inst.Operands[1];
    if (inst.Op == Op_mov) {
        u16 val = read_src(r_op, reg_state);
        write_dst(l_op, val, reg_state);
    } else if (inst.Op == Op_sub) {
        u16 val = read_src(l_op, reg_state);
        u16 val2 = read_src(r_op, reg_state);
        u16 new_val = val - val2;
        write_dst(l_op, new_val, reg_state);
        flags_set(FLAG_PARITY, parity(new_val & 0xFF), &reg_state->flags);
        flags_set(FLAG_ZERO, (new_val == 0), &reg_state->flags);
        flags_set(FLAG_SIGN, (new_val & 0x8000) >> 15, &reg_state->flags);
    } else if (inst.Op == Op_add) {
        u16 val = read_src(l_op, reg_state);
        u16 val2 = read_src(r_op, reg_state);
        u16 new_val = val + val2;
        write_dst(l_op, new_val, reg_state);
        flags_set(FLAG_PARITY, parity(new_val & 0xFF), &reg_state->flags);
        flags_set(FLAG_ZERO, (new_val == 0), &reg_state->flags);
        flags_set(FLAG_SIGN, (new_val & 0x8000) >> 15, &reg_state->flags);
    } else if (inst.Op == Op_cmp) {
        u16 val = read_src(l_op, reg_state);
        u16 val2 = read_src(r_op, reg_state);
        u16 new_val = val - val2;
        flags_set(FLAG_PARITY, parity(new_val & 0xFF), &reg_state->flags);
        flags_set(FLAG_ZERO, (new_val == 0), &reg_state->flags);
        flags_set(FLAG_SIGN, (new_val & 0x8000) >> 15, &reg_state->flags);
    } else {
        print_err("ERROR [simulate_inst]: Operation not implemented\n");
    }
}
