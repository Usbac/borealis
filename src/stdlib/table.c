#include <stdlib.h>
#include <string.h>
#include "../state.h"
#include "../engine/processor_helper.h"
#include "../engine/processor.h"
#include "table.h"

struct function *func_compare = NULL;


static void removeAndReturnTableEl(struct element_list *el)
{
    if (el == NULL || el->ptr->unset) {
        statePushResultNull();
    } else {
        statePushResult(state->stack, getResultFromElement(el->ptr));
        elementFree(&el->ptr);
    }
}


static double getDouble(struct element *el)
{
    struct element *ref = getTrueElement(el);
    RETURN_DOUBLE_FROM_VALUE(ref->type, ref->value);
}


void stdGet(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    char *key = getValueStr(args->first->next);
    struct element *el = elementGet(key, table, NULL);

    statePushResult(state->stack, getResultFromElement(el));
    free(key);
}


void stdGetSize(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    double size = 0;

    if (table == NULL) {
        goto end;
    }

    for (struct element_list *i = table->first; i != NULL; i = i->next) {
        size += !i->ptr->unset;
    }

    end: statePushResultD(size);
}


void stdPush(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    char *key = strFromSizet(table->next_index);
    struct element *prop = elementInit(key, NULL, 0, T_Null);
    mapResultToElement(prop, args->first->next);
    elementTablePush(&table, prop);

    free(key);
}


void stdPrepend(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    char *key = strFromSizet(table->next_index);
    struct element *prop = elementInit(key, NULL, 0, T_Null);
    mapResultToElement(prop, args->first->next);
    elementTablePrepend(&table, prop);

    free(key);
}


void stdPop(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    removeAndReturnTableEl(table->last);
}


void stdShift(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    removeAndReturnTableEl(table->first);
}


void stdGetKeys(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    struct element_table *keys = elementTableInit();
    size_t index = 0;

    for (struct element_list *i = table != NULL ? table->first : NULL;
        i != NULL;
        i = i->next) {
        if (!i->ptr->unset) {
            char *key = strFromSizet(index++);
            struct element *prop = elementInit(key, NULL, 0, T_String);
            prop->value.string = strDup(i->ptr->key);
            elementTablePush(&keys, prop);
            free(key);
        }
    }

    statePushResultTable(keys);
}


void stdHasKey(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    char *key = getValueStr(args->first->next);
    struct element *el = elementGet(key, table, NULL);

    statePushResultD(el != NULL && !el->unset);
    free(key);
}


void stdJoin(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    char *glue = args->first->next != NULL ?
        getValueStr(args->first->next) :
        strDup("");
    char *result = strInit();

    for (struct element_list *ite = table != NULL ? table->first : NULL;
        ite != NULL;
        ite = ite->next) {
        struct element *el = getTrueElement(ite->ptr);
        char *str;

        if (el->unset) {
            continue;
        }

        str = getUnionStr(el->type, el->value);
        strAppend(&result, str);
        free(str);
        if (ite->next != NULL && !ite->next->ptr->unset) {
            strAppend(&result, glue);
        }
    }

    statePushResultStr(result);
    free(glue);
}


void stdHas(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    struct result *val = args->first->next;
    bool has = false;

    for (struct element_list *ite = table != NULL ? table->first : NULL;
        ite != NULL;
        ite = ite->next) {
        struct element *el = getTrueElement(ite->ptr);

        if (!el->unset) {
            struct result *comp = getResultFromElement(el);
            has = compareResults(val, comp);
            resultFree(comp);
            if (has) {
                break;
            }
        }
    }

    statePushResultD(has);
}


void stdReverse(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    struct element_list *temp;

    if (table == NULL) {
        return;
    }

    for (struct element_list *ite = table->first; ite != NULL;) {
        temp = ite->next;
        ite->next = ite->prev;
        ite->prev = temp;
        ite = temp;
    }

    temp = table->last;
    table->last = table->first;
    table->first = temp;
}


