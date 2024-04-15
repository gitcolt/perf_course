#include "ansi_colors.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

static const bool debug = false;

static char *prog_name;
static uint8_t buf;
static FILE *f;

enum Mnemonic {
    MOV,
    PUSH,
    POP,
    ADD,
    SUB,
    CMP,
    JNZ,
    JE,
    JL,
    JLE,
    JG,
    JGE,
    JB,
    JBE,
    JP,
    JO,
    JS,
    JNE,
    JNP,
    JNO,
    JNS,
    JNB,
    JA,
    JCXZ,
    LOOP,
    LOOPZ,
    LOOPNZ,
    MNEMONIC_MAX,
};

static const char *_mnem_str[MNEMONIC_MAX] = {
    "mov",
    "push",
    "pop",
    "add",
    "sub",
    "cmp",
    "jnz",
    "je",
    "jl",
    "jle",
    "jg",
    "jge",
    "jb",
    "jbe",
    "jp",
    "jo",
    "js",
    "jne",
    "jnp",
    "jno",
    "jns",
    "jnb",
    "ja",
    "jcxz",
    "loop",
    "loopz",
    "loopnz",
};
#define MNEM_STR(mnem) \
    ((mnem) < 0 || (mnem) >= MNEMONIC_MAX) \
        ? "INVALID MNEMONIC" \
        : _mnem_str[(mnem)]

enum Mode {
    MOD_MEM,
    MOD_MEM_8,
    MOD_MEM_16,
    MOD_REG,
    MOD_BAD = -1,
};

enum Reg {
    AL, AX,
    CL, CX,
    DL, DX,
    BL, BX,
    AH, SP,
    CH, BP,
    DH, SI,
    BH, DI,
    REG_MAX,
};

enum Reg reg_lkp[][2] = {
    { AL, AX },
    { CL, CX },
    { DL, DX },
    { BL, BX },
    { AH, SP },
    { CH, BP },
    { DH, SI },
    { BH, DI },
};

// extract bits from start to end (inclusive)
int get_field(uint8_t byte, int start, int end) {
    if (end > start)
        return -1;
    uint8_t mask = pow(2, start - end + 1) - 1;
    mask <<= end;
    return (mask & byte) >> end;
}

int get_bit(uint8_t byte, int which) {
    return get_field(byte, which, which);
}

static const char *_reg_str[REG_MAX] = {
    "al", "ax", "cl", "cx", "dl", "dx", "bl", "bx",
    "ah", "sp", "ch", "bp", "dh", "si", "bh", "di",
};
#define REG_STR(reg) \
    ((reg) < 0 || (reg) >= REG_MAX) \
        ? "INVALID REGISTER" \
        : _reg_str[(reg)]


void swap_strings(char **str1, char **str2) {
    char *tmp = *str1;
    *str1 = *str2;
    *str2 = tmp;
}

const char *effective_addr_strs[] = {
    "bx + si",
    "bx + di",
    "bp + si",
    "bp + di",
    "si",
    "di",
    "bp", // !!!
    "bx",
};

 unsigned long advance_byte(uint8_t *buffer) {
    return fread(buffer, 1, 1, f);
}

void get_mod_rm_fields(int8_t *mod_field,
                       uint8_t *rm_field,
                       uint8_t *buffer) {
    *mod_field = get_field(*buffer, 7, 6);
    *rm_field = get_field(*buffer, 2, 0);
}

void get_mod_zeros_rm_fields(int8_t *mod_field,
                             int8_t *zeros,
                             uint8_t *rm_field,
                             uint8_t *buffer) {
    *mod_field = get_field(*buffer, 7, 6);
    *zeros = get_field(*buffer, 5, 3);
    *rm_field = get_field(*buffer, 2, 0);
}

#define _DIRECT_ADDR_MODE(mod_field, rm_field) \
    ((mod_field) == 0b00 && (rm_field) == 0b110)

#define DIRECT_ADDR_MODE \
    _DIRECT_ADDR_MODE(mod_field, rm_field)

