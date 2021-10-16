#ifndef REPL_H
#define REPL_H

#define REPL_CLEAR ".clear"
#define REPL_EXIT ".exit"
#define REPL_HELP ".help"
#define REPL_SAVE ".save"
#define REPL_SAVED "REPL history successfully saved to file %s!\n"
#define REPL_DEFAULT_FILE "./repl.bor"
#define REPL_MSG "Borealis\n" \
    "Version "BOREALIS_VERSION"\n" \
    "The last given value is stored in the _ (underscore) variable\n" \
    "Type \""REPL_CLEAR"\" to clear the terminal\n" \
    "Type \""REPL_EXIT"\" to exit the REPL\n" \
    "Type \""REPL_HELP"\" to show this message\n" \
    "Type \""REPL_SAVE"\" to save the REPL history to a file\n\n" \

/**
 * Process the REPL.
 */
void replProcess(bool debug);

#endif /* REPL_H */
