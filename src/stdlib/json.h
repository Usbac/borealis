#ifndef JSON_H
#define JSON_H

/**
 * Returns a new table from the given json.
 * @param list the json tokens list.
 * @param error the error status.
 * @return the new table.
 */
struct element_table *parseArrjJson(struct token_list *list, bool *error);

/**
 * Returns a new table from the given json.
 * @param list the json tokens list.
 * @param error the error status.
 * @return the new table.
 */
struct element_table *parseObjJson(struct token_list *list, bool *error);

/**
 * Returns the given value as a JSON string.
 */
void stdJsonStringify(struct result_list *args);

/**
 * Returns the value of the given JSON string.
 */
void stdJsonParse(struct result_list *args);

/**
 * Returns true if the given string is a valid JSON, false otherwise.
 */
void stdIsvalid(struct result_list *args);

#endif /* JSON_H */
