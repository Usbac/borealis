#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../utils.h"
#include "../state.h"
#include "../error.h"
#include "../stdlib.h"
#include "../stdlib/string.h"
#include "lexer.h"
#include "parser.h"
#include "parser_bytecode.h"
#include "processor_helper.h"
#include "preprocessor.h"
#include "processor.h"


static void processJmpSection(struct token **node)
{
    struct token *end = (*node);
    size_t i = 0;

    while (end != NULL && i++ < (*node)->jmp) {
        end = end->next;
    }

    *node = (*node)->next;
    while (*node != NULL) {
        evalNode(node);
        if (*node == end || !stateCanContinue()) {
            break;
        }

        *node = (*node)->next;
    }
}


static void evalOperator(struct token *node)
{
    struct result *a = statePopResult();
    struct result *b = statePopResult();
    statePushResultD(getOpResult(node->opcode, getValueD(b), getValueD(a)));

    resultFree(a);
    resultFree(b);
}


static void evalIdentifier(struct token *node)
{
    statePushResultEl(stateGetElement(node->value, state->file));
    state->stack->last->value.string = node->value;
}


static void evalAnonFunc(struct token **node)
{
    struct function *func;

    if ((*node)->next->type == T_Identifier) {
        while (*node != NULL && (*node)->next != NULL &&
            (*node)->next->opcode != OP_Stmt_sep) {
            *node = (*node)->next;
        }

        return;
    }

    func = funcInit();
    func->def_file = strDup(state->file);
    func->params = listDup((*node)->next->body);
    func->stmts = listDup((*node)->next->next->body);
    func->params_n = getParamsNumber(func->params);
    func->return_type = T_Null;

    statePushResultFunc(func);

    *node = (*node)->next->next;
}


static void evalDefinition(struct token **node)
{
    struct result *identifier;
    struct element *el;
    bool redeclared_out, redeclared_obj;
    char *key;

    if (isFunctionNode(*node)) {
        evalAnonFunc(node);
        return;
    }

    evalNode(&(*node)->next);
    identifier = statePopResultSafe();
    key = identifier->value.string;

    redeclared_out = stateGetElement(key, state->file) != NULL;
    redeclared_obj = elementGet(key, state->current_obj, NULL) != NULL;
    if (redeclared_obj || (state->current_obj == NULL && redeclared_out)) {
        errorF(identifier->line_n, E_REDECLARE, key);
    }

    el = elementInit(key, state->file, state->scope, T_Null);
    el->static_type = getOpcodeType((*node)->opcode);
    stateDeclareElement(&el);

    resultFree(identifier);
}


static void evalAssignation(struct token **node)
{
    struct result *identifier = statePopResult();
    struct result *val;
    struct element *el = getValueEl(identifier);

    processJmpSection(node);
    val = statePopResult();

    validateElementMod(identifier, getResultType(val));

    if (el != getValueEl(val)) {
        mapResultToElement(el, val);
        statePushResultEl(el);
    }

    resultFree(val);
    resultFree(identifier);
}


static void evalOpEqual(struct token *node)
{
    struct result *val = statePopResult();
    struct result *identifier = statePopResult();
    struct element *el = getValueEl(identifier);
    double value;

    validateElementMod(identifier, T_Number);

    value = getOpResult(node->opcode, getValueD(identifier), getValueD(val));

    elementFreeValues(&el);
    el->value.number = value;
    el->type = T_Number;

    statePushResultD(value);

    resultFree(val);
    resultFree(identifier);
}


static void evalOpConcat(void)
{
    struct result *val = statePopResult();
    struct result *identifier = statePopResult();
    struct element *el = getValueEl(identifier);
    char *el_val, *new_str;

    validateElementMod(identifier, T_String);

    el_val = getUnionStr(el->type, el->value);
    new_str = getValueStr(val);
    elementFreeValues(&el);
    el->value.string = strJoin(el_val, new_str);
    el->type = T_String;

    statePushResultStr(strDup(el->value.string));

    free(new_str);
    free(el_val);
    resultFree(val);
    resultFree(identifier);
}


