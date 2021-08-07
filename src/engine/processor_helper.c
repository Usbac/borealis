#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "lexer.h"
#include "../state.h"
#include "../error.h"
#include "processor_helper.h"


void mapTypeVal(const struct result *node, enum TYPE *type, union VALUE *value)
{
    struct element *el = getValueEl(node);

    if (el != NULL) {
        *type = el->type;
        *value = el->value;
    } else {
        *type = node->type;
        *value = node->value;
    }
}


char *getUnionStr(enum TYPE type, union VALUE value)
{
    switch (type) {
        case T_Object: return strDup("["TYPEOF_OBJECT"]");
        case T_Array: return strDup("["TYPEOF_ARRAY"]");
        case T_Function: return strDup("["TYPEOF_FUNCTION"]");
        case T_Number: return strFromD(value.number);
        case T_String: return strDup(value.string);
        default: return strDup("["TYPEOF_NULL"]");
    }
}


double getValueD(struct result *node)
{
    enum TYPE type;
    union VALUE value;

    mapTypeVal(node, &type, &value);

    switch (type) {
        case T_Array:
        case T_Object:
        case T_Function: return 1;
        case T_Null: return 0;
        case T_Number: return value.number;
        default: return strToD(value.string);
    }
}


struct element *getValueEl(const struct result *node)
{
    return getTrueElement(node->type == T_Reference ?
        node->value.reference :
        node->p_el);
}


struct element_table *getValueArr(const struct result *node)
{
    enum TYPE type;
    union VALUE value;
    mapTypeVal(node, &type, &value);

    return type == T_Array ? value.values : NULL;
}


struct element_table *getValueObj(const struct result *node)
{
    enum TYPE type;
    union VALUE value;
    mapTypeVal(node, &type, &value);

    return type == T_Object ? value.values : NULL;
}


struct function *getValueFunc(const struct result *node)
{
    enum TYPE type;
    union VALUE value;
    mapTypeVal(node, &type, &value);

    return type == T_Function ? value.function : NULL;
}


char *getValueStr(struct result *node)
{
    struct element *el = getValueEl(node);

    return el != NULL ?
        getUnionStr(el->type, el->value) :
        getUnionStr(node->type, node->value);
}


struct element *getTrueElement(struct element *node)
{
    struct element *el = node;

    while (el != NULL && el->type == T_Reference &&
        !el->value.reference->unset) {
        el = el->value.reference;
    }

    if (el != NULL && el->type == T_Reference && el->value.reference->unset) {
        el->value.string = NULL;
        el->type = T_Null;
    }

    return el;
}


bool isTrue(const struct result *node)
{
    enum TYPE type;
    union VALUE value;

    mapTypeVal(node, &type, &value);

    switch (type) {
        case T_String: return value.string[0] != '\0';
        case T_Number: return value.number != 0;
        default: return type != T_Null;
    }
}


bool isEmptyReturn(const struct token *node)
{
    return node->prev == NULL || node->prev->opcode == OP_Stmt_sep ||
        (node->prev->type == T_Chunk && (node->prev->prev == NULL ||
        (node->prev->prev->type != T_Parameters && node->prev->prev->opcode != OP_Object)));
}


static bool compareElementTables(struct element_table *a,
                                 struct element_table *b)
{
    struct element_list *a_ite = a->first;
    struct element_list *b_ite = b->first;

    while (a_ite != NULL && b_ite != NULL) {
        struct element *a_el = getTrueElement(a_ite->ptr);
        struct element *b_el = getTrueElement(b_ite->ptr);

        while (a_ite->ptr->unset && a_ite != NULL) {
            a_ite = a_ite->next;
        }

        while (b_ite->ptr->unset && b_ite != NULL) {
            b_ite = b_ite->next;
        }

        if ((a_ite == NULL && b_ite != NULL) ||
            (a_ite != NULL && b_ite == NULL) ||
            a_el->type != b_el->type ||
            strcmp(a_el->key, b_el->key) != 0) {
            return false;
        }

        switch (a_el->type) {
            case T_Number:
                if (a_el->value.number != b_el->value.number) {
                    return false;
                }
                break;
            case T_String:
                if (strcmp(a_el->value.string, b_el->value.string) != 0) {
                    return false;
                }
                break;
            case T_Object:
            case T_Array:
                if (!compareElementTables(a_el->value.values,
                    b_el->value.values)) {
                    return false;
                }
                break;
            default: break;
        }

        a_ite = a_ite->next;
        b_ite = b_ite->next;
    }

    return a_ite == b_ite;
}


static bool compareNumbers(double a, double b)
{
    return isnan(a) && isnan(b) ? true : a == b;
}


bool compareResults(struct result *a, struct result *b)
{
    enum TYPE a_type, b_type;
    union VALUE a_val, b_val;

    mapTypeVal(a, &a_type, &a_val);
    mapTypeVal(b, &b_type, &b_val);

    if (a_type != b_type) {
        return false;
    }

    switch (a_type) {
        case T_Array:
        case T_Object: return compareElementTables(a_val.values, b_val.values);
        case T_Number: return compareNumbers(a_val.number, b_val.number);
        case T_String: return !strcmp(a_val.string, b_val.string);
        default: return true;
    }
}


enum TYPE getResultType(struct result *node)
{
    struct element *el;

    if (node == NULL) {
        return T_Null;
    }

    switch (node->type) {
        case T_String:
        case T_Number:
        case T_Array:
        case T_Object:
        case T_Function:
        case T_Null: return node->type;
        default:
            el = getValueEl(node);
            return el == NULL ? T_Null : el->type;
    }
}


