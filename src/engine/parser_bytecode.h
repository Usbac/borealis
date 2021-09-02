#ifndef BYTECODE_H
#define BYTECODE_H

/**
 * Returns a bytecode list from the given AST list.
 * @param ast the AST list.
 * @return The bytecode list.
 */
struct token_list *bytecodeFromList(struct token_list *ast);

/**
 * Pushes the node into the given list as a bytecode node.
 * @param list the list.
 * @param node the node.
 */
void bytecodePush(struct token_list *list, struct token *node);

#endif /* BYTECODE_H */