int16_t get_disp_val_and_advance(int8_t mod_field, uint8_t rm_field, uint8_t *buffer) {
    int16_t res;

    if (DIRECT_ADDR_MODE) {
        advance_byte(buffer);
        res = *buffer;
        advance_byte(buffer);
        res |= ((*buffer) << 8);
        return res;
    }

    switch (mod_field) {
        case MOD_MEM:
            res = 0;
            break;
        case MOD_MEM_8:
            advance_byte(buffer);
            res = *buffer;
            if (((int8_t)*buffer) < 0) // sign-extend
                res |= 0b1111111100000000;
            break;
        case MOD_MEM_16:
            advance_byte(buffer);
            res = *buffer;
            advance_byte(buffer);
            res |= ((*buffer) << 8);
            break;
        case MOD_BAD:
            fprintf(stderr, "Invalid mode value\n");
            break;
        default:
            fprintf(stderr, "Unknown mode value: %d\n", mod_field);
    }
    return res;
}

#define PRINT_INS(fmt_str, ...) \
    do { \
        if (debug) { \
            printf(GRN_TXT "d w mod zeros  rm disp\n" RST_TXT); \
            char zeros_str[64]; \
            if (zeros == -1) \
                strcpy(zeros_str, "XXX"); \
            else \
                sprintf(zeros_str, "%03b", zeros); \
            printf("%01b %01b  %02b   %s %03b %d\n", \
                   d_field == 1 ? 1 : 0, \
                   w_field, \
                   mod_field, \
                   zeros_str, \
                   rm_field, \
                   disp); \
        } \
        printf((fmt_str), __VA_ARGS__); \
    } while (0)

#define DECLARE_LOCALS() \
    uint8_t d_field; \
    uint8_t s_field; \
    uint8_t w_field; \
    int8_t mod_field; \
    int8_t zeros = -1; \
    uint8_t rm_field; \
    int16_t disp = 0; \
    int8_t reg_field = -1

void fetch_to_either(enum Mnemonic mnem, uint8_t *buffer) {
    if (debug)
        printf(GRN_TXT "fetch_to_either()\n" RST_TXT);
    DECLARE_LOCALS();

    d_field = get_bit(*buffer, 1);
    w_field = get_bit(*buffer, 0);

    advance_byte(buffer);

    get_mod_rm_fields(&mod_field, &rm_field, buffer);
    reg_field = get_field(*buffer, 5, 3);
    enum Reg reg1 = reg_lkp[reg_field][w_field];
    if (mod_field == MOD_REG) {
        enum Reg reg2 = reg_lkp[rm_field][w_field];
        char *dst = malloc(99);
        char *src = malloc(99);
        strcpy(dst, REG_STR(reg2));
        strcpy(src, REG_STR(reg1));
        if (d_field)
            swap_strings(&dst, &src);
        PRINT_INS("%s %s, %s", MNEM_STR(mnem), dst, src);

        free(dst);
        free(src);
    } else {
        disp = get_disp_val_and_advance(mod_field, rm_field, buffer);

        char *r_operand = malloc(99);
        strcpy(r_operand, REG_STR(reg1));

        char *l_operand = malloc(99);
        if (DIRECT_ADDR_MODE) {
            sprintf(l_operand, "[%d", disp);
        } else {
            sprintf(l_operand, "[%s", effective_addr_strs[rm_field]);
            if (disp) {
                char disp_str[99];
                sprintf(disp_str, " %s %d",
                        disp < 0 ? "-" : "+",
                        abs(disp));
                strcat(l_operand, disp_str);
            }
        }
        strcat(l_operand, "]");

        if (d_field)
            swap_strings(&l_operand, &r_operand);

        PRINT_INS("%s %s, %s",
                MNEM_STR(mnem),
                l_operand,
                r_operand);

        free(l_operand);
        free(r_operand);
    }
}

