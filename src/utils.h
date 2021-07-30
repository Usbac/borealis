#ifndef HELPER_H_
#define HELPER_H_

#include <stdio.h>
#include <stdbool.h>

#ifndef M_E
#define M_E 2.7182818284590452354
#endif
#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

#define ERROR_MEM_MSG "Error: Could not allocate memory\n"
#define MAX_PATH 4096
#define BUFFER 32
#define HASHMAP_SIZE 2048
#define SNPRINTF_DOUBLE(str, n) snprintf(str, BUFFER, "%.17g", n)
#define SNPRINTF_INT(str, n) snprintf(str, BUFFER, "%i", n)
#define SNPRINTF_SIZE_T(str, n) snprintf(str, BUFFER, "%zu", n)
#define FREE_AND_NULL(ptr) do { free(ptr); (ptr) = NULL; } while (0)

/**
 * The functions defined here are general helper functions
 * that aren't related to any specific module of this software.
 */

/**
 * Just a safer malloc function.
 * @param size the memory to allocate.
 */
void *malloc_(size_t size);

/**
 * Just a safer calloc function.
 * @param nmemb the elements
 * @param size the elements size.
 */
void *calloc_(size_t nmemb, size_t size);

/**
 * Returns the current working directory.
 * @return the current working directory.
 */
char *getcwd_(void);

/**
 * Makes a new directory.
 * @param path the directory path.
 * @param mode the mode.
 * @return 0 in case of success, 1 otherwise.
 */
int mkdir_(char *path, unsigned int mode);

/**
 * Returns the string with all the instances of the old substring replaced by
 * the new substring.
 * @param str the string.
 * @param old the old substring.
 * @param rep the new substring.
 */
char *strReplace(const char *str, const char *old, const char *rep);

/**
 * Returns a copy of the given string.
 * @param src the source string.
 * @return a copy of the given string.
 */
char *strDup(const char *src);

/**
 * Returns a new empty string.
 * @return the empty string.
 */
char *strInit(void);

/**
 * Turns the given allocated string into
 * a new empty string.
 * @param str the string.
 */
void strEmpty(char **str);

/**
 * Returns the double value of the given string.
 * @param str the string.
 * @return the double value of the given string.
 */
double strToD(const char *str);

/**
 * Returns the string representation of the given int.
 * @param i the integer.
 * @return the string representation.
 */
char *strFromInt(int i);

/**
 * Returns the string representation of the given unsigned long.
 * @param i the integer.
 * @return the string representation.
 */
char *strFromSizet(size_t n);

/**
 * Returns the string representation of the given double.
 * @param n the double.
 * @return the string representation.
 */
char *strFromD(double n);

/**
 * Returns true if the given string is a valid int, false otherwise.
 * @param str the string.
 * @return true if the given string is a valid int, false otherwise.
 */
bool strIsInt(const char *str);

/**
 * Appends a string to the given string
 * @param a the string.
 * @param b the substring to append to the string.
 */
void strAppend(char **a, const char *b);

/**
 * Prepends a string to the given string
 * @param a the string.
 * @param b the substring to prepend to the string.
 */
void strPrepend(char **a, const char *b);

/**
 * Appends a char to the given string
 * @param str the string.
 * @param ch the char to append to the string.
 */
void strAppendC(char **str, char ch);

/**
 * Prepends a char to the given string
 * @param str the string.
 * @param ch the char to prepend to the string.
 */
void strPrependC(char **str, char ch);

/*
 * Returns the result of concatenating the two given strings.
 * @param a the first string.
 * @param b the second string.
 * @return the concatenation of the two given strings.
 */
char *strJoin(const char *a, const char *b);

/*
 * Returns the result of concatenating the two given paths.
 * @param a the first path.
 * @param b the second path.
 * @return the concatenation of the two given paths.
 */
char *strJoinPaths(const char *a, const char *b);

/**
 * Returns the given string hashed with the djb2 hash.
 * @param str the string.
 * @param limit the hash range.
 * @return the given string hashed.
 */
size_t getHash(const char *str, size_t limit);

/**
 * Returns the length of the file.
 * @param fp the file pointer.
 * @return the length.
 */
long getFileSize(FILE *fp);

/**
 * Returns the content of a file.
 * In case of errors, NULL is returned.
 * @param path the file path.
 * @param binary read as binary or not.
 * @return the file's content.
 */
char *readF(const char *path, bool binary);

/**
 * Returns the system name.
 * ('windows', 'linux', 'mac', 'bsd' or 'unknown').
 * @return the system name.
 */
char *getPlatform(void);

/**
 * Returns the number of bytes required to store the given integer.
 * @param n the integer.
 * @return the number of bytes.
 */
size_t getIntBytes(int n);

/**
 * Returns true if the given path is absolute, false otherwise.
 * @param path the path.
 * @return true if the given path is absolute, false otherwise.
 */
bool isAbsolutePath(const char *path);

/**
 * Returns true if the given path refers to a file, false otherwise.
 * @param path the path.
 * @return true if the given path refers to a file, false otherwise.
 */
bool isFilePath(const char *path);

#if defined(_WIN32) || defined(WIN32)
/**
 * Reference to the snprintf function of the stdio library.
 * Because you know, some compilers are not compliant with the C standard.
 */
int snprintf(char *buf, size_t size, const char *fmt, ...);
#endif

#endif /* HELPER_H_ */
