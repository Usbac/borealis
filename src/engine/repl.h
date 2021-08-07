#ifndef REPL_H
#define REPL_H

#define REPL_EXIT_KEYWORD ".exit"
#define REPL_HELP_KEYWORD ".help"
#define REPL_CLEAR_KEYWORD ".clear"
#define MSG_REPL "Borealis\n" \
    "Version "BOREALIS_VERSION"\n" \
    "The last given value is stored in _\n" \
    "Type \""REPL_HELP_KEYWORD"\" to show this message\n" \
    "Type \""REPL_CLEAR_KEYWORD"\" to clear the terminal\n" \
    "Type \""REPL_EXIT_KEYWORD"\" to exit\n\n"

/**
 * Process the REPL.
 */
void processRepl(void);

#endif /* REPL_H */
