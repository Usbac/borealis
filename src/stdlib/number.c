#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "../engine/processor_helper.h"
#include "../state.h"
#include "number.h"


void stdMin(struct result_list *args)
{
    struct result *node = args->first;
    double min = node != NULL ? getValueD(node) : 0;

    while (node != NULL) {
        double n = getValueD(node);
        if (n < min) {
            min = n;
        }

        node = node->next;
    }

    statePushResultD(min);
}


void stdMax(struct result_list *args)
{
    struct result *node = args->first;
    double max = node != NULL ? getValueD(node) : 0;

    while (node != NULL) {
        double n = getValueD(node);
        if (n > max) {
            max = n;
        }

        node = node->next;
    }

    statePushResultD(max);
}


void stdIsNan(struct result_list *args)
{
    statePushResultD(isnan(getValueD(args->first)));
}


void stdIsFinite(struct result_list *args)
{
    statePushResultD(isfinite(getValueD(args->first)));
}


void stdRand(struct result_list *args)
{
    double max = args->first != NULL ? getValueD(args->first) : 1;

    statePushResultD((double) fabs(rand() * max) / RAND_MAX);
}


void stdSRand(struct result_list *args)
{
    srand((int) getValueD(args->first));
}


void stdFormat(struct result_list *args)
{
    int dot_index, char_n = 0;
    char *dot, *tmp = NULL;
    char *str = getValueStr(args->first);
    char *decimals_sep;
    char *thousands_sep = args->first->next != NULL ?
        getValueStr(args->first->next) :
        strDup("");
    size_t len, new_len;

    if (args->first->next != NULL && args->first->next->next != NULL) {
        decimals_sep = getValueStr(args->first->next->next);
        tmp = strReplace(str, ".", decimals_sep);
        free(str);
        str = tmp;
    } else {
        decimals_sep = strDup(".");
    }

    len = strlen(str);
    new_len = len;
    dot = strstr(str, decimals_sep);

    if (dot != NULL) {
        dot_index = dot - str;
        tmp = malloc_(len - dot_index + 1);
        snprintf(tmp, len - dot_index + 1, "%s", str + dot_index);
    } else {
        dot_index = len;
        tmp = malloc_(1);
        tmp[0] = '\0';
    }

    for (int i = dot_index - 1; i >= 0; i--) {
        if (i >= 0 && char_n > 0 && char_n % 3 == 0) {
            strPrepend(&tmp, thousands_sep);
            new_len += strlen(thousands_sep);
        }

        strPrependC(&tmp, str[i]);
        char_n++;
    }

    statePushResultStr(tmp);

    free(str);
    free(thousands_sep);
    free(decimals_sep);
}
