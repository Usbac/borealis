#ifndef PARSER_H_
#define PARSER_H_
#include <stdbool.h>

/**
 * Returns a copy of the given node.
 * @param src the node.
 * @return a copy of the given node.
 */
struct token *tokenDup(const struct token *src);

/**
 * Pushes in the node into the given list.
 * @param list the list.
 * @param node the node.
 */
void push(struct token_list *list, struct token *node);

/**
 * Returns an AST list based on the given code.
 * @param code the code.
 * @param stmt_sep the statement separator.
 * @param strict_end true for strict ends, false otherwise.
 * @return the AST list.
 */
struct token_list *codeToList(const char *code,
                              const char *stmt_sep,
                              bool strict_end,
                              size_t line_n);

/**
 * Frees the given list.
 * @param list the list.
 */
void listFree(struct token_list *list);

/**
 * Frees the given token.
 * @param node the token node.
 * @param recursive free or not the token's leafs.
 */
void tokenFree(struct token *node, bool recursive);

/**
 * Frees the given AST list.
 * @param list the AST list.
 */
void ListFreeR(struct token_list *list);

/**
 * Returns a copy of the given node of an abstract syntax tree.
 * @param node the starting node.
 * @return the copy of the given node of an abstract syntax tree.
 */
struct token *tokenDupR(const struct token *node);

/**
 * Returns a copy of the given AST list.
 * @param list the list to copy
 * @return the copy of the given AST list.
 */
struct token_list *listDup(struct token_list *list);

#endif /* PARSER_H_ */
