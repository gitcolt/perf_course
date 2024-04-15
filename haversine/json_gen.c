#include "json_gen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

typedef struct JGContext {
    char *str;
    int indent;
    size_t cap;
    size_t size;
} JGContext;

static JGContext ctx;

void increase_indent() {
    ctx.indent += 2;
}

void decrease_indent() {
    ctx.indent -= 2;
}

void increase_cap_if_needed() {
    if (ctx.cap - ctx.size < 100) {
        ctx.cap += 999;
        ctx.str = realloc(ctx.str, ctx.cap);
    }
}

void indent() {
    increase_cap_if_needed();
    for (int i = 0; i < ctx.indent; ++i)
        strcat(ctx.str, " ");
}

void jg_start() {
    size_t size = 999;
    ctx.str = malloc(size);
    ctx.indent = 0;
    ctx.cap = size;
    ctx.size = 0;

    jg_obj_start(NULL);
}

void jg_end() {
    increase_cap_if_needed();
    int i = strlen(ctx.str);
    ctx.str[i - 2] = ' ';

    decrease_indent();

    indent();

    strcat(ctx.str, "}\n");
}

void jg_destroy() {
    free(ctx.str);
}

void jg_obj_start(char *key) {
    increase_cap_if_needed();
    indent();
    if (key) {
        char buf[99];
        sprintf(buf, "\"%s\": {\n", key);
        strcat(ctx.str, buf);
    }
    else
        strcat(ctx.str, "{\n");

    increase_indent();
}

void jg_obj_end() {
    increase_cap_if_needed();
    int i = strlen(ctx.str);
    ctx.str[i - 2] = ' ';

    decrease_indent();

    indent();

    strcat(ctx.str, "},\n");
}

void jg_obj_key_val_int(char *key, int val) {
    increase_cap_if_needed();
    char buf[99];

    indent();

    sprintf(buf, "\"%s\": %d,\n", key, val);
    strcat(ctx.str, buf);
}

void jg_obj_key_val_float(char *key, float val) {
    increase_cap_if_needed();
    char buf[99];

    indent();

    sprintf(buf, "\"%s\": %f,\n", key, val);
    strcat(ctx.str, buf);
}

void jg_obj_key_val_str(char *key, char *val) {
    increase_cap_if_needed();
    indent();

    char buf[99];
    sprintf(buf, "\"%s\": \"%s\",\n", key, val);
    strcat(ctx.str, buf);
}

void jg_print() {
    printf("%s\n", ctx.str);
}

void jg_write_to_file(const char *filename) {
    FILE *f;
    if ((f = fopen(filename, "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    // fwrite(ctx.str, 1, strlen(ctx.str), f);
    fwrite(ctx.str, 1, ctx.size, f);
}

void jg_int(char *key, int val) {
    increase_cap_if_needed();
    indent();

    char buf[99];
    sprintf(buf, "\"%s\": \"%d\",\n", key, val);
    strcat(ctx.str, buf);
}

void jg_str(char *key, char *val) {
    increase_cap_if_needed();
    indent();

    char buf[99];
    sprintf(buf, "\"%s\": \"%s\",\n", key, val);
    strcat(ctx.str, buf);
}

void jg_arr_start(char *key) {
    increase_cap_if_needed();
    indent();

    char buf[99];
    sprintf(buf, "\"%s\": [\n", key);
    strcat(ctx.str, buf);

    increase_indent();
}

void jg_arr_end() {
    increase_cap_if_needed();
    int i = strlen(ctx.str);
    ctx.str[i - 2] = ' ';

    decrease_indent();
    indent();
    strcat(ctx.str, "],\n");
}

void jg_arr_item_int(int val) {
    increase_cap_if_needed();
    indent();

    char buf[99];
    sprintf(buf, "%d,\n", val);
    strcat(ctx.str, buf);
}

void jg_arr_item_float(float val) {
    increase_cap_if_needed();
    indent();

    char buf[99];
    sprintf(buf, "%f,\n", val);
    strcat(ctx.str, buf);
}

void jg_arr_item_str(char *val) {
    increase_cap_if_needed();
    indent();

    char buf[99];
    sprintf(buf, "%s,\n", val);
    strcat(ctx.str, buf);
}
