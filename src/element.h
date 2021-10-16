#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <stdbool.h>
#include "engine/lexer.h"
#include "utils.h"

struct element_list {
    struct element *ptr;
    struct element_list *next;
    struct element_list *prev;
};

struct element_table {
    struct element *map[HASHMAP_SIZE];
    struct element_list *first;
    struct element_list *last;
    size_t next_index;
};

union VALUE {
    char *string;
    double number;
    struct element_table *values;
    struct element *reference;
    struct function *function;
};

struct result {
    union VALUE value;
    enum TYPE type;
    struct element *p_el;
    size_t line_n;
    struct result *next;
    struct result *prev;
};

struct result_list {
    struct result *first;
    struct result *last;
};

typedef void std_func(struct result_list *);

struct function {
    std_func *func_ptr;
    char *def_file;
    char *func_ptr_params;
    struct token_list *params;
    struct token_list *stmts;
    size_t params_n;
    bool native_code;
    enum TYPE return_type;
};

struct element {
    char *key;
    union VALUE value;
    char *file;
    enum TYPE type;
    enum TYPE static_type;
    bool unset;
    bool public;
    bool constant;
    size_t scope;
    struct element *next;
};

extern struct element_table *elements;

/**
 * Returns a new element table.
 * @return the element table.
 */
struct element_table *elementTableInit(void);

/**
 * Adds the given element to the start of the given table.
 * @param table the table.
 * @param el the element.
 */
void elementTablePrepend(struct element_table **table, struct element *el);

/**
 * Adds the given element to the end of the given table.
 * @param table the table.
 * @param el the element.
 */
void elementTablePush(struct element_table **table, struct element *el);

/**
 * Returns a new function.
 * @return the new function.
 */
struct function *functionInit(void);

/**
 * Returns a copy of the given function.
 * @param func the function.
 * @return the function copy.
 */
struct function *functionDup(struct function *func);

/**
 * Returns a new element.
 * @param key the element key.
 * @param file the path of the file where the element is defined.
 * @param scope the scope level.
 * @param type the element type.
 * @return the new element.
 */
struct element *elementInit(const char *key,
                            const char *file,
                            size_t scope,
                            enum TYPE type);

/**
 * Returns a copy of the given element table.
 * @param src the element table.
 * @return a copy of the given element table.
 */
struct element_table *elementTableDup(const struct element_table *src);

/**
 * Frees the given element.
 * @param node the element.
 */
void elementFree(struct element **node);

/**
 * Frees the given function.
 * @param func the function.
 */
void functionFree(struct function *func);

/**
 * Frees all the values of the given element.
 * @param el the element.
 */
void elementFreeValues(struct element **el);

/**
 * Returns a copy of the given union value.
 * @param val the union value.
 * @param type the union value type.
 * @return the copy of the union value.
 */
union VALUE valueDup(union VALUE val, enum TYPE type);

/**
 * Copies the values of the source element into the destination element.
 * The 'key' and 'scope' values are preserved in the destination element.
 * @param dest the destination element.
 * @param src the source element.
 */
void elementDupValues(struct element **dest, struct element *src);

/**
 * Returns the element with the given key in the given list.
 * @param key the element key.
 * @param list the list to search.
 * @param file the file where the element is being called.
 * @return the element with the given key.
 */
struct element *elementGet(const char *key,
                           struct element_table *list,
                           const char *file);

/**
 * Frees the elements in the given table
 * with an equal or higher scope than the specified one.
 * @param table the elements table.
 * @param scope the scope level to remove.
 */
void elementsTableFree(struct element_table *table, size_t scope);

/**
 * Frees all the elements in the global list.
 */
void elementsFree(void);

#endif /* ELEMENT_H_ */
