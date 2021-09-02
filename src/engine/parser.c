#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"
#include "../utils.h"
#include "../error.h"
#include "parser.h"


struct token *tokenDup(const struct token *src)
{
    struct token *new = malloc_(sizeof(struct token));
    *new = (struct token) {
        .prev = src->prev,
        .next = src->next,
        .ls = src->ls,
        .rs = src->rs,
        .type = src->type,
        .opcode = src->opcode,
        .jmp = src->jmp,
        .line_n = src->line_n,
        .body = listDup(src->body),
        .value = strDup(src->value),
    };

    return new;
}


static struct token *pop(struct token_list *list)
{
    struct token *new;

    if (list == NULL || list->last == NULL) {
        return NULL;
    }

    new = list->last;

    if (list->last->prev != NULL) {
        list->last = list->last->prev;
        list->last->next = NULL;
    } else {
        list->last = NULL;
        list->first = NULL;
    }

    return new;
}


static void tokenListMove(struct token_list *dest, struct token_list *src)
{
    if (src->last->opcode == OP_Open_parenthesis) {
        struct token *tmp = pop(src);
        free(tmp->value);
        free(tmp);
    }

    if (src->last != NULL) {
        push(dest, pop(src));
    }
}


static void migrateUntilParenthesis(struct token_list *dest, struct token_list *src)
{
    struct token *tmp;

    while (src->last != NULL && src->last->opcode != OP_Open_parenthesis) {
        tokenListMove(dest, src);
    }

    if (src->last != NULL && src->last->opcode == OP_Open_parenthesis) {
        tmp = src->last;
        src->last = src->last->prev;
        free(tmp->value);
        free(tmp);
    }
}


/* The nodes with a higher value are evaluated first */
static size_t getPrec(const struct token *node)
{
    if (node->type == T_Parameters || node->type == T_Arguments ||
        node->type == T_Index) {
        return 15;
    }

    for (struct reserved_token *i = reserved_tokens_head; i != NULL; i = i->next) {
        if (node->opcode == i->opcode) {
            return i->precedence;
        }
    }

    return 0;
}


static bool isUnaryExpr(struct token *node)
{
    return node->type == T_Index || node->opcode == OP_Negation ||
        node->opcode == OP_Negative || node->opcode == OP_Positive ||
        node->opcode == OP_Import || node->type == T_Arguments ||
        node->opcode == OP_Reference || node->opcode == OP_Object ||
        node->opcode == OP_Increment_pre || node->opcode == OP_Increment_pos ||
        node->opcode == OP_Decrement_pre || node->opcode == OP_Decrement_pos;
}


static bool isRightAssoc(struct token *node)
{
    for (struct reserved_token *i = reserved_tokens_head; i != NULL; i = i->next) {
        if (node->opcode == i->opcode && i->right_associated) {
            return true;
        }
    }

    return false;
}


static void handleMigration(struct token *node,
                            struct token_list *operators,
                            struct token_list *output)
{
    size_t prec = getPrec(node);

    while (operators->last != NULL && operators->last->opcode != OP_Open_parenthesis &&
        (prec < getPrec(operators->last) ||
        (prec == getPrec(operators->last) && !isRightAssoc(node)))) {
        push(output, pop(operators));
    }

    push(operators, tokenDup(node));
}


static bool isExpr(const struct token *node)
{
    return node->opcode != OP_This &&
        (isOperator(node->opcode) || isKeyword(node->opcode));
}


static bool isClosureChunk(struct token *node)
{
    return node->type == T_Chunk &&
        node->prev != NULL && node->prev->type == T_Parameters &&
        node->prev->prev != NULL && node->prev->prev->opcode == OP_Closure;
}


static bool isObjChunk(struct token *node)
{
    return node->type == T_Chunk &&
        node->prev != NULL && node->prev->opcode == OP_Object;
}


static bool isEndChunk(struct token *node)
{
    return node->type == T_Chunk && !isClosureChunk(node) && !isObjChunk(node);
}


static bool isUnexpectedEnd(struct token *node)
{
    return node->next == NULL && node->opcode != OP_Stmt_sep && !isEndChunk(node);
}


/**
 * Returns true only if a function name node remains in the given tree and
 * must be put into the given node
 */
static bool remainsFunctionNameNode(struct token_list *tree, struct token *node)
{
    return node->rs != NULL && node->rs->type == T_Parameters &&
        (node->next == NULL || node->next->opcode == OP_Public) &&
        tree->last != NULL &&
        (tree->last->type == T_Identifier || tree->last->opcode == OP_Dot || tree->last->opcode == OP_Dot_safe);
}


static struct token_list *infixToPostfix(const struct token_list *tokens)
{
    struct token_list *output = listInit();
    struct token_list *operators = listInit();
    struct token *node = tokens->first;

