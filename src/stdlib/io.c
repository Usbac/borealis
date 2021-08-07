#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../engine/processor_helper.h"
#include "../state.h"
#include "../../lib/linenoise.h"
#include "io.h"


static void execPrint(struct result_list *list)
{
    for (struct result *node = list->first; node != NULL;) {
        char *str = getValueStr(node);
        node = node->next;
        printf(node != NULL ? "%s " : "%s", str != NULL ? str : TYPEOF_NULL);
        free(str);
    }
}


static char *getElementTableStr(struct element_table *a)
{
    char *str = strInit();
    bool added = false;

    for (struct element_list *i = a->first; i != NULL; i = i->next) {
        struct element *a_el = getTrueElement(i->ptr);
        char *val;

        if (added) {
            strAppendC(&str, ',');
        }

        added = true;
        strAppend(&str, i->ptr->key);
        strAppendC(&str, ':');

        val = getResultStr(a_el->type, a_el->value);
        strAppend(&str, val);
        free(val);
    }

    return str;
}


char *getResultStr(enum TYPE type, union VALUE val)
{
    char *table;
    char *str_type = getElementTypeAsStr(type);
    char *str_val;
    char *str = strInit();

    switch (type) {
        case T_Number: str_val = strFromD(val.number); break;
        case T_String:
            str_val = strInit();
            strAppendC(&str_val, '"');
            strAppend(&str_val, val.string);
            strAppendC(&str_val, '"');
            break;
        case T_Function: str_val = strDup(TYPEOF_FUNCTION); break;
        case T_Array:
        case T_Object:
            table = getElementTableStr(val.values);
            str_val = strInit();
            strAppendC(&str_val, type == T_Array ? '[' : '{');
            strAppend(&str_val, table);
            strAppendC(&str_val, type == T_Array ? ']' : '}');
            free(table);
            break;
        default: str_val = strDup(TYPEOF_NULL);
    }

    strAppendC(&str, '(');
    strAppend(&str, str_type);
    strAppend(&str, ") ");
    strAppend(&str, str_val);

    free(str_val);

    return str;
}


void stdPrint(struct result_list *args)
{
    execPrint(args);
}


void stdPrintLine(struct result_list *args)
{
    execPrint(args);
    printf("\n");
}


void stdPrintV(struct result_list *args)
{
    enum TYPE type;
    union VALUE val;
    char *str;

    mapTypeVal(args->first, &type, &val);

    str = getResultStr(type, val);
    printf("%s", str);
    free(str);
}


void stdReadLine(struct result_list *args)
{
    char *prompt = args->first != NULL ? getValueStr(args->first) : NULL;
    char *result;

    if (args->first != NULL && args->first->next != NULL &&
        isTrue(args->first->next)) {
        linenoiseMaskModeEnable();
    }

    result = linenoise(prompt != NULL ? prompt : "");
    linenoiseMaskModeDisable();

    if (result == NULL) {
        statePushResultNull();
    } else {
        statePushResultStr(result);
    }

    free(prompt);
}


void stdReadChar(struct result_list *args)
{
    char *result = calloc_(1, 2);
    result[0] = (char) getchar();

    statePushResultStr(result);
}


void stdFlush(struct result_list *args)
{
    fflush(stdout);
}


void stdClear(struct result_list *args)
{
    linenoiseClearScreen();
}


void stdHistoryAdd(struct result_list *args)
{
    linenoiseHistoryAdd(getValueStr(args->first));
}


void stdHistorySize(struct result_list *args)
{
    linenoiseHistorySetMaxLen((int) getValueD(args->first));
}
