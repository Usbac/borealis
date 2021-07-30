#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#include "engine/lexer.h"
#include "engine/parser.h"
#include "element.h"

/**
 * Elements list.
 */
struct element_table *elements = NULL;

/**
 * Garbage collector list.
 */
struct element *garbage_head = NULL;


struct element_table *newElementTable(void)
{
    struct element_table *table = malloc_(sizeof(struct element_table));
    *table = (struct element_table) {0};

    return table;
}


static bool matchesElement(const struct element *el,
                           const char *key,
                           const char *file)
{
    return el->key != NULL && !strcmp(el->key, key) &&
        (file == NULL || el->file == NULL || !strcmp(el->file, file));
}


static void addToGarbage(struct element *el)
{
    el->next = garbage_head;
    garbage_head = el;
}


static void updateArrayNextIndex(struct element_table *el)
{
    char *key = strFromSizet(el->next_index);

    while (getElementInTable(key, el, NULL) != NULL) {
        SNPRINTF_SIZE_T(key, ++el->next_index);
    }

    free(key);
}


static struct element *copyElement(struct element *src)
{
    struct element *dest;

    if (src == NULL) {
        return NULL;
    }

    dest = newElement(src->key, src->file, src->scope, T_Null);
    dest->public = src->public;
    dest->constant = src->constant;
    copyElementValues(&dest, src);

    return dest;
}


struct element_table *copyElementTable(const struct element_table *src)
{
    struct element_table *table;

    if (src == NULL) {
        return NULL;
    }

    table = newElementTable();
    for (struct element_list *i = src->first; i != NULL; i = i->next) {
        if (!i->ptr->unset) {
            pushElementToTable(&table, copyElement(i->ptr));
        }
    }

    return table;
}


static void insertElementInHashmap(struct element_table **table,
                                   struct element *el)
{
    const size_t hash = getHash(el->key, HASHMAP_SIZE);

    if (*table == NULL) {
        *table = newElementTable();
    }

    el->next = (*table)->map[hash];
    (*table)->map[hash] = el;
    updateArrayNextIndex(*table);
}


void prependElementToTable(struct element_table **table, struct element *el)
{
    struct element_list *new = malloc_(sizeof(struct element_list));
    insertElementInHashmap(table, el);
    *new = (struct element_list) {
        .ptr = el,
    };

    /* add to ordered list */
    if ((*table)->first == NULL) {
        (*table)->first = new;
        (*table)->last = new;
    } else {
        new->next = (*table)->first;
        (*table)->first->prev = new;
        (*table)->first = new;
    }
}


void pushElementToTable(struct element_table **table, struct element *el)
{
    struct element_list *new = malloc_(sizeof(struct element_list));
    insertElementInHashmap(table, el);
    *new = (struct element_list) {
        .ptr = el,
    };

    /* add to ordered list */
    if ((*table)->first == NULL) {
        (*table)->first = new;
        (*table)->last = new;
    } else {
        (*table)->last->next = new;
        new->prev = (*table)->last;
        (*table)->last = (*table)->last->next;
    }
}


struct function *initFunc(void)
{
    struct function *func = malloc_(sizeof(struct function));
    *func = (struct function) {0};

    return func;
}


struct function *copyFunction(struct function *func)
{
    struct function *new;

    if (func == NULL) {
        return NULL;
    }

    new = initFunc();
    new->def_file = strDup(func->def_file);
    new->params = copyList(func->params);
    new->stmts = copyList(func->stmts);
    new->num_params = func->num_params;
    new->func_ptr = func->func_ptr;
    new->func_ptr_params = func->func_ptr_params;
    new->return_type = func->return_type;
    new->native_code = func->native_code;

    return new;
}


struct element *newElement(const char *key,
                           const char *file,
                           size_t scope,
                           enum TYPE type)
{
    struct element *el = malloc_(sizeof(struct element));
    *el = (struct element) {
        .scope = scope,
        .type = type,
        .file = strDup(file),
        .key = strDup(key),
    };

