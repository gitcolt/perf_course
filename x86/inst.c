#include "inst.h"

#include <math.h>
#include <assert.h>

const char *registers[][2] = {
    { "al", "ax" },
    { "cl", "cx" },
    { "dl", "dx" },
    { "bl", "bx" },
    { "ah", "sp" },
    { "ch", "bp" },
    { "dh", "si" },
    { "bh", "di" },
};

void print_mnem(enum Mnem mnem) {
    switch (mnem) {
        case MNEM_NONE:
            printf("NONE\n");
            break;
        case MNEM_MOV:
            printf("MOV\n");
            break;
        case MNEM_ADD:
            printf("ADD\n");
            break;
        case MNEM_SUB:
            printf("SUB\n");
            break;
        case MNEM_CMP:
            printf("CMP\n");
            break;
    }
}

void print_bits_type(enum BitsType bits_type) {
    switch (bits_type) {
        case BITS_END:
            printf("BITS_END\n");
            break;
        case BITS_LITERAL:
            printf("BITS_LITERAL\n");
            break;
        case BITS_D:
            printf("BITS_D\n");
            break;
        case BITS_W:
            printf("BITS_W\n");
            break;
        case BITS_S:
            printf("BITS_S\n");
            break;
        case BITS_MOD:
            printf("BITS_MOD\n");
            break;
        case BITS_REG:
            printf("BITS_REG\n");
            break;
        case BITS_RM:
            printf("BITS_END\n");
            break;
        case BITS_SR:
            printf("BITS_SR\n");
            break;
        case BITS_DATA:
            printf("BITS_DATA\n");
            break;
        case BITS_DATA_IF_W1:
            printf("BITS_DATA_IF_W1\n");
            break;
        case BITS_ADDR_LO:
            printf("BITS_ADDR_LO\n");
            break;
        case BITS_ADDR_HI:
            printf("BITS_ADDR_HI\n");
            break;
    }
}

void parse_inst(Instruction *inst, EncodedInstruction encoded_inst, uint8_t *byte_start) {
    int bits_arr_offset = 0;
    int bits_offset = 0;
    inst->op = encoded_inst.mnem;
    uint8_t d;
    uint8_t w;
    uint8_t mod;
    uint8_t reg;
    uint8_t rm;
    uint16_t disp;
    int is_mem_mode = -1;
    Operand operands[2];
    int operand_idx = 0;

    for (;;) {
        Bits bits = encoded_inst.bits[bits_arr_offset];

        if (bits.type == BITS_END)
            break;

        int byte_offset = bits_offset % 8;
        int shift_amt = 8 - bits.size;
        int mask = pow(2, bits.size) - 1;

        uint8_t val = *(byte_start + byte_offset);
        val >>= shift_amt;
        val &= mask;

        if (bits.type == BITS_D)
            d = val;
        else if (bits.type == BITS_W)
            w = val;
        else if (bits.type == BITS_MOD) {
            mod = val;
            if (mod == 0b01)
                disp = 8;
            else if (mod == 0b10)
                disp = 16;
            else
                disp = 0;
        }
        else if (bits.type == BITS_REG) {
            reg = val;
            assert(operand_idx < 2);
            operands[operand_idx].type = OPERAND_REG;
            operands[operand_idx].reg.i = reg;
            operands[operand_idx].reg.j = reg;
            ++operand_idx;
        }
        else if (bits.type == BITS_RM) {
            rm = val;
            if (rm == 0b110 && mod == 0b00)
                disp = 16;
            else if (mod == 0b11) {
                assert(operand_idx < 2);
                operands[operand_idx].type = OPERAND_REG;
                operands[operand_idx].reg.i = rm;
                operands[operand_idx].reg.j = mod;
                ++operand_idx;
            }
        }

        bits_offset += bits.size;
        ++bits_arr_offset;
    }
}
