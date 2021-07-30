#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_SEP ";"
#define NULL_KEYWORD "null"

enum TYPE {
    T_Null, T_Identifier,
    T_Number, T_String,
    T_Keyword, T_Operator,
    T_Chunk, T_Parameters,
    T_Index, T_Array,
    T_Object, T_Function,
    T_Arguments, T_Reference,
};

enum OPCODE {
    OP_None, OP_Negation,
    OP_Plus, OP_Minus,
    OP_Multi, OP_Div,
    OP_Negative, OP_Positive,
    OP_Assignation, OP_Plus_equal,
    OP_Minus_equal, OP_Multi_equal,
    OP_Div_equal, OP_Concat_equal,
    OP_Lower, OP_Higher,
    OP_Lower_equal, OP_Higher_equal,
    OP_And, OP_Or, OP_Comma,
    OP_Compare, OP_Different,
    OP_Compare_string, OP_Different_string,
    OP_Compare_strict, OP_Different_strict,
    OP_Stmt_sep, OP_Open_square,
    OP_Closed_square, OP_Open_parenthesis,
    OP_Closed_parenthesis, OP_Closure,
    OP_Return, OP_If,
    OP_Else, OP_While,
    OP_Foreach, OP_Break,
    OP_Continue, OP_Definition,
    OP_Concat, OP_Public,
    OP_Import, OP_Bang,
    OP_Spread, OP_Dot,
    OP_Reference, OP_Colon,
    OP_Case, OP_Pow,
    OP_Object, OP_This,
    OP_Dot_safe, OP_Type_string,
    OP_Type_number, OP_Type_array,
    OP_Type_object, OP_Type_function,
    OP_Constant, OP_Increment_pre,
    OP_Increment_pos, OP_Decrement_pre,
    OP_Decrement_pos, OP_Mod,
    OP_Mod_equal, OP_Null_coalesce,
};

struct operator {
    enum OPCODE opcode;
    char *string;
    uint8_t precedence;
    bool right_associated;
    struct operator *next;
};

struct token {
    char *value;
    enum TYPE type;
    enum OPCODE opcode;
    size_t line_n;
    struct list *body;
    size_t jmp;
    struct token *prev;
    struct token *next;
    struct token *ls;
    struct token *rs;
};

struct list {
    struct token *first;
    struct token *last;
};


extern struct operator *operators_head;

/**
 * Initializes the lexer.
 */
void initLexer(void);

/**
 * Frees the lexer.
 */
void freeLexer(void);

/**
 * Returns a list based on the given string with infix notation.
 * @param code the code.
 * @param stmt_sep the statement separator.
 * @param line_n the starting line number.
 * @return the list based on the given string.
 */
struct list *tokenize(const char *code, const char *stmt_sep, size_t line_n);

/**
 * Returns a list based on the given json string.
 * @param code the json code.
 * @param error the error status to update.
 * @return the list.
 */
struct list *tokenizeJson(const char *code, bool *error);

/**
 * Returns a new list initialized.
 * return the new list.
 */
struct list *newList(void);

/**
 * Returns true if the given opcode is a keyword, false otherwise.
 * @param opcode the opcode.
 * @return true if the given opcode is a keyword, false otherwise.
 */
bool isKeyword(enum OPCODE opcode);

/**
 * Returns true if the given opcode is an operator, false otherwise.
 * @param opcode the opcode.
 * @return true if the given opcode is an operator, false otherwise.
 */
bool isOperator(enum OPCODE opcode);

/**
 * Returns true if the given opcode is a static keyword, false otherwise.
 * @param opcode the opcode.
 * @return true if the given opcode is a static keyword, false otherwise.
 */
bool isDefKeyword(enum OPCODE opcode);

/**
 * Returns true if the given type is a code block, false otherwise.
 * @param type the type.
 * @return true if the given type is a code block, false otherwise.
 */
bool isPartial(enum TYPE type);

/**
 * Returns true if the given string represents a valid number, false otherwise.
 * @param str the string.
 * @return true if the given string represents a valid number, false otherwise.
 */
bool isNumber(const char *str);
