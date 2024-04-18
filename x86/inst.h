#ifndef INST_H
#define INST_H

#include <stdint.h>
#include <stdio.h>

enum Mnem {
    MNEM_NONE,

    MNEM_MOV,
    MNEM_ADD,
    MNEM_SUB,
    MNEM_CMP,
};

enum OperandType {
    OPERAND_NONE,
    OPERAND_IMM,
    OPERAND_ADDR,
    OPERAND_REG,
};

typedef struct RegisterAccess { int i; int j; } RegisterAccess;

typedef struct Operand {
    enum OperandType type;
    union {
        RegisterAccess reg;
        int16_t immediate_val;
    };
} Operand;

typedef struct Instruction {
    enum Mnem op;
    Operand operands[2];
} Instruction;

void print_mnem(enum Mnem mnem);

enum BitsType {
    BITS_END,

    BITS_LITERAL,
    BITS_D,
    BITS_W,
    BITS_S,
    BITS_MOD,
    BITS_REG,
    BITS_RM,
    BITS_SR,
    BITS_DATA,
    BITS_DATA_IF_W1,
    BITS_ADDR_LO,
    BITS_ADDR_HI,
};

void print_bits_type(enum BitsType bits_type);

#define BITS(bits) { BITS_LITERAL, sizeof(#bits) - 1, 0b##bits }

typedef struct Bits {
    enum BitsType type;
    uint8_t size;
    uint8_t val;
} Bits;

typedef struct EncodedInstruction {
    enum Mnem mnem;
    Bits bits[16];
} EncodedInstruction;

typedef struct InstructionTable {
    EncodedInstruction *instructions;
    int n_instructions;
} InstructionTable;

#define D { BITS_D, 1 }
#define W { BITS_W, 1 }
#define S { BITS_S, 1 }
#define MOD { BITS_MOD, 2 }
#define REG { BITS_REG, 3 }
#define RM { BITS_RM, 3 }
#define SR { BITS_SR, 2 }
#define DATA { BITS_DATA, 8 }
#define DATA_IF_W1 { BITS_DATA_IF_W1, 8 }
#define ADDR_LO { BITS_ADDR_LO, 8 }
#define ADDR_HI { BITS_ADDR_HI, 8 }

static EncodedInstruction encoded_instructions[] = {
    { MNEM_MOV, { BITS(100010), D, W, MOD, REG, RM } },
    { MNEM_MOV, { BITS(1100011), W, MOD, BITS(000), RM } },
    { MNEM_MOV, { BITS(1011), W, REG, DATA, DATA_IF_W1 } },
    { MNEM_MOV, { BITS(1010000), ADDR_LO, ADDR_HI } },
    { MNEM_MOV, { BITS(1010001), ADDR_LO, ADDR_HI } },
    { MNEM_MOV, { BITS(10001110), MOD, BITS(0), SR, RM } },
    { MNEM_MOV, { BITS(10001100), MOD, BITS(0), SR, RM } },
};

#define ARRLEN(arr) (sizeof(arr) / sizeof(*(arr)))

static InstructionTable instruction_table = {
    .instructions = encoded_instructions,
    .n_instructions = ARRLEN(encoded_instructions),
};

void parse_inst(Instruction *inst, EncodedInstruction encoded_inst, uint8_t *byte_start);

#undef D

#endif