void stdRange(struct result_list *args)
{
    struct element_table *table = elementTableInit();
    int start = (int) getValueD(args->first);
    int end = (int) getValueD(args->first->next);
    int steps = args->first->next->next != NULL ?
        (int) getValueD(args->first->next->next) : 1;

    if (start > end) {
        steps = -steps;
        end--;
    } else {
        end++;
    }

    for (int i = start, key = 0;
        start > end ? i > end : i < end;
        i+= steps, key++) {
        char *key_str = strFromInt(key);
        struct element *prop = elementInit(key_str, NULL, 0, T_Number);
        prop->value.number = (double) i;
        elementTablePush(&table, prop);

        free(key_str);
    }

    statePushResultTable(table);
}


static int sortCompare(const void *a, const void *b)
{
    double a_num = getDouble((*(struct element_list **) a)->ptr);
    double b_num = getDouble((*(struct element_list **) b)->ptr);

    if (a_num > b_num) {
        return 1;
    } else if (a_num < b_num) {
        return -1;
    }

    return 0;
}


static int customSortCompare(const void *a, const void *b)
{
    struct result_list *args = resultListInit();
    struct result *func_result;
    int result;

    statePushResult(args, getResultFromElement((*(struct element_list **) a)->ptr));
    statePushResult(args, getResultFromElement((*(struct element_list **) b)->ptr));

    funcExec(func_compare, args, state->current_table);
    func_result = statePopResult();
    result = (int) getValueD(func_result);

    resultFree(func_result);
    resultListFree(args);

    return result;
}


void stdSort(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    int i, len = 0;
    struct element_list *el = table->first;
    struct element_list **values;
    while (el != NULL) {
        el = el->next;
        len++;
    }

    if (table->first == NULL || table->first->next == NULL) {
        return;
    }

    values = malloc_(len * sizeof(struct element_list *));
    i = 0;
    el = table->first;
    while (el != NULL) {
        values[i++] = el;
        el = el->next;
    }

    if (args->first->next != NULL) {
        func_compare = getValueFunc(args->first->next);
        func_compare->params_n = 2;
        qsort(values, len, sizeof(struct element_list *), customSortCompare);
    } else {
        qsort(values, len, sizeof(struct element_list *), sortCompare);
    }

    for (i = 0; i < len; i++) {
        values[i]->prev = i == 0 ? NULL : values[i - 1];
        values[i]->next = i == len - 1 ? NULL : values[i + 1];
    }

    table->first = values[0];
    table->last = values[len - 1];
    free(values);
}


void stdColumn(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    char *column_name = getValueStr(args->first->next);
    struct element_table *result = elementTableInit();

    for (struct element_list *ite = table != NULL ? table->first : NULL;
        ite != NULL;
        ite = ite->next) {
        struct element *src = getTrueElement(ite->ptr);
        struct element *column;

        if (src->unset || src->type != T_Table) {
            continue;
        }

        column = elementGet(column_name, src->value.values, NULL);

        if (column != NULL) {
            struct element *el = elementInit(NULL, NULL, 0, T_Null);
            el->key = strFromSizet(result->next_index);
            elementDupValues(&el, column);
            elementTablePush(&result, el);
        }
    }

    statePushResultTable(result);
    free(column_name);
}


static void mapIteratorArg(struct function *func,
                           struct result_list *func_args,
                           size_t *i,
                           size_t arg_index)
{
    struct result *index;

    if (func->params_n < arg_index) {
        return;
    }

    index = malloc_(sizeof(struct result));
    *index = (struct result) {
        .value.number = (double) (*i)++,
        .type = T_Number,
        .line_n = state->line_n,
    };

    statePushResult(func_args, index);
}


void stdMap(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    struct element_table *result = elementTableInit();
    struct function *func = getValueFunc(args->first->next);
    size_t index = 0;

    for (struct element_list *i = table->first; i != NULL; i = i->next) {
        struct result_list *func_args;
        struct element *el;
        struct result *func_result;

        if (i->ptr->unset) {
            continue;
        }

        func_args = resultListInit();
        el = elementInit(i->ptr->key, NULL, 0, T_Null);

        statePushResult(func_args, getResultFromElement(i->ptr));
        mapIteratorArg(func, func_args, &index, 2);

        funcExec(func, func_args, state->current_table);
        func_result = statePopResult();
        mapResultToElement(el, func_result);
        elementTablePush(&result, el);

        resultFree(func_result);
        resultListFree(func_args);
    }

    statePushResultTable(result);
}


