#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../state.h"
#include "../engine/parser.h"
#include "../engine/processor_helper.h"
#include "../../lib/utf8.h"
#include "../../lib/base64.h"
#include "string.h"


static void strLeftTrim(char **str, const char *needle)
{
    size_t len_needle = strlen(needle);
    size_t len = strlen(*str);

    while (!utf8ncmp(*str, needle, len_needle) && len != 0) {
        len -= len_needle;
        memmove(*str, (*str) + len_needle, len);
    }

    (*str)[len] = '\0';
}


static void strRightTrim(char **str, const char *needle)
{
    size_t len_needle = strlen(needle);
    size_t len = strlen(*str);

    while (!utf8cmp(*str + len - len_needle, needle)) {
        len -= len_needle;
        (*str)[len] = '\0';
    }
}


static char *fillStr(char *pad, double size)
{
    utf8_int32_t codepoint;
    char *str = strInit();
    size_t i = 0;

    if (strlen(pad) == 0) {
        return str;
    }

    for (void *next = utf8codepoint(pad, &codepoint); i < size;) {
        size_t codepoint_size = utf8codepointsize(codepoint);
        char *sub;

        if (codepoint == '\0') {
            next = utf8codepoint(pad, &codepoint);
            continue;
        }

        sub = calloc_(1, codepoint_size + 1);
        utf8catcodepoint(sub, codepoint, codepoint_size);
        strAppend(&str, sub);
        free(sub);

        next = utf8codepoint(next, &codepoint);
        i++;
    }

    return str;
}


static bool checkCase(char *str, bool as_lower)
{
    size_t str_len = utf8len(str);
    utf8_int32_t codepoint;
    bool result = true;
    size_t i = 0;

    for (void *next = utf8codepoint(str, &codepoint);
         i < str_len;
         next = utf8codepoint(next, &codepoint), i++) {
        if (as_lower ? utf8isupper(codepoint) : utf8islower(codepoint)) {
            result = false;
            break;
        }
    }

    return result;
}


void stdSlice(struct result_list *args)
{
    char *value = getValueStr(args->first);
    char *start_str = getValueStr(args->first->next);
    bool has_end = args->first->next->next != NULL;
    char *end_str = has_end ?
        getValueStr(args->first->next->next) :
        strDup("-1");
    size_t len = strlen(value);
    int64_t start = floor(strToD(start_str));
    int64_t end = floor(strToD(end_str));
    utf8_int32_t codepoint;
    size_t i = 0;
    size_t end_i = 0;
    size_t start_i = 0;

    if (start >= end && has_end) {
        free(value);
        value = strInit();
        goto end;
    }

    for (void *next = utf8codepoint(value, &codepoint);
        codepoint != '\0' && i++ != (size_t) start;
        next = utf8codepoint(next, &codepoint)) {
        start_i += utf8codepointsize(codepoint);
    }

    i = 0;
    for (void *next = utf8codepoint(value, &codepoint);
        codepoint != '\0' && i++ != (size_t) end;
        next = utf8codepoint(next, &codepoint)) {
        end_i += utf8codepointsize(codepoint);
    }

    if (start_i > 0 && start_i < len) {
        len -= start_i;
        end_i -= start_i;
        memmove(value, value + start_i, len);
    } else if (start_i >= len) {
        free(value);
        value = strInit();
        goto end;
    }

    if (end_i <= len) {
        value[end_i] = '\0';
    }

    end:
    statePushResultStr(value);
        free(start_str);
        free(end_str);
}


void stdTrim(struct result_list *args)
{
    char *result = getValueStr(args->first);
    char *needle = args->first->next != NULL ?
        getValueStr(args->first->next) :
        strDup(" ");

    strLeftTrim(&result, needle);
    if (result[0] != '\0') {
        strRightTrim(&result, needle);
    }

    statePushResultStr(result);

    free(needle);
}


void stdLeftTrim(struct result_list *args)
{
    char *result = getValueStr(args->first);
    char *needle = args != NULL && args->first->next != NULL ?
        getValueStr(args->first->next) :
        strDup(" ");

    strLeftTrim(&result, needle);
    statePushResultStr(result);

    free(needle);
}


void stdRightTrim(struct result_list *args)
{
    char *result = getValueStr(args->first);
    char *needle = args != NULL && args->first->next != NULL ?
        getValueStr(args->first->next) :
        strDup(" ");

    strRightTrim(&result, needle);
    statePushResultStr(result);

    free(needle);
}


void stdGetLength(struct result_list *args)
{
    char *value = getValueStr(args->first);
    statePushResultD((double) utf8len(value));
    free(value);
}


void stdGetByteLength(struct result_list *args)
{
    char *value = getValueStr(args->first);
    statePushResultD(strlen(value));
    free(value);
}


