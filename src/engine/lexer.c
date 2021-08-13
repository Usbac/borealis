#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../../lib/utf8.h"
#include "../utils.h"
#include "../error.h"
#include "lexer.h"

struct operator *operators_head = NULL;


static void addOperator(const char *string,
                        enum OPCODE opcode,
                        size_t precedence,
                        bool right_associated)
{
    struct operator *new = malloc_(sizeof(struct operator));
    *new = (struct operator) {
        .opcode = opcode,
        .string = strDup(string),
        .precedence = precedence,
        .right_associated = right_associated,
    };

    new->next = operators_head;
    operators_head = new;
}


static enum OPCODE getTokenOpcode(const char *str, const char *stmt_sep)
{
    struct operator *node = operators_head;

    if (!strcmp(str, stmt_sep)) {
        return OP_Stmt_sep;
    }

    while (node != NULL) {
        if (!strcmp(str, node->string)) {
            return node->opcode;
        }

        node = node->next;
    }

    return OP_None;
}


static enum TYPE getTokenType(const char *str, enum OPCODE opcode)
{
    if (isKeyword(opcode)) {
        return T_Keyword;
    } else if (isOperator(opcode)) {
        return T_Operator;
    } else if (isNumber(str)) {
        return T_Number;
    } else if ((opcode == OP_None && !strcmp(str, NULL_KEYWORD)) ||
        !strcmp(str, SEPARATOR_DEFAULT)) {
        return T_Null;
    }

    return T_Identifier;
}


static bool isNumberSign(const struct token *last, const struct token *node)
{
    if (node->opcode != OP_Minus && node->opcode != OP_Plus) {
        return false;
    } else if (last == NULL) {
        return true;
    }

    return
        last->type != T_String && last->type != T_Number &&
        last->type != T_Identifier && last->type != T_Parameters &&
        last->type != T_Index && last->type != T_Null &&
        last->type != T_Array && last->opcode != OP_Closed_parenthesis &&
        last->opcode != OP_Decrement_pre && last->opcode != OP_Decrement_pos &&
        last->opcode != OP_Increment_pre && last->opcode != OP_Increment_pos &&
        last->type != T_Arguments;
}


static void tokenListPush(struct token_list *list,
                          const char *token,
                          enum TYPE type,
                          enum OPCODE opcode,
                          size_t line_n)
{
    struct token *new = malloc_(sizeof(struct token));
    *new = (struct token) {
        .opcode = opcode,
        .line_n = line_n,
        .type = type,
        .value = strDup(token),
    };

    if (isNumberSign(list->last, new)) {
        new->opcode = new->opcode == OP_Minus ? OP_Negative : OP_Positive;
    }

    if (list->last != NULL &&
        (list->last->type == T_Identifier || list->last->type == T_Index)) {
        if (opcode == OP_Increment_pre || opcode == OP_Increment_pos) {
            new->opcode = OP_Increment_pos;
        } else if (opcode == OP_Decrement_pre || opcode == OP_Decrement_pos) {
            new->opcode = OP_Decrement_pos;
        }
    }

    if (list->last == NULL) {
        list->last = new;
        list->first = list->last;
    } else {
        list->last->next = new;
        new->prev = list->last;
        list->last = new;
    }
}


