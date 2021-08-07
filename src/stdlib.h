#ifndef STDLIB_H
#define STDLIB_H

/**
 * Initializes the standard library (constants and functions).
 * @param args the script arguments.
 * @param cwd the starting current working directory.
 * @param file the starting file.
 */
void stdlibInit(struct element_table *args, const char *cwd, const char *file);

/**
 * Calls a standard function.
 * @param func the standard function.
 * @param args the arguments list.
 */
void stdlibFuncCall(struct function *func, struct result_list *args);

#endif /* STDLIB_H */