static void evalSign(struct token *node)
{
    struct result *val = statePopResult();
    double result = getValueD(val);
    statePushResultD(node->opcode == OP_Positive ? +result : -result);
    resultFree(val);
}


static void evalCompString(struct token *node)
{
    struct result *a = statePopResult();
    struct result *b = statePopResult();
    char *a_str = getValueStr(a);
    char *b_str = getValueStr(b);

    statePushResultD((node->opcode == OP_Compare_string) == !strcmp(a_str, b_str));

    free(a_str);
    free(b_str);
    resultFree(a);
    resultFree(b);
}


static void evalCompStrict(struct token *node)
{
    struct result *a = statePopResult();
    struct result *b = statePopResult();

    statePushResultD((node->opcode == OP_Compare_strict) == compareResults(a, b));

    resultFree(a);
    resultFree(b);
}


static void evalAnd(struct token **node)
{
    struct result *a = statePopResult(), *b;

    if (!isTrue(a)) {
        statePushResultD(false);
        jmpStmts(node);
    } else {
        processJmpSection(node);

        b = statePopResult();
        statePushResultD(isTrue(a) && isTrue(b));
        resultFree(b);
    }

    resultFree(a);
}


static void evalOr(struct token **node)
{
    struct result *a = statePopResult(), *b;

    if (isTrue(a)) {
        statePushResultD(true);
        jmpStmts(node);
    } else {
        processJmpSection(node);

        b = statePopResult();
        statePushResultD(isTrue(b));
        resultFree(b);
    }

    resultFree(a);
}


/**
 * Used in conjunction with continue, break and return statements.
 */
static void evalBodylessIf(struct token **node)
{
    *node = (*node)->next;
    while (*node != NULL && (*node)->opcode != OP_Stmt_sep) {
        struct result *cond = NULL;
        if ((*node)->opcode == OP_Continue || (*node)->opcode == OP_Break ||
            (*node)->opcode == OP_Return) {
            cond = statePopResult();
        }

        if (cond == NULL || isTrue(cond)) {
            evalNode(node);
        }

        resultFree(cond);
        *node = (*node)->next;
    }
}


static void evalIf(struct token **node)
{
    struct result *cond = statePopResult();
    bool evaluated = false;

    if (cond == NULL) {
        evalBodylessIf(node);
        return;
    }

    while (*node != NULL && (*node)->opcode != OP_Stmt_sep) {
        if (evaluated) {
            goto next;
        }

        if ((*node)->opcode == OP_If) {
            if (cond == NULL) {
                cond = statePopResult();
            }

            *node = (*node)->next;

            if (isTrue(cond)) {
                evalNode(node);
                evaluated = true;
            }

            resultFree(cond);
            cond = NULL;
        } else {
            evalNode(node);
        }

        next: *node = (*node)->next;
    }
}


static void evalWhile(struct token **node)
{
    struct result *cond = statePopResult();
    struct token *start = *node;

    /* Empty body while */
    if (cond == NULL) {
        do {
            resultFree(cond);
            *node = (*node)->next;
            while (*node != NULL && (*node)->opcode != OP_Stmt_sep) {
                evalNode(node);
                *node = (*node)->next;
            }

            cond = statePopResult();
            *node = start;
        } while (cond != NULL && isTrue(cond));
        goto end;
    } else if (!isTrue(cond)) {
        *node = (*node)->next;
        goto end;
    }

    while (start->prev != NULL && start->opcode != OP_Stmt_sep) {
        start = start->prev;
    }

    state->scope++;
    statePushContext(C_Loop);
    *node = (*node)->next;
    evalBytecode((*node)->body);
    stateElementsFree(state->scope--);
    statePopContext();

    if (!state->breaking_loop) {
        *node = start;
        evalNode(node);
    }

    state->breaking_loop = false;
    state->continuing_loop = false;

    end:
    resultFree(cond);
}


