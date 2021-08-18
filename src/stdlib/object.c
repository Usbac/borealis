#include <stdlib.h>
#include "../state.h"
#include "../engine/processor_helper.h"
#include "object.h"


void stdGetObjectKeys(struct result_list *args)
{
    struct element_table *arr = getValueObj(args->first);
    struct element_table *result = elementTableInit();
    size_t index = 0;

    for (struct element_list *i = arr != NULL ? arr->first : NULL; i != NULL; i = i->next) {
        if (!i->ptr->unset && i->ptr->public) {
            char *key = strFromSizet(index++);
            struct element *el = elementInit(key, NULL, 0, T_String);
            el->public = true;
            el->value.string = strDup(i->ptr->key);
            elementTablePush(&result, el);
            free(key);
        }
    }

    statePushResultArr(result);
}


void stdGetObjectValues(struct result_list *args)
{
    struct element_table *arr = getValueObj(args->first);
    struct element_table *result = elementTableInit();
    size_t index = 0;

    for (struct element_list *i = arr != NULL ? arr->first : NULL;
        i != NULL;
        i = i->next) {
        if (!i->ptr->unset && i->ptr->public) {
            char *key = strFromSizet(index++);
            struct element *el = elementInit(key, NULL, 0, T_Null);
            el->public = true;
            elementDupValues(&el, i->ptr);
            elementTablePush(&result, el);
            free(key);
        }
    }

    statePushResultArr(result);
}
