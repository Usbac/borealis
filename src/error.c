#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "state.h"
#include "error.h"


void errorF(size_t line_n, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("\n");
    vprintf(format, args);
    printf("\n");

    if (line_n != 0) {
        printf("Line: %ld\n", line_n);
    }

    if (state->file != NULL) {
        printf("File: %s\n", state->file);
    }

    va_end(args);
    if (state->jmp_buffer != NULL) {
        longjmp(*state->jmp_buffer, true);
    } else {
        exit(EXIT_FAILURE);
    }
}