static bool evalCaseCond(struct token **ite, struct result *val)
{
    struct result *ite_val = statePopResult();
    bool equals = compareResults(val, ite_val);
    resultFree(ite_val);

    if (!equals) {
        jmpStmts(ite);
        return false;
    }

    state->scope++;
    while (*ite != NULL && (*ite)->opcode != OP_Stmt_sep) {
        evalNode(ite);
        *ite = (*ite)->next;
    }

    stateElementsFree(state->scope--);
    return true;
}


static void evalCase(struct token **node)
{
    struct result *val = statePopResult();

    for (struct token *i = (*node)->next->body->first; i != NULL; i = i->next) {
        if (i->opcode == OP_Colon) {
            evalCaseCond(&i, val);
        } else if (i->type != T_Chunk) {
            evalNode(&i);
        }
    }

    *node = (*node)->next;
    resultFree(val);
}


static void evalForeachIterations(struct token **node,
                                  struct element_table *arr,
                                  struct result *key,
                                  struct result *val)
{
    struct token *code = (*node)->next;
    *node = code;

    for (struct element_list *i = arr != NULL ? arr->first : NULL;
        i != NULL && !state->breaking_loop;
        i = i->next) {
        struct element *val_el;

        if (!i->ptr->public) {
            continue;
        }

        if (key != NULL) {
            struct element *key_el;

            if (stateGetElement(i->ptr->key, state->file) != NULL) {
                errorF((*node)->line_n, E_REDECLARE, i->ptr->key);
            }

            key_el = elementInit(key->value.string, state->file, state->scope, T_String);
            key_el->value.string = strDup(i->ptr->key);
            stateDeclareElement(&key_el);
        }

        if (stateGetElement(val->value.string, state->file) != NULL) {
            errorF((*node)->line_n, E_REDECLARE, val->value.string);
        }

        val_el = elementInit(val->value.string, state->file, state->scope, T_Null);
        elementDupValues(&val_el, i->ptr);
        stateDeclareElement(&val_el);

        evalBytecode(code->body);
        stateElementsFree(state->scope);
    }
}


static void evalForeach(struct token **node)
{
    struct result *val, *ite_key = NULL, *ite_val;
    struct element_table *arr;

    *node = (*node)->next;
    while ((*node)->opcode != OP_Comma) {
        evalNode(node);
        *node = (*node)->next;
    }

    ite_val = statePopResultSafe();
    if ((*node)->prev->prev->opcode == OP_Colon) {
        ite_key = statePopResultSafe();
    }

    val = statePopResult();
    arr = getValueArr(val);

    if (arr == NULL) {
        errorF((*node)->line_n, E_INVALID_ITERATOR);
    }

    state->scope++;
    statePushContext(C_Loop);
    evalForeachIterations(node, arr, ite_key, ite_val);
    state->breaking_loop = false;
    state->scope--;
    statePopContext();

    resultFree(val);
    resultFree(ite_key);
    resultFree(ite_val);
}


static void evalBreak(struct token **node)
{
    if (!stateIsInsideContext(C_Loop)) {
        errorF((*node)->line_n, E_LOOP, "break");
    }

    state->breaking_loop = true;
}


static void evalContinue(struct token **node)
{
    if (!stateIsInsideContext(C_Loop)) {
        errorF((*node)->line_n, E_LOOP, "continue");
    }

    state->continuing_loop = true;
}


static void evalReturn(struct token *node)
{
    if (isEmptyReturn(node)) {
        statePushResultNull();
    }

    state->returning = true;
}


static void evalObject(struct token **node)
{
    struct element_table *aux_current_obj = state->current_obj;
    struct result_list *aux_stack = stateResultListDup(state->stack);
    struct element_table *obj = elementTableInit();

    resultListFree(state->stack);
    state->stack = resultListInit();
    state->current_obj = obj;

    evalBytecode((*node)->next->body);

    resultListFree(state->stack);
    state->stack = aux_stack;
    state->current_obj = aux_current_obj;

    statePushResultObj(obj);
    *node = (*node)->next;
}


