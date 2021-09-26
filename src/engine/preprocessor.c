#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser_bytecode.h"
#include "processor_helper.h"
#include "../state.h"
#include "../error.h"
#include "preprocessor.h"


static bool isExpression(const struct token *node)
{
    return ((node->type == T_Operator && node->opcode != OP_Comma) ||
        (node->type == T_String || node->type == T_Number ||
        node->type == T_Identifier || node->type == T_Array ||
        node->type == T_Arguments || node->type == T_Index ||
        node->type == T_Null || node->opcode == OP_Import ||
        node->opcode == OP_Closure || node->opcode == OP_Null_coalesce ||
        node->opcode == OP_This || node->opcode == OP_Object));
}


static bool isElement(const struct token *node)
{
    return node->type == T_Identifier || node->type == T_Index ||
        node->opcode == OP_Dot || node->opcode == OP_Dot_safe ||
        node->opcode == OP_Bang;
}


static bool isPossibleFunc(const struct token *node)
{
    return isElement(node) || node->type == T_Arguments ||
        node->opcode == OP_Import || node->opcode == OP_Closure ||
        node->opcode == OP_Null_coalesce;
}

static void prevalidateFuncDefinition(struct token *node,
                                      struct pre_state *pre_state)
{
    if (node->ls == NULL || node->rs == NULL ||
        node->ls->type != T_Chunk || node->rs->type != T_Parameters ||
        (node->rs->ls != NULL && node->rs->ls->type != T_Identifier)) {
        errorF(node->line_n, E_FUNC_DEFINITION);
    } else if (node->rs->ls != NULL &&
            stateElementGet(node->rs->ls->value, pre_state->file) != NULL) {
        errorF(node->line_n, E_REDECLARE, node->rs->ls->value);
    }
}


static bool isFunction(struct token *node)
{
    return isDefKeyword(node->opcode) &&
        node->rs != NULL && node->rs->type == T_Parameters &&
        node->rs->ls != NULL && node->rs->ls->type == T_Identifier;
}


