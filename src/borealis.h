#ifndef BOREALIS_H_
#define BOREALIS_H_

#define BOREALIS_VERSION "0.1"
#define REPL_EXIT_KEYWORD "exit"
#define MSG_REPL "Borealis\nVersion "BOREALIS_VERSION"\nType \""REPL_EXIT_KEYWORD"\" to exit\n\n"
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