void lexerInit(void)
{
    addOperator("!", OP_Negation, 12, true);
    addOperator("-", OP_Negative, 12, true);
    addOperator("+", OP_Positive, 12, true);
    addOperator("^", OP_Concat, 9, false);
    addOperator("+", OP_Plus, 10, false);
    addOperator("-", OP_Minus, 10, false);
    addOperator("*", OP_Multi, 11, false);
    addOperator("/", OP_Div, 11, false);
    addOperator("%", OP_Mod, 11, false);
    addOperator("=", OP_Assignation, 3, true);
    addOperator("+=", OP_Plus_equal, 3, true);
    addOperator("-=", OP_Minus_equal, 3, true);
    addOperator("*=", OP_Multi_equal, 3, true);
    addOperator("/=", OP_Div_equal, 3, true);
    addOperator("^=", OP_Concat_equal, 3, true);
    addOperator("%=", OP_Mod_equal, 3, true);
    addOperator("<", OP_Lower, 8, false);
    addOperator(">", OP_Higher, 8, false);
    addOperator("<=", OP_Lower_equal, 8, false);
    addOperator(">=", OP_Higher_equal, 8, false);
    addOperator("&&", OP_And, 6, false);
    addOperator("||", OP_Or, 5, false);
    addOperator("public", OP_Public, 0, false);
    addOperator("return", OP_Return, 0, false);
    addOperator("const", OP_Constant, 1, false);
    addOperator("...", OP_Spread, 2, false);
    addOperator("string", OP_Type_string, 2, false);
    addOperator("number", OP_Type_number, 2, false);
    addOperator("array", OP_Type_array, 2, false);
    addOperator("object", OP_Type_object, 2, false);
    addOperator("function", OP_Type_function, 2, false);
    addOperator("any", OP_Definition, 2, true);
    addOperator("==", OP_Compare, 7, false);
    addOperator("!=", OP_Different, 7, false);
    addOperator("eq", OP_Compare_string, 7, false);
    addOperator("ne", OP_Different_string, 7, false);
    addOperator("===", OP_Compare_strict, 7, false);
    addOperator("!==", OP_Different_strict, 7, false);
    addOperator(",", OP_Comma, 0, false);
    addOperator("(", OP_Open_parenthesis, 18, false);
    addOperator(")", OP_Closed_parenthesis, 18, false);
    addOperator("[", OP_Open_square, 0, false);
    addOperator("]", OP_Closed_square, 0, false);
    addOperator("else", OP_Else, 13, false);
    addOperator("break", OP_Break, 13, false);
    addOperator("continue", OP_Continue, 13, false);
    addOperator("obj", OP_Object, 14, false);
    addOperator(".", OP_Dot, 15, false);
    addOperator("?.", OP_Dot_safe, 15, false);
    addOperator(":", OP_Colon, 15, false);
    addOperator("++", OP_Increment_pos, 15, false);
    addOperator("--", OP_Decrement_pos, 15, false);
    addOperator("++", OP_Increment_pre, 12, true);
    addOperator("--", OP_Decrement_pre, 12, true);
    addOperator("??", OP_Null_coalesce, 4, true);
    addOperator("**", OP_Pow, 16, false);
    addOperator("&", OP_Reference, 12, true);
    addOperator("import", OP_Import, 12, true);
    addOperator("f", OP_Closure, 17, false);
    addOperator("if", OP_If, 17, false);
    addOperator("while", OP_While, 17, false);
    addOperator("foreach", OP_Foreach, 17, false);
    addOperator("case", OP_Case, 17, false);
    addOperator("this", OP_This, 17, false);
    addOperator("<<", OP_Bang, 2, false);
}


void lexerFree(void)
{
    struct operator *node;

    while ((node = operators_head) != NULL) {
        operators_head = operators_head->next;
        free(node->string);
        free(node);
    }
}


static inline bool isSign(const char ch)
{
    return ch == '+' || ch == '-';
}


bool isNumber(const char *str)
{
    const size_t len = strlen(str);
    bool has_dot = false;
    size_t i = isSign(str[0]);

    for (; i < len; i++) {
        /* Exponent number */
        if (i != 0 && i+1 < len && str[i] == 'e') {
            continue;
        }

        if (str[i] == '.') {
            if (has_dot) {
                return false;
            }

            has_dot = true;
            continue;
        }

        if (isSign(str[i]) && i != 0 && str[i-1] == 'e') {
            continue;
        }

        if (!isdigit(str[i])) {
            return false;
        }
    }

    return true;
}