void stdFilter(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    struct element_table *result = elementTableInit();
    struct function *func = getValueFunc(args->first->next);
    size_t index = 0;

    for (struct element_list *i = table->first; i != NULL; i = i->next) {
        struct result_list *func_args;
        struct result *func_result;

        if (i->ptr->unset) {
            continue;
        }

        func_args = resultListInit();
        statePushResult(func_args, getResultFromElement(i->ptr));
        mapIteratorArg(func, func_args, &index, 2);

        funcExec(func, func_args, state->current_table);
        func_result = statePopResult();

        if (isTrue(func_result)) {
            struct element *el = elementInit(i->ptr->key, NULL, 0, T_Null);
            elementDupValues(&el, i->ptr);
            elementTablePush(&result, el);
        }

        resultFree(func_result);
        resultListFree(func_args);
    }

    statePushResultTable(result);
}


void stdReduce(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    struct function *func = getValueFunc(args->first->next);
    struct result *accumulator = malloc_(sizeof(struct result));
    struct result *accum_arg = args->first->next->next;
    size_t index = 0;

    if (accum_arg != NULL) {
        *accumulator = (struct result) {
            .type = accum_arg->type,
            .line_n = accum_arg->line_n,
            .p_el = accum_arg->p_el,
            .value = valueDup(accum_arg->value, accum_arg->type),
        };
    } else {
        *accumulator = (struct result) {
            .type = T_Null,
            .value.string = NULL,
        };
    }

    for (struct element_list *i = table->first; i != NULL; i = i->next) {
        struct result_list *func_args;

        if (i->ptr->unset) {
            continue;
        }

        func_args = resultListInit();
        statePushResult(func_args, accumulator);
        statePushResult(func_args, getResultFromElement(i->ptr));
        mapIteratorArg(func, func_args, &index, 3);

        funcExec(func, func_args, state->current_table);
        accumulator = statePopResult();

        resultListFree(func_args);
    }

    statePushResult(state->stack, accumulator);
}


void stdMerge(struct result_list *args)
{
    struct element_table *result = elementTableInit();

    for (struct result *i = args->first; i != NULL; i = i->next) {
        struct element_table *table = getValueTable(i);
        if (table == NULL) {
            continue;
        }

        for (struct element_list *index = table->first; index != NULL; index = index->next) {
            struct element *el;
            if (index->ptr->unset) {
                continue;
            }

            if ((el = elementGet(index->ptr->key, result, NULL)) != NULL) {
                elementFreeValues(&el);
                elementDupValues(&el, index->ptr);
            } else {
                el = elementInit(index->ptr->key, NULL, 0, T_Null);
                elementDupValues(&el, index->ptr);
                elementTablePush(&result, el);
            }
        }
    }

    statePushResultTable(result);
}


void stdSome(struct result_list *args)
{
    struct element_table *table = getValueTable(args->first);
    struct function *func = getValueFunc(args->first->next);
    size_t index = 0;

    for (struct element_list *i = table->first; i != NULL; i = i->next) {
        struct result_list *func_args;
        struct result *func_result;

        if (i->ptr->unset) {
            continue;
        }

        func_args = resultListInit();
        statePushResult(func_args, getResultFromElement(i->ptr));
        mapIteratorArg(func, func_args, &index, 2);

        funcExec(func, func_args, state->current_table);
        func_result = statePopResult();
        resultListFree(func_args);

        if (isTrue(func_result)) {
            statePushResultD(true);
            resultFree(func_result);
            return;
        }

        resultFree(func_result);
    }

    statePushResultD(false);
}


void stdFill(struct result_list *args)
{
    int start = (int) getValueD(args->first);
    int count = (int) getValueD(args->first->next);
    struct element_table *result = elementTableInit();

    for (int i = 0; i < count; i++) {
        char *key = strFromInt(start++);
        struct result *val = resultDup(args->first->next->next);
        struct element *el = elementInit(key, NULL, 0, T_Null);
        mapResultToElement(el, val);
        elementTablePush(&result, el);

        free(key);
        resultFree(val);
    }

    statePushResultTable(result);
}
