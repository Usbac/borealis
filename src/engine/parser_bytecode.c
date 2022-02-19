#include <string.h>
#include <stdbool.h>
#include "../utils.h"
#include "lexer.h"
#include "parser.h"
#include "parser_bytecode.h"


static void bytecodePushToken(struct token_list *list, struct token *node)
{
    push(list, node);
    list->last->ls = NULL;
    list->last->rs = NULL;
}


static void bytecodePushCode(struct token_list *list, struct token *node)
{
    struct token *code = malloc_(sizeof(struct token));
    *code = (struct token) {
        .type = node->type,
        .line_n = node->line_n,
        .body = bytecodeFromList(node->body),
    };

    bytecodePushToken(list, code);
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


static bool isTokenWithJmp(struct token *node)
{
    return node->opcode == OP_And || node->opcode == OP_Or ||
        node->opcode == OP_Colon || node->opcode == OP_Assignation;
}


static bool isTokenWithDifferentFlow(struct token *node)
{
    return node->opcode == OP_If || node->opcode == OP_While ||
        node->opcode == OP_Else || node->opcode == OP_Case;
}


void bytecodePush(struct token_list *list, struct token *node)
{
    bool added = false;
    bool is_token_with_jmp = false;
    struct token *start = NULL;

    if (node == NULL) {
        return;
    }

    if (node->opcode == OP_Closure || node->opcode == OP_Foreach ||
        isDefKeyword(node->opcode)) {
        bytecodePushToken(list, tokenDup(node));
        added = true;
    }

    if (node->opcode == OP_Null_coalesce) {
        bytecodePush(list, node->ls);
        bytecodePushToken(list, tokenDup(node));
        start = list->last;
        bytecodePush(list, node->rs);
        start->jmp = getJmpLength(start, list->last);
        return;
    }

    bytecodePush(list, node->rs);

    is_token_with_jmp = isTokenWithJmp(node);
    if (is_token_with_jmp || isTokenWithDifferentFlow(node)) {
        bytecodePushToken(list, tokenDup(node));
        added = true;

        if (is_token_with_jmp) {
            start = list->last;
        }
    } else if (node->type == T_Chunk) {
        bytecodePushCode(list, node);
        added = true;
    }

    bytecodePush(list, node->ls);

    if (start != NULL) {
        start->jmp = getJmpLength(start, list->last);
    }

    if (isPartial(node->type) && node->type != T_Chunk) {
        bytecodePushCode(list, node);
    } else if (!added) {
        bytecodePushToken(list, tokenDup(node));
    }
}


struct token_list *bytecodeFromList(struct token_list *ast)
{
    struct token_list *list = listInit();
    struct token *node = ast != NULL ? ast->first : NULL;
    struct token *stmt_sep = malloc_(sizeof(struct token));
    *stmt_sep = (struct token) {
        .opcode = OP_Stmt_sep,
    };

    while (node != NULL) {
        bytecodePush(list, node);
        if (node->next == NULL || node->next->opcode != OP_Else) {
            stmt_sep->line_n = node->line_n;
            bytecodePush(list, stmt_sep);
        }

        node = node->next;
    }

    tokenFree(stmt_sep, true);

    return list;
}
