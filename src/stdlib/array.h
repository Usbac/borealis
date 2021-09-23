#ifndef ARRAY_H
#define ARRAY_H

/**
 * Returns the array index with the given key.
 */
void stdGet(struct result_list *args);

/**
 * Returns the size of the array.
 */
void stdGetSize(struct result_list *args);

/**
 * Pushes a value in the array.
 */
void stdPush(struct result_list *args);

/**
 * Prepends a value in the array.
 */
void stdPrepend(struct result_list *args);

/**
 * Pops a value from the array.
 */
void stdPop(struct result_list *args);

/**
 * Shifts a value from the array.
 */
void stdShift(struct result_list *args);

/**
 * Returns an array with all the array keys.
 */
void stdGetKeys(struct result_list *args);

/**
 * Returns true if the given key exists in the array, false otherwise.
 */
void stdHasKey(struct result_list *args);

/**
 * Returns the given array joined as a string.
 */
void stdJoin(struct result_list *args);

/**
 * Returns true if the given value exists in the array, false otherwise.
 */
void stdHas(struct result_list *args);

/**
 * Reverses the given array.
 */
void stdReverse(struct result_list *args);

/**
 * Returns an array of numbers based on the given range.
 */
void stdRange(struct result_list *args);

/**
 * Sorts the given array.
 */
void stdSort(struct result_list *args);

/**
 * Returns the values from a column in the given array.
 */
void stdColumn(struct result_list *args);

/**
 * Applies the closure to the elements of the array.
 */
void stdMap(struct result_list *args);

/**
 * Filters elements of an array using a closure.
 */
void stdFilter(struct result_list *args);

/**
 * Reduces the array to a single value using a closure.
 */
void stdReduce(struct result_list *args);

/**
 * Returns a merge of multiple arrays.
 */
void stdMerge(struct result_list *args);

/**
 * Returns true whether at least one element in the array passes the test
 * implemented by the given closure, false otherwise.
 */
void stdSome(struct result_list *args);

/**
 * Returns an array filled with the given value.
 */
void stdFill(struct result_list *args);

#endif /* ARRAY_H */
