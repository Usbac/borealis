#ifndef MATH_H
#define MATH_H

/**
 * Returns the cosine of the given number.
 */
void stdCos(struct result_list *args);

/**
 * Returns the arc cosine of the given number.
 */
void stdAcos(struct result_list *args);

/**
 * Returns the hyperbolic cosine of the given number.
 */
void stdCosh(struct result_list *args);

/**
 * Returns the hyperbolic arc cosine of the given number.
 */
void stdAcosh(struct result_list *args);

/**
 * Returns the sine of the given number.
 */
void stdSin(struct result_list *args);

/**
 * Returns the arc sine of the given number.
 */
void stdAsin(struct result_list *args);

/**
 * Returns the hyperbolic sine of the given number.
 */
void stdSinh(struct result_list *args);

/**
 * Returns the hyperbolic arc sine of the given number.
 */
void stdAsinh(struct result_list *args);

/**
 * Returns the tangent of the given number.
 */
void stdTan(struct result_list *args);

/**
 * Returns the arc tangent of the given number.
 */
void stdAtan(struct result_list *args);

/**
 * Returns the hyperbolic tangent of the given number.
 */
void stdTanh(struct result_list *args);

/**
 * Returns the hyperbolic arc tangent of the given number.
 */
void stdAtanh(struct result_list *args);

/**
 * Returns the square root of the given number.
 */
void stdSqrt(struct result_list *args);

/**
 * Returns the natural logarithm of the given number.
 */
void stdLog(struct result_list *args);

/**
 * Returns the absolute value of the given number.
 */
void stdAbs(struct result_list *args);

/**
 * Returns the given number rounded downwards to the nearest integer.
 */
void stdFloor(struct result_list *args);

/**
 * Returns the given number rounded upwards to the nearest integer.
 */
void stdCeil(struct result_list *args);

/**
 * Returns the given number rounded to the nearest integer.
 */
void stdRound(struct result_list *args);

#endif /* MATH_H */
