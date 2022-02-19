#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../engine/lexer.h"
#include "../engine/parser.h"
#include "../engine/processor_helper.h"
#include "../state.h"
#include "json.h"


static char *strAddQuotes(char *str)
{
    size_t len = strlen(str) + 3;
    char *result = malloc_(len);
    snprintf(result, len, "\"%s\"", str);

    return result;
}


static void elementTableToJson(char **result,
                               struct element_table *table,
                               bool as_obj)
{
    bool prev = false;

    strAppendC(result, as_obj ? '{' : '[');

    for (struct element_list *i = table->first; i != NULL; i = i->next) {
        struct element *el = getTrueElement(i->ptr);
        char *aux = NULL;

        if (el->unset) {
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
            case T_Table:
                elementTableToJson(result, el->value.values, true);
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
        case T_Table:
            result = strInit();
            elementTableToJson(&result, getValueTable(arg), true);
            break;
        case T_Null: result = strDup(TYPEOF_NULL); break;
        default: result = strInit(); break;
    }

    statePushResultStr(result);
}


static struct token_list *preprocessJson(char *code)
{
    bool error = false;
    struct token_list *tokens = tokenizeJson(code, &error);

    if (error) {
        goto end_error;
    }

    for (struct token *i = tokens->first; i != NULL; i = i->next) {
        if (i->type == T_Chunk || i->type == T_Table) {
            i->body = preprocessJson(i->value);
            if (i->body == NULL) {
                goto end_error;
            }
        }
    }

    return tokens;

    end_error:
        listFree(tokens);
        return NULL;
}


static bool isValidJsonVal(struct token *token)
{
    return token->type == T_String || token->type == T_Number ||
        token->type == T_Null || token->type == T_Chunk ||
           token->type == T_Table || (token->type == T_Identifier &&
                                      (!strcmp(token->value, "true") || !strcmp(token->value, "false")));
}


static struct element *getJsonEl(char *key, struct token *node, bool *error)
{
    struct element *el = elementInit(key, NULL, 0, T_Null);
    el->type = node->type;

    switch (node->type) {
        case T_String: el->value.string = strDup(node->value); break;
        case T_Number: el->value.number = strToD(node->value); break;
        case T_Table: el->value.values = parseObjJson(node->body, error); break;
        case T_Identifier:
            el->type = T_Number;
            el->value.number = !strcmp(node->value, "true");
            break;
        case T_Null: el->value.string = NULL; break;
        default: break;
    }

    return el;
}


struct element_table *parseArrjJson(struct token_list *list, bool *error)
{
    struct element_table *table = elementTableInit();

    for (struct token *ite = list->first; ite != NULL; ite = ite->next) {
        char *key;

        if (ite->opcode == OP_Stmt_sep) {
            if (ite->next == NULL) {
                *error = true;
                break;
            }

            ite = ite->next;
        }

        if (!isValidJsonVal(ite) ||
            (ite->next != NULL && ite->next->opcode != OP_Stmt_sep)) {
            *error = true;
            break;
        }

        key = strFromSizet(table->next_index++);
        elementTablePush(&table, getJsonEl(key, ite, error));
        free(key);
    }

    return table;
}


struct element_table *parseObjJson(struct token_list *list, bool *error)
{
    struct element_table *table = elementTableInit();

    for (struct token *i = list->first; i != NULL; i = i->next->next->next) {
        if (i->opcode == OP_Stmt_sep) {
            if (i->next == NULL) {
                *error = true;
                break;
            }

            i = i->next;
        }

        if (i->type != T_String ||
            (i->next == NULL || i->next->opcode != OP_Colon || i->next->next == NULL) ||
            !isValidJsonVal(i->next->next) ||
            (i->next->next->next != NULL && i->next->next->next->opcode != OP_Stmt_sep)) {
            *error = true;
            break;
        }

        elementTablePush(&table, getJsonEl(i->value, i->next->next, error));
    }

    return table;
}


static struct result *jsonParse(char *str, bool *error)
{
    struct token_list *list = preprocessJson(str);
    struct result *result = malloc_(sizeof(struct result));

    if (list == NULL || list->first == NULL ||
        !isValidJsonVal(list->first) || list->first->next != NULL) {
        *result = (struct result) {0};
        goto end;
    }

    *result = (struct result) {
        .type = list->first->type,
    };

    switch (list->first->type) {
        case T_Table:
            result->value.values = parseObjJson(list->first->body, error);
            break;
        case T_String:
            result->value.string = strDup(list->first->value);
            break;
        case T_Number:
            result->value.number = strToD(list->first->value);
            break;
        case T_Null:
            result->value.string = NULL;
            break;
        case T_Identifier:
            result->value.number = !strcmp(list->first->value, "true");
            break;
        default: break;
    }

    end:
        listFreeR(list);
        return result;
}


void stdJsonParse(struct result_list *args)
{
    char *str = getValueStr(args->first);
    bool error = false;
    struct result *result = jsonParse(str, &error);

    if (error) {
        statePushResultNull();
        resultFree(result);
    } else {
        statePushResult(state->stack, result);
    }

    free(str);
}


void stdIsvalid(struct result_list *args)
{
    char *str = getValueStr(args->first);
    bool error = false;
    struct result *result = jsonParse(str, &error);
    statePushResultD(!error);
    resultFree(result);
    free(str);
}