static void evalImport(void)
{
    struct result *returned = NULL, *val = statePopResult();
    char *folder = strDup(state->file);
    char *slash = strrchr(folder, '/');
    char *module = getValueStr(val);
    char *file;
    struct state *aux_state;
    bool imported_file = false;
    struct element *el;

    if (slash != NULL) {
        *slash = '\0';
    }

    file = isAbsolutePath(module) ?
        strDup(module) :
        strJoinPaths(folder, module);

    if (!strcmp(file, state->file)) {
        errorF(val->line_n, E_RECURSIVE_FILE, file);
    } else if (!isFilePath(file)) {
        errorF(val->line_n, E_READ_FILE, file);
    }

    statePushContext(C_Import);
    aux_state = saveState(NULL, file);

    if (!(imported_file = stateIsFileImported(file))) {
        processFile(file, val->line_n);

        if (state->returning) {
            returned = statePopResult();
        }
    } else {
        el = stateGetElement(file, NULL);
    }

    restoreState(aux_state);
    statePopContext();

    if (returned != NULL) {
        el = elementInit(file, NULL, 0, T_Null);
        mapResultToElement(el, returned);
        elementTablePush(&elements, el);
        statePushResultRef(el);
        resultFree(returned);
    } else if (!imported_file) {
        statePushResultNull();
    } else {
        statePushResultRef(el);
    }

    free(file);
    free(folder);
    free(module);
    resultFree(val);
}


static struct element *getVariadicArg(const char *key, struct result *arg)
{
    struct element *el = elementInit(key, state->file, state->scope, T_Array);
    size_t i = 0;

    while (arg != NULL) {
        char *index = strFromSizet(i++);
        struct element *prop = elementInit(index, NULL, 0, T_Null);
        prop->public = true;
        mapResultToElement(prop, arg);
        elementTablePush(&el->value.values, prop);

        free(index);

        arg = arg->next;
    }

    return el;
}


static void mapDefaultArgs(struct element_table **table, struct token **param)
{
    while (*param != NULL) {
        if ((*param)->type == T_Identifier) {
            struct element *arg;

            if ((*param)->next != NULL && (*param)->next->opcode == OP_Spread) {
                break;
            }

            arg = elementInit((*param)->value, state->file, state->scope, T_Null);
            statePushResultEl(arg);
            *param = (*param)->next;
            evalAssignation(param);
            elementTablePush(table, arg);
        }

        *param = (*param)->next;
    }
}


static void mapArg(struct element_table **table,
                   struct token **ite_params,
                   struct result *value)
{
    struct element *arg;
    enum TYPE type = getOpcodeType((*ite_params)->opcode);
    char *key;

    if (isDefKeyword((*ite_params)->opcode)) {
        key = (*ite_params)->next->value;
        if (getResultType(value) != type) {
            errorF((*ite_params)->line_n,
                E_FUNC_PARAM_STATIC,
                key,
                getElementTypeAsStr(type));
        }
    } else {
        key = (*ite_params)->value;
    }

    arg = elementInit(key, state->file, state->scope, T_Null);
    arg->static_type = type;
    mapResultToElement(arg, value);
    elementTablePush(table, arg);

    while (*ite_params != NULL) {
        *ite_params = (*ite_params)->next;

        if (*ite_params != NULL && (*ite_params)->opcode == OP_Stmt_sep) {
            *ite_params = (*ite_params)->next;
            break;
        }
    }
}


static struct result *getFuncResult(struct function *func,
                                    const char *calling_file)
{
    struct result *returned = statePopResult();
    enum TYPE returned_type = getResultType(returned);
    struct result *result = malloc_(sizeof(struct result));
    struct element *el = elementInit(NULL, calling_file, state->scope, T_Null);

    if (returned != NULL) {
        mapResultToElement(el, returned);
        resultFree(returned);
    }

    if (func->return_type != T_Null && func->return_type != returned_type) {
        errorF(state->line_n,
            E_FUNC_RETURN,
            getElementTypeAsStr(func->return_type),
            getElementTypeAsStr(returned_type));
    }

    elementTablePush(state->callstack->prev != NULL ?
                     &state->callstack->prev->elements :
                     &elements,
                     el);

    *result = (struct result) {
        .type = T_Identifier,
        .p_el = el,
        .line_n = state->line_n,
    };

    return result;
}