static bool isReserved(const char *str, size_t i, size_t len)
{
    const char ch = str[i];
    const char next = i+1 <= len ?
        str[i+1] :
        '\0';

    if (ch == 'e' && (isdigit(next) || isSign(next))) {
        return true;
    }

    if (isSign(ch) && i != 0 && str[i-1] == 'e') {
        return true;
    }

    if ((ch == '+' && next == '+') ||
        (ch == '-' && next == '-')) {
        return true;
    }

    if (ch == '?' && (next == '?' || next == '.')) {
        return true;
    }

    if (ch == ':' && (next == '=' || next == ':')) {
        return true;
    }

    if (ch == '*' && next == '*') {
        return true;
    }

    if (ch == '!' && next == ':') {
        return true;
    }

    if (ch == '<' && next == '<') {
        return true;
    }

    if ((ch == '&' && next == '&') ||
        (ch == '|' && next == '|')) {
        return true;
    }

    if (next == '=' &&
        (ch == '=' || ch == '>' || ch == '<' ||
        ch == '/' || ch == '*' || ch == '^' ||
        ch == '+' || ch == '-' || ch == '!' ||
        ch == '%')) {
        return true;
    }

    if ((isdigit(ch) && next == '.' && i+2 <= len && isdigit(str[i+2])) ||
        (i != 0 && isdigit(str[i-1]) && ch == '.' && isdigit(next))) {
        return true;
    }

    if (ch == '.' && next == '.') {
        return true;
    }

    if ((isalnum(ch) || ch == '_') &&
        (isalnum(next) || next == '_')) {
        return true;
    }

    return false;
}


static bool isValidIdentifier(const char *str)
{
    const size_t len = strlen(str);

    if (str[0] != '_' && !isalpha(str[0])) {
        return false;
    }

    for (size_t i = 1; i < len; i++) {
        if (str[i] != '_' && !isalnum(str[i])) {
            return false;
        }
    }

    return true;
}


bool isKeyword(enum OPCODE opcode)
{
    return isDefKeyword(opcode) || opcode == OP_Break ||
        opcode == OP_Continue || opcode == OP_Return ||
        opcode == OP_Closure || opcode == OP_If ||
        opcode == OP_Else || opcode == OP_While ||
        opcode == OP_Foreach || opcode == OP_Public ||
        opcode == OP_Import || opcode == OP_Case ||
        opcode == OP_Object || opcode == OP_This ||
        opcode == OP_Constant;
}


bool isOperator(enum OPCODE opcode)
{
    return
        opcode == OP_Concat || opcode == OP_Concat_equal ||
        opcode == OP_Plus || opcode == OP_Minus ||
        opcode == OP_Multi || opcode == OP_Div ||
        opcode == OP_Positive || opcode == OP_Negative ||
        opcode == OP_Plus_equal || opcode == OP_Minus_equal ||
        opcode == OP_Multi_equal || opcode == OP_Div_equal ||
        opcode == OP_Lower || opcode == OP_Higher ||
        opcode == OP_Lower_equal || opcode == OP_Higher_equal ||
        opcode == OP_And || opcode == OP_Or ||
        opcode == OP_Compare || opcode == OP_Null_coalesce ||
        opcode == OP_Compare_string || opcode == OP_Different_string ||
        opcode == OP_Compare_strict || opcode == OP_Different_strict ||
        opcode == OP_Stmt_sep || opcode == OP_Assignation ||
        opcode == OP_Foreach || opcode == OP_Comma ||
        opcode == OP_Open_parenthesis || opcode == OP_Closed_parenthesis ||
        opcode == OP_Negation || opcode == OP_Different ||
        opcode == OP_Spread || opcode == OP_Dot ||
        opcode == OP_Reference || opcode == OP_Colon ||
        opcode == OP_Pow || opcode == OP_Dot_safe ||
        opcode == OP_Decrement_pre || opcode == OP_Decrement_pos ||
        opcode == OP_Increment_pre || opcode == OP_Increment_pos ||
        opcode == OP_Mod || opcode == OP_Mod_equal ||
        opcode == OP_Bang;
}


bool isDefKeyword(enum OPCODE opcode)
{
    return opcode == OP_Definition || opcode == OP_Type_string ||
        opcode == OP_Type_number || opcode == OP_Type_array ||
        opcode == OP_Type_object || opcode == OP_Type_function;
}


bool isPartial(enum TYPE type)
{
    return type == T_Chunk || type == T_Parameters ||
        type == T_Index || type == T_Array ||
        type == T_Arguments;
}


