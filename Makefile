CC=gcc
CFLAGS=-lm -lc -std=c99 -pedantic -g -W -Wall -Wmissing-prototypes -Wno-missing-field-initializers -Wmissing-braces -Wno-error=missing-braces -Wno-unused-parameter -Wstrict-prototypes -Wold-style-definition -Wshadow -Wdouble-promotion
PREFIX=/usr/local
FILES=src/borealis.c src/utils.c src/state.c src/engine/lexer.c src/engine/parser.c src/engine/parser_bytecode.c src/element.c src/engine/preprocessor.c src/engine/processor.c src/engine/processor_helper.c src/engine/repl.c src/error.c src/stdlib.c src/stdlib/general.c src/stdlib/io.c src/stdlib/string.c src/stdlib/table.c src/stdlib/number.c src/stdlib/file.c src/stdlib/json.c src/stdlib/math.c src/stdlib/date.c src/stdlib/os.c src/stdlib/crypt.c src/stdlib/socket.c src/stdlib/bit.c src/stdlib/error.c lib/linenoise.c lib/sha1.c lib/sha256.c lib/md5.c lib/base64.c
ifndef NAME
    NAME=borealis
endif

all: borealis

borealis: $(FILES)
	$(CC) -o $(NAME) $(FILES) $(CFLAGS) -Werror

install: borealis
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/$(NAME)

test:
	$(MAKE); ./$(NAME) -f ./tests/main.bor

safe:
	$(CC) -o $(NAME) $(FILES) $(CFLAGS) -DSAFE_MODE=1

clean:
	rm $(NAME)