void imm_to_either(enum Mnemonic mnem, uint8_t *buffer) {
    if (debug)
        printf(GRN_TXT "imm_to_either()\n" RST_TXT);
    DECLARE_LOCALS();

    s_field = get_bit(*buffer, 1);
    w_field = get_bit(*buffer, 0);

    advance_byte(buffer);
    get_mod_zeros_rm_fields(&mod_field, &zeros, &rm_field, buffer);

    char l_operand[64];
    char r_operand[64];
    if (mod_field == MOD_REG) {
        enum Reg dst_reg = reg_lkp[rm_field][w_field];
        strcpy(l_operand, REG_STR(dst_reg));
    } else {
        disp = get_disp_val_and_advance(mod_field, rm_field, buffer);
        sprintf(l_operand, "[%s", effective_addr_strs[rm_field]);
        if (disp) {
            char disp_str[64];
            sprintf(disp_str, " %c %d",
                    disp < 0 ? '-' : '+',
                    disp < 0 ? disp * -1 : disp);
            strcat(l_operand, disp_str);
        }
        strcat(l_operand, "]");
    }

    advance_byte(buffer);

    int16_t imm_val = *buffer;
    // FIX?
    if (mnem == ADD /* || mnem == SUB || mnem == CMP */ ) {
        if (s_field && ((*buffer) & 0x80)) { // sign-extend
            imm_val |= 0xFF00;
        }
    }
    if (mnem == MOV) {
        if (w_field) {
            advance_byte(buffer);
            imm_val |= ((*buffer) << 8);
        }
    }

    if (mod_field == MOD_REG)
        sprintf(r_operand, "%d", imm_val);
    else {
        sprintf(r_operand, "%s %d",
                w_field ? "word" : "byte",
                imm_val);
    }
    PRINT_INS("%s %s, %s", MNEM_STR(mnem), l_operand, r_operand);
}

void imm_to_acc(enum Mnemonic mnem, uint8_t *buffer) {
    DECLARE_LOCALS();
    w_field = get_bit(*buffer, 0);
    advance_byte(buffer);
    int16_t imm_val = *buffer;
    if (w_field) {
        advance_byte(buffer);
        imm_val = ((int8_t)(*buffer) << 8) | imm_val;
    } else { // sign-extend 8-bit value
        if ((*buffer) & 0x80)
            imm_val |= 0xFF00;
    }
    PRINT_INS("%s %s, %d",
            MNEM_STR(mnem),
            w_field ? REG_STR(AX) : REG_STR(AL),
            imm_val);
}