char *getElementTypeAsStr(enum TYPE type)
{
    switch (type) {
        case T_String: return TYPEOF_STRING;
        case T_Number: return TYPEOF_NUMBER;
        case T_Array: return TYPEOF_ARRAY;
        case T_Object: return TYPEOF_OBJECT;
        case T_Function: return TYPEOF_FUNCTION;
        default: return TYPEOF_NULL;
    }
}


double getOpResult(enum OPCODE opcode, double a, double b)
{
    switch (opcode) {
        case OP_Plus:
        case OP_Plus_equal: return a + b;
        case OP_Minus:
        case OP_Minus_equal: return a - b;
        case OP_Div:
        case OP_Div_equal: return a / b;
        case OP_Multi:
        case OP_Multi_equal: return a * b;
        case OP_Mod:
        case OP_Mod_equal: return fmod(a, b);
        case OP_Lower: return a < b;
        case OP_Higher: return a > b;
        case OP_Lower_equal: return a <= b;
        case OP_Higher_equal: return a >= b;
        case OP_Compare: return compareNumbers(a, b);
        case OP_Different: return !compareNumbers(a, b);
        case OP_Pow: return pow(a, b);
        default: return 0;
    }
}


static struct element *getElementRef(struct result *node)
{
    struct element *el = node->type == T_Reference ? node->value.reference : NULL;

    while (el != NULL && el->type == T_Reference && !el->value.reference->unset) {
        el = el->value.reference;
    }

    return el;
}


struct result *getResultFromElement(struct element *el)
{
    struct result *node = malloc_(sizeof(struct result));
    struct element *el_ref = el;
    union VALUE val;
    enum TYPE type;

    while (el_ref != NULL && el_ref->type == T_Reference &&
        !el_ref->value.reference->unset) {
        el_ref = el_ref->value.reference;
    }

    if (el_ref != NULL && !el_ref->unset) {
        val = valueDup(el_ref->value, el_ref->type);
        type = el_ref->type;
    } else {
        val.string = NULL;
        type = T_Null;
    }

    *node = (struct result) {
        .value = val,
        .type = type,
        .line_n = state->line_n,
    };

    return node;
}


void mapResultToElement(struct element *el, struct result *node)
{
    struct element *ref = getElementRef(node);

    elementFreeValues(&el);

    if (ref != NULL) {
        if (el->static_type != ref->static_type) {
            errorF(node->line_n, E_STATIC, getElementTypeAsStr(el->static_type));
        }

        el->type = T_Reference;
        el->value.reference = ref;
    } else if (node->p_el != NULL) {
        elementDupValues(&el, node->p_el);
    } else {
        el->type = node->type;
        el->value = node->value;
        node->type = T_Null;
        node->value.string = NULL;
    }
}


struct state *saveState(struct element_table *obj, char *file)
{
    struct state *aux = malloc_(sizeof(struct state));
    *aux = (struct state) {
        .null_coalescing = state->null_coalescing,
        .stack = stateResultListDup(state->stack),
        .file = strDup(state->file),
        .current_obj = state->current_obj,
    };

    resultListFree(state->stack);

    if (file != state->file) {
        stateSetFile(strDup(file));
    }

    state->null_coalescing = false;
    state->stack = resultListInit();
    state->current_obj = obj;

    return aux;
}


void restoreState(struct state *aux)
{
    resultListFree(state->stack);

    stateSetFile(aux->file);
    state->null_coalescing = aux->null_coalescing;
    state->stack = aux->stack;
    state->current_obj = aux->current_obj;
    state->breaking_loop = false;
    state->continuing_loop = false;
    state->returning = false;

    free(aux);
}


void jmpStmts(struct token **node)
{
    size_t end = (*node)->jmp;
    for (size_t i = 0; *node != NULL && i < end; i++) {
        *node = (*node)->next;
    }
}


size_t getParamsNumber(struct token_list *params)
{
    size_t n = 0;

    for (struct token *i = params->first; i != NULL; i = i->next) {
        n += (i->prev == NULL || i->prev->opcode != OP_Spread) &&
            (i->next == NULL || i->next->opcode == OP_Stmt_sep) &&
            i->type == T_Identifier;
    }

    return n;
}


bool inSpreadParam(struct token *node)
{
    return node != NULL &&
        node->next != NULL && node->next->opcode == OP_Spread;
}


bool isFunctionNode(struct token *node)
{
    return isDefKeyword(node->opcode) && node->next != NULL &&
        (node->next->type == T_Parameters ||
        (node->next->type == T_Identifier &&
        node->next->next != NULL && node->next->next->type == T_Parameters));
}


void validateElementMod(struct result *identifier, enum TYPE type)
{
    struct element *el = getValueEl(identifier);
    struct element *assign_el = identifier->p_el != NULL &&
        identifier->p_el->type == T_Reference ?
        identifier->p_el :
        el;

    if (el == NULL) {
        errorF(identifier->line_n, E_UNDEFINED, identifier->value.string);
    } else if (el->constant) {
        errorF(identifier->line_n, E_CONST_MODIFY, el->key);
    } else if (assign_el->static_type != T_Null && assign_el->static_type != type) {
        errorF(identifier->line_n,
            E_STATIC,
            getElementTypeAsStr(assign_el->static_type));
    }
}


enum TYPE getOpcodeType(enum OPCODE opcode)
{
    switch (opcode) {
        case OP_Type_string: return T_String;
        case OP_Type_number: return T_Number;
        case OP_Type_array: return T_Array;
        case OP_Type_object: return T_Object;
        case OP_Type_function: return T_Function;
        default: return T_Null;
    }
}