static void evalReference(void)
{
    struct result *val = statePopResult();

    if (val->p_el == NULL) {
        errorF(val->line_n, E_NULL_REFERENCE);
    }

    statePushResultRef(val->p_el);
    resultFree(val);
}


static void evalUnaryOp(struct token *node)
{
    struct result *identifier = statePopResult();
    struct element *el = getValueEl(identifier);
    double value, result;

    validateElementMod(identifier, T_Number);

    value = getValueD(identifier);
    switch (node->opcode) {
        case OP_Decrement_pre: result = --value; break;
        case OP_Decrement_pos: result = value--; break;
        case OP_Increment_pre: result = ++value; break;
        case OP_Increment_pos: result = value++; break;
        default: result = value; break;
    }

    statePushResultD(result);
    elementFreeValues(&el);
    el->value.number = value;
    el->type = T_Number;

    resultFree(identifier);
}


static void evalElementModifier(struct token *node)
{
    struct result *val = statePopResult();

    switch (node->opcode) {
        case OP_Public: val->p_el->public = true; break;
        case OP_Constant: val->p_el->constant = true; break;
        default: break;
    }

    statePushResultEl(val->p_el);
    resultFree(val);
}


static void evalConcat(void)
{
    struct result *a = statePopResult();
    struct result *b = statePopResult();
    char *a_str = getValueStr(a);
    char *b_str = getValueStr(b);
    statePushResultStr(strJoin(b_str, a_str));

    free(a_str);
    free(b_str);
    resultFree(b);
    resultFree(a);
}


static void evalNegation(void)
{
    struct result *val = statePopResult();
    statePushResultD(!isTrue(val));
    resultFree(val);
}


static void evalArrayProp(struct element_table *arr)
{
    struct result *val = statePopResult();
    char *key = strFromSizet(arr->next_index);
    struct element *prop = elementInit(key, NULL, 0, T_Null);
    prop->public = true;
    mapResultToElement(prop, val);
    elementTablePush(&arr, prop);

    if (val->p_el != NULL && val->p_el->unset) {
        elementFree(&val->p_el);
        free(val->p_el);
    }

    resultFree(val);
    free(key);
}


static void evalArrayAssocProp(struct element_table *arr, struct token **node)
{
    struct result *val, *key = statePopResult();
    struct element *existing = NULL;
    char *key_str = getValueStr(key);

    processJmpSection(node);
    val = statePopResult();

    existing = elementGet(key_str, arr, NULL);
    if (existing != NULL) {
        mapResultToElement(existing, val);
    } else {
        struct element *prop = elementInit(key_str, NULL, 0, T_Null);
        mapResultToElement(prop, val);
        prop->public = true;
        elementTablePush(&arr, prop);
    }

    if (val->p_el != NULL && val->p_el->unset) {
        elementFree(&val->p_el);
        free(val->p_el);
    }

    *node = (*node)->next;

    resultFree(key);
    resultFree(val);
    free(key_str);
}


static void evalArraySpread(struct element_table *arr, struct token **node)
{
    struct result *sub_arr_val = statePopResult();
    struct element_table *sub_arr = getValueArr(sub_arr_val);

    for (struct element_list *i = sub_arr->first; i != NULL; i = i->next) {
        if (!i->ptr->unset) {
            struct element *el = elementGet(i->ptr->key, arr, NULL);
            if (el != NULL) {
                elementFreeValues(&el);
                elementDupValues(&el, i->ptr);
            } else {
                el = elementInit(i->ptr->key, NULL, 0, T_Null);
                elementDupValues(&el, i->ptr);
                elementTablePush(&arr, el);
            }
        }
    }

    resultFree(sub_arr_val);
    *node = (*node)->next;
}


static void evalArray(struct token **node)
{
    struct result_list *aux_stack = stateResultListDup(state->stack);
    struct element_table *arr = elementTableInit();

    resultListFree(state->stack);
    state->stack = resultListInit();

    for (struct token *i = (*node)->body->first; i != NULL; i = i->next) {
        if (i->opcode == OP_Stmt_sep) {
            evalArrayProp(arr);
        } else if (i->opcode == OP_Assignation) {
            evalArrayAssocProp(arr, &i);
        } else if (i->opcode == OP_Spread) {
            evalArraySpread(arr, &i);
        } else {
            evalNode(&i);
        }
    }

    resultListFree(state->stack);
    state->stack = aux_stack;
    statePushResultArr(arr);
}