void disass(uint8_t *buffer) {
    DECLARE_LOCALS();

    if (debug)
        printf(CYN_TXT "first byte: %08b\n" RST_TXT, *buffer);

    // ============== JMP ===================
    if ((*buffer) == 0b01110101) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JNZ), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01110100) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JE), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01111100) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JL), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01111110) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JLE), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01111111) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JG), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01111101) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JGE), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01110010) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JB), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01110110) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JBE), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01111010) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JP), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01110000) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JO), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01111000) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JS), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01110101) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JNE), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01111011) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JNP), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01110001) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JNO), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01111001) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JNS), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01110011) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JNB), (int8_t)(*buffer));
    } else if ((*buffer) == 0b01110111) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JA), (int8_t)(*buffer));
    } else if ((*buffer) == 0b11100011) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(JCXZ), (int8_t)(*buffer));
    } else if ((*buffer) == 0b11100010) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(LOOP), (int8_t)(*buffer));
    } else if ((*buffer) == 0b11100001) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(LOOPZ), (int8_t)(*buffer));
    } else if ((*buffer) == 0b11100000) {
        advance_byte(buffer); printf(MAG_TXT "%s %d" RST_TXT, MNEM_STR(LOOPNZ), (int8_t)(*buffer));
    }

    // ============== MOV ===================
    else if ((*buffer & 0b11111100) == 0b10001000) { // reg/mem to/from reg
        fetch_to_either(MOV, buffer);
    } else if ((*buffer & 0b11111110) == 0b11000110) { // imm to reg/mem
        imm_to_either(MOV, buffer);
    } else if ((*buffer & 0b11110000) == 0b10110000) { // imm to reg
        w_field = get_bit(*buffer, 3);
        reg_field = get_field(*buffer, 2, 0);
        enum Reg reg = reg_lkp[reg_field][w_field];
        advance_byte(buffer);
        if (w_field) {
            int16_t imm_val = *buffer;
            advance_byte(buffer);
            imm_val += ((*buffer) << 8);
            PRINT_INS("%s %s, %d", MNEM_STR(MOV), REG_STR(reg), imm_val);
        } else {
            int8_t imm_val = *buffer;
            PRINT_INS("%s %s, %d", MNEM_STR(MOV), REG_STR(reg), imm_val);
        }

    } else if ((*buffer & 0b11111110) == 0b10100000) { // mem to acc
        const char *l_operand = REG_STR(AX);
        w_field = get_bit(*buffer, 0);
        advance_byte(buffer);
        uint16_t addr = *buffer;
        if (w_field) {
            advance_byte(buffer);
            addr |= ((*buffer) << 8);
        }
        printf("%s %s, [%d]",
                MNEM_STR(MOV),
                l_operand,
                addr);
    } else if ((*buffer & 0b11111110) == 0b10100010) { // acc to mem
        const char *r_operand = REG_STR(AX);
        w_field = get_bit(*buffer, 0);
        advance_byte(buffer);
        uint16_t addr = *buffer;
        if (w_field) {
            advance_byte(buffer);
            addr |= ((*buffer) << 8);
        }
        PRINT_INS("%s [%d], %s",
                MNEM_STR(MOV),
                addr,
                r_operand);
    } else if ((*buffer & 0b11111111) == 0b10001110) { // reg/mem to seg reg
        printf("TODO: REG/MEM to SEG REG\n");
        exit(EXIT_FAILURE);
    } else if ((*buffer & 0b11111111) == 0b100011) { // seg reg to reg/mem
        printf("TODO: SEG REG to REG/MEM\n");
        exit(EXIT_FAILURE);
    }

    // ============== ADD ==============
    else if ((*buffer & 0b11111100) == 0b00000000) { // reg/mem with reg to either
        fetch_to_either(ADD, buffer);
    } else if ((*buffer & 0b11111110) == 0b00000100) { // imm to acc
        imm_to_acc(ADD, buffer);
    }

    // ============== SUB ==============
    else if ((*buffer & 0b11111100) == 0b00101000) { // reg/mem with reg to either
        fetch_to_either(SUB, buffer);
    } else if ((*buffer & 0b11111110) == 0b00101100) { // imm to acc
        imm_to_acc(SUB, buffer);
    }

    // ============== CMP ==============
    else if ((*buffer & 0b11111100) == 0b00111000) { // reg/mem with reg to either
        fetch_to_either(CMP, buffer);
    } else if ((*buffer & 0b11111110) == 0b00111100) { // imm to acc
        imm_to_acc(CMP, buffer);
    }

    // ======= common ADD/SUB/CMP ======
    else if ((*buffer & 0b11111100) == 0b10000000) { // imm to reg/mem
        /* Have to grab the current file stream pos, read the next byte,
         * and then reset the file index back to that original pos.
         * Should really read two bytes here in the main loop instead
         */
        fpos_t pos;
        fgetpos(f, &pos);
        advance_byte(buffer);
        if (((*buffer) & 0b00111000) == 0b00000000) {
            fsetpos(f, &pos);
            imm_to_either(ADD, buffer);
        } else if (((*buffer) & 0b00111000) == 0b00101000) {
            fsetpos(f, &pos);
            imm_to_either(SUB, buffer);
        } else if (((*buffer) & 0b00111000) == 0b00111000) {
            fsetpos(f, &pos);
            imm_to_either(CMP, buffer);
        }
    }
    // ???
    else {
        // fprintf(stderr, "buffer: %08b\n", *buffer);
        // fprintf(stderr, "%s", MNEM_STR(MNEMONIC_MAX));
    }

    printf("\n");
}

void print_usage() {
    printf("Usage: %s <file>\n", prog_name);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        prog_name = argv[0];
        print_usage();
        exit(EXIT_FAILURE);
    }

    // const char *filename = "listing_0037_single_register_mov";
    const char *filename = argv[1];
    if (!(f = fopen(filename, "r"))) {
        fprintf(stderr, "Couldn't open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    const size_t line_max = 100;
    char *line = malloc(line_max);
    while (advance_byte(&buf))
        disass(&buf);
}
