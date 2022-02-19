#ifndef ERROR_H_
#define ERROR_H_

#define RUNTIME_ERROR(msg) "Runtime error: "msg
#define PARSER_ERROR(msg) "Parser error: "msg

/* Parser errors */
#define E_TOKEN PARSER_ERROR("Invalid token '%s'")
#define E_INVALID_NAME PARSER_ERROR("Invalid identifier name '%s'")
#define E_END PARSER_ERROR("Unexpected end, expecting '%s'")
#define E_BLOCK_END PARSER_ERROR("Unexpected block end, expected closing tag '%c'")
#define E_FUNC_DEFINITION PARSER_ERROR("Invalid format for function definition")
#define E_FUNC_VARIADIC PARSER_ERROR("Only the last parameter can be variadic")
#define E_FUNC_OPTIONAL PARSER_ERROR("A required parameter cannot be after an optional parameter")
#define E_INDEX_EXPRESSION PARSER_ERROR("Index expression without an element")
#define E_INVALID_LOOP_COND PARSER_ERROR("Invalid condition for loop")
#define E_INVALID_REFERENCE PARSER_ERROR("Expected an identifier for reference")
/* Runtime errors */
#define E_STACK RUNTIME_ERROR("Too many recursive calls (stack overflow)")
#define E_LOOP RUNTIME_ERROR("Cannot %s from a non-loop context")
#define E_FUNC_PARAM_TYPE RUNTIME_ERROR("Expected parameter %zu to be a %s, given %s")
#define E_FUNC_PARAM_STATIC RUNTIME_ERROR("Value for parameter '%s' must be of type %s")
#define E_FUNC_PARAM_NUM RUNTIME_ERROR("Invalid number of arguments supplied, expected ~%zu")
#define E_FUNC_RETURN RUNTIME_ERROR("Return value must be of type %s, returned %s")
#define E_UNDEFINED RUNTIME_ERROR("'%s' is not defined")
#define E_NON_CALLABLE RUNTIME_ERROR("Cannot call a non-function value")
#define E_INDEX_MULTI_STMT RUNTIME_ERROR("Cannot define multiple statements in an index")
#define E_PUSH_ELEMENT RUNTIME_ERROR("Cannot make a push operation to a non-array element")
#define E_INDEX RUNTIME_ERROR("Cannot access the index of a %s")
#define E_NON_OBJECT RUNTIME_ERROR("Cannot access the property of a %s")
#define E_REDECLARE RUNTIME_ERROR("Identifier '%s' already in use")
#define E_CONST_MODIFY RUNTIME_ERROR("Cannot modify constant '%s'")
#define E_STATIC RUNTIME_ERROR("Invalid value for element of type %s")
#define E_INVALID_ITERATOR RUNTIME_ERROR("Invalid value supplied for iterator")
#define E_READ_FILE RUNTIME_ERROR("Cannot read file '%s'")
#define E_RECURSIVE_FILE RUNTIME_ERROR("Cannot import a file recursively '%s'")
#define E_THIS RUNTIME_ERROR("No object context for 'this'")
#define E_NULL_REFERENCE RUNTIME_ERROR("Cannot reference a non-existent element")
#define E_SPREAD RUNTIME_ERROR("Expected spread argument to be an array, given %s")

/**
 * Prints an error message in the standard output and exits the program.
 * @param line_n the line number of the error.
 * @param format the format for the error message.
 */
void errorF(size_t line_n, const char *format, ...);

#endif /* ERROR_H_ */
