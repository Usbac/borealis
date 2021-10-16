#include <stdlib.h>
#include <string.h>
#include "state.h"
#include "stdlib.h"
#include "engine/parser.h"
#include "engine/parser_bytecode.h"
#include "engine/processor.h"
#include "engine/preprocessor.h"
#include "engine/repl.h"
#include "borealis.h"

/**
 * Current working directory.
 */
char *cwd = NULL;

/**
 * List of arguments.
 */
struct element_table *args_table;

/**
 * Arguments options.
 */
struct {
    char *code;
    char *file;
    bool interactive_mode;
} args;


static void runExit(void)
{
    int status_code = EXIT_SUCCESS;

    if (state != NULL) {
        status_code = state->code;
        stateFree();
    }

    lexerFree();
    elementsFree();
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

    args_table = elementTableInit();
    for (int i = 0; i < argc; i++) {
        struct element *arg = elementInit(NULL, NULL, 0, T_String);
        arg->key = strFromInt(i);
        arg->value.string = strDup(argv[i]);
        elementTablePush(&args_table, arg);
    }

    for (int i = 0; i < argc; i++) {
        /* Help */
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            printf(MSG_HELP);
            return true;
        }

        /* REPL */
        if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--interactive")) {
            args.interactive_mode = true;
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
            args.code = argv[++i];
        }

        /* File */
        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file")) {
            args.file = argv[++i];
        }
    }

    return false;
}


static void runFile(void)
{
    stateInit(args.file);
    stdlibInit(args_table, cwd, args.file);
    processFile(args.file, 0);
}


static void runRepl(void)
{
    stateInit(NULL);
    stdlibInit(args_table, cwd, NULL);
    printf(REPL_MSG);
    state->in_repl = true;
    replProcess(false);
}


static void runCodeLine(void)
{
    struct token_list *stmts, *bytecode;

    stateInit(NULL);
    stdlibInit(args_table, cwd, NULL);

    stmts = codeToList(args.code, SEPARATOR_DEFAULT, true, 0);
    preprocess(stmts, NULL);
    bytecode = bytecodeFromList(stmts);
    evalBytecode(bytecode);

    listFreeR(stmts);
    listFreeR(bytecode);
}


int main(int argc, char* argv[])
{
    atexit(runExit);
    if (mapArgs(argc, argv)) {
        return EXIT_SUCCESS;
    }

    cwd = getcwd_();
    lexerInit();

    if (args.file != NULL) {
        runFile();
    } else if (args.code != NULL) {
        runCodeLine();
    } else if (args.interactive_mode) {
        runRepl();
    }
}