static void evalNullcoalesce(struct token **node)
{
    struct result *a, *b, *result;
    state->null_coalescing = true;

    processJmpSection(node);

    a = statePopResultSafe();
    b = statePopResult();
    result = getResultType(a) != T_Null ? a : b;
    statePushResult(state->stack, result);

    resultFree(a == result ? b : a);
}


static void evalThis(struct token *node)
{
    if (state->current_obj == NULL) {
        errorF(node->line_n, E_THIS);
    }

    statePushResultObj(state->current_obj);
}


static void evalIndexChunk(struct token_list *list)
{
    for (struct token *i = list->first; i != NULL; i = i->next) {
        if (i->opcode != OP_Stmt_sep) {
            evalNode(&i);
        } else if (i->next != NULL) {
            errorF(i->line_n, E_INDEX_MULTI_STMT);
        } else {
            break;
        }
    }
}


static void evalIndex(struct token *node)
{
    struct result *val, *index;
    struct element *prop;
    struct element_table *el;
    enum TYPE type;
    char *key;

    evalIndexChunk(node->body);

    if (node->body->first == NULL) {
        statePushResultD(0);
    }

    index = statePopResult();
    val = statePopResult();
    type = getResultType(val);
    el = type == T_Array ?
        getValueArr(val) :
        type == T_Object ?
        getValueObj(val) :
        NULL;

    if (node->body->first == NULL) {
        if (type != T_Array) {
            errorF(node->line_n, E_PUSH_ELEMENT);
        }

        index->value.number = (double) el->next_index;
    }

    if (type == T_String) {
        struct result_list *args = resultListInit();
        statePushResult(args, val);
        statePushResult(args, index);
        stdGetChar(args);
        free(args);
        goto end;
    }

    if (el == NULL && state->null_coalescing) {
        statePushResultNull();
        goto end;
    } else if (el == NULL || (type != T_Array && type != T_Object)) {
        errorF(node->line_n, E_INDEX, getElementTypeAsStr(type));
    }

    key = getValueStr(index);
    prop = elementGet(key, el, NULL);

    if (prop == NULL && node->next->opcode == OP_Assignation) {
        prop = elementInit(key, NULL, 0, T_Null);
        prop->public = type == T_Array || state->current_obj != el;
        elementTablePush(&el, prop);
        statePushResultEl(prop);
    } else if (prop == NULL) {
        statePushResultNull();
    } else {
        if ((val->type == T_Array || val->type == T_Object) &&
            val->value.values != state->current_obj) {
            statePushResult(state->stack, getResultFromElement(prop));
        } else {
            statePushResultEl(prop);
        }
    }

    free(key);
    end:
    resultFree(index);
    resultFree(val);
}


static void evalChunk(struct token *node)
{
    state->scope++;
    evalBytecode(node->body);
    stateElementsFree(state->scope--);
}


static struct element_table *getArgsTable(struct function *func,
                                          struct result_list *args)
{
    struct element_table *table = elementTableInit();
    struct result *arg = args != NULL ? args->first : NULL;
    struct token *param = func->params != NULL ? func->params->first : NULL;

    while (arg != NULL && param != NULL &&
        !(param->next != NULL && param->next->opcode == OP_Spread)) {
        mapArg(&table, &param, arg);
        arg = arg->next;
    }

    if (!inSpreadParam(param)) {
        if (param != NULL && isDefKeyword(param->opcode)) {
            param = param->next;
        }

        if (arg == NULL && param != NULL && param->type == T_Identifier &&
            param->next != NULL && param->next->opcode != OP_Assignation) {
            errorF(state->line_n, E_FUNC_PARAM_NUM, func->params_n);
        }

        mapDefaultArgs(&table, &param);

        if ((arg == NULL && param != NULL) || (arg != NULL && param == NULL)) {
            errorF(state->line_n, E_FUNC_PARAM_NUM, func->params_n);
        }
    } else {
        elementTablePush(&table, getVariadicArg(param->value, arg));
    }