static bool isIndex(const struct token_list *list)
{
    return list->last != NULL &&
        (list->last->type == T_Array || list->last->type == T_Identifier ||
        list->last->type == T_String || list->last->type == T_Number ||
        list->last->type == T_Arguments || list->last->type == T_Index ||
        list->last->type == T_Null || list->last->opcode == OP_Closed_parenthesis);
}


static void processOctalEsc(char **token, const char *str, size_t *i, size_t len)
{
    char *octal = strInit();
    size_t aux_i = 0;

    while (*i + aux_i < len) {
        char ch = str[*i + aux_i + 1];
        if (ch >= '0' && ch < '8' && aux_i < 3) {
            strAppendC(&octal, ch);
        } else {
            strAppendC(token, (char) strtol(octal, NULL, 8));
            free(octal);
            *i += aux_i - 1;

            break;
        }

        aux_i++;
    }
}


static void processCharEsc(char **token,
                           const char *str,
                           size_t *i,
                           size_t len,
                           size_t chars_n)
{
    char *unicode = strInit();
    size_t aux_i = 0;

    while (*i + aux_i < len) {
        char ch = str[*i + aux_i + 1];
        if (aux_i < chars_n) {
            strAppendC(&unicode, ch);
        } else {
            utf8_int32_t codepoint = (int) strtol(unicode, NULL, 16);
            size_t bytes_n = getIntBytes(codepoint);
            char *sub = calloc_(1, bytes_n + 1);
            utf8catcodepoint(sub, codepoint, bytes_n);
            strAppend(token, sub);
            free(unicode);
            free(sub);
            *i += aux_i - 1;
            break;
        }

        aux_i++;
    }
}


static bool processEscSeq(char **token, const char *str, size_t len, size_t *i)
{
    if (*i+1 >= len || str[*i] != '\\') {
        return false;
    }

    switch (str[*i+1]) {
        case 'n': strAppendC(token, '\n'); goto added;
        case 't': strAppendC(token, '\t'); goto added;
        case 'r': strAppendC(token, '\r'); goto added;
        case '0': strAppendC(token, '\0'); goto added;
        default:
            if (isdigit(str[*i+1])) {
                processOctalEsc(token, str, i, len);
                goto added;
            } else if (str[*i+1] == 'u' || str[*i+1] == 'x') {
                (*i)++;
                processCharEsc(token, str, i, len, str[*i] == 'x' ? 2 : 4);
                goto added;
            } else if (str[*i+1] != '\\') {
                strAppendC(token, '\\');
                strAppendC(token, str[*i+1]);
                goto added;
            }

            return false;
    }

    added:
        (*i)++;
        return true;
}


static void addStringChars(char **token,
                           const char *str,
                           size_t len,
                           size_t *i,
                           size_t *line_n,
                           bool add_all)
{
    bool esc = false;

    /* empty string */
    if (*i+1 <= len && str[*i+1] == '"') {
        (*i)++;
        return;
    }

    while (++(*i) < len) {
        if (!esc && processEscSeq(token, str, len, i)) {
            goto check_end;
        }

        esc = str[*i] == '\\' && !esc;
        if (!esc || add_all) {
            strAppendC(token, str[*i]);
        }

        *line_n += str[*i] == '\n';

        check_end:
            if (!esc && *i+1 < len && str[*i+1] == '"') {
                break;
            }
    }

    (*i)++;
}


static void addRawStringChars(char **token,
                              const char *str,
                              size_t len,
                              size_t *i,
                              size_t *line_n)
{
    bool esc = false;

    /* empty string */
    if (*i+1 <= len && str[*i+1] == '\'') {
        (*i)++;
        return;
    }

    while (++(*i) < len) {
        esc = !esc && *i < len && str[*i] == '\\' &&
            (str[(*i)+1] == '\'' || str[(*i)+1] == '\\');
        if (!esc) {
            strAppendC(token, str[*i]);
        }

        *line_n += str[*i] == '\n';

        if (!esc && *i+1 < len && str[*i+1] == '\'') {
            break;
        }
    }

    (*i)++;
}


