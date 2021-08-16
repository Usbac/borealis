#ifndef PROCESSOR_HELPER_H_
#define PROCESSOR_HELPER_H_

#include <stdbool.h>
#include "../element.h"

#define TYPEOF_STRING "string"
#define TYPEOF_NUMBER "number"
#define TYPEOF_ARRAY "array"
#define TYPEOF_OBJECT "object"
#define TYPEOF_FUNCTION "function"
#define TYPEOF_NULL "null"

/**
 * Sets the given type and value based on the given result.
 * @param node the result.
 * @param type the type to set.
 * @param value the value to set.
 */
void mapTypeVal(const struct result *node, enum TYPE *type, union VALUE *value);

/**
 * Returns the string value of the given union.
 * @param type the union type.
 * @param value the union value.
 * @return the string value of the given union.
 */
char *getUnionStr(enum TYPE type, union VALUE value);

/**
 * Returns the double value of the given node.
 * @param node the node.
 * @return the double value of the given node.
 */
double getValueD(struct result *node);

/**
 * Returns the element value of the given node.
 * @param node the node.
 * @return the element value of the given node.
 */
struct element *getValueEl(const struct result *node);

/**
 * Returns the value of the given node as an array.
 * @param node the node.
 * @return the value of the given node as an array.
 */
struct element_table *getValueArr(const struct result *node);

/**
 * Returns the value of the given node as an object.
 * @param node the node.
 * @return the value of the given node as an object.
 */
struct element_table *getValueObj(const struct result *node);

/**
 * Returns the value of the given node as a function.
 * @param node the node.
 * @return the value of the given node as a function.
 */
struct function *getValueFunc(const struct result *node);

/**
 * Returns the value of the given node as a string.
 * @param node the node.
 * @return the value of the given node as a string.
 */
char *getValueStr(struct result *node);

/**
 * Returns the true element.
 * Useful when dealing with elements that are a reference.
 * @param node the element.
 * @return the true element.
 */
struct element *getTrueElement(struct element *node);

/**
 * Returns true if the node represents a truly value, false otherwise.
 * (Only numeric values that are not equal to zero,
 * and string values that are not empty can be
 * considered true).
 * @param node the node.
 * @return true if the given node represents
 * a truly value, false otherwise.
 */
bool isTrue(const struct result *node);

/**
 * Returns true if the node represents a return statement without value, false otherwise.
 * @param node the node.
 * @return true if the node represents a return statement without value, false otherwise.
 */
bool isEmptyReturn(const struct token *node);

/**
 * Returns true if both results have equal values, false otherwise.
 * @param a the first result.
 * @param b the second result.
 * @return true if both results have equal values, false otherwise.
 */
bool compareResults(struct result *a, struct result *b);

/**
 * Returns the type of the given node.
 * @param node the node.
 * @return the type of the given node.
 */
enum TYPE getResultType(struct result *node);

/**
 * Returns the given element type as a string.
 * @param type the type.
 * @return the element type as a string.
 */
char *getElementTypeAsStr(enum TYPE type);

/**
 * Returns the result of an operation.
 * @param opcode the operator opcode.
 * @param a the first value.
 * @param b the second value.
 * @return the result of the operation.
 */
double getOpResult(enum OPCODE opcode, double a, double b);

/**
 * Creates a new result from the given element.
 * @param el the element.
 * @return the result.
 */
struct result *getResultFromElement(struct element *el);

/**
 * Maps the result value into the given element.
 * @param el the element.
 * @param node the result.
 */
void mapResultToElement(struct element *el, struct result *node);

/**
 * Returns a partial backup of the current state
 * after freeing some values of the original.
 * @param obj the current object.
 * @param file the new file for the current state.
 * @return the copy of the current state.
 */
struct state *saveState(struct element_table *obj, char *file);

/**
 * Partially restores the current state based on the given one.
 * @param aux the state.
 */
void restoreState(struct state *aux);

/**
 * Moves forward the given node by its number of jumps.
 * @param node the node.
 */
void jmpStmts(struct token **node);

/**
 * Returns the numbers of parameters in the given list.
 * @param params the parameters list.
 * @return the number of parameters.
 */
size_t getParamsNumber(struct token_list *params);

/**
 * Returns true if the node is the identifier of a spread parameter,
 * false otherwise.
 * @param node the node.
 * @return true if the node is the identifier of a spread parameter,
 * false otherwise.
 */
bool inSpreadParam(struct token *node);

/**
 * Returns true if the node is a function definition, false otherwise.
 * @param node the node.
 * @return true if the node is a function definition, false otherwise.
 */
bool isFunctionNode(struct token *node);

/**
 * Validates the modification of an element.
 * @param type the type of the new value for the element.
 * @param identifier the identifier result.
 */
void validateElementMod(struct result *identifier, enum TYPE type);

/**
 * Returns the type equilavent of the given opcode.
 * @param opcode the opcode.
 * @return the type representation.
 */
enum TYPE getOpcodeType(enum OPCODE opcode);

#endif /* PROCESSOR_HELPER_H_ */