void stdIncludes(struct result_list *args)
{
    char *haystack = getValueStr(args->first);
    char *needle = getValueStr(args->first->next);

    statePushResultD(utf8str(haystack, needle) != NULL);

    free(needle);
    free(haystack);
}


void stdIndexOf(struct result_list *args)
{
    char *haystack = getValueStr(args->first);
    char *needle = getValueStr(args->first->next);
    char *p = utf8str(haystack, needle);

    if (p == NULL) {
        statePushResultD(-1);
    } else {
        haystack[p - haystack] = '\0';
        statePushResultD(utf8len(haystack));
    }

    free(needle);
    free(haystack);
}


void stdLastIndexOf(struct result_list *args)
{
    char *haystack = getValueStr(args->first);
    char *needle = getValueStr(args->first->next);
    char *p = utf8str(haystack, needle);
    size_t needle_len = utf8len(needle);

    while (p != NULL) {
        char *aux = utf8str(p + needle_len, needle);
        if (!aux) {
            haystack[p - haystack] = '\0';
            statePushResultD(utf8len(haystack));
            goto end;
        }

        p = aux;
    }

    statePushResultD(-1);

    end:
        free(needle);
        free(haystack);
}


void stdSplit(struct result_list *args)
{
    struct element_table *arr = elementTableInit();
    char *haystack = getValueStr(args->first);
    char *needle = getValueStr(args->first->next);
    size_t needle_len = strlen(needle);
    char *p;
    size_t i = 0;

    while (true) {
        struct element *el = elementInit(NULL, NULL, 0, T_String);
        el->key = strFromSizet(arr->next_index);
        el->public = true;

        p = utf8str(&haystack[i], needle);
        if (p == NULL || needle_len == 0) {
            el->value.string = utf8dup(&haystack[i]);
            elementTablePush(&arr, el);
            break;
        }

        el->value.string = utf8ndup(&haystack[i], p - &haystack[i]);
        elementTablePush(&arr, el);
        i += strlen(el->value.string) + needle_len;
    }

    StatePushResultArr(arr);

    free(haystack);
    free(needle);
}


void stdEndsWith(struct result_list *args)
{
    char *haystack = getValueStr(args->first);
    char *needle = getValueStr(args->first->next);
    size_t needle_pos = strlen(haystack) - strlen(needle);

    statePushResultD(!utf8cmp(haystack + needle_pos, needle));

    free(needle);
    free(haystack);
}


void stdStartsWith(struct result_list *args)
{
    char *haystack = getValueStr(args->first);
    char *needle = getValueStr(args->first->next);
    statePushResultD(!utf8ncmp(haystack, needle, strlen(needle)));

    free(needle);
    free(haystack);
}


void stdToUpper(struct result_list *args)
{
    char *str = getValueStr(args->first);
    utf8upr(str);
    statePushResultStr(str);
}


void stdToLower(struct result_list *args)
{
    char *str = getValueStr(args->first);
    utf8lwr(str);
    statePushResultStr(str);
}


void stdGetChar(struct result_list *args)
{
    char *value = getValueStr(args->first);
    char *arg = getValueStr(args->first->next);
    int64_t arg_i = floor(strToD(arg));
    size_t i = 0;
    size_t length_at_index = 0;
    utf8_int32_t codepoint;

    if (!strIsInt(arg) || arg_i < 0 || (size_t) arg_i >= utf8len(value)) {
        statePushResultNull();
        free(value);
        goto end;
    }

    for (void *next = utf8codepoint(value, &codepoint);
         codepoint != '\0' && i != (size_t) arg_i;
         next = utf8codepoint(next, &codepoint),
         i++,
         length_at_index += utf8codepointsize(codepoint));

    memmove(value, &value[length_at_index], strlen(value) - length_at_index);
    value[utf8codepointsize(codepoint)] = '\0';

    statePushResultStr(value);

    end: free(arg);
}


void stdRepeat(struct result_list *args)
{
    char *result;
    char *str = getValueStr(args->first);
    char *arg = getValueStr(args->first->next);
    int64_t repeat = floor(strToD(arg));
    size_t len, final_len, i = 0;

    if (repeat <= 0) {
        statePushResultStr(strDup(""));
        goto end;
    }

    len = strlen(str);
    final_len = len * repeat;
    result = calloc_(final_len + 1, sizeof(char));

    while (i < final_len) {
        memmove(&result[i], str, len);
        i += len;
    }

    statePushResultStr(result);

    end:
        free(str);
        free(arg);
}


void stdIsWhitespace(struct result_list *args)
{
    char *str = getValueStr(args->first);
    size_t len = strlen(str);
    bool result = true;

    for (size_t i = 0; i < len; i++) {
        if (!isspace(str[i])) {
            result = false;
        }
    }

    statePushResultD(result);
    free(str);
}


