#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../lib/utf8.h"
#include "utils.h"

#if defined(WIN32) || defined(_WIN32)
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#include <sys/stat.h>
#endif


inline void *malloc_(size_t size)
{
    void *mem = malloc(size);

    if (!mem) {
        printf(ERROR_MEM_MSG);
        exit(-1);
    }

    return mem;
}


inline void *calloc_(size_t nmemb, size_t size)
{
    void *mem = calloc(nmemb, size);

    if (!mem) {
        printf(ERROR_MEM_MSG);
        exit(-1);
    }

    return mem;
}


char *getcwd_(void)
{
    char *buff = malloc_(MAX_PATH);
    return getcwd(buff, MAX_PATH);
}


int mkdir_(char *path, unsigned int mode)
{
#if defined(_WIN32)
    return _mkdir(path);
#else
    return mkdir(path, (mode_t) mode);
#endif
}


char *strDup(const char *src)
{
    char *new;
    size_t len;

    if (src == NULL) {
        return NULL;
    }

    len = strlen(src) + 1;
    new = malloc_(len);
    memcpy(new, src, len);

    return new;
}


inline char *strInit(void)
{
    return (char *) calloc_(1, 1);
}


inline void strEmpty(char **str)
{
    free(*str);
    *str = malloc_(1);
    *str[0] = '\0';
}


inline double strToD(const char *str)
{
    if (str == NULL) {
        return 0.0;
    }

    return strtod(str, NULL);
}


inline char *strFromInt(int i)
{
    char *str = malloc_(BUFFER);
    SNPRINTF_INT(str, i);
    return str;
}


inline char *strFromSizet(size_t n)
{
    char *str = malloc_(BUFFER);
    SNPRINTF_SIZE_T(str, n);
    return str;
}


inline char *strFromD(double n)
{
    char *str = malloc_(BUFFER);
    SNPRINTF_DOUBLE(str, n);
    return str;
}


bool strIsInt(const char *str)
{
    size_t i = 0;

    if (str == NULL || !*str) {
        return false;
    }

    if (str[0] == '-' || str[0] == '+') {
        i++;
    }

    for (; i < strlen(str); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }

    return true;
}


void strAppend(char **a, const char *b)
{
    size_t len = 1;
    char *str;

    if (*a != NULL) {
        len += strlen(*a);
    } else {
        *a = "\0";
    }

    if (b != NULL) {
        len += strlen(b);
    } else {
        b = "\0";
    }

    str = malloc_(len);
    snprintf(str, len, "%s%s", *a, b);

    free(*a);
    *a = str;
}


void strPrepend(char **a, const char *b)
{
    size_t len = 2;
    char *str;

    if (*a != NULL) {
        len += strlen(*a);
    } else {
        *a = "\0";
    }

    if (b != NULL) {
        len += strlen(b);
    } else {
        b = "\0";
    }

    str = malloc_(len);
    snprintf(str, len, "%s%s", b, *a);

    free(*a);
    *a = str;
}


void strAppendC(char **str, char ch)
{
    const size_t len = strlen(*str) + 2;
    char *result = malloc(len);
    snprintf(result, len, "%s%c", *str, ch);
    free(*str);
    *str = result;
}


void strPrependC(char **str, char ch)
{
    size_t len = strlen(*str) + 2;
    char *tmp = strDup(*str);

    free(*str);
    *str = malloc_(len);
    *str[0] = '\0';
    snprintf(*str, len, "%c%s", ch, tmp);

    free(tmp);
}


char *strJoin(const char *a, const char *b)
{
    size_t len = 2;
    char *str;

    if (a != NULL) {
        len += strlen(a);
    } else {
        a = "\0";
    }

    if (b != NULL) {
        len += strlen(b);
    } else {
        b = "\0";
    }

    str = malloc_(len);
    snprintf(str, len, "%s%s", a, b);

    return str;
}