static void processString(char **token,
                          struct token_list *list,
                          const char *str,
                          bool raw,
                          size_t len,
                          size_t *i,
                          size_t *line_n)
{
    if (raw) {
        addRawStringChars(token, str, len, i, line_n);
    } else {
        addStringChars(token, str, len, i, line_n, false);
    }

    tokenListPush(list, *token, T_String, OP_None, *line_n);
    strEmpty(token);
}


static void processScope(char **token,
                         const char *str,
                         size_t len,
                         size_t *i,
                         size_t *line_n,
                         const char start,
                         const char end)
{
    int64_t scoping = 1;

    while (++(*i) < len && scoping > 0) {
        *line_n += str[*i] == '\n';

        if (str[*i] == '"') {
            strAppendC(token, str[*i]);
            addStringChars(token, str, len, i, line_n, true);
        }

        if (str[*i] == start) {
            scoping++;
        } else if (str[*i] == end) {
            scoping--;
        }

        if (scoping > 0) {
            strAppendC(token, str[*i]);
        }
    }

    if (scoping != 0) {
        errorF(*line_n, E_BLOCK_END, end);
    }
}


static void processLineComment(const char *str, size_t *i, size_t *line_n)
{
    while (*i <= strlen(str) && str[*i] != '\n') {
        (*i)++;
    }

    (*line_n)++;
}


static void processComment(const char *str,
                           size_t len,
                           size_t *i,
                           size_t *line_n)
{
    *i += 2;
    while (++(*i) <= len && !(str[*i] == '*' && str[(*i) + 1] == '/')) {
        *line_n += str[*i] == '\n';
    }

    (*i)++;
}


static void processChunk(char **token,
                         struct token_list *list,
                         const char *str,
                         size_t len,
                         size_t *i,
                         size_t *line_n)
{
    size_t ori_line_n = *line_n;
    processScope(token, str, len, i, line_n, '{', '}');
    tokenListPush(list, *token, T_Chunk, OP_None, ori_line_n);
    strEmpty(token);
    (*i)--;
}


static void processIndexArray(char **token,
                              struct token_list *list,
                              const char *str,
                              size_t len,
                              size_t *i,
                              size_t *line_n)
{
    size_t ori_line_n = *line_n;
    enum TYPE type = isIndex(list) ? T_Index : T_Array;

    processScope(token, str, len, i, line_n, '[', ']');
    tokenListPush(list, *token, type, OP_None, ori_line_n);

    strEmpty(token);
    (*i)--;
}


static bool inParametersList(struct token_list *list)
{
    return list->last != NULL && (list->last->opcode == OP_Closure ||
        (list->last->type == T_Identifier && list->last->prev != NULL &&
        isDefKeyword(list->last->prev->opcode)));
}


static void processParameters(char **token,
                              struct token_list *list,
                              const char *str,
                              size_t len,
                              size_t *i,
                              size_t *line_n)
{
    size_t ori_line_n = *line_n;
    enum TYPE type = inParametersList(list) ? T_Parameters : T_Arguments;

    processScope(token, str, len, i, line_n, '(', ')');
    tokenListPush(list, *token, type, OP_None, ori_line_n);
    strEmpty(token);
    (*i)--;
}


static void validateToken(char *token,
                          struct token_list *list,
                          enum OPCODE opcode,
                          enum TYPE type,
                          size_t line_n)
{
    if (type == T_Identifier && !isValidIdentifier(token)) {
        errorF(line_n, E_INVALID_NAME, token);
    }

    if (opcode != OP_Open_parenthesis && list->last != NULL &&
        (list->last->opcode == OP_While || list->last->opcode == OP_If ||
        list->last->opcode == OP_Foreach || list->last->opcode == OP_Case)) {
        errorF(line_n, E_TOKEN, token);
    }
}


static void processToken(char **token,
                         struct token_list *list,
                         const char *stmt_sep,
                         size_t line_n)
{
    const enum OPCODE opcode = getTokenOpcode(*token, stmt_sep);
    const enum TYPE type = getTokenType(*token, opcode);

    validateToken(*token, list, opcode, type, line_n);
    tokenListPush(list, *token, type, opcode, line_n);
    strEmpty(token);
}


