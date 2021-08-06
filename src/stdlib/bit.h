#ifndef BIT_H
#define BIT_H

/**
 * Returns the bitwise operation and.
 */
void stdAnd(struct result_list *args);

/**
 * Returns the bitwise operation or.
 */
void stdOr(struct result_list *args);

/**
 * Returns the bitwise operation left shift.
 */
void stdLeftShift(struct result_list *args);

/**
 * Returns the bitwise operation right shift.
 */
void stdRightShift(struct result_list *args);

/**
 * Returns the bitwise operation not.
 */
void stdNot(struct result_list *args);

#endif /* BIT_H */