    return table;
}


void funcExec(struct function *func,
              struct result_list *args,
              struct element_table *obj)
{
    struct state *aux_state;
    struct result *result;

    if (func == NULL) {
        errorF(state->line_n, E_NON_CALLABLE);
    } else if (func->native_code) {
        stdlibFuncCall(func, args);
        return;
    }

    aux_state = saveState(obj, func->def_file);

    statePushContext(C_Function);
    statePushCallstack(getArgsTable(func, args), obj);
    evalBytecode(func->stmts);
    result = getFuncResult(func, aux_state->file);
    statePopContext();
    statePopCallstack();
    restoreState(aux_state);

    statePushResult(state->stack, result);
}


static struct result_list *getArgsResultList(struct token_list *tokens)
{
    struct result_list *args = resultListInit();

    for (struct token *i = tokens->first; i != NULL; i = i->next) {
        if (i->opcode == OP_Spread) {
            struct result *arr_value = statePopResult();
            struct element_table *arr = getValueArr(arr_value);
            enum TYPE type = getResultType(arr_value);
            struct element_list *arr_ite;

            if (type != T_Array) {
                errorF(state->line_n, E_SPREAD, getElementTypeAsStr(type));
            }

            arr_ite = arr->first;
            while (arr_ite != NULL) {
                if (!arr_ite->ptr->unset) {
                    statePushResult(args, getResultFromElement(arr_ite->ptr));
                }

                arr_ite = arr_ite->next;
            }

            i = i->next;
            resultFree(arr_value);
        } else if (i->opcode != OP_Stmt_sep) {
            evalNode(&i);
        } else {
            statePushResult(args, statePopResult());
        }
    }

    return args;
}


static void evalFuncCall(struct token *node, struct element_table *obj)
{
    struct result *result = statePopResult();
    struct function *func = getValueFunc(result);
    struct result_list *args = getArgsResultList(node->body);
    funcExec(func, args, obj);

    resultFree(result);
    resultListFree(args);
}


static void evalProp(struct token **node)
{
    struct result *prop = statePopResultSafe();
    struct result *obj_val = statePopResultSafe();
    enum TYPE obj_type = getResultType(obj_val);
    char *prop_key = strDup(prop->value.string);
    struct element_table *obj = getValueObj(obj_val);
    struct element *el;
    bool safe = (*node)->opcode == OP_Dot_safe;

    if (obj_type != T_Object) {
        if (obj_type == T_Null && (state->null_coalescing || safe)) {
            statePushResultNull();
            goto end;
        }

        errorF(obj_val->line_n, E_NON_OBJECT, getElementTypeAsStr(obj_type));
    }

    el = elementGet(prop_key, obj, NULL);

    if (el == NULL && (*node)->next->opcode == OP_Assignation) {
        el = elementInit(prop_key, NULL, 0, T_Null);
        el->public = state->current_obj != obj;
        elementTablePush(&obj, el);
        statePushResultEl(el);
    } else if (el == NULL) {
        statePushResultNull();
    } else if (!el->public && state->current_obj != obj) {
        if (safe) {
            statePushResultNull();
        } else {
            errorF((*node)->line_n, E_PUBLIC, prop->value);
        }
    } else {
        if (obj_val->type == T_Object && obj_val->value.values != state->current_obj) {
            statePushResult(state->stack, getResultFromElement(el));
        } else {
            statePushResultEl(el);
        }
    }

    if ((*node)->next != NULL && (*node)->next->type == T_Arguments) {
        evalFuncCall((*node)->next, obj);
        *node = (*node)->next;
    }

    end:
        resultFree(obj_val);
        free(prop_key);
        resultFree(prop);
}


