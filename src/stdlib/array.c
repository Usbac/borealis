#include <stdlib.h>
#include <string.h>
#include "../state.h"
#include "../engine/processor_helper.h"
#include "../engine/processor.h"
#include "array.h"


static void removeAndReturnArrEl(struct element_list *el)
{
    if (el == NULL || el->ptr->unset) {
        pushResultNull();
    } else {
        pushResult(state->stack, getResultFromElement(el->ptr));
        freeElement(&el->ptr);
    }
}


static void swap(struct element_list **a, struct element_list **b)
{
    struct element_list *aux = *a;
    *a = *b;
    *b = aux;
}


static double getDouble(struct element *el)
{
    struct element *ref = getTrueElement(el);
    switch (ref->type) {
        case T_Array:
        case T_Object:
        case T_Function: return 1;
        case T_Null: return 0;
        case T_Number: return ref->value.number;
        default: return strToD(ref->value.string);
    }
}


static int64_t partition(struct element_list **arr, int64_t low, int64_t high)
{
    struct element_list *pivot = arr[high];
    int64_t i = low - 1;

    for (int64_t j = low; j <= high; j++) {
        if (getDouble(arr[j]->ptr) < getDouble(pivot->ptr)) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}


static void quickSort(struct element_list **arr, int64_t low, int64_t high)
{
    if (low < high) {
        int64_t pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}


void stdGet(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    char *key = getValueStr(args->first->next);
    struct element *el = getElementInTable(key, arr, NULL);

    pushResult(state->stack, getResultFromElement(el));
    free(key);
}


void stdGetSize(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    double size = 0;

    if (arr == NULL) {
        goto end;
    }

    for (struct element_list *i = arr->first; i != NULL; i = i->next) {
        size += !i->ptr->unset;
    }

    end: pushResultD(size);
}


void stdPush(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    struct result *arg = args->first->next;
    char *key = strFromSizet(arr->next_index);
    struct element *prop = newElement(key, NULL, 0, T_Null);
    prop->public = true;
    mapResultToElement(prop, arg);
    pushElementToTable(&arr, prop);

    free(key);
}


void stdPrepend(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    struct result *arg = args->first->next;
    char *key = strFromSizet(arr->next_index);
    struct element *prop = newElement(key, NULL, 0, T_Null);
    prop->public = true;
    mapResultToElement(prop, arg);
    prependElementToTable(&arr, prop);

    free(key);
}


void stdPop(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    removeAndReturnArrEl(arr->last);
}


void stdShift(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    removeAndReturnArrEl(arr->first);
}


void stdGetKeys(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    struct element_table *keys = newElementTable();
    size_t index = 0;

    for (struct element_list *i = arr != NULL ? arr->first : NULL;
        i != NULL;
        i = i->next) {
        if (!i->ptr->unset) {
            char *key = strFromSizet(index++);
            struct element *prop = newElement(key, NULL, 0, T_String);
            prop->public = true;
            prop->value.string = strDup(i->ptr->key);
            pushElementToTable(&keys, prop);
            free(key);
        }
    }

    pushResultArr(keys);
}


void stdHasKey(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    char *key = getValueStr(args->first->next);
    struct element *el = getElementInTable(key, arr, NULL);

    pushResultD(el != NULL && !el->unset);
    free(key);
}


void stdJoin(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    char *glue = args->first->next != NULL ?
        getValueStr(args->first->next) :
        strDup("");
    char *result = strInit();

    for (struct element_list *ite = arr != NULL ? arr->first : NULL;
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

    pushResultStr(result);
    free(glue);
}


void stdHas(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    struct result *val = args->first->next;
    bool has = false;

    for (struct element_list *ite = arr != NULL ? arr->first : NULL;
        ite != NULL;
        ite = ite->next) {
        struct element *el = getTrueElement(ite->ptr);

        if (!el->unset) {
            struct result *comp = getResultFromElement(el);
            has = compareResults(val, comp);
            freeResult(comp);
            if (has) {
                break;
            }
        }
    }

    pushResultD(has);
}


void stdReverse(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    struct element_list *temp;

    if (arr == NULL) {
        return;
    }

    for (struct element_list *ite = arr->first; ite != NULL;) {
        temp = ite->next;
        ite->next = ite->prev;
        ite->prev = temp;
        ite = temp;
    }

    temp = arr->last;
    arr->last = arr->first;
    arr->first = temp;
}


void stdRange(struct result_list *args)
{
    struct element_table *arr = newElementTable();
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
        struct element *prop = newElement(key_str, NULL, 0, T_Number);
        prop->value.number = (double) i;
        prop->public = true;
        pushElementToTable(&arr, prop);

        free(key_str);
    }

    pushResultArr(arr);
}


void stdSort(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    int i, len = 0;
    struct element_list *el = arr->first;
    struct element_list **values;
    while (el != NULL) {
        el = el->next;
        len++;
    }

    if (arr->first == NULL || arr->first->next == NULL) {
        return;
    }

    values = malloc_(len * sizeof(struct element_list));
    i = 0;
    el = arr->first;
    while (el != NULL) {
        values[i++] = el;
        el = el->next;
    }

    quickSort(values, 0, len - 1);

    for (i = 0; i < len; i++) {
        values[i]->prev = i == 0 ? NULL : values[i - 1];
        values[i]->next = i == len - 1 ? NULL : values[i + 1];
    }

    arr->first = values[0];
    arr->last = values[len - 1];
    free(values);
}


void stdColumn(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    char *column_name = getValueStr(args->first->next);
    struct element_table *result = newElementTable();

    for (struct element_list *ite = arr != NULL ? arr->first : NULL;
        ite != NULL;
        ite = ite->next) {
        struct element *src = getTrueElement(ite->ptr);
        struct element *column;

        if (src->unset || (src->type != T_Array && src->type != T_Object)) {
            continue;
        }

        column = getElementInTable(column_name, src->value.values, NULL);

        if (column != NULL) {
            struct element *el = newElement(NULL, NULL, 0, T_Null);
            el->key = strFromSizet(result->next_index);
            copyElementValues(&el, column);
            pushElementToTable(&result, el);
        }
    }

    pushResultArr(result);
    free(column_name);
}


static void mapIteratorArg(struct function *func,
                           struct result_list *func_args,
                           size_t *i,
                           size_t arg_index)
{
    struct result *index;

    if (func->num_params < arg_index) {
        return;
    }

    index = malloc_(sizeof(struct result));
    *index = (struct result) {
        .value.number = (double) (*i)++,
        .type = T_Number,
        .line_n = state->line_n,
    };

    pushResult(func_args, index);
}


void stdMap(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    struct element_table *result = newElementTable();
    struct function *func = getValueFunc(args->first->next);
    size_t index = 0;

    for (struct element_list *i = arr->first; i != NULL; i = i->next) {
        struct result_list *func_args;
        struct element *el;
        struct result *func_result;

        if (i->ptr->unset) {
            continue;
        }

        func_args = newResultList();
        el = newElement(i->ptr->key, NULL, 0, T_Null);

        pushResult(func_args, getResultFromElement(i->ptr));
        mapIteratorArg(func, func_args, &index, 2);

        execFunc(func, func_args, state->current_obj);
        func_result = popResult();
        mapResultToElement(el, func_result);
        pushElementToTable(&result, el);

        freeResult(func_result);
        freeResultList(func_args);
    }

    pushResultArr(result);
}


void stdFilter(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    struct element_table *result = newElementTable();
    struct function *func = getValueFunc(args->first->next);
    size_t index = 0;

    for (struct element_list *i = arr->first; i != NULL; i = i->next) {
        struct result_list *func_args;
        struct result *func_result;

        if (i->ptr->unset) {
            continue;
        }

        func_args = newResultList();
        pushResult(func_args, getResultFromElement(i->ptr));
        mapIteratorArg(func, func_args, &index, 2);

        execFunc(func, func_args, state->current_obj);
        func_result = popResult();

        if (isTrue(func_result)) {
            struct element *el = newElement(i->ptr->key, NULL, 0, T_Null);
            copyElementValues(&el, i->ptr);
            pushElementToTable(&result, el);
        }

        freeResult(func_result);
        freeResultList(func_args);
    }

    pushResultArr(result);
}


void stdReduce(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
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

    for (struct element_list *i = arr->first; i != NULL; i = i->next) {
        struct result_list *func_args;

        if (i->ptr->unset) {
            continue;
        }

        func_args = newResultList();
        pushResult(func_args, accumulator);
        pushResult(func_args, getResultFromElement(i->ptr));
        mapIteratorArg(func, func_args, &index, 3);

        execFunc(func, func_args, state->current_obj);
        accumulator = popResult();

        freeResultList(func_args);
    }

    pushResult(state->stack, accumulator);
}


void stdMerge(struct result_list *args)
{
    struct element_table *result = newElementTable();

    for (struct result *i = args->first; i != NULL; i = i->next) {
        struct element_table *arr = getValueArr(i);
        if (arr == NULL) {
            continue;
        }

        for (struct element_list *index = arr->first; index != NULL; index = index->next) {
            struct element *el;
            if (index->ptr->unset) {
                continue;
            }

            if ((el = getElementInTable(index->ptr->key, result, NULL)) != NULL) {
                freeElementValues(&el);
                copyElementValues(&el, index->ptr);
            } else {
                el = newElement(index->ptr->key, NULL, 0, T_Null);
                copyElementValues(&el, index->ptr);
                pushElementToTable(&result, el);
            }
        }
    }

    pushResultArr(result);
}


void stdSome(struct result_list *args)
{
    struct element_table *arr = getValueArr(args->first);
    struct function *func = getValueFunc(args->first->next);
    size_t index = 0;

    for (struct element_list *i = arr->first; i != NULL; i = i->next) {
        struct result_list *func_args;
        struct result *func_result;

        if (i->ptr->unset) {
            continue;
        }

        func_args = newResultList();
        pushResult(func_args, getResultFromElement(i->ptr));
        mapIteratorArg(func, func_args, &index, 2);

        execFunc(func, func_args, state->current_obj);
        func_result = popResult();
        freeResultList(func_args);

        if (isTrue(func_result)) {
            pushResultD(true);
            freeResult(func_result);
            return;
        }

        freeResult(func_result);
    }

    pushResultD(false);
}
