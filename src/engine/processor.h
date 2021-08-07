#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "../element.h"

/**
 * Executes the given function.
 * @param func the function.
 * @param args the arguments.
 * @param obj the function's object.
 */
void funcExec(struct function *func, struct result_list *args, struct element_table *obj);

/**
 * Evaluates the given node.
 * @param node the node.
 */
void evalNode(struct token **node);

/**
 * Process the code in the file with the given path.
 * @param file the file path.
 * @param line_n the line number where the code is being loaded.
 */
void processFile(const char *file, size_t line_n);

/**
 * Process the bytecode list.
 * @param list the bytecode.
 */
void evalBytecode(struct token_list *list);

#endif /* PROCESSOR_H_ */
