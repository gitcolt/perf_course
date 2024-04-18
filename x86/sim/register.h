#ifndef REGISTER_H
#define REGISTER_H

#include "sim86_shared.h"

enum Register {
    REG_NONE,

    REG_A,
    REG_B,
    REG_C,
    REG_D,
    REG_SP,
    REG_BP,
    REG_SI,
    REG_DI,
    REG_ES,
    REG_CS,
    REG_SS,
    REG_DS,
    REG_IP,
    REG_FLAGS,

    REG_MAX,
};

const char *get_reg_name(register_access reg_access);

union RegState {
#define REG_16(r) \
    union { struct { u8 r##l; u8 r##h; }; u16 r##x; }

    struct {
        u16 zero;
        REG_16(a);
        REG_16(b);
        REG_16(c);
        REG_16(d);
        u16 sp;
        u16 bp;
        u16 si;
        u16 di;
        u16 es;
        u16 cs;
        u16 ss;
        u16 ds;
        u16 ip;
        u16 flags;
    };

    u8 u8[REG_MAX][2];
    u16 u16[REG_MAX];

#undef REG_16
};

void print_reg_state(union RegState *reg_state);

#endif
