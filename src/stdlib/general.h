#ifndef GENERAL_H
#define GENERAL_H


/**
 * Returns the given value as a string.
 */
void stdToString(struct result_list *args);

/**
 * Returns the given value as a number.
 */
void stdToNumber(struct result_list *args);

/**
 * Returns the given value as a boolean.
 */
void stdToBool(struct result_list *args);

/**
 * Returns the given value as an array.
 */
void stdToArray(struct result_list *args);

/**
 * Sleeps the given time in seconds.
 */
void stdSleep(struct result_list *args);

/**
 * Exits the program with an error status if the given condition is false.
 */
void stdAssert(struct result_list *args);

/**
 * Exits the program with an status.
 */
void stdExit(struct result_list *args);

/**
 * Returns true if the given value is blank, false otherwise.
 */
void stdIsEmpty(struct result_list *args);

/**
 * Runs the REPL.
 */
void stdDebug(struct result_list *args);

/**
 * Evaluates the code in the given string.
 */
void stdEval(struct result_list *args);

/**
 * If the first parameter is true, returns the second parameter,
 * otherwise the third is returned.
 */
void stdTernary(struct result_list *args);

/**
 * Returns the type of the given value.
 */
void stdTypeof(struct result_list *args);

#endif /* GENERAL_H */
