#ifndef BYTECODE_H
#define BYTECODE_H

/**
 * Returns the AST list as a bytecode list.
 * @param ast the AST list.
 * @return The AST list as a bytecode list.
 */
struct token_list *listToBytecode(struct token_list *ast);

/**
 * Pushes the node into the given list as a bytecode node.
 * @param list the list.
 * @param node the node.
 */
void pushBytecode(struct token_list *list, struct token *node);

#endif /* BYTECODE_H */