static void evalBang(void)
{
    struct result *func_value = statePopResult();
    struct result *element_value = statePopResult();
    struct function *func = getValueFunc(func_value);
    struct result_list *args = resultListInit();
    struct element *element = getValueEl(element_value);
    struct result *result;

    statePushResult(args, element_value);

    funcExec(func, args, state->current_obj);
    result = statePopResult();
    validateElementMod(element_value, getResultType(result));
    mapResultToElement(element, result);
    statePushResultEl(element);

    resultListFree(args);
    resultFree(func_value);
    resultFree(result);
}


void evalNode(struct token **node)
{
    state->line_n = (*node)->line_n;

    if ((*node)->opcode == OP_Stmt_sep) {
        if (!state->in_repl) {
            resultFree(statePopResult());
        } else {
            state->last_val = statePopResult();
        }

        return;
    }

    switch ((*node)->type) {
        case T_Arguments: evalFuncCall(*node, state->current_obj); break;
        case T_Index: evalIndex(*node); break;
        case T_Array: evalArray(node); break;
        case T_Identifier: evalIdentifier(*node); break;
        case T_Chunk: evalChunk(*node); break;
        case T_Number:
            statePushResultD(strToD((*node)->value)); break;
        case T_Null:
            statePushResultNull(); break;
        case T_String:
            statePushResultStr(strDup((*node)->value)); break;
        case T_Keyword:
            if (isDefKeyword((*node)->opcode)) {
                evalDefinition(node);
                break;
            }

            switch ((*node)->opcode) {
                case OP_Closure: evalAnonFunc(node); break;
                case OP_Return: evalReturn(*node); break;
                case OP_If: evalIf(node); break;
                case OP_While: evalWhile(node); break;
                case OP_Case: evalCase(node); break;
                case OP_Foreach: evalForeach(node); break;
                case OP_Break: evalBreak(node); break;
                case OP_Continue: evalContinue(node); break;
                case OP_Object: evalObject(node); break;
                case OP_Import: evalImport(); break;
                case OP_This: evalThis(*node); break;
                case OP_Constant:
                case OP_Public: evalElementModifier(*node); break;
                default: break;
            }
            break;
        case T_Operator:
            switch ((*node)->opcode) {
                case OP_Concat: evalConcat(); break;
                case OP_Negation: evalNegation(); break;
                case OP_Assignation: evalAssignation(node); break;
                case OP_And: evalAnd(node); break;
                case OP_Or: evalOr(node); break;
                case OP_Null_coalesce: evalNullcoalesce(node); break;
                case OP_Plus_equal:
                case OP_Minus_equal:
                case OP_Div_equal:
                case OP_Multi_equal:
                case OP_Mod_equal: evalOpEqual(*node); break;
                case OP_Concat_equal: evalOpConcat(); break;
                case OP_Negative:
                case OP_Positive: evalSign(*node); break;
                case OP_Compare_string:
                case OP_Different_string: evalCompString(*node); break;
                case OP_Compare_strict:
                case OP_Different_strict: evalCompStrict(*node); break;
                case OP_Dot:
                case OP_Dot_safe: evalProp(node); break;
                case OP_Reference: evalReference(); break;
                case OP_Decrement_pre:
                case OP_Decrement_pos:
                case OP_Increment_pre:
                case OP_Increment_pos: evalUnaryOp(*node); break;
                case OP_Comma:
                case OP_Colon: break;
                case OP_Bang: evalBang(); break;
                default: evalOperator(*node);
            }
            break;
        default: break;
    }
}


void evalBytecode(struct token_list *list)
{
    for (struct token *node = list != NULL ? list->first : NULL;
        node != NULL && stateCanContinue();
        node = node->next) {
        evalNode(&node);
    }
}


void processFile(const char *file, size_t line_n)
{
    struct token_list *stmts, *bytecode;
    char *code = readF(file, false);

    if (code == NULL) {
        errorF(line_n, E_READ_FILE, file);
    }

    stateAddFileImport(file);
    stmts = codeToList(code, SEPARATOR_DEFAULT, true, 1);
    preprocess(stmts, file);
    bytecode = bytecodeFromList(stmts);
    evalBytecode(bytecode);

    listFreeR(stmts);
    listFreeR(bytecode);
    free(code);
}
