#include "ansi_colors.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>

#define DEBUG 1

static char *prog_name;
static uint8_t buf;
static FILE *f;

void print_usage() {
    printf("Usage: %s <file>\n", prog_name);
}

int main(int argc, char *argv[]) {
#if DEBUG
    const char *filename = "listing_0040_challenge_movs";
#else
    if (argc != 2) {
        prog_name = argv[0];
        print_usage();
        exit(EXIT_FAILURE);
    }
    const char *filename = argv[1];
#endif
    if (!(f = fopen(filename, "r"))) {
        fprintf(stderr, "Couldn't open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    const size_t line_max = 100;
    char *line = malloc(line_max);
    while (fread(&buf, 1, 1, f))
        printf("%08b\n", buf);
}