static bool inParameterList(char ch, struct token_list *list)
{
    return ch == '(' && list != NULL && list->last != NULL &&
        (list->last->type == T_Identifier || list->last->type == T_Arguments ||
        list->last->type == T_Index || list->last->opcode == OP_Closure ||
        list->last->type == T_Chunk || list->last->opcode == OP_Closed_parenthesis);
}


static void processChars(char **token,
                         struct token_list *list,
                         const char *str,
                         size_t len,
                         size_t *i,
                         const char *stmt_sep,
                         size_t *line_n)
{
    *line_n += str[*i] == '\n';

    if (isspace(str[*i])) {
        return;
    } else if (str[*i] == '/' && *i < len && str[(*i)+1] == '*') {
        processComment(str, len, i, line_n);
    } else if (str[*i] == '#') {
        processLineComment(str, i, line_n);
    } else if (str[*i] == '"') {
        processString(token, list, str, false, len, i, line_n);
    } else if (str[*i] == '\'') {
        processString(token, list, str, true, len, i, line_n);
    } else if (str[*i] == '{') {
        processChunk(token, list, str, len, i, line_n);
    } else if (str[*i] == '[') {
        processIndexArray(token, list, str, len, i, line_n);
    } else if (inParameterList(str[*i], list)) {
        processParameters(token, list, str, len, i, line_n);
    } else {
        strAppendC(token, str[*i]);

        if (!isReserved(str, *i, len)) {
            processToken(token, list, stmt_sep, *line_n);
        }
    }
}


struct token_list *tokenize(const char *code, const char *stmt_sep, size_t line_n)
{
    struct token_list *list = listInit();
    const size_t len = strlen(code) + 1;
    char *token = strInit();

    for (size_t i = 0; i < len - 1; i++) {
        processChars(&token, list, code, len, &i, stmt_sep, &line_n);
    }

    free(token);

    return list;
}


static bool processJsonScope(char **token,
                             const char *str,
                             size_t len,
                             size_t *i,
                             const char start,
                             const char end)
{
    int64_t scoping = 1;
    size_t line_n = 0;

    while (++(*i) < len && scoping > 0) {
        if (str[*i] == '"') {
            strAppendC(token, str[*i]);
            addStringChars(token, str, len, i, &line_n, true);
        }

        if (str[*i] == start) {
            scoping++;
        } else if (str[*i] == end) {
            scoping--;
        }

        if (scoping > 0) {
            strAppendC(token, str[*i]);
        }
    }

    return scoping == 0;
}


struct token_list *tokenizeJson(const char *code, bool *error)
{
    struct token_list *list = listInit();
    const size_t len = strlen(code) + 1;
    char *token = strInit();
    size_t line_n = 0;

    for (size_t i = 0; i < len - 1; i++) {
        if (isspace(code[i])) {
            continue;
        } else if (code[i] == '"') {
            processString(&token, list, code, false, len, &i, &line_n);
        } else if (code[i] == '{') {
            if (!processJsonScope(&token, code, len, &i, '{', '}')) {
                *error = true;
            }

            tokenListPush(list, token, T_Chunk, OP_None, line_n);
            strEmpty(&token);
            i--;
        } else if (code[i] == '[') {
            if (!processJsonScope(&token, code, len, &i, '[', ']')) {
                *error = true;
            }

            tokenListPush(list, token, T_Array, OP_None, line_n);
            strEmpty(&token);
            i--;
        } else {
            strAppendC(&token, code[i]);

            if (!isReserved(code, i, len)) {
                const enum OPCODE opcode = getTokenOpcode(token, ",");
                tokenListPush(list, token, getTokenType(token, opcode), opcode, line_n);
                strEmpty(&token);
            }
        }
    }

    free(token);
    return list;
}


struct token_list *listInit(void)
{
    struct token_list *list = malloc_(sizeof(struct token_list));
    *list = (struct token_list) {0};
    return list;
}
