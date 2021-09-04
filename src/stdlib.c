#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include "engine/lexer.h"
#include "engine/processor_helper.h"
#include "borealis.h"
#include "state.h"
#include "error.h"
#include "stdlib/general.h"
#include "stdlib/os.h"
#include "stdlib/string.h"
#include "stdlib/array.h"
#include "stdlib/object.h"
#include "stdlib/number.h"
#include "stdlib/file.h"
#include "stdlib/json.h"
#include "stdlib/math.h"
#include "stdlib/date.h"
#include "stdlib/crypt.h"
#include "stdlib/socket.h"
#include "stdlib/io.h"
#include "stdlib/bit.h"
#include "stdlib.h"

#if defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/errno.h>
#endif

#if defined(_WIN32) || defined(WIN32)
#define END_OF_LINE "\r\n"
#else
#define END_OF_LINE "\n"
#endif

/**
 * Formats for the standard function parameters:
 * s - string
 * d - number
 * a - array
 * f - function
 * o - object
 * v - any
 * * - Takes any number of parameters of any type
 * , - Indicates a new parameter
 * | - Indicates that the remaining parameters are optional
 */

static size_t getStrParamsNumber(char *str)
{
    size_t len = strlen(str);
    size_t n = 0;

    for (size_t i = 0; i < len && str[i] != '|'; i++) {
        n += strchr("sdafov", str[i]) != NULL;
    }

    return n;
}


static struct element *declareModule(const char *key)
{
    struct element *el = elementInit(key, NULL, 0, T_Object);
    el->constant = true;
    stateDeclareElement(&el);

    return el;
}


static void declareModuleFunc(struct element *module,
                              const char *key,
                              std_func *func,
                              char *params)
{
    struct element *el = elementInit(key, NULL, 0, T_Function);
    el->public = true;
    el->constant = true;
    el->value.function->func_ptr = func;
    el->value.function->func_ptr_params = params;
    el->value.function->params_n = getStrParamsNumber(params);
    el->value.function->native_code = true;

    if (module == NULL) {
        stateDeclareElement(&el);
    } else {
        elementTablePush(&module->value.values, el);
    }
}


static void declareModuleConst(struct element *module,
                               const char *key,
                               union VALUE val,
                               enum TYPE type)
{
    struct element *el = elementInit(key, NULL, 0, T_Null);
    el->public = true;
    el->constant = true;
    el->type = type;
    el->value = val;

    if (module == NULL) {
        stateDeclareElement(&el);
    } else {
        elementTablePush(&module->value.values, el);
    }
}


static void initConstants(struct element_table *args, const char *cwd, const char *file)
{
    union VALUE true_val = { .number = 1 };
    union VALUE false_val = { .number = 0 };
    union VALUE start_dir_val = { .string = strDup(cwd) };
    union VALUE start_file_val = { .string = strDup(file) };
    union VALUE file_val = { .string = strDup(state->file) };
    union VALUE version_val = { .string = strDup(BOREALIS_VERSION) };
    union VALUE args_val = { .values = args };

    declareModuleConst(NULL, "true", true_val, T_Number);
    declareModuleConst(NULL, "false", false_val, T_Number);
    declareModuleConst(NULL, "_START_DIR", start_dir_val, T_String);
    declareModuleConst(NULL, "_START_FILE", start_file_val, T_String);
    declareModuleConst(NULL, "_VERSION", version_val, T_String);
    declareModuleConst(NULL, "_ARGS", args_val, T_Array);
    declareModuleConst(NULL, STATE_FILE_KEY, file_val, T_String);
}


