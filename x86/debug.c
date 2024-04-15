#include "ansi_colors.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 99

static int actual_line_num = 1;

void get_next_good_line(FILE *fp, char *line) {
    while (fgets(line, MAX_LINE, fp)) {
        if (!isalpha(line[0])) {
            ++actual_line_num;
            continue;
        }
        if (line == strstr(line, "bits")) {
            ++actual_line_num;
            continue;
        }
        break;
    }
}

int compare_lines(char *line_ref, char *line_mine) {
    int ref_idx = 0;
    int my_idx = 0;

    for (;;) {
        // get next non-whitespace character
        while (line_ref[ref_idx] == ' ')
            ref_idx++;
        while (line_mine[my_idx] == ' ')
            my_idx++;

        if (line_ref[ref_idx] != line_mine[my_idx]) {
            printf("Mismatch '%c' != '%c'\n", line_ref[ref_idx], line_mine[my_idx]);
            return 1;
        }

        if (ref_idx > strlen(line_ref) - 1)
            break;

        ref_idx++;
        my_idx++;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char ref_line[MAX_LINE];
    char my_line[MAX_LINE];

    const char *curr_listing_filename = "listing_0041_add_sub_cmp_jnz.asm";
    FILE *ref_file = fopen(curr_listing_filename, "r");

    int curr_line_num = 0;

    for (int i = 0; i < 4; ++i) {
        get_next_good_line(ref_file, ref_line);
        get_next_good_line(stdin, my_line);
        curr_line_num++;
        int res = compare_lines(ref_line, my_line);
        if (res) {
            fprintf(stderr, RED_TXT "Failed on line %d\n" RST_TXT, actual_line_num);
            fprintf(stderr, "Expected:\n" MAG_TXT "%s" RST_TXT "\nGot:\n" MAG_TXT "%s" RST_TXT,
                    ref_line, my_line);
            break;
        }
        ++actual_line_num;
    }

    fclose(ref_file);
}
