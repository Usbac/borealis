cmake_minimum_required(VERSION 3.9)
project(borealis)

set(CMAKE_CXX_STANDARD 11)

add_executable(borealis
        src/borealis.c
        src/engine/lexer.c src/engine/lexer.h
        src/engine/parser.c src/engine/parser.h
        src/engine/parser_bytecode.c src/engine/parser_bytecode.h
        src/engine/preprocessor.c src/engine/preprocessor.h
        src/engine/processor.c src/engine/processor.h
        src/engine/processor_helper.c src/engine/processor_helper.h
        src/error.c src/error.h
        src/utils.c src/utils.h
        src/element.c src/element.h
        src/state.c src/state.h
        src/stdlib.c src/stdlib.h
        src/stdlib/io.c src/stdlib/io.h
        src/stdlib/string.c src/stdlib/string.h
        src/stdlib/os.c src/stdlib/os.h
        src/stdlib/general.c src/stdlib/general.h
        src/stdlib/array.c src/stdlib/array.h
        src/stdlib/object.c src/stdlib/object.h
        src/stdlib/number.c src/stdlib/number.h
        src/stdlib/file.c src/stdlib/file.h
        src/stdlib/json.c src/stdlib/json.h
        src/stdlib/math.c src/stdlib/math.h
        src/stdlib/date.c src/stdlib/date.h
        src/stdlib/crypt.c src/stdlib/crypt.h
        src/stdlib/socket.c src/stdlib/socket.h
        src/stdlib/bit.c src/stdlib/bit.h
        lib/linenoise.c lib/linenoise.h
        lib/sha1.c lib/sha1.h
        lib/sha256.c lib/sha256.h
        lib/md5.c lib/md5.h
        lib/base64.c lib/base64.h)

target_link_libraries(borealis m)
