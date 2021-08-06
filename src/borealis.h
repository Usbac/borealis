#ifndef BOREALIS_H_
#define BOREALIS_H_

#define BOREALIS_VERSION "0.1.0"
#define REPL_EXIT_KEYWORD ".exit"
#define REPL_HELP_KEYWORD ".help"
#define REPL_CLEAR_KEYWORD ".clear"
#define MSG_REPL "Borealis\n" \
    "Version "BOREALIS_VERSION"\n" \
    "The last given value is stored in _\n" \
    "Type \""REPL_HELP_KEYWORD"\" to show this message\n" \
    "Type \""REPL_CLEAR_KEYWORD"\" to clear the terminal\n" \
    "Type \""REPL_EXIT_KEYWORD"\" to exit\n\n"
#define MSG_VERSION "Borealis "BOREALIS_VERSION"\n"
#define MSG_HELP "Borealis: The programming language\n\n" \
    "USAGE: borealis [options...]\n\n" \
    "OPTIONS:\n\n" \
    "-f --file [%%s]       Run a file.\n" \
    "-h --help            Show help.\n" \
    "-i --interactive     Run REPL.\n" \
    "-r --run [%%s]        Run a single line of code.\n" \
    "-v --version         Show the version.\n\n"

#endif /* BOREALIS_H_ */
