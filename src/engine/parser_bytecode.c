#include <string.h>
#include <stdbool.h>
#include "../utils.h"
#include "lexer.h"
#include "parser.h"
#include "parser_bytecode.h"


static void pushTokenToBytecode(struct token_list *list, struct token *node)
{
    push(list, node);
    list->last->ls = NULL;
    list->last->rs = NULL;
}


static void pushBytecodeCode(struct token_list *list, struct token *node)
{
    struct token *code = malloc_(sizeof(struct token));
    *code = (struct token) {
        .type = node->type,
        .line_n = node->line_n,
        .body = listToBytecode(node->body),
    };

    pushTokenToBytecode(list, code);
}


static size_t getJmpLength(struct token *start, struct token *end)
{
    size_t n = 0;

    for (struct token *i = start; i != NULL && i != end;) {
        i = i->next;
        n++;
    }

    return n;
}


void pushBytecode(struct token_list *list, struct token *node)
{
    bool added = false;
    struct token *start = NULL;

    if (node == NULL) {
        return;
    }

    if (node->opcode == OP_Closure || node->opcode == OP_Object ||
        node->opcode == OP_Foreach || isDefKeyword(node->opcode)) {
        pushTokenToBytecode(list, tokenDup(node));
        added = true;
    }

    if (node->opcode == OP_Null_coalesce) {
        pushBytecode(list, node->ls);
        pushTokenToBytecode(list, tokenDup(node));
        start = list->last;
        pushBytecode(list, node->rs);
        start->jmp = getJmpLength(start, list->last);
        return;
    }

    pushBytecode(list, node->rs);

    if (node->opcode == OP_If || node->opcode == OP_While ||
        node->opcode == OP_Else || node->opcode == OP_Case) {
        pushTokenToBytecode(list, tokenDup(node));
        added = true;
    } else if (node->opcode == OP_And || node->opcode == OP_Or ||
        node->opcode == OP_Colon || node->opcode == OP_Assignation) {
        pushTokenToBytecode(list, tokenDup(node));
        start = list->last;
        added = true;
    } else if (node->type == T_Chunk) {
        pushBytecodeCode(list, node);
        added = true;
    }

    pushBytecode(list, node->ls);

    if (start != NULL) {
        start->jmp = getJmpLength(start, list->last);
    }

    if (node->type == T_Parameters || node->type == T_Array ||
        node->type == T_Index || node->type == T_Arguments) {
        pushBytecodeCode(list, node);
    } else if (!added) {
        pushTokenToBytecode(list, tokenDup(node));
    }
}


struct token_list *listToBytecode(struct token_list *ast)
{
    struct token_list *list = listInit();
    struct token *node = ast != NULL ? ast->first : NULL;
    struct token *stmt_sep = malloc_(sizeof(struct token));
    *stmt_sep = (struct token) {
        .opcode = OP_Stmt_sep,
    };

    while (node != NULL) {
        pushBytecode(list, node);
        if (node->next == NULL || node->next->opcode != OP_Else) {
            stmt_sep->line_n = node->line_n;
            pushBytecode(list, stmt_sep);
        }

        node = node->next;
    }

    tokenFree(stmt_sep, true);

    return list;
}
