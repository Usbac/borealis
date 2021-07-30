#ifndef NUMBER_H
#define NUMBER_H


/**
 * Returns the minimum value of all the arguments treating them as numbers.
 */
void stdMin(struct result_list *args);

/**
 * Returns the maximum value of all the arguments treating them as numbers.
 */
void stdMax(struct result_list *args);

/**
 * Returns true if the given value is NaN, false otherwise.
 */
void stdIsNan(struct result_list *args);

/**
 * Returns true if the given value is finite, false otherwise.
 */
void stdIsFinite(struct result_list *args);

/**
 * Returns a pseudo random number between 0 and the given value
 * or 1 if no value is supplied.
 */
void stdRand(struct result_list *args);

/**
 * Seeds the pseudo random number generator.
 */
void stdSRand(struct result_list *args);

/**
 * Formats the given number with the specified thousands and
 * decimal separator.
 */
void stdFormat(struct result_list *args);

#endif /* NUMBER_H */
