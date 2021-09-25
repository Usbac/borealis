#ifndef ERROR_H
#define ERROR_H

/**
 * Initializes the error module.
 */
void initErrorModule(struct element *err);

/**
 * Sets the value of the last error variable.
 */
void setLastError(int val);

#endif /* ERROR_H */
