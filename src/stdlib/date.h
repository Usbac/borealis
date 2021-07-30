#ifndef DATE_H
#define DATE_H

/**
 * Returns the current time as an unix timestamp.
 */
void stdDateNow(struct result_list *args);

/**
 * Returns a string representing the date of the
 * given unix timestamp with the specified format.
 */
void stdDateToString(struct result_list *args);

/**
 * Returns a unix timestamp from the given string date.
 * Valid strings date formats are:
 * "year-month-day hours:minutes:seconds" and "year-month-day".
 */
void stdFromString(struct result_list *args);

/**
 * Returns a date object created from the given unix timestamp.
 * The date object has the properties:
 * 'seconds', 'minutes', 'hours', 'day', 'month', 'year', 'yday' and 'wday'.
 */
void stdDateToObject(struct result_list *args);

/**
 * Returns the time difference between the current timezone and GMT
 * in unix timestamp.
 */
void stdGetGmtOffset(struct result_list *args);

#endif /* DATE_H */