char *strJoinPaths(const char *a, const char *b)
{
    char *path = strDup(a);
    char *new_path = strInit();
    char *sub = strInit();
    size_t len;

    if (path[0] != '\0' && path[strlen(path)] != '/' && b[0] != '/') {
        strAppendC(&path, '/');
    }

    strAppend(&path, b);
    len = strlen(path);

    for (size_t i = 0; i < len; i++) {
        if (path[i] != '/') {
            strAppendC(&sub, path[i]);
            continue;
        }

        if (i > 0 && path[i-1] == '/' && path[i] == '/') {
            continue;
        }

        if (!strcmp(sub, "..")) {
            size_t j = strlen(new_path);
            if (j > 0 && new_path[j-1] == '/') {
                new_path[j-1] = '\0';
                j--;
            }

            while (new_path[0] != '\0' && new_path[j] != '/') {
                new_path[j] = '\0';
                j--;
            }
        } else if (strcmp(sub, ".") != 0) {
            strAppend(&new_path, sub);
            strAppendC(&new_path, '/');
        }

        free(sub);
        sub = strInit();
    }

    if (sub[0] != '\0') {
        strAppend(&new_path, sub);
    }

    free(sub);
    free(path);

    return new_path;
}


char *strReplace(const char *str, const char *old, const char *rep)
{
    char *result;
    size_t i = 0, matches = 0;
    size_t str_len = strlen(str);
    size_t old_len = strlen(old);
    size_t rep_len = strlen(rep);
    utf8_int32_t codepoint;

    for (void *next = utf8codepoint(str, &codepoint);
        codepoint != '\0';
        next = utf8codepoint(next, &codepoint)) {
        if (i < str_len && !utf8ncmp(str + i, old, old_len)) {
            matches++;
            i += old_len;
        } else {
            i += utf8codepointsize(codepoint);
        }
    }

    result = calloc_(1, i + (matches * (rep_len - old_len)) + 1);

    i = 0;
    while (*str) {
        if (!utf8ncmp(str, old, old_len)) {
            memmove(&result[i], rep, rep_len + 1);
            i += rep_len;
            str += old_len;
        } else {
            size_t size;
            utf8codepoint(str, &codepoint);
            size = utf8codepointsize(codepoint);
            utf8catcodepoint(&result[i], codepoint, size);
            i += size;
            str += size;
        }
    }

    return result;
}


size_t getHash(const char *str, size_t limit)
{
    size_t hash = 5381;
    char c;

    if (str == NULL) {
        return 0;
    }

    while ((c = *str++)) {
        hash = ((hash << 5u) + hash) + c;
    }

    return hash % limit;
}


long getFileSize(FILE *fp)
{
    long len;

    fseek(fp, -1, SEEK_END);
    len = ftell(fp) + 1;
    rewind(fp);

    return len;
}


char *readF(const char *path, bool binary)
{
    FILE *fp = fopen(path, binary ? "rb" : "r");
    char *buffer;
    size_t len;

    if (!fp) {
        return NULL;
    }

    len = (size_t) getFileSize(fp);
    buffer = calloc_(1, len + 1);
    fread(buffer, len, 1, fp);
    fclose(fp);

    return buffer;
}


char *getPlatform(void)
{
#if defined(_WIN32) || defined(WIN32)
    return "windows";
#elif defined(__linux__)
    return "linux";
#elif defined(__APPLE__)
    return "mac";
#elif defined(__FreeBSD__)
    return "bsd";
#else
    return "unknown"
#endif
}


size_t getIntBytes(int n)
{
    size_t size = 0;
    while (n > 0) {
        n >>= 8;
        size++;
    }

    return size;
}


bool isAbsolutePath(const char *path)
{
#if defined(_WIN32) || defined(WIN32)
    return strlen(path) > 1 && isupper(path[0]) && path[1] == ':';
#else
    return path[0] == '/' || path[0] == '~';
#endif
}


bool isFilePath(const char *path)
{
    struct stat info;

    if (stat(path, &info) < 0) {
        return false;
    }

    return S_ISREG(info.st_mode) && !S_ISDIR(info.st_mode);
}
