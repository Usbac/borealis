#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "../engine/processor_helper.h"
#include "../state.h"
#include "date.h"

const char *DATE_FORMAT = "%d-%d-%d %d:%d:%d";

const char *DATE_FORMAT_SHORT = "%d-%d-%d";


static struct element *getTimePropEl(char *key, double val)
{
    struct element *el = newElement(key, NULL, 0, T_Number);
    el->value.number = val;
    el->public = true;
    return el;
}


void stdDateNow(struct result_list *args)
{
    pushResultD(time(NULL));
}


void stdDateToString(struct result_list *args)
{
    const int SIZE = 2048;
    char *result = calloc_(1, SIZE);
    char *format = getValueStr(args->first);
    time_t secs = getValueD(args->first->next);
    struct tm *time = localtime(&secs);

    strftime(result, SIZE - 1, format, time);

    pushResultStr(result);
    free(format);
}


void stdFromString(struct result_list *args)
{
    struct tm tm = {0};
    int dd, mm, yy, hrs, min, sec;
    char *str = getValueStr(args->first);

    if (sscanf(str, DATE_FORMAT, &yy, &mm, &dd, &hrs, &min, &sec) == 6 ||
        sscanf(str, DATE_FORMAT_SHORT, &yy, &mm, &dd) == 3) {
        goto ok;
    } else {
        pushResultD(-1);
        goto end;
    }

    ok:
    tm.tm_sec = sec;
    tm.tm_min = min;
    tm.tm_hour = hrs;
    tm.tm_mday = dd;
    tm.tm_mon = mm - 1;
    tm.tm_year = yy - 1900;
    pushResultD(mktime(&tm));
    end: free(str);
}


void stdDateToObject(struct result_list *args)
{
    time_t secs = getValueD(args->first);
    struct tm *time = localtime(&secs);
    struct element_table *obj = newElementTable();

    pushElementToTable(&obj, getTimePropEl("seconds", time->tm_sec));
    pushElementToTable(&obj, getTimePropEl("minutes", time->tm_min));
    pushElementToTable(&obj, getTimePropEl("hours", time->tm_hour));
    pushElementToTable(&obj, getTimePropEl("day", time->tm_mday));
    pushElementToTable(&obj, getTimePropEl("month", time->tm_mon + 1));
    pushElementToTable(&obj, getTimePropEl("year", time->tm_year + 1900));
    pushElementToTable(&obj, getTimePropEl("yday", time->tm_yday + 1));
    pushElementToTable(&obj, getTimePropEl("wday", time->tm_wday + 1));
    pushResultObj(obj);
}


void stdGetGmtOffset(struct result_list *args)
{
    time_t now = time(NULL);
    pushResultD((double) now - mktime(gmtime(&now)));
}
