#include "debug.h"
#include "ansi_colors.h"

#include <stdarg.h>
#include <stdio.h>

void print_err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[64];
    sprintf(buf, YEL_TXT "%s" RST_COL, fmt);
    vfprintf(stderr, buf, args);
    va_end(args);
}
