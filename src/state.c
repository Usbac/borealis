#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "engine/lexer.h"
#include "state.h"

/**
 * Current state.
 */
struct state *state = NULL;


static struct element_table **getDefinitionTable(void)
{
    if (state->callstack != NULL && state->callstack->object == state->current_obj) {
        return &state->callstack->elements;
    } else if (state->current_obj != NULL) {
        return &state->current_obj;
    } else {
        return &elements;
    }
}


void stateInit(const char *file)
{
    state = malloc_(sizeof(struct state));
    *state = (struct state) {
        .file = strDup(file),
        .stack = resultListInit(),
    };

    for (size_t i = 0; i < HASHMAP_SIZE; i++) {
        state->imports[i] = NULL;
    }
}


void stateFree(void)
{
    while (state->callstack != NULL) {
        stateCallstackPop();
    }

    while (state->context_head != NULL) {
        statePopContext();
    }

    for (size_t i = 0; i < HASHMAP_SIZE; i++) {
        while (state->imports[i] != NULL) {
            struct import *node = state->imports[i];
            state->imports[i] = node->next;
            free(node->file);
            free(node);
        }
    }

    resultListFree(state->stack);
    FREE_AND_NULL(state->file);
    FREE_AND_NULL(state);
}


void stateSetFile(char *file)
{
    FREE_AND_NULL(state->file);
    state->file = file;
    struct element *el = stateElementGet(STATE_FILE_KEY, NULL);
    if (el != NULL) {
        free(el->value.string);
        el->value.string = strDup(file);
        el->type = T_String;
    }
}


void stateAddFileImport(const char *file)
{
    struct import *new = malloc_(sizeof(struct import));
    const size_t hash = getHash(file, HASHMAP_SIZE);

    new->file = strDup(file);
    new->next = state->imports[hash];
    state->imports[hash] = new;
}


bool stateIsFileImported(const char *file)
{
    const size_t hash = getHash(file, HASHMAP_SIZE);

    for (struct import *i = state->imports[hash]; i != NULL; i = i->next) {
        if (!strcmp(i->file, file)) {
            return true;
        }
    }

    return false;
}


void statePushContext(enum CONTEXT context)
{
    struct context *node = malloc_(sizeof(struct context));
    node->value = context;
    node->prev = state->context_head;

    if (state->context_head != NULL) {
        state->context_head->next = node;
    }

    state->context_head = node;
}


bool stateIsInsideContext(enum CONTEXT context)
{
    for (struct context *i = state->context_head; i != NULL; i = i->prev) {
        if (i->value == context) {
            return true;
        } else if (i->value == C_Function || i->value == C_Import) {
            break;
        }
    }

    return false;
}


void statePopContext(void)
{
    if (state->context_head == NULL) {
        return;
    }

    if (state->context_head->prev != NULL) {
        state->context_head = state->context_head->prev;
        FREE_AND_NULL(state->context_head->next);
    } else {
        FREE_AND_NULL(state->context_head);
    }
}


bool stateCanContinue(void)
{
    return !state->continuing_loop && !state->breaking_loop &&
        !state->returning && !state->exiting;
}


struct result_list *resultListInit(void)
{
    struct result_list *list = malloc_(sizeof(struct result_list));
    *list = (struct result_list) {0};

    return list;
}


void statePushResult(struct result_list *list, struct result *node)
{
    if (list->last == NULL) {
        list->first = node;
        node->prev = NULL;
    } else {
        list->last->next = node;
        node->prev = list->last;
    }

    list->last = node;
    node->next = NULL;
}


void statePushResultD(double n)
{
    struct result *node = malloc_(sizeof(struct result));
    *node = (struct result) {
        .value.number = n,
        .type = T_Number,
        .line_n = state->line_n,
    };

    statePushResult(state->stack, node);
}


void statePushResultStr(char *str)
{
    struct result *node = malloc_(sizeof(struct result));
    *node = (struct result) {
        .value.string = str,
        .type = T_String,
        .line_n = state->line_n,
    };

    statePushResult(state->stack, node);
}


void statePushResultRef(struct element *el)
{
    struct result *node = malloc_(sizeof(struct result));
    *node = (struct result) {
        .value.reference = el,
        .type = T_Reference,
        .line_n = state->line_n,
    };

    statePushResult(state->stack, node);
}


