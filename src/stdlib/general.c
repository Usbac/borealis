#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../engine/lexer.h"
#include "../engine/parser.h"
#include "../engine/parser_bytecode.h"
#include "../engine/preprocessor.h"
#include "../engine/processor_helper.h"
#include "../engine/processor.h"
#include "../engine/repl.h"
#include "../state.h"
#include "general.h"

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#define _sleep(x * 1000)
#else
#include <unistd.h>
#define _sleep(x) sleep(x)
#endif


void stdToString(struct result_list *args)
{
    pushResultStr(getValueStr(args->first));
}


void stdToNumber(struct result_list *args)
{
    int base = args->first->next != NULL ?
        (int) floor(getValueD(args->first->next)) :
        10;

    if (base == 10) {
        pushResultD(getValueD(args->first));
    } else {
        int64_t i = 1;
        int64_t result = 0;
        int64_t num = (int64_t) floor(getValueD(args->first));

        while (num != 0) {
            result += (num % base) * i;
            num /= base;
            i *= 10;
        }

        pushResultD((double) result);
    }
}


void stdToBool(struct result_list *args)
{
    pushResultD(isTrue(args->first));
}


void stdToObject(struct result_list *args)
{
    enum TYPE type = getResultType(args->first);
    struct element_table *obj;

    switch (type) {
        case T_Array: obj = copyElementTable(getValueArr(args->first)); break;
        case T_Object: obj = copyElementTable(getValueObj(args->first)); break;
        default: obj = newElementTable();
    }

    pushResultObj(obj);
}


void stdToArray(struct result_list *args)
{
    enum TYPE type = getResultType(args->first);
    struct element_table *arr;

    switch (type) {
        case T_Array: arr = copyElementTable(getValueArr(args->first)); break;
        case T_Object: arr = copyElementTable(getValueObj(args->first)); break;
        default: arr = newElementTable();
    }

    pushResultArr(arr);
}


void stdSleep(struct result_list *args)
{
    _sleep(getValueD(args->first));
}


void stdAssert(struct result_list *args)
{
    if (isTrue(args->first)) {
        return;
    }

    if (args->first->next != NULL) {
        char *str = getValueStr(args->first->next);
        printf("%s", str);
        free(str);
    }

    state->exiting = true;
    state->status_code = EXIT_FAILURE;
}


void stdExit(struct result_list *args)
{
    state->exiting = true;
    if (args->first != NULL) {
        state->status_code = (int) getValueD(args->first);
    }
}


void stdIsEmpty(struct result_list *args)
{
    struct element_table *table;
    enum TYPE type = getResultType(args->first);
    bool result = false;
    char *str;

    switch (type) {
        case T_String:
            str = getValueStr(args->first);
            result = strlen(str) == 0;
            free(str);
            break;
        case T_Number:
            result = getValueD(args->first) == 0;
            break;
        case T_Object:
        case T_Array:
            table = type == T_Array ?
                getValueArr(args->first) :
                getValueObj(args->first);
            result = true;

            for (struct element_list *i = table->first; i != NULL; i = i->next) {
                if (!i->ptr->unset) {
                    result = false;
                    break;
                }
            }

            break;
        default: result = type == T_Null;
    }

    pushResultD(result);
}


void stdDebug(struct result_list *args)
{
    const size_t SIDE_LINES = 5;
    FILE *fp;
    char line[256];
    size_t current_line = 0;
    int line_chars = 1;
    size_t line_aux = state->line_n + SIDE_LINES;
    bool cond = args->first != NULL ? isTrue(args->first) : true;

    if (!cond || state->file == NULL || (fp = fopen(state->file, "r")) == NULL) {
        return;
    }

    while (line_aux >= 10) {
        line_aux %= 10;
        line_chars++;
    }

    printf("\nFile: %s @ line %zu\n", state->file, state->line_n);
    while (fgets(line, sizeof(line), fp)) {
        current_line++;
        if ((current_line >= state->line_n && current_line - state->line_n <= SIDE_LINES) ||
            (state->line_n >= current_line && state->line_n - current_line <= SIDE_LINES)) {
            printf(current_line == state->line_n ?
                "=> %*zu: %s" :
                "   %*zu: %s",
                line_chars, current_line, line);
        }
    }

    printf("\n");
    fclose(fp);

    processRepl();
    state->exiting = false;
}


void stdEval(struct result_list *args)
{
    char *code = getValueStr(args->first);
    struct list *stmts = codeToList(code, DEFAULT_SEP, true, state->line_n);
    struct list *bytecode;

    preprocess(stmts, state->file);
    bytecode = listToBytecode(stmts);
    evalBytecode(bytecode);

    freeListRecursive(stmts);
    freeListRecursive(bytecode);
    free(code);
}


void stdTernary(struct result_list *args)
{
    struct result *val = isTrue(args->first) ? args->first->next : args->last;
    struct result *result = malloc_(sizeof(struct result));
    result->line_n = val->line_n;
    result->type = val->type;
    result->p_el = val->p_el;
    result->value = valueDup(val->value, val->type);

    pushResult(state->stack, result);
}


void stdTypeof(struct result_list *args)
{
    pushResultStr(strDup(getElementTypeAsStr(getResultType(args->first))));
}
