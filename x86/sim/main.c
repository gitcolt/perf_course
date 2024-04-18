#include "sim86_shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// source includes
#include "register.c"
#include "sim.c"

static u8 *mem;

void read_mem(u8 *mem, FILE *fp, size_t size) {
    fread(mem, 1, size, fp);
}

int check_version() {
    u32 version = Sim86_GetVersion();
    if (version != SIM86_VERSION) {
        fprintf(stderr, "ERROR: Header file version doesn't match library\n");
        return -1;
    }
    return 0;
}

int main() {
    if (check_version())
        return -1;

    const char *filename = "listing_0043_immediate_movs";
    FILE *f = fopen(filename, "r");
    struct stat bin_stat;
    fstat(fileno(f), &bin_stat);
    mem = malloc(bin_stat.st_size);
    read_mem(mem, f, bin_stat.st_size);

    union RegState reg_state = {};

    u32 offset = 0;
    while (offset < bin_stat.st_size) {
        instruction decoded;
        Sim86_Decode8086Instruction(sizeof(mem) - offset,
                                    mem + offset,
                                    &decoded);
        if (decoded.Op) {
            simulate_inst(decoded, &reg_state);
            offset += decoded.Size;
        } else {
            printf("Unrecognized instruction\n");
            break;
        }
        // break;
    }
    print_reg_state(&reg_state);

    free(mem);

    return 0;
}
