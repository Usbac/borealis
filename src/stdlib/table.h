#ifndef TABLE_H
#define TABLE_H

/**
 * Returns the table index with the given key.
 */
void stdGet(struct result_list *args);

/**
 * Returns the size of the table.
 */
void stdGetSize(struct result_list *args);

/**
 * Pushes a value in the table.
 */
void stdPush(struct result_list *args);

/**
 * Prepends a value in the table.
 */
void stdPrepend(struct result_list *args);

/**
 * Pops a value from the table.
 */
void stdPop(struct result_list *args);

/**
 * Shifts a value from the table.
 */
void stdShift(struct result_list *args);

/**
 * Returns a table with all the table keys.
 */
void stdGetKeys(struct result_list *args);

/**
 * Returns true if the given key exists in the table, false otherwise.
 */
void stdHasKey(struct result_list *args);

/**
 * Returns the given table joined as a string.
 */
void stdJoin(struct result_list *args);

/**
 * Returns true if the given value exists in the table, false otherwise.
 */
void stdHas(struct result_list *args);

/**
 * Reverses the given table.
 */
void stdReverse(struct result_list *args);

/**
 * Returns a table of numbers based on the given range.
 */
void stdRange(struct result_list *args);

/**
 * Sorts the given table.
 */
void stdSort(struct result_list *args);

/**
 * Returns the values from a column in the given table.
 */
void stdColumn(struct result_list *args);

/**
 * Applies the closure to the elements of the table.
 */
void stdMap(struct result_list *args);

/**
 * Filters elements of an table using a closure.
 */
void stdFilter(struct result_list *args);

/**
 * Reduces the table to a single value using a closure.
 */
void stdReduce(struct result_list *args);

/**
 * Returns a merge of multiple table.
 */
void stdMerge(struct result_list *args);

/**
 * Returns true whether at least one element in the table passes the test
 * implemented by the given closure, false otherwise.
 */
void stdSome(struct result_list *args);

/**
 * Returns a table filled with the given value.
 */
void stdFill(struct result_list *args);

#endif /* TABLE_H */