    switch (type) {
        case T_Object:
        case T_Array: el->value.values = newElementTable(); break;
        case T_Number: el->value.number = 0; break;
        case T_Function: el->value.function = initFunc(); break;
        default: el->value.string = NULL;
    }

    return el;
}


void freeFunction(struct function *func)
{
    if (func->params != NULL) {
        freeListRecursive(func->params);
        func->params = NULL;
    }

    if (func->stmts != NULL) {
        freeListRecursive(func->stmts);
        func->stmts = NULL;
    }

    if (func->def_file != NULL) {
        free(func->def_file);
    }
}


void freeElementValues(struct element **el)
{

    switch ((*el)->type) {
        case T_Reference: (*el)->value.reference = NULL; break;
        case T_Number: (*el)->value.number = 0; break;
        case T_Object:
        case T_Array:
            freeElementsTable((*el)->value.values, 0);
            FREE_AND_NULL((*el)->value.values);
            break;
        case T_Function:
            freeFunction((*el)->value.function);
            FREE_AND_NULL((*el)->value.function);
            break;
        case T_String: FREE_AND_NULL((*el)->value.string);
        default: break;
    }
}


union VALUE valueDup(union VALUE val, enum TYPE type)
{
    union VALUE new;

    switch (type) {
        case T_Reference: new.reference = val.reference; break;
        case T_Number: new.number = val.number; break;
        case T_Object:
        case T_Array: new.values = copyElementTable(val.values); break;
        case T_Function: new.function = copyFunction(val.function); break;
        case T_String: new.string = strDup(val.string); break;
        default: new.string = NULL; break;
    }

    return new;
}


void copyElementValues(struct element **dest, struct element *src)
{
    if (src == NULL || src == *dest) {
        return;
    }

    (*dest)->value = valueDup(src->value, src->type);
    (*dest)->type = src->type;
}


struct element *getElementInTable(const char *key,
                                  struct element_table *list,
                                  const char *file)
{
    if (key == NULL || list == NULL) {
        return NULL;
    }

    for (struct element *node = list->map[getHash(key, HASHMAP_SIZE)];
        node != NULL;
        node = node->next) {
        if (matchesElement(node, key, file)) {
            return node;
        }
    }

    return NULL;
}


static void freeElementsList(struct element_table *table)
{
    struct element_list *tmp = table->first;

    while (tmp != NULL) {
        if (!tmp->ptr->unset) {
            tmp = tmp->next;
            continue;
        }

        if (tmp == table->first) {
            table->first = table->first->next;
            if (table->first != NULL) {
                table->first->prev = NULL;
            }
            addToGarbage(tmp->ptr);
            FREE_AND_NULL(tmp);
            tmp = table->first;
        } else if (tmp == table->last) {
            table->last = table->last->prev;
            table->last->next = NULL;
            addToGarbage(tmp->ptr);
            FREE_AND_NULL(tmp);
        } else {
            struct element_list *aux = tmp->next;
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            addToGarbage(tmp->ptr);
            FREE_AND_NULL(tmp);
            tmp = aux;
        }
    }
}


void freeElementsTable(struct element_table *table, size_t scope)
{
    if (table == NULL) {
        return;
    }

    for (size_t i = 0; i < HASHMAP_SIZE; i++) {
        struct element *node = table->map[i], *tmp = NULL;

        while (node != NULL && node->scope >= scope) {
            table->map[i] = node->next;
            freeElement(&node);
            node = table->map[i];
        }

        for (node = table->map[i]; node != NULL; node = node->next) {
            while (node->next != NULL && node->next->scope >= scope) {
                tmp = node->next;
                node->next = tmp->next;

                freeElement(&tmp);
            }
        }
    }

    freeElementsList(table);
}


void freeElement(struct element **node)
{
    if (*node == NULL) {
        return;
    }

    (*node)->unset = true;
    freeElementValues(node);
    FREE_AND_NULL((*node)->key);
    FREE_AND_NULL((*node)->file);
}


void freeElements(void)
{
    struct element *node;

    freeElementsTable(elements, 0);
    free(elements);

    while ((node = garbage_head) != NULL) {
        garbage_head = garbage_head->next;
        FREE_AND_NULL(node);
    }
}