static void prevalidateDefinition(struct token *node)
{
    if (isFunction(node)) {
        return;
    }

    if (node->ls == NULL || node->rs != NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->ls->opcode != OP_Assignation && node->ls->type != T_Identifier) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateConstant(struct token *node)
{
    if (node->ls == NULL || node->rs != NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    }

    prevalidateDefinition(node->ls);
}


static void prevalidateAssignation(struct token *node)
{
    if (node->rs == NULL || node->ls == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->rs->type != T_Identifier && node->rs->type != T_String &&
        node->rs->type != T_Number && node->rs->type != T_Index &&
        node->rs->opcode != OP_Dot && node->rs->opcode != OP_Assignation &&
        node->rs->opcode != OP_Dot_safe) {
        errorF(node->rs->line_n, E_TOKEN, node->rs->value);
    } else if (!isExpression(node->ls)) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateReturn(struct token *node)
{
    if (node->ls != NULL && !isExpression(node->ls) &&
        !isOperator(node->ls->opcode) && node->ls->opcode != OP_If) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static bool isControlFlowStmt(struct token *node)
{
    return node->opcode ==  OP_If || node->opcode == OP_Else ||
        node->opcode == OP_While || node->opcode == OP_Foreach;
}


static void prevalidateCase(struct token *node)
{
    if (node->ls == NULL || node->rs == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (!isExpression(node->rs)) {
        errorF(node->rs->line_n, E_TOKEN, node->rs->value);
    }

    for (struct token *i = node->ls->body->first; i != NULL; i = i->next) {
        if (i->opcode != OP_Colon || i->ls == NULL) {
            errorF(i->line_n, E_TOKEN, i->value);
        } else if (isControlFlowStmt(i->ls) || isFunction(i->ls)) {
            errorF(i->ls->line_n, E_TOKEN, i->ls->value);
        }
    }
}


static void prevalidateWhileIf(struct token *node)
{
    if (node->ls == NULL || (node->ls->type == T_Chunk && node->rs == NULL)) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->rs != NULL && !isExpression(node->rs)) {
        errorF(node->rs->line_n, E_TOKEN, node->rs->value);
    }
}


static void prevalidateElse(struct token *node)
{
    if (node->prev == NULL || node->ls == NULL ||
        (node->prev->opcode != OP_If && node->prev->opcode != OP_Else)) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->prev->opcode == OP_Else && node->prev->ls->opcode != OP_If) {
        errorF(node->prev->ls->line_n, E_TOKEN, node->prev->ls->value);
    } else if (node->ls->type != T_Chunk && node->ls->opcode != OP_If) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateOpEqual(struct token *node)
{
    if (node->rs == NULL ||
        (node->rs->type != T_Identifier &&
        node->rs->type != T_Index)) {
        errorF(node->line_n, E_TOKEN, node->value);
    }
}


static bool isPosibleObj(struct token *node)
{
    return node->opcode == OP_Object || node->type == T_Identifier ||
        node->type == T_Arguments || node->type == T_Index ||
        node->opcode == OP_Dot || node->opcode == OP_Dot_safe ||
        node->opcode == OP_This;
}


static void prevalidateDot(struct token *node)
{
    if (node->ls == NULL || node->rs == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->ls->type != T_Identifier) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    } else if (!isPosibleObj(node->rs)) {
        errorF(node->rs->line_n, E_TOKEN, node->rs->value);
    }
}


static void prevalidateReference(struct token *node)
{
    if (node->rs != NULL) {
        errorF(node->rs->line_n, E_TOKEN, node->rs->value);
    } else if (node->ls == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (!isElement(node->ls)) {
        errorF(node->ls->line_n, E_INVALID_REFERENCE);
    }
}


static void prevalidateForeach(struct token *node)
{
    struct token *foreach = node->rs;
    struct token *arr = foreach->rs;

    if (node->rs == NULL || node->ls == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (!isExpression(arr)) {
        errorF(arr->line_n, E_TOKEN, foreach->rs->value);
    }

    if (foreach->ls->opcode != OP_Colon) {
        return;
    }

    /* validate iteration with key and value */
    if (foreach->ls->ls == NULL || foreach->ls->rs == NULL) {
        errorF(foreach->ls->line_n, E_INVALID_LOOP_COND);
    } else if (foreach->ls->rs->type != T_Identifier) {
        errorF(foreach->ls->rs->line_n, E_TOKEN, foreach->ls->rs->value);
    } else if (foreach->ls->ls->type != T_Identifier) {
        errorF(foreach->ls->ls->line_n, E_TOKEN, foreach->ls->ls->value);
    }
}


static void prevalidateLoopStmt(struct token *node)
{
    if (node->ls != NULL && node->ls->opcode != OP_If) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateImport(struct token *node)
{
    if (node->ls == NULL || node->rs != NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (!isExpression(node->ls)) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidatePublic(struct token *node)
{
    if (node->ls == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->ls->opcode != OP_Constant && !isDefKeyword(node->ls->opcode)) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateNegation(struct token *node)
{
    if (!isExpression(node)) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateOp(struct token *node)
{
    if (node->ls == NULL || node->rs == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->opcode == OP_Colon) {
        if (!isExpression(node->rs) || node->rs->opcode == OP_Colon) {
            errorF(node->rs->line_n, E_TOKEN, node->rs->value);
        }

        return;
    } else if (!isExpression(node->ls) && node->ls->type != T_Operator) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    } else if ((!isExpression(node->rs) && node->rs->type != T_Operator)) {
        errorF(node->rs->line_n, E_TOKEN, node->rs->value);
    }
}


static void prevalidateIndex(struct token *node)
{
    if (node->ls == NULL) {
        errorF(node->line_n, E_INDEX_EXPRESSION);
    }
}


static void prevalidateSign(struct token *node)
{
    if (node->ls == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (!isExpression(node->ls) && node->ls->type != T_Operator) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateUnaryOp(struct token *node)
{
    if (node->ls == NULL || node->rs != NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->ls->type != T_Identifier && node->ls->type != T_Index &&
        node->ls->opcode != OP_Dot && node->ls->opcode != OP_Dot_safe) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateOptionalParam(struct token *node)
{
    prevalidateAssignation(node);

    if (node->next != NULL && node->next->opcode != OP_Assignation &&
        node->next->opcode != OP_Spread && !isDefKeyword(node->next->opcode)) {
        errorF(node->line_n, E_FUNC_OPTIONAL);
    }
}


static void prevalidateParams(struct token_list *params)
{
    struct token *node = params->first;
    while (node != NULL) {
        bool is_spread = node->opcode == OP_Spread;
        struct token *param = is_spread ? node->ls : node;

        if (node->opcode == OP_Assignation) {
            prevalidateOptionalParam(node);
        } else if (isDefKeyword(node->opcode) && node->opcode != OP_Definition) {
            prevalidateDefinition(node);
        } else if (param == NULL || param->type != T_Identifier ||
            param->ls != NULL || param->rs != NULL) {
            errorF(node->line_n, E_TOKEN, node->value);
        } else if (is_spread && node->next != NULL) {
            errorF(node->line_n, E_FUNC_VARIADIC);
        }

        node = node->next;
    }
}


static void declareFunc(struct token *node,
                        struct pre_state *pre_state,
                        bool public)
{
    enum TYPE aux_type = pre_state->current_type;
    struct element *func;

    prevalidateFuncDefinition(node, pre_state);
    pre_state->current_type = T_Chunk;
    preprocessTokenList(node->ls->body, pre_state);
    prevalidateParams(node->rs->body);

    func = elementInit(node->rs->ls != NULL ? node->rs->ls->value : NULL,
        pre_state->file,
        0,
        T_Function);
    func->value.function->def_file = strDup(pre_state->file);
    func->value.function->params = bytecodeFromList(node->rs->body);
    func->value.function->stmts = bytecodeFromList(node->ls->body);
    func->value.function->params_n = getParamsNumber(func->value.function->params);
    func->value.function->return_type = getOpcodeType(node->opcode);
    func->public = public;
    func->constant = true;

    stateElementDeclare(&func);

    pre_state->current_type = aux_type;
}


static void prevalidateClosure(struct token *node)
{
    if (node->ls == NULL || node->rs == NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (node->ls->type != T_Chunk) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    } else if (node->rs->type != T_Parameters) {
        errorF(node->rs->line_n, E_TOKEN, node->rs->value);
    }

    prevalidateParams(node->rs->body);
}


static void prevalidateBang(struct token *node)
{
    if (!isElement(node->rs)) {
        errorF(node->rs->line_n, E_TOKEN, node->rs->value);
    } else if (!isPossibleFunc(node->ls)) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static void prevalidateSpread(struct token *node)
{
    if (node->ls == NULL || node->rs != NULL) {
        errorF(node->line_n, E_TOKEN, node->value);
    } else if (!isElement(node->ls) && node->ls->type != T_Array) {
        errorF(node->ls->line_n, E_TOKEN, node->ls->value);
    }
}


static bool preprocessFuncDef(struct token *node,
                              struct pre_state *pre_state)
{
    bool is_public = node->opcode == OP_Public;

    if (node->opcode == OP_Closure) {
        prevalidateClosure(node);
        return true;
    } else if (is_public && node->ls != NULL && isFunction(node->ls)) {
        declareFunc(node->ls, pre_state, is_public);
        return true;
    } else if (isFunction(node)) {
        declareFunc(node, pre_state, false);
        return true;
    }

    return false;
}


static bool isStmt(struct token *node)
{
    return node->type == T_Chunk || node->opcode == OP_Case ||
        node->opcode == OP_Return || node->opcode == OP_Break ||
        node->opcode == OP_Continue || isControlFlowStmt(node) ||
        isFunction(node);
}


static bool preprocessObj(struct token *node)
{
    if (node->opcode != OP_Object) {
        return false;
    }

    if (node->rs != NULL || node->ls == NULL || node->ls->type != T_Chunk) {
        errorF(node->line_n, E_TOKEN, node->value);
    }

    return true;
}


static void prevalidateNode(struct token *node, struct pre_state *pre_state)
{
    if (isDefKeyword(node->opcode)) {
        prevalidateDefinition(node);
        return;
    }

    switch (node->opcode) {
        case OP_Constant: prevalidateConstant(node); break;
        case OP_Return: prevalidateReturn(node); break;
        case OP_Case: prevalidateCase(node); break;
        case OP_While:
        case OP_If: prevalidateWhileIf(node); break;
        case OP_Else: prevalidateElse(node); break;
        case OP_Foreach: prevalidateForeach(node); break;
        case OP_Break:
        case OP_Continue: prevalidateLoopStmt(node); break;
        case OP_Import: prevalidateImport(node); break;
        case OP_Public: prevalidatePublic(node); break;
        case OP_Negation: prevalidateNegation(node); break;
        case OP_Assignation: prevalidateAssignation(node); break;
        case OP_Dot:
        case OP_Dot_safe: prevalidateDot(node); break;
        case OP_Reference: prevalidateReference(node); break;
        case OP_Concat_equal:
        case OP_Plus_equal:
        case OP_Minus_equal:
        case OP_Div_equal:
        case OP_Multi_equal:
        case OP_Mod_equal: prevalidateOpEqual(node); break;
        case OP_Negative:
        case OP_Positive: prevalidateSign(node); break;
        case OP_Decrement_pre:
        case OP_Decrement_pos:
        case OP_Increment_pre:
        case OP_Increment_pos: prevalidateUnaryOp(node); break;
        case OP_Bang: prevalidateBang(node); break;
        case OP_Spread:
            if (pre_state->current_type == T_Arguments ||
                pre_state->current_type == T_Array) {
                prevalidateSpread(node);
            } else {
                errorF(node->line_n, E_TOKEN, node->value);
            }
            break;
        default:
            if (node->type == T_Operator) {
                prevalidateOp(node);
            } else if (node->type == T_Index) {
                prevalidateIndex(node);
            }
    }
}


static void preprocessNode(struct token *node, struct pre_state *pre_state)
{
    if (node == NULL) {
        return;
    }

    if (pre_state->current_type != T_Chunk && isStmt(node)) {
        errorF(node->line_n, E_TOKEN, node->value);
    }

    if (preprocessFuncDef(node, pre_state) || preprocessObj(node)) {
        return;
    }

    preprocessNode(node->ls, pre_state);
    preprocessNode(node->rs, pre_state);

    if (isPartial(node->type)) {
        enum TYPE aux_type = pre_state->current_type;

        pre_state->current_type = node->type;
        preprocessTokenList(node->body, pre_state);
        pre_state->current_type = aux_type;
    }

    prevalidateNode(node, pre_state);
}


void preprocessTokenList(const struct token_list *list, struct pre_state *pre_state)
{
    for (struct token *node = list->first; node != NULL; node = node->next) {
        preprocessNode(node, pre_state);
    }
}


void preprocess(const struct token_list *list, const char *file)
{
    struct pre_state *pre_state = malloc_(sizeof(struct pre_state));
    *pre_state = (struct pre_state) {
        .file = file,
        .current_type = T_Chunk,
    };

    preprocessTokenList(list, pre_state);
    free(pre_state);
}
