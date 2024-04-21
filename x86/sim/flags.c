#include "flags.h"

#include <stdio.h>
#include <assert.h>

int parity(u16 val) {
    int ones = 0;
    while (val) {
        if (val & 1)
            ++ones;
        val >>= 1;
    }
    return (ones % 2) == 0;
}

u8 flags_read(enum Flag flag, u16 flags_state) {
    return (flags_state >> flag);
}

void flags_print(u16 flags_state) {
    printf("FLAGS:\n");
    printf("\t S: %d\n", (flags_state >> FLAG_SIGN)      & 1);
    printf("\t Z: %d\n", (flags_state >> FLAG_ZERO)      & 1);
    printf("\tAC: %d\n", (flags_state >> FLAG_AUX_CARRY) & 1);
    printf("\t P: %d\n", (flags_state >> FLAG_PARITY)    & 1);
    printf("\tCY: %d\n", (flags_state >> FLAG_CARRY)     & 1);
}

void flags_set(enum Flag flag, u8 val, u16 *flags_state) {
    assert(val == 0 || val == 1);

    *flags_state = val
        ? *flags_state | (1 << flag)
        : *flags_state & ~(1 << flag);
}

const char *flag_to_str(enum Flag flag) {
    switch (flag) {
        case FLAG_CARRY:
            return "FLAG_CARRY";
        case FLAG_PARITY:
            return "FLAG_PARITY";
        case FLAG_AUX_CARRY:
            return "FLAG_AUX_CARRY";
        case FLAG_ZERO:
            return "FLAG_ZERO";
        case FLAG_SIGN:
            return "FLAG_SIGN";
        case FLAG_TRAP:
            return "FLAG_TRAP";
        case FLAG_INTERRUPT:
            return "FLAG_INTERRUPT";
        case FLAG_DIRECTION:
            return "FLAG_DIRECTION";
        case FLAG_OVERFLOW:
            return "FLAG_OVERFLOW";
    }
}