    while (node != NULL) {
        if (node->opcode == OP_Open_parenthesis) {
            push(operators, tokenDup(node));
        } else if (node->opcode == OP_Closed_parenthesis) {
            migrateUntilParenthesis(output, operators);
        } else if (isExpr(node) || isUnaryExpr(node)) {
            handleMigration(node, operators, output);
        } else {
            push(output, tokenDup(node));
        }

        node = node->next;
    }

    if (operators->last != NULL &&
        operators->last->opcode == OP_Open_parenthesis) {
        errorF(operators->last->line_n, E_TOKEN, operators->last->value);
    }

    while (operators->last != NULL) {
        tokenListMove(output, operators);
    }

    free(operators);
    return output;
}


/* Used by proxy functions */
static void listFree_(struct token_list *list, bool recursive)
{
    struct token *node;

    if (list == NULL) {
        return;
    }

    while ((node = list->first) != NULL) {
        list->first = list->first->next;
        tokenFree(node, recursive);
    }

    FREE_AND_NULL(list);
}


void listFree(struct token_list *list)
{
    listFree_(list, false);
}


void listFreeR(struct token_list *list)
{
    listFree_(list, true);
}


static char *getNodeCodeStr(enum TYPE type)
{
    switch (type) {
        case T_Index:
        case T_Array: return "[";
        case T_Arguments:
        case T_Parameters: return "(";
        default: return "{";
    }
}


static void processNodeCode(struct token *node)
{
    bool is_chunk = node->type == T_Chunk;
    node->body = codeToList(node->value,
        is_chunk ? SEPARATOR_DEFAULT : SEPARATOR_LIST,
        is_chunk,
        node->line_n);

    FREE_AND_NULL(node->value);
    node->value = strDup(getNodeCodeStr(node->type));
}


static struct token_list *infixToAST(const struct token_list *infix)
{
    struct token_list *tree = listInit();
    struct token_list *postfix = infixToPostfix(infix);
    struct token *node;

    node = postfix->first;
    while (node != NULL) {
        if (isPartial(node->type)) {
            processNodeCode(node);
        }

        if (isUnaryExpr(node)) {
            node->ls = pop(tree);
        } else if (isExpr(node)) {
            node->ls = pop(tree);
            node->rs = pop(tree);

            if (remainsFunctionNameNode(tree, node)) {
                node->rs->ls = pop(tree);
            }
        }

        push(tree, tokenDup(node));
        node = node->next;
    }

    listFree(postfix);
    return tree;
}


static bool inStmtEnd(bool in_sep, bool strict_end, struct token *node)
{
    return in_sep || (!strict_end && node->next == NULL) || isEndChunk(node);
}


static struct token_list *tokensToList(const struct token_list *tokens, bool strict_end)
{
    struct token_list *list = listInit();
    struct token_list *stmts = listInit();

    for (struct token *i = tokens->first; i != NULL; i = i->next) {
        bool in_sep = i->opcode == OP_Stmt_sep;
        if (!in_sep) {
            push(stmts, tokenDup(i));
        }

        if (inStmtEnd(in_sep, strict_end, i)) {
            struct token_list *ast = infixToAST(stmts);

            if (ast->last != NULL && ast->last->prev != NULL) {
                struct token *prev = ast->last->prev;
                errorF(prev->line_n, E_TOKEN, prev->value);
            }

            push(list, ast->last);
            listFree(stmts);
            free(ast);
            stmts = listInit();
        }

        if (strict_end && isUnexpectedEnd(i)) {
            errorF(i->line_n, E_END, SEPARATOR_DEFAULT);
        }
    }

    listFree(stmts);
    return list;
}


void push(struct token_list *list, struct token *node)
{
    if (node == NULL) {
        return;
    }

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


struct token_list *codeToList(const char *code,
                              const char *stmt_sep,
                              bool strict_end,
                              size_t line_n)
{
    struct token_list *tokens = tokenize(code, stmt_sep, line_n);
    struct token_list *list = tokensToList(tokens, strict_end);
    listFree(tokens);

    return list;
}


void tokenFree(struct token *node, bool recursive)
{
    if (node == NULL) {
        return;
    }

    if (recursive) {
        tokenFree(node->ls, true);
        tokenFree(node->rs, true);
    }

    listFreeR(node->body);
    free(node->value);
    FREE_AND_NULL(node);
}


struct token *tokenDupR(const struct token *node)
{
    struct token *new = NULL;

    if (node != NULL) {
        new = tokenDup(node);
        new->ls = tokenDupR(node->ls);
        new->rs = tokenDupR(node->rs);
    }

    return new;
}


struct token_list *listDup(struct token_list *list)
{
    struct token_list *new = NULL;

    if (list != NULL) {
        struct token *node = list->first;
        new = listInit();

        while (node != NULL) {
            push(new, tokenDupR(node));
            node = node->next;
        }
    }

    return new;
}
