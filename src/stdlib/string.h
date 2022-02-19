#ifndef STRING_H
#define STRING_H

/**
 * Returns a slice of the string from the given start to the given end.
 */
void stdSlice(struct result_list *args);

/**
 * Returns the string without the trailing characters at both sides.
 */
void stdTrim(struct result_list *args);

/**
 * Returns the string without the leading characters
 */
void stdLeftTrim(struct result_list *args);

/**
 * Returns the string without the trailing characters.
 */
void stdRightTrim(struct result_list *args);

/**
 * Returns the length of the given string.
 */
void stdGetLength(struct result_list *args);

/**
 * Returns the byte length of the given string.
 */
void stdGetByteLength(struct result_list *args);

/**
 * Returns true if the given substring exists in the string, false otherwise.
 */
void stdIncludes(struct result_list *args);

/**
 * Returns the position of the given substring in the string.
 */
void stdIndexOf(struct result_list *args);

/**
 * Returns the last position of the given substring in the string.
 */
void stdLastIndexOf(struct result_list *args);

/**
 * Returns a string divided into a table by the given separator.
 */
void stdSplit(struct result_list *args);

/**
 * Returns true if the string ends with the given substring, false otherwise.
 */
void stdEndsWith(struct result_list *args);

/**
 * Returns true if the string starts with the given substring, false otherwise.
 */
void stdStartsWith(struct result_list *args);

/**
 * Converts the string to uppercase.
 */
void stdToUpper(struct result_list *args);

/**
 * Converts the string to lowercase.
 */
void stdToLower(struct result_list *args);

/**
 * Returns the char at the specified position.
 */
void stdGetChar(struct result_list *args);

/**
 * Repeats the string the given number of times.
 */
void stdRepeat(struct result_list *args);

/**
 * Return true if the string only contains whitespace characters,
 * false otherwise.
 */
void stdIsWhitespace(struct result_list *args);

/**
 * Returns the string with all the instances of the old substring replaced by
 * the new substring.
 */
void stdReplace(struct result_list *args);

/**
 * Returns the string without all the instances of the substring.
 */
void stdRemove(struct result_list *args);

/**
 * Returns true if the given strings are equal, false otherwise.
 */
void stdCompare(struct result_list *args);

/**
 * Returns true if the given strings are equal case insensitive, false otherwise.
 */
void stdCompareI(struct result_list *args);

/**
 * Returns true if all the chars in the given string are inside the value,
 * false otherwise.
 */
void stdContainsChars(struct result_list *args);

/**
 * Interpolates the table values into the string with the format {key}.
 */
void stdInterpolate(struct result_list *args);

/**
 * Returns a formatted string.
 * Formats: %s %d %i %e.
 */
void stdSprintf(struct result_list *args);

/**
 * Pads the start of the string to a certain length with a substring.
 */
void stdPadStart(struct result_list *args);

/**
 * Pads the end of the string to a certain length with a substring.
 */
void stdPadEnd(struct result_list *args);

/**
 * Returns the given string reversed.
 */
void stdStringReverse(struct result_list *args);

/**
 * Encodes the given string with the specified encoding.
 */
void stdEncode(struct result_list *args);

/**
 * Decodes the given string with the specified encoding.
 */
void stdDecode(struct result_list *args);

/**
 * Returns the number of times the given substring occurs in a string.
 */
void stdCount(struct result_list *args);

/**
 * Returns true if all the string characters are uppercase, false otherwise.
 */
void stdIsUpper(struct result_list *args);

/**
 * Returns true if all the string characters are lowercase, false otherwise.
 */
void stdIsLower(struct result_list *args);

/**
 * Returns the given string with the first character in uppercase.
 */
void stdCapitalize(struct result_list *args);

/**
 * Returns a string created from the specified sequence of UTF-8 code units.
 */
void stdFromCharCodes(struct result_list *args);

#endif /* STRING_H */
