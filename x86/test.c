#include "ansi_colors.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

#define PROG_NAME "./main"
#define LISTINGS_DIR "."

char asm_filename[99] = "\0";
char bytecode_filename[99] = "\0";

void print_usage() {
    printf("Usage: test <LISTING #>\n");
}

void get_listing_filenames(const char *listing_substring) {
    DIR *dp = opendir(LISTINGS_DIR);
    struct dirent *dirent;

    int files_found = 0;
    if (dp) {
        while ((dirent = readdir(dp))) {
            if (strstr(dirent->d_name, listing_substring)) {
                if (!strstr(dirent->d_name, ".asm"))
                    strcpy(bytecode_filename, dirent->d_name);
                else
                    strcpy(asm_filename, dirent->d_name);
                files_found++;
                if (files_found == 2)
                    break;
            }
        }
        closedir(dp);
    }

    if (files_found < 2) {
        if (strlen(asm_filename) == 0) {
            fprintf(stderr, "Could not find assembly listing %s\n", listing_substring);
            exit(EXIT_FAILURE);
        }
        if (strlen(bytecode_filename) == 0) {
            fprintf(stderr, "Could not find bytecode listing %s\n", listing_substring);
            exit(EXIT_FAILURE);
        }
    }
}

bool run_test(const char *listing_substr) {
    get_listing_filenames(listing_substr);

    system("mkdir -p ./tmp");

    char cmd_buf[999];

    strcpy(cmd_buf, PROG_NAME " ");
    strcat(cmd_buf, bytecode_filename);
    strcat(cmd_buf, " ");
    strcat(cmd_buf, "> ./tmp/mine.asm");

    // printf("command: %s\n", cmd_buf);
    system(cmd_buf);

    strcpy(cmd_buf, "nasm -o ./tmp/mine ./tmp/mine.asm");
    system(cmd_buf);

    strcpy(cmd_buf, "diff ");
    strcat(cmd_buf, bytecode_filename);
    strcat(cmd_buf, " ./tmp/mine");
    int res = system(cmd_buf);

    system("rm -rf ./tmp");

    printf("LISTING %s RESULT:\n", listing_substr);
    return res == 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        const char *listing_substring = argv[i];
        bool res = run_test(listing_substring);
        if (res)
            printf(GRN_TXT "PASS\n" RST_TXT);
        else
            fprintf(stderr, RED_TXT "FAIL\n" RST_TXT);
        printf("\n");
    }

    return 0;
}
