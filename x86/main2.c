#include "inst.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>

static FILE *f;

size_t advance_byte(uint8_t *byte) {
    return fread(byte, 1, 1, f);
}

size_t peek_byte(uint8_t *byte) {
    fpos_t fpos;
    fgetpos(f, &fpos);
    int res = advance_byte(byte);
    fsetpos(f, &fpos);
    return res;
}

void print_byte(uint8_t byte) {
    printf("%08b\n", byte);
}

int match_ins(uint8_t *byte_start, EncodedInstruction ins) {
    int bits_arr_offset = 0;
    int bits_offset = 0; // actual offset in bits
    for (;;) {
        Bits bits = ins.bits[bits_offset];
        if (bits.type == BITS_END)
            break;
        if (bits.type == BITS_LITERAL) {
            int byte_offset = bits_offset % 8;
            int shift_amt = 8 - bits.size;
            uint8_t check_val = (*(byte_start + byte_offset)) >> shift_amt;
            uint8_t mask = pow(2, bits.size) - 1;
            if (bits.val != (check_val & mask)) {
                return 0;
            }
        }
        bits_offset += bits.size;
        ++bits_arr_offset;
    }
    return 1;
}

int main(int argc, char *argv[]) {
#define MAX_INS_LEN 5
    uint8_t b;
    // const char *filename = argv[1];
    const char *filename = "listing_0038_many_register_mov";
    f = fopen(filename, "r");

    while (advance_byte(&b)) {
        Instruction res = {};
        for(int i = 0; i < instruction_table.n_instructions; ++i) { 
            EncodedInstruction ins = instruction_table.instructions[i];
            if (!match_ins(&b, ins))
                continue;
            res.op = ins.mnem;
            parse_inst(&res, ins, &b);
        }
        if (res.op) {
            printf("Matched!\n");
            print_mnem(res.op);
        }

        else
            printf("No match\n");
        break;
    }

    fclose(f);
}
