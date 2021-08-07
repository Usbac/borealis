#include <string.h>
#include <stdlib.h>
#include "../element.h"
#include "../state.h"
#include "../borealis.h"
#include "parser.h"
#include "preprocessor.h"
#include "parser_bytecode.h"
#include "processor.h"
#include "processor_helper.h"
#include "../../lib/linenoise.h"
#include "repl.h"


static char *hints(const char *buf, int *color, int *bold)
{
    *color = 35;
    *bold = 0;

    if (!strcmp(buf, "foreach ")) {
        return "(ite, key[:val]) { }";
    } else if (!strcmp(buf, "if ")) {
        return "(cond) { }";
    } else if (!strcmp(buf, "any ")) {
        return "name [= val];";
    } else if (!strcmp(buf, "case ")) {
        return "(cond) { val1: {} val2: {} }";
    } else if (!strcmp(buf, "f ")) {
        return "(params) {}";
    }

    return NULL;
}


static bool runLine(char *line, struct element *el)
{
    struct list *stmts, *bytecode;
    if (!strcmp(line, REPL_EXIT)) {
        state->exiting = true;
        goto end;
    } else if (!strcmp(line, REPL_HELP)) {
        printf(MSG_REPL);
        goto end;
    } else if (!strcmp(line, REPL_CLEAR)) {
        linenoiseClearScreen();
        goto end;
    } else if (!strncmp(line, REPL_SAVE, strlen(REPL_SAVE))) {
        char *file, *token;
        strtok(line, " ");
        token = strtok(NULL, " ");
        file = token == NULL ? REPL_DEFAULT_FILE : token;

        linenoiseHistorySave(file);
        printf(REPL_SAVED, file);
        goto end;
    }

    linenoiseHistoryAdd(line);

    stmts = codeToList(line, DEFAULT_SEP, true, state->line_n);
    preprocess(stmts, NULL);
    bytecode = listToBytecode(stmts);
    evalBytecode(bytecode);

    if (state->last_val != NULL) {
        freeElementValues(&el);
        mapResultToElement(el, state->last_val);
        freeResult(state->last_val);
        state->last_val = NULL;
    }

    freeListRecursive(stmts);
    freeListRecursive(bytecode);

    end:
        linenoiseFree(line);
        return !state->exiting;
}


void processRepl(void)
{
    struct element *el = newElement("_", NULL, 0, T_Null);
    char *line;

    linenoiseSetMultiLine(1);
    linenoiseHistorySetMaxLen(UINT16_MAX);
    linenoiseSetHintsCallback(hints);

    declareElement(&el);
    state->jmp_buffer = malloc_(sizeof(jmp_buf));
    setjmp(*state->jmp_buffer);
    while ((line = linenoise("> ")) != NULL && runLine(line, el));
    free(state->jmp_buffer);
}
