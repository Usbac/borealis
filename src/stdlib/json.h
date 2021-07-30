#ifndef JSON_H
#define JSON_H

/**
 * Returns a new array from the given json.
 * @param list the json tokens list.
 * @return the new array.
 */
struct element_table *parseArrjJson(struct list *list);

/**
 * Returns a new object from the given json.
 * @param list the json tokens list.
 * @return the new object.
 */
struct element_table *parseObjJson(struct list *list);

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
