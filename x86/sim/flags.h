#ifndef FLAGS_H
#define FLAGS_H

#include "register.h"

#include "sim86_shared.h"

enum Flag {
    // value is the shift amount to get the single bit value
    FLAG_OVERFLOW = 11,
    FLAG_DIRECTION = 10,
    FLAG_INTERRUPT = 9,
    FLAG_TRAP = 8,
    FLAG_SIGN = 7,
    FLAG_ZERO = 6,
    FLAG_AUX_CARRY = 4,
    FLAG_PARITY = 2,
    FLAG_CARRY = 0,
};

u8 flags_read(enum Flag flag, u16 flags_state);

void flags_print(u16 flags_state);

void flags_set(enum Flag flag, u8 val, u16 *flags_state);

const char *flag_to_str(enum Flag flag);

int parity(u16 val);

#endif
