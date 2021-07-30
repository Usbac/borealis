#ifndef IO_H
#define IO_H

/**
 * Returns the string verbose representation of the given value.
 * @param type the value type.
 * @param val the value.
 * @return the string verbose representation of the given value.
 */
char *getResultStr(enum TYPE type, union VALUE val);

/*
 * Prints all the given parameters in the standard output.
 */
void stdPrint(struct result_list *args);

/**
 * Prints all the given parameters and a line break in the standard output.
 */
void stdPrintLine(struct result_list *args);

/**
 * Prints in verbose mode the given parameter.
 */
void stdPrintV(struct result_list *args);

/**
 * Reads one line from the standard input.
 */
void stdReadLine(struct result_list *args);

/**
 * Reads one character from the standard input.
 */
void stdReadChar(struct result_list *args);

/*
 * Flushes the standard output.
 */
void stdFlush(struct result_list *args);

/**
 * Clears the screen.
 */
void stdClear(struct result_list *args);

/**
 * Adds a string to the history.
 */
void stdHistoryAdd(struct result_list *args);

/**
 * Sets the history maximum size.
 */
void stdHistorySize(struct result_list *args);

#endif /* IO_H */
