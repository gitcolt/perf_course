#include "register.h"
#include "flags.h"

#include <stdio.h>

static const char *register_names[][3] = {
    { "", "", "" },
    { "al", "ah", "ax" },
    { "bl", "bh", "bx" },
    { "cl", "ch", "cx" },
    { "dl", "dh", "dx" },
    { "sp", "sp", "sp" },
    { "bp", "bp", "bp" },
    { "si", "si", "si" },
    { "di", "di", "di" },
    { "es", "es", "es" },
    { "cs", "cs", "cs" },
    { "ss", "ss", "ss" },
    { "ds", "ds", "ds" },
    { "ip", "ip", "ip" },
    { "flags", "flags", "flags" },
    { "", "", "" },
};

const char *get_reg_name(register_access reg_access) {
    return register_names[reg_access.Index][reg_access.Count == 2 ? 2 : reg_access.Offset&1];
}

void print_reg_state(union RegState *reg_state) {
    for (int i = 0; i < REG_MAX; ++i) {
        if (i == REG_NONE || i == REG_MAX)
            continue;

        const char *reg16_name =
            get_reg_name((register_access){ .Count = 2, .Index = i, .Offset = 0 });
        u16 val16 = reg_state->u16[i];

        if (i <= REG_D) {
            const char *reg8_name_lo =
                get_reg_name((register_access){ .Count = 1, .Index = i, .Offset = 0 });
            const char *reg8_name_hi =
                get_reg_name((register_access){ .Count = 1, .Index = i, .Offset = 1 });
            u8 val8_lo = reg_state->u8[i][0];
            u8 val8_hi = reg_state->u8[i][1];
            printf("%5s: %5d | %s: %3d, %s: %3d\n",
                    reg16_name, val16, reg8_name_hi, val8_hi, reg8_name_lo, val8_lo);
        } else
            printf("%5s: %5d |\n", reg16_name, val16);
    }
    printf("\n");
    flags_print(reg_state->flags);
}
