
## Install

The easiest way to install Borealis in your system is by downloading the binaries for your operating system (Windows, MacOs or Linux) [here](https://getborealis.com/download).

## Compile

You can compile Borealis by your own. Borealis has been made thinking about simplicity in deployment and compilation.

The language strictly follows [ANSI C99](https://en.wikipedia.org/wiki/ANSI_C) and does not depend on external dependencies or libraries, meaning that if you can compile a hello world program in C, most probably you can compile Borealis without problems.

You can use a C compiler like `gcc` or `clang`.

The source code includes a Makefile, and a CMake file. With make, compiling it is as easy as running the `make` command.

### Safe version

The `make safe` command can be used to compile a safe version of Borealis.

This version does not have any definition of functions that use disk, system or network access. It can be used to mount an online code tester.