void stdReplace(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *ori = getValueStr(args->first->next);
    char *rep = getValueStr(args->first->next->next);

    statePushResultStr(strReplace(str, ori, rep));

    free(str);
    free(ori);
    free(rep);
}


void stdRemove(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *needle = getValueStr(args->first->next);
    statePushResultStr(strReplace(str, needle, ""));
    free(str);
    free(needle);
}


void stdCompare(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *arg = getValueStr(args->first->next);
    statePushResultD(!utf8cmp(str, arg));
    free(str);
    free(arg);
}


void stdCompareI(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *arg = getValueStr(args->first->next);
    statePushResultD(!utf8casecmp(str, arg));
    free(str);
    free(arg);
}


void stdContainsChars(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *arg = getValueStr(args->first->next);
    utf8_int32_t codepoint_i, codepoint_j;
    bool result = true;

    for (void *i = utf8codepoint(arg, &codepoint_i);
        codepoint_i != '\0';
        i = utf8codepoint(i, &codepoint_i)) {
        bool contains = false;

        for (void *j = utf8codepoint(str, &codepoint_j);
            codepoint_j != '\0';
            j = utf8codepoint(j, &codepoint_j)) {
            if (codepoint_i == codepoint_j) {
                contains = true;
            }
        }

        if (!contains) {
            result = false;
            break;
        }
    }

    statePushResultD(result);
    free(str);
    free(arg);
}


void stdInterpolate(struct result_list *args)
{
    char *str = getValueStr(args->first);
    struct element_table *arr = getValueArr(args->first->next);
    char *result = strDup(str);

    for (struct element_list *i = arr->first; i != NULL; i = i->next) {
        struct element *el = getTrueElement(i->ptr);
        char *aux, *key, *val;
        size_t len;

        if (el->unset) {
            continue;
        }

        len = strlen(el->key) + 3;
        key = malloc_(len);
        snprintf(key, len, "{%s}", el->key);
        val = getUnionStr(el->type, el->value);
        aux = strReplace(result, key, val);

        free(result);
        free(key);
        free(val);
        result = aux;
    }

    statePushResultStr(result);
    free(str);
}


void stdSprintf(struct result_list *args)
{
    char *str = getValueStr(args->first);
    struct result *ite = args->first->next;
    size_t len = strlen(str);
    char *result = strInit();

    for (size_t i = 0; i < len; i++) {
        char *aux;

        if (str[i] != '%' || i+1 >= len ||
            (str[i+1] != 's' && str[i+1] != 'd' && str[i+1] != 'i' && str[i+1] != 'e')) {
            strAppendC(&result, str[i]);
            continue;
        } else if (i != 0 && str[i-1] == '%') {
            continue;
        }

        if (ite == NULL) {
            break;
        }

        switch (str[i+1]) {
            case 's': aux = getValueStr(ite); break;
            case 'd': aux = strFromD(getValueD(ite)); break;
            case 'i': aux = strFromInt((int) getValueD(ite)); break;
            case 'e':
                aux = malloc_(BUFFER);
                snprintf(aux, BUFFER, "%e", getValueD(ite));
                break;
            default: aux = NULL;
        }

        strAppend(&result, aux);
        free(aux);
        ite = ite->next;
        i++;
    }

    statePushResultStr(result);
    free(str);
}


void stdPadStart(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *pad = args->first->next->next != NULL ?
        getValueStr(args->first->next->next) :
        strDup(" ");
    char *filled_str = fillStr(pad, getValueD(args->first->next));
    size_t str_len, filled_len;

    str_len = utf8len(str);
    filled_len = utf8len(filled_str);
    if (str_len >= filled_len) {
        statePushResultStr(str);
    } else {
        size_t codepoint_count = 0;
        size_t end = filled_len - str_len;
        char *result;
        size_t i = 0;
        utf8_int32_t codepoint;

        for (void *next = utf8codepoint(filled_str, &codepoint);
            i < end;
            utf8codepoint(next, &codepoint)) {
            codepoint_count += utf8codepointsize(codepoint);
            i++;
        }

        result = calloc_(1, codepoint_count + 1);
        memmove(result, filled_str, codepoint_count);
        strAppend(&result, str);
        statePushResultStr(result);
        free(str);
    }

    free(pad);
    free(filled_str);
}


void stdPadEnd(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *pad = args->first->next->next != NULL ?
        getValueStr(args->first->next->next) :
        strDup(" ");
    char *filled_str = fillStr(pad, getValueD(args->first->next));
    size_t str_len, filled_len;

    str_len = utf8len(str);
    filled_len = utf8len(filled_str);
    if (str_len >= filled_len) {
        statePushResultStr(str);
    } else {
        size_t codepoint_count = 0;
        size_t end = filled_len - str_len;
        char *result;
        size_t i = 0;
        utf8_int32_t codepoint;

        for (void *next = utf8codepoint(filled_str, &codepoint);
            i < end;
            utf8codepoint(next, &codepoint)) {
            codepoint_count += utf8codepointsize(codepoint);
            i++;
        }

        result = calloc_(1, str_len + codepoint_count + 1);
        memmove(result, str, str_len);
        memmove(&result[str_len], filled_str, codepoint_count);
        statePushResultStr(result);
        free(str);
    }

    free(pad);
    free(filled_str);
}


