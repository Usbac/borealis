#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../engine/lexer.h"
#include "../engine/parser.h"
#include "../engine/processor_helper.h"
#include "../state.h"
#include "json.h"

bool json_error = false;


static char *strAddQuotes(char *str)
{
    size_t len = strlen(str) + 3;
    char *result = malloc_(len);
    snprintf(result, len, "\"%s\"", str);

    return result;
}


static void elementTableToJson(char **result,
                               struct element_table *obj,
                               bool as_obj)
{
    bool prev = false;

    strAppendC(result, as_obj ? '{' : '[');

    for (struct element_list *i = obj->first; i != NULL; i = i->next) {
        struct element *el = getTrueElement(i->ptr);
        char *aux = NULL;

        if (el->unset || !el->public) {
            continue;
        }

        if (prev) {
            strAppendC(result, ',');
        }

        if (as_obj) {
            char *key = strAddQuotes(el->key);
            strAppend(result, key);
            strAppendC(result, ':');
            free(key);
        }

        switch (el->type) {
            case T_String:
                aux = strAddQuotes(el->value.string);
                strAppend(result, aux);
                break;
            case T_Number:
                aux = strFromD(el->value.number);
                strAppend(result, aux);
                break;
            case T_Function:
                aux = getUnionStr(el->type, el->value);
                strAppend(result, aux);
                break;
            case T_Array:
            case T_Object:
                elementTableToJson(result, el->value.values,
                    el->type == T_Object);
                break;
            case T_Null: strAppend(result, TYPEOF_NULL); break;
            default: break;
        }

        free(aux);
        prev = true;
    }

    strAppendC(result, as_obj ? '}' : ']');
}


void stdJsonStringify(struct result_list *args)
{
    struct result *arg = args->first;
    enum TYPE type = getResultType(arg);
    char *result, *aux;

    switch (type) {
        case T_String:
            aux = getValueStr(arg);
            result = strAddQuotes(aux);
            free(aux);
            break;
        case T_Number: result = strFromD(getValueD(arg)); break;
        case T_Function: result = getValueStr(arg); break;
        case T_Array:
        case T_Object:
            result = strInit();
            elementTableToJson(&result,
                type == T_Object ? getValueObj(arg) : getValueArr(arg),
                type == T_Object);
            break;
        case T_Null: result = strDup(TYPEOF_NULL); break;
        default: result = strInit(); break;
    }

    statePushResultStr(result);
}


static struct token_list *preprocessJson(char *code)
{
    struct token_list *tokens = tokenizeJson(code, &json_error);

    if (json_error) {
        goto error;
    }

    for (struct token *i = tokens->first; i != NULL; i = i->next) {
        if (i->type == T_Chunk || i->type == T_Array) {
            i->body = preprocessJson(i->value);
            if (i->body == NULL) {
                goto error;
            }
        }
    }

    return tokens;

    error:
        json_error = false;
    listFree(tokens);
        return NULL;
}


static bool isValidJsonVal(struct token *token)
{
    return token->type == T_String || token->type == T_Number ||
        token->type == T_Null || token->type == T_Chunk ||
        token->type == T_Array || (token->type == T_Identifier &&
        (!strcmp(token->value, "true") || !strcmp(token->value, "false")));
}


static struct element *getJsonEl(char *key, struct token *node)
{
    struct element *el = elementInit(key, NULL, 0, T_Null);
    el->type = node->type;
    el->public = true;

    switch (node->type) {
        case T_String: el->value.string = strDup(node->value); break;
        case T_Number: el->value.number = strToD(node->value); break;
        case T_Array: el->value.values = parseArrjJson(node->body); break;
        case T_Identifier:
            el->type = T_Number;
            el->value.number = !strcmp(node->value, "true");
            break;
        case T_Null: el->value.string = NULL; break;
        case T_Chunk:
            el->type = T_Object;
            el->value.values = parseObjJson(node->body); break;
        default: break;
    }

    return el;
}


struct element_table *parseArrjJson(struct token_list *list)
{
    struct element_table *arr = elementTableInit();

    for (struct token *ite = list->first; ite != NULL; ite = ite->next) {
        char *key;

        if (ite->opcode == OP_Stmt_sep) {
            if (ite->next == NULL) {
                json_error = true;
                break;
            }

            ite = ite->next;
        }

        if (!isValidJsonVal(ite) ||
            (ite->next != NULL && ite->next->opcode != OP_Stmt_sep)) {
            json_error = true;
            break;
        }

        key = strFromSizet(arr->next_index++);
        elementTablePush(&arr, getJsonEl(key, ite));
        free(key);
    }

    return arr;
}


struct element_table *parseObjJson(struct token_list *list)
{
    struct element_table *obj = elementTableInit();

    for (struct token *i = list->first; i != NULL; i = i->next->next->next) {
        if (i->opcode == OP_Stmt_sep) {
            if (i->next == NULL) {
                json_error = true;
                break;
            }

            i = i->next;
        }

        if (i->type != T_String ||
            (i->next == NULL || i->next->opcode != OP_Colon || i->next->next == NULL) ||
            !isValidJsonVal(i->next->next) ||
            (i->next->next->next != NULL && i->next->next->next->opcode != OP_Stmt_sep)) {
            json_error = true;
            break;
        }

        elementTablePush(&obj, getJsonEl(i->value, i->next->next));
    }

    return obj;
}


void stdJsonParse(struct result_list *args)
{
    char *str = getValueStr(args->first);
    struct token_list *list = preprocessJson(str);
    struct element_table *aux_table;
    json_error = false;

    if (list == NULL || list->first == NULL ||
        !isValidJsonVal(list->first) || list->first->next != NULL) {
        json_error = true;
        goto end;
    }

    switch (list->first->type) {
        case T_Array:
        case T_Chunk:
            aux_table = list->first->type == T_Array ?
                parseArrjJson(list->first->body) :
                parseObjJson(list->first->body);
            if (json_error) {
                elementsTableFree(aux_table, 0);
                free(aux_table);
            } else {
                list->first->type == T_Array ?
                StatePushResultArr(aux_table) :
                statePushResultObj(aux_table);
            }
            break;
        case T_String:
            statePushResultStr(strDup(list->first->value));
            break;
        case T_Number:
            statePushResultD(strToD(list->first->value));
            break;
        case T_Null:
            statePushResultNull();
            break;
        case T_Identifier:
            statePushResultD(!strcmp(list->first->value, "true"));
            break;
        default: break;
    }

    end:
        if (json_error) {
            statePushResultNull();
        }

    ListFreeR(list);
        free(str);
}


void stdIsvalid(struct result_list *args)
{
    stdJsonParse(args);
    statePushResultD(!json_error);
}
