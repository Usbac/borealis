#ifndef STATE_H
#define STATE_H

#include <inttypes.h>
#include <setjmp.h>
#include "utils.h"
#include "element.h"

#define STATE_FILE_KEY "_FILE"

enum CONTEXT {
    C_Loop, C_Function,
    C_Import,
};

struct context {
    enum CONTEXT value;
    struct context *prev;
    struct context *next;
};

struct import {
    char *file;
    struct import *next;
};

struct call {
    struct element_table *elements;
    struct element_table *object;
    struct call *prev;
};

/**
 * Structure used to keep track of the interpreter's state.
 */
struct state {
    char *file;
    struct import *imports[HASHMAP_SIZE];
    struct call *callstack;
    struct result_list *stack;
    struct context *context_head;
    struct element_table *current_obj;
    jmp_buf *jmp_buffer;
    bool null_coalescing;
    bool returning;
    bool breaking_loop;
    bool continuing_loop;
    bool exiting;
    int status_code;
    size_t line_n;
    size_t scope;
    uint8_t callstack_size;
};

extern struct state *state;

/**
 * Initializes the current state.
 * @param file the source file.
 */
void initState(const char *file);

/**
 * Frees the current state.
 */
void freeState(void);

/**
 * Sets the current state file.
 * @param file the file.
 */
void setStateFile(char *file);

/**
 * Adds a file import.
 * @param file the file.
 */
void addFileImport(const char *file);

/**
 * Returns true if the given file has been imported, false otherwise.
 * @param file the file.
 * @return true if the given file has been imported, false otherwise.
 */
bool isFileImported(const char *file);

/**
 * Pushes a new context into the list.
 * @param context the context to push.
 */
void pushContext(enum CONTEXT context);

/**
 * Returns true if the context exists false otherwise.
 * @param context the context to search.
 * @return true if the context exists false otherwise.
 */
bool insideContext(enum CONTEXT context);

/**
 * Pops one context from the top of the list.
 */
void popContext(void);

/**
 * Returns true if the interpreter can continue to the next statement,
 * false otherwise.
 * @return true if the interpreter can continue to the next statement,
 * false otherwise.
 */
bool canContinue(void);

/**
 * Returns a new result list.
 * @return the result list.
 */
struct result_list *newResultList(void);

/**
 * Pushes a new result into the given result list.
 * @param list the result list.
 * @param node the result.
 */
void pushResult(struct result_list *list, struct result *node);

/**
 * Pushes a new result of type number into the result list.
 * @param n the number value.
 */
void pushResultD(double n);

/**
 * Pushes a new result of type string into the result list.
 * @param str the string value.
 */
void pushResultStr(char *str);

/**
 * Pushes a new result of type reference into the result list.
 * @param el the element reference.
 */
void pushResultRef(struct element *el);

/**
 * Pushes a new result of type element into the result list.
 * @param el the element.
 */
void pushResultEl(struct element *el);

/**
 * Pushes a new result of type array into the result list.
 * @param values the array values.
 */
void pushResultArr(struct element_table *values);

/**
 * Pushes a new result of type object into the result list.
 * @param values the object values.
 */
void pushResultObj(struct element_table *values);

/**
 * Pushes a new result of type function into the result list.
 * @param func the function.
 */
void pushResultFunc(struct function *func);

/**
 * Pushes a new result of type null into the result list.
 */
void pushResultNull(void);

/**
 * Pops one result from the result stack and returns it.
 * @return the result.
 */
struct result *popResult(void);

/**
 * Pops one result from the result stack without throwing errors
 * and returns it.
 * @return the result.
 */
struct result *popResultSafe(void);

/**
 * Returns a copy of the given result list.
 * @param list the result list to copy.
 * @return the copy of the result list.
 */
struct result_list *copyResultList(struct result_list *list);

/**
 * Frees the given result.
 * @param node the result.
 */
void freeResult(struct result *node);

/**
 * Frees the given result list.
 * @param node the  result list.
 */
void freeResultList(struct result_list *list);

/**
 * Declares the given element.
 * It will be declared in the first valid table:
 * current object -> callstack -> global.
 * @param el the element.
 */
void declareElement(struct element **el);

/**
 * Returns the element with the given key.
 * It will be searched in the order:
 * curent element -> callstack -> global.
 * @param key the element key.
 * @param file the file where the element is being called.
 * @return the element.
 */
struct element *getElement(const char *key, const char *file);

/**
 * Frees the elements in the global list and callstack
 * with an equal or higher scope than the specified one.
 * @param scope the scope.
 */
void freeElementsByScope(size_t scope);

/**
 * Pushes a new call into the callstack.
 * @param args the calling arguments.
 * @param obj the function's object
 */
void pushCallstack(struct element_table *args, struct element_table *obj);

/**
 * Pops a call from the callstack.
 */
void popCallstack(void);

#endif /* STATE_H */
