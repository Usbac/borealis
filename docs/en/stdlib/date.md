The standard object `Date` gives you multiple methods related with dates.

_All of the unix timestamps explained below are in (or expected to be given in) seconds._

## Functions

### Current date

`now(): number`

Returns the current time as a unix timestamp.

```borealis
Date.now(); # Can return something like 1620149607
```

### Date to string

`toString(string format, number date): string`

Returns a string representing the date of the given unix timestamp with the specified format.

```borealis
Date.toString('%Y-%m-%d', 1617557432); # Returns '2021-04-04'
```

```borealis
Date.toString('%Y', Date.now()); # Will return the current year
```

This function uses internally the `strftime` function of C, so it follows the same format specifiers.

| Specifier| Description                                                | Example                 |
|----------|----------------------------------------------------------- |-------------------------|
| %a       | Abbreviated weekday name                                   | Sun                     |
| %A       | Full weekday name                                          | Sunday                  |
| %b       | Abbreviated month name	                                    | Mar                     |
| %B       | Full month name	                                        | March                   |
| %c       | Date and time representation	                            | Tue May 4 16:25:44 2021 |
| %d       | Day of the month (01-31)                                   | 11                      |
| %H       | Hour in 24h format (00-23)                                 | 16                      |
| %I       | Hour in 12h format (01-12)                                 | 02                      |
| %j       | Day of the year (001-366)                                  | 234                     |
| %m       | Month as a decimal number (01-12)                          | 09                      |
| %M       | Minute (00-59)                                             | 58                      |
| %p       | AM or PM designation                                       | AM                      |
| %S       | Second (00-61)                                             | 06                      |
| %U       | Week number with the first Sunday as the first day (00-53) | 33                      |
| %w       | Weekday as a decimal number with Sunday as 0 (0-6)         | 5                       |
| %W       | Week number with the first Monday as the first day (00-53) | 44                      |
| %x       | Date representation	                                    | 08/19/12                |
| %X       | Time representation	                                    | 05:54:09                |
| %y       | Year, last two digits (00-99)                              | 21                      |
| %Y       | Year                                                       | 2021                    |
| %Z       | Timezone name or abbreviation	                            | CDT                     |
| %%       | A % sign                                                   | %                       |

### Date from string

`fromString(string date): number`

Returns a unix timestamp from the given string date.

Valid date formats are 'year-month-day hours:minutes:seconds' (%Y-%m-%d %H:%M:%S) and 'year-month-day' (%Y-%m-%d).

```borealis
Date.fromString('2021-04-04 13:30:32'); # Returns 1617557432
```

### Date to object

`toObject(number date): object`

Returns an object with data from the given unix timestamp.

The date object has the following properties:

| Name    | Description      |
|---------|------------------|
| seconds | Seconds.         |
| minutes | Minutes.         |
| hours   | Hours.           |
| day     | Day.             |
| month   | Month.           |
| year    | Year.            |
| yday    | Day of the year. |
| wday    | Day of the week. |

```borealis
Date.toObject(1617557432);

/* The above code will return:
 * obj {
 *     pub any seconds = 32;
 *     pub any minutes = 30;
 *     pub any hours = 13;
 *     pub any day = 4;
 *     pub any month = 4;
 *     pub any year = 2021;
 *     pub any yday = 94;
 *     pub any wday = 1;
 * }
 */
```

```
object date = Date.toObject(1617557432);

printLine("The date's year is: " ^ date.year);
```

### Get GMT offset

`getGMTOffset(): number`

Returns the unix timestamp difference between the current timezone and GMT.

```borealis
Date.getGMTOffset(); # In New York it will return -14400
```

```borealis
number offset = Date.getGMTOffset() / 60 / 60;

printLine('Your GMT offset in hours is: ' ^ offset);
```