static void initFunctions(void)
{
    struct element *io = declareModule("Io");
    struct element *os = declareModule("Os");
    struct element *str = declareModule("String");
    struct element *arr = declareModule("Array");
    struct element *obj = declareModule("Object");
    struct element *num = declareModule("Number");
    struct element *file = declareModule("File");
    struct element *json = declareModule("Json");
    struct element *math = declareModule("Math");
    struct element *crypt = declareModule("Crypt");
    struct element *date = declareModule("Date");
    struct element *socket = declareModule("Socket");
    struct element *bit = declareModule("Bit");
    union VALUE value;

    /* io */
    declareModuleFunc(io, "print", stdPrint, "*");
    declareModuleFunc(io, "printLine", stdPrintLine, "*");
    declareModuleFunc(io, "printV", stdPrintV, "v");
    declareModuleFunc(io, "readChar", stdReadChar, "");
    declareModuleFunc(io, "readLine", stdReadLine, "|s,n");
    declareModuleFunc(io, "flush", stdFlush, "");
    declareModuleFunc(io, "historyAdd", stdHistoryAdd, "s");
    declareModuleFunc(io, "historySetSize", stdHistorySize, "d");
    declareModuleFunc(io, "clear", stdClear, "");
    /* os */
    declareModuleFunc(os, "clearPath", stdClearPath, "s");
    declareModuleFunc(os, "isAbsolute", stdIsAbsolute, "s");
#ifndef SAFE_MODE
    declareModuleFunc(os, "getCWD", stdGetCwd, "");
    declareModuleFunc(os, "isFile", stdIsFile, "s");
    declareModuleFunc(os, "isDir", stdIsDir, "s");
    declareModuleFunc(os, "exists", stdExists, "s");
    declareModuleFunc(os, "rename", stdRename, "s,s");
    declareModuleFunc(os, "remove", stdUnlink, "s");
    declareModuleFunc(os, "makeDir", stdMakeDir, "s|s");
    declareModuleFunc(os, "getFiles", stdGetFiles, "s");
    declareModuleFunc(os, "getPermissions", stdGetPermissions, "s");
    declareModuleFunc(os, "chmod", stdChmod, "s,s");
    declareModuleFunc(os, "getEnv", stdGetEnv, "s");
    declareModuleFunc(os, "getTime", stdGetTime, "");
    declareModuleFunc(os, "exec", stdExec, "s");
    declareModuleFunc(os, "getModTime", stdGetModTime, "s");
    declareModuleFunc(os, "getChangeTime", stdGetChangeTime, "s");
    declareModuleFunc(os, "getAccessTime", stdGetAccessTime, "s");
#endif
    value.string = strDup(END_OF_LINE);
    declareModuleConst(os, "EOL", value, T_String);
    value.string = strDup(getPlatform());
    declareModuleConst(os, "PLATFORM", value, T_String);
    /* string */
    declareModuleFunc(str, "toUpper", stdToUpper, "s");
    declareModuleFunc(str, "toLower", stdToLower, "s");
    declareModuleFunc(str, "getLength", stdGetLength, "s");
    declareModuleFunc(str, "getByteLength", stdGetByteLength, "s");
    declareModuleFunc(str, "includes", stdIncludes, "s,s");
    declareModuleFunc(str, "trim", stdTrim, "s|s");
    declareModuleFunc(str, "leftTrim", stdLeftTrim, "s|s");
    declareModuleFunc(str, "rightTrim", stdRightTrim, "s|s");
    declareModuleFunc(str, "indexOf", stdIndexOf, "s,s");
    declareModuleFunc(str, "lastIndexOf", stdLastIndexOf, "s,s");
    declareModuleFunc(str, "split", stdSplit, "s,s");
    declareModuleFunc(str, "endsWith", stdEndsWith, "s,s");
    declareModuleFunc(str, "startsWith", stdStartsWith, "s,s");
    declareModuleFunc(str, "getChar", stdGetChar, "s,d");
    declareModuleFunc(str, "slice", stdSlice, "s,d|d");
    declareModuleFunc(str, "repeat", stdRepeat, "s,d");
    declareModuleFunc(str, "isWhitespace", stdIsWhitespace, "s");
    declareModuleFunc(str, "replace", stdReplace, "s,s,s");
    declareModuleFunc(str, "remove", stdRemove, "s,s");
    declareModuleFunc(str, "compare", stdCompare, "s,s");
    declareModuleFunc(str, "compareI", stdCompareI, "s,s");
    declareModuleFunc(str, "containsChars", stdContainsChars, "s,s");
    declareModuleFunc(str, "interpolate", stdInterpolate, "s,a");
    declareModuleFunc(str, "sprintf", stdSprintf, "s,*");
    declareModuleFunc(str, "padStart", stdPadStart, "s,d|s");
    declareModuleFunc(str, "padEnd", stdPadEnd, "s,d|s");
    declareModuleFunc(str, "reverse", stdStringReverse, "s");
    declareModuleFunc(str, "encode", stdEncode, "s,s");
    declareModuleFunc(str, "decode", stdDecode, "s,s");
    declareModuleFunc(str, "count", stdCount, "s,s");
    declareModuleFunc(str, "isUpper", stdIsUpper, "s");
    declareModuleFunc(str, "isLower", stdIsLower, "s");
    declareModuleFunc(str, "capitalize", stdCapitalize, "s");
    declareModuleFunc(str, "fromCharCodes", stdFromCharCodes, "*");
    /* array */
    declareModuleFunc(arr, "get", stdGet, "a,s");
    declareModuleFunc(arr, "getSize", stdGetSize, "a");
    declareModuleFunc(arr, "push", stdPush, "a,v");
    declareModuleFunc(arr, "prepend", stdPrepend, "a,v");
    declareModuleFunc(arr, "pop", stdPop, "a");
    declareModuleFunc(arr, "shift", stdShift, "a");
    declareModuleFunc(arr, "getKeys", stdGetKeys, "a");
    declareModuleFunc(arr, "hasKey", stdHasKey, "a,s");
    declareModuleFunc(arr, "join", stdJoin, "a|s");
    declareModuleFunc(arr, "has", stdHas, "a,v");
    declareModuleFunc(arr, "reverse", stdReverse, "a");
    declareModuleFunc(arr, "range", stdRange, "d,d|d");
    declareModuleFunc(arr, "sort", stdSort, "a|f");
    declareModuleFunc(arr, "column", stdColumn, "a,s");
    declareModuleFunc(arr, "map", stdMap, "a,f");
    declareModuleFunc(arr, "filter", stdFilter, "a,f");
    declareModuleFunc(arr, "reduce", stdReduce, "a,f|v");
    declareModuleFunc(arr, "some", stdSome, "a,f");
    declareModuleFunc(arr, "merge", stdMerge, "a,*");
    /* number */
    declareModuleFunc(num, "min", stdMin, "*");
    declareModuleFunc(num, "max", stdMax, "*");
    declareModuleFunc(num, "isNan", stdIsNan, "v");
    declareModuleFunc(num, "isFinite", stdIsFinite, "v");
    declareModuleFunc(num, "rand", stdRand, "|d");
    declareModuleFunc(num, "sRand", stdSRand, "d");
    declareModuleFunc(num, "format", stdFormat, "d|s,s");
    value.number = (double) NAN;
    declareModuleConst(num, "NAN", value, T_Number);
    value.number = DBL_MAX;
    declareModuleConst(num, "MAX", value, T_Number);
    value.number = -DBL_MAX;
    declareModuleConst(num, "MIN", value, T_Number);
    /* object */
    declareModuleFunc(obj, "getKeys", stdGetObjectKeys, "o");
    declareModuleFunc(obj, "getValues", stdGetObjectValues, "o");
    /* file */
#ifndef SAFE_MODE
    declareModuleFunc(file, "create", stdCreateFile, "s");
    declareModuleFunc(file, "read", stdReadFile, "s|s");
    declareModuleFunc(file, "write", stdWriteFile, "s,s|d");
    declareModuleFunc(file, "append", stdAppendFile, "s,s");
    declareModuleFunc(file, "copy", stdCopy, "s,s");
    declareModuleFunc(file, "getType", stdGetType, "s");
    declareModuleFunc(file, "getSize", stdGetFileSize, "s");
#endif
    /* json */
    declareModuleFunc(json, "stringify", stdJsonStringify, "v");
    declareModuleFunc(json, "parse", stdJsonParse, "s");
    declareModuleFunc(json, "isValid", stdIsvalid, "s");
    /* math */
    declareModuleFunc(math, "cos", stdCos, "d");
    declareModuleFunc(math, "acos", stdAcos, "d");
    declareModuleFunc(math, "cosh", stdCosh, "d");
    declareModuleFunc(math, "acosh", stdAcosh, "d");
    declareModuleFunc(math, "sin", stdSin, "d");
    declareModuleFunc(math, "asin", stdAsin, "d");
    declareModuleFunc(math, "sinh", stdSinh, "d");
    declareModuleFunc(math, "asinh", stdAsinh, "d");
    declareModuleFunc(math, "tan", stdTan, "d");
    declareModuleFunc(math, "atan", stdAtan, "d");
    declareModuleFunc(math, "tanh", stdTanh, "d");
    declareModuleFunc(math, "atanh", stdAtanh, "d");
    declareModuleFunc(math, "sqrt", stdSqrt, "d");
    declareModuleFunc(math, "log", stdLog, "d");
    declareModuleFunc(math, "abs", stdAbs, "d");
    declareModuleFunc(math, "floor", stdFloor, "d");
    declareModuleFunc(math, "ceil", stdCeil, "d");
    declareModuleFunc(math, "round", stdRound, "d|d");
    value.number = M_PI;
    declareModuleConst(math, "PI", value, T_Number);
    value.number = M_E;
    declareModuleConst(math, "E", value, T_Number);
    value.number = M_LN2;
    declareModuleConst(math, "LN2", value, T_Number);
    value.number = M_SQRT2;
    declareModuleConst(math, "SQRT2", value, T_Number);
    /* crypt */
    declareModuleFunc(crypt, "sha1", stdSha1, "s");
    declareModuleFunc(crypt, "sha256", stdSha256, "s");
    declareModuleFunc(crypt, "md5", stdMd5, "s");
    /* date */
    declareModuleFunc(date, "now", stdDateNow, "");
    declareModuleFunc(date, "toString", stdDateToString, "s,d");
    declareModuleFunc(date, "fromString", stdFromString, "s");
    declareModuleFunc(date, "toObject", stdDateToObject, "d");
    declareModuleFunc(date, "getGMTOffset", stdGetGmtOffset, "");
    /* bit */
    declareModuleFunc(bit, "and", stdAnd, "d,d");
    declareModuleFunc(bit, "or", stdOr, "d,d");
    declareModuleFunc(bit, "leftShift", stdLeftShift, "d,d");
    declareModuleFunc(bit, "rightShift", stdRightShift, "d,d");
    declareModuleFunc(bit, "not", stdNot, "d");
    /* socket */
    declareModuleFunc(socket, "htons", stdHtons, "n");
    declareModuleFunc(socket, "htonl", stdHtonl, "n");
#ifndef SAFE_MODE
    declareModuleFunc(socket, "new", stdNew, "d,d|d");
    declareModuleFunc(socket, "bind", stdBind, "o,s,d");
    declareModuleFunc(socket, "listen", stdListen, "o,d");
    declareModuleFunc(socket, "accept", stdAccept, "o");
    declareModuleFunc(socket, "close", stdClose, "o");
    declareModuleFunc(socket, "shutdown", stdShutdown, "o|d");
    declareModuleFunc(socket, "recv", stdRecv, "o,d|d");
    declareModuleFunc(socket, "send", stdSend, "o,s|d");
    declareModuleFunc(socket, "setTimeout", stdSetTimeout, "o,n");
    declareModuleFunc(socket, "getTimeout", stdGetTimeout, "o");
#endif
    value.number = AF_INET;
    declareModuleConst(socket, "AF_INET", value, T_Number);
    value.number = AF_INET6;
    declareModuleConst(socket, "AF_INET6", value, T_Number);
    value.number = AF_UNIX;
    declareModuleConst(socket, "AF_UNIX", value, T_Number);
    value.number = SOCK_STREAM;
    declareModuleConst(socket, "SOCK_STREAM", value, T_Number);
    value.number = SOCK_DGRAM;
    declareModuleConst(socket, "SOCK_DGRAM", value, T_Number);
    value.number = SOCK_RAW;
    declareModuleConst(socket, "SOCK_RAW", value, T_Number);
    value.number = SOCK_RDM;
    declareModuleConst(socket, "SOCK_RDM", value, T_Number);
    value.number = EADDRINUSE;
    declareModuleConst(socket, "EADDRINUSE", value, T_Number);
    value.number = EBADF;
    declareModuleConst(socket, "EBADF", value, T_Number);
    value.number = ENOTSOCK;
    declareModuleConst(socket, "ENOTSOCK", value, T_Number);
    value.number = EOPNOTSUPP;
    declareModuleConst(socket, "EOPNOTSUPP", value, T_Number);
    value.number = MSG_OOB;
    declareModuleConst(socket, "MSG_OOB", value, T_Number);
    value.number = MSG_PEEK;
    declareModuleConst(socket, "MSG_PEEK", value, T_Number);
    /* general */
    declareModuleFunc(NULL, "toString", stdToString, "v");
    declareModuleFunc(NULL, "toNumber", stdToNumber, "v|d");
    declareModuleFunc(NULL, "toBool", stdToBool, "v");
    declareModuleFunc(NULL, "toObject", stdToObject, "v");
    declareModuleFunc(NULL, "toArray", stdToArray, "v");
    declareModuleFunc(NULL, "sleep", stdSleep, "d");
    declareModuleFunc(NULL, "assert", stdAssert, "v|s");
    declareModuleFunc(NULL, "exit", stdExit, "|d");
    declareModuleFunc(NULL, "isEmpty", stdIsEmpty, "v");
    declareModuleFunc(NULL, "debug", stdDebug, "|v");
    declareModuleFunc(NULL, "eval", stdEval, "s");
    declareModuleFunc(NULL, "t", stdTernary, "v,v,v");
    declareModuleFunc(NULL, "typeof", stdTypeof, "v");
    declareModuleFunc(NULL, "print", stdPrint, "*");
    declareModuleFunc(NULL, "printLine", stdPrintLine, "*");
}