void stdStringReverse(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *result;
    utf8_int32_t codepoint;
    size_t len = utf8len(str);
    utf8_int32_t chars[len];
    size_t i = 0;

    if (len == 0) {
        statePushResultStr(str);
        return;
    }

    for (void *next = utf8codepoint(str, &codepoint);
        i < len;
        next = utf8codepoint(next, &codepoint), i++) {
        chars[i] = codepoint;
    }

    result = calloc_(1, strlen(str) + 1);
    i = len - 1;
    do {
        size_t codepoint_len = utf8codepointsize(chars[i]);
        char *sub = calloc_(1, codepoint_len + 1);
        utf8catcodepoint(sub, chars[i], codepoint_len);
        strAppend(&result, sub);
        free(sub);
    } while (i-- != 0);

    statePushResultStr(result);
    free(str);
}


void stdEncode(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *encoding = getValueStr(args->first->next);
    char *result;
    size_t str_len = strlen(str) + 1;

    if (!strcmp(encoding, "base64")) {
        size_t len = base64_encoded_length(str_len);
        result = malloc_(len);
        base64_encode(result, len, str, str_len);
    } else if (!strcmp(encoding, "hex")) {
        result = calloc_(1, str_len * 2);

        for (size_t i = 0; i <= str_len; i++) {
            snprintf(&result[i * 2], 3, "%02X", str[i]);
        }
    } else {
        result = strDup(str);
    }

    statePushResultStr(result);
    free(str);
    free(encoding);
}


void stdDecode(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *encoding = getValueStr(args->first->next);
    char *result;
    size_t str_len = strlen(str) + 1;

    if (!strcmp(encoding, "base64")) {
        size_t len = base64_decoded_length(str_len);
        result = malloc_(len);
        base64_decode(result, len, str, str_len);
    } else if (!strcmp(encoding, "hex")) {
        char *data = str;
        unsigned int byte;
        int offset;
        size_t i = 0;
        result = calloc_(1, str_len / 2);

        while (sscanf(data, " %02x%n", &byte, &offset) == 1) {
            result[i++] = (char) byte;
            data += offset;
        }
    } else {
        result = strDup(str);
    }

    statePushResultStr(result);
    free(str);
    free(encoding);
}


void stdCount(struct result_list *args)
{
    char *str = getValueStr(args->first);
    char *needle = getValueStr(args->first->next);
    size_t i = 0, matches = 0, needle_len = strlen(needle);
    size_t str_len = strlen(str);
    utf8_int32_t codepoint;

    for (void *next = utf8codepoint(str, &codepoint);
        i < str_len;
         next = utf8codepoint(next, &codepoint)) {
        if (utf8str(&str[i], needle) == &str[i]) {
            matches++;
            i += needle_len;
        } else {
            i += utf8codepointsize(codepoint);
        }
    }

    statePushResultD(matches);
    free(str);
    free(needle);
}


void stdIsUpper(struct result_list *args)
{
    char *str = getValueStr(args->first);
    statePushResultD(checkCase(str, false));
    free(str);
}


void stdIsLower(struct result_list *args)
{
    char *str = getValueStr(args->first);
    statePushResultD(checkCase(str, true));
    free(str);
}


void stdCapitalize(struct result_list *args)
{
    char *str = getValueStr(args->first);
    utf8_int32_t codepoint;
    void *next = utf8codepoint(str, &codepoint);
    utf8_int32_t codepoint_upper = utf8uprcodepoint(codepoint);
    size_t next_len = strlen(next);
    size_t codepoint_upper_size = utf8codepointsize(codepoint_upper);
    char *result = calloc_(1, codepoint_upper_size + next_len + 1);

    utf8catcodepoint(result, codepoint_upper, codepoint_upper_size);
    memmove(&result[codepoint_upper_size], next, next_len + 1);

    statePushResultStr(result);
    free(str);
}


void stdFromCharCodes(struct result_list *args)
{
    struct result *arg = args->first;
    char *result = strInit();

    while (arg != NULL) {
        utf8_int32_t codepoint = (int) getValueD(arg);
        size_t codepoint_len = getIntBytes(codepoint);
        char *sub = calloc_(1, codepoint_len + 1);
        utf8catcodepoint(sub, codepoint, codepoint_len);
        strAppend(&result, sub);
        free(sub);

        arg = arg->next;
    }

    statePushResultStr(result);
}
