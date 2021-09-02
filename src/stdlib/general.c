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
    statePushResultStr(getValueStr(args->first));
}


void stdToNumber(struct result_list *args)
{
    int64_t base = args->first->next != NULL ?
        (int64_t) floor(getValueD(args->first->next)) :
        10;

    if (base == 10) {
        statePushResultD(getValueD(args->first));
    } else {
        int64_t i = 1;
        int64_t result = 0;
        int64_t num = (int64_t) floor(getValueD(args->first));

        while (num != 0) {
            result += (num % base) * i;
            num /= base;
            i *= 10;
        }

        statePushResultD((double) result);
    }
}


void stdToBool(struct result_list *args)
{
    statePushResultD(isTrue(args->first));
}


void stdToObject(struct result_list *args)
{
    enum TYPE type = getResultType(args->first);
    struct element_table *obj;

    switch (type) {
        case T_Array: obj = elementTableDup(getValueArr(args->first)); break;
        case T_Object: obj = elementTableDup(getValueObj(args->first)); break;
        default: obj = elementTableInit();
    }

    statePushResultObj(obj);
}


void stdToArray(struct result_list *args)
{
    enum TYPE type = getResultType(args->first);
    struct element_table *arr;

    switch (type) {
        case T_Array: arr = elementTableDup(getValueArr(args->first)); break;
        case T_Object: arr = elementTableDup(getValueObj(args->first)); break;
        default: arr = elementTableInit();
    }

    statePushResultArr(arr);
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
    state->code = EXIT_FAILURE;
}


void stdExit(struct result_list *args)
{
    state->exiting = true;
    if (args->first != NULL) {
        state->code = (int) getValueD(args->first);
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

    statePushResultD(result);
}


void stdDebug(struct result_list *args)
{
    const size_t SIDE_LINES = 5;
    FILE *fp;
    char line[UINT8_MAX];
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

    replProcess(true);
    state->exiting = false;
}


void stdEval(struct result_list *args)
{
    char *code = getValueStr(args->first);
    struct token_list *stmts = codeToList(code, SEPARATOR_DEFAULT, true, state->line_n);
    struct token_list *bytecode;

    preprocess(stmts, state->file);
    bytecode = bytecodeFromList(stmts);
    evalBytecode(bytecode);

    listFreeR(stmts);
    listFreeR(bytecode);
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

    statePushResult(state->stack, result);
}


void stdTypeof(struct result_list *args)
{
    statePushResultStr(strDup(getElementTypeAsStr(getResultType(args->first))));
}
