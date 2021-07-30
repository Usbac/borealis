#include <stdlib.h>
#include <string.h>
#include "state.h"
#include "stdlib.h"
#include "engine/parser.h"
#include "engine/parser_bytecode.h"
#include "engine/processor.h"
#include "engine/preprocessor.h"
#include "borealis.h"

/**
 * Current working directory.
 */
char *cwd = NULL;

/**
 * List of arguments.
 */
struct element_table *args;

/**
 * Single code line to run.
 */
char *arg_code = NULL;

/**
 * Current file.
 */
char *arg_file = NULL;

/**
 * Interactive mode or not.
 */
bool arg_interactive_mode = false;

/**
 * Number of flags.
 */
size_t flags_quantity = 0;


static void runExit(void)
{
    int status_code = 0;

    if (state != NULL) {
        status_code = state->status_code;
        freeState();
    }

    freeLexer();
    freeElements();
    free(cwd);
    exit(status_code);
}


/**
 * Returns true if the program flow must be stopped, false otherwise.
 */
static bool mapArgs(int argc, char *argv[])
{
    if (argc <= 1) {
        printf(MSG_HELP);
        return true;
    }

    args = newElementTable();
    for (int i = 0; i < argc; i++) {
        struct element *arg = newElement(NULL, NULL, 0, T_Null);
        arg->key = strFromInt(i);
        arg->type = T_String;
        arg->value.string = strDup(argv[i]);
        pushElementToTable(&args, arg);
    }

    for (int i = 0; i < argc; i++) {
        /* Help */
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            printf(MSG_HELP);
            return true;
        }

        /* REPL */
        if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--interactive")) {
            arg_interactive_mode = true;
        }

        /* Version */
        if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
            printf(MSG_VERSION);
            return true;
        }

        /* The flags below work with values */
        if (i+1 >= argc) {
            return false;
        }

        /* Run code line */
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--run")) {
            arg_code = argv[++i];
            flags_quantity += 2;
        }

        /* File */
        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file")) {
            arg_file = argv[++i];
            flags_quantity += 2;
        }
    }

    return false;
}


static void runFile(void)
{
    initState(arg_file);
    initStdlib(args, cwd, arg_file);
    processFile(arg_file, 0);
}


static void runRepl(void)
{
    initState(NULL);
    initStdlib(args, cwd, NULL);
    printf(MSG_REPL);
    processRepl();
}


static void runCodeLine(void)
{
    struct list *stmts, *bytecode;

    initState(NULL);
    initStdlib(args, cwd, NULL);

    stmts = codeToList(arg_code, DEFAULT_SEP, true, 0);
    preprocess(stmts, NULL);
    bytecode = listToBytecode(stmts);
    evalBytecode(bytecode);

    freeListRecursive(stmts);
    freeListRecursive(bytecode);
}


int main(int argc, char* argv[])
{
    atexit(runExit);
    if (mapArgs(argc, argv)) {
        return 0;
    }

    cwd = getcwd_();
    initLexer();

    if (arg_file != NULL) {
        runFile();
    } else if (arg_code != NULL) {
        runCodeLine();
    } else if (arg_interactive_mode) {
        runRepl();
    }
}