void statePushResultArr(struct element_table *values)
{
    struct result *node = malloc_(sizeof(struct result));
    *node = (struct result) {
        .value.values = values,
        .type = T_Array,
        .line_n = state->line_n,
    };

    statePushResult(state->stack, node);
}


void statePushResultFunc(struct function *func)
{
    struct result *node = malloc_(sizeof(struct result));
    *node = (struct result) {
        .value.function = func,
        .type = T_Function,
        .line_n = state->line_n,
    };

    statePushResult(state->stack, node);
}


void statePushResultEl(struct element *el)
{
    struct result *node = malloc_(sizeof(struct result));
    *node = (struct result) {
        .value.string = NULL,
        .type = T_Identifier,
        .p_el = el,
        .line_n = state->line_n,
    };

    statePushResult(state->stack, node);
}


void statePushResultNull(void)
{
    struct result *node = malloc_(sizeof(struct result));
    *node = (struct result) {
        .value.string = NULL,
        .type = T_Null,
        .line_n = state->line_n,
    };

    statePushResult(state->stack, node);
}


struct result *statePopResult(void)
{
    struct result *node = statePopResultSafe();
    if (node != NULL && node->type == T_Identifier && node->p_el == NULL) {
        errorF(node->line_n, E_UNDEFINED, node->value);
    }

    return node;
}


struct result *statePopResultSafe(void)
{
    struct result *new;

    if (state->stack == NULL || state->stack->first == NULL) {
        return NULL;
    }

    new = state->stack->last;

    if (state->stack->last->prev != NULL) {
        state->stack->last = state->stack->last->prev;
        state->stack->last->next = NULL;
    } else {
        state->stack->last = NULL;
        state->stack->first = NULL;
    }

    return new;
}


struct result_list *resultListDup(struct result_list *list)
{
    struct result_list *new_list;

    if (list == NULL) {
        return NULL;
    }

    new_list = resultListInit();

    for (struct result *node = list->first; node != NULL; node = node->next) {
        statePushResult(new_list, resultDup(node));
    }

    return new_list;
}


void resultListFree(struct result_list *list)
{
    while (list->last != NULL) {
        struct result *node = list->last;

        if (list->last->prev != NULL) {
            list->last = list->last->prev;
            list->last->next = NULL;
        } else {
            list->last = NULL;
            list->first = NULL;
        }

        resultFree(node);
    }

    free(list);
}


void resultFree(struct result *node)
{
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case T_String:
            free(node->value.string);
            break;
        case T_Array:
            if (state->current_obj != node->value.values) {
                elementsTableFree(node->value.values, 0);
                free(node->value.values);
            }
            break;
        case T_Function:
            functionFree(node->value.function);
            free(node->value.function);
            break;
        default: break;
    }

    FREE_AND_NULL(node);
}


struct result *resultDup(struct result *node)
{
    struct result *cpy = malloc_(sizeof(struct result));
    cpy->line_n = node->line_n;
    cpy->type = node->type;
    cpy->p_el = node->p_el;
    cpy->value = valueDup(node->value, node->type);

    return cpy;
}


void stateElementDeclare(struct element **el)
{
    elementTablePush(getDefinitionTable(), *el);
}


struct element *stateElementGet(const char *key, const char *file)
{
    struct element *el = NULL;

    if (state->current_obj != NULL) {
        el = elementGet(key, state->current_obj, NULL);
    }

    if (el == NULL && state->callstack != NULL) {
        el = elementGet(key, state->callstack->elements, NULL);
    }

    if (el == NULL) {
        el = elementGet(key, elements, file);
    }

    return el;
}


void stateElementsFree(size_t scope)
{
    if (state->callstack != NULL) {
        elementsTableFree(state->callstack->elements, scope);
    }

    elementsTableFree(elements, scope);
}


void stateCallstackPush(struct element_table *args, struct element_table *obj)
{
    struct call *node;

    if (state->callstack_size >= UINT8_MAX) {
        errorF(state->line_n, E_STACK);
    }

    node = malloc_(sizeof(struct call));
    node->elements = args;
    node->object = obj;
    node->prev = state->callstack;
    state->callstack = node;

    state->callstack_size++;
}


void stateCallstackPop(void)
{
    struct call *tmp;

    if (state->callstack == NULL) {
        return;
    }

    tmp = state->callstack->prev;
    elementsTableFree(state->callstack->elements, 0);
    free(state->callstack->elements);
    FREE_AND_NULL(state->callstack);
    state->callstack = tmp;
    state->callstack_size--;
}