static void validateStdArgs(struct function *func, struct result_list *args)
{
    char *params = func->func_ptr_params;
    size_t len = strlen(params);
    size_t n_params = 0;
    struct result *arg = args->first;
    bool in_optionals = false;

    for (size_t i = 0; i < len; i++) {
        enum TYPE type = getResultType(arg);
        char *type_str = getElementTypeAsStr(type);
        bool in_param = isalpha(params[i]) != 0;
        char *wrong_type = NULL;

        n_params += in_param;

        if (params[i] == '*') {
            return;
        } else if (params[i] == '|') {
            in_optionals = true;
        }

        if (!in_param) {
            continue;
        }

        if (arg == NULL) {
            if (!in_optionals) {
                errorF(state->line_n, E_FUNC_PARAM_NUM, func->params_n);
            }

            continue;
        }

        if (params[i] == 's' && type != T_String) {
            wrong_type = TYPEOF_STRING;
        } else if (params[i] == 'a' && type != T_Array) {
            wrong_type = TYPEOF_ARRAY;
        } else if (params[i] == 'f' && type != T_Function) {
            wrong_type = TYPEOF_FUNCTION;
        } else if (params[i] == 'd' && type != T_Number) {
            wrong_type = TYPEOF_NUMBER;
        } else if (params[i] == 'o' && type != T_Object) {
            wrong_type = TYPEOF_OBJECT;
        }

        if (wrong_type != NULL) {
            errorF(arg->line_n, E_FUNC_PARAM_TYPE, n_params, wrong_type, type_str);
        }

        arg = arg->next;
    }

    if (arg != NULL && n_params == 0) {
        errorF(arg->line_n, E_FUNC_PARAM_NUM, func->params_n);
    }
}


void stdlibInit(struct element_table *args, const char *cwd, const char *file)
{
    srand((unsigned int) time(NULL));
    initConstants(args, cwd, file);
    initFunctions();
}


void stdlibFuncCall(struct function *func, struct result_list *args)
{
    struct result *aux_stack = state->stack->last;
    validateStdArgs(func, args);
    func->func_ptr(args);
    if (aux_stack == state->stack->last) {
        statePushResultNull();
    }
}
