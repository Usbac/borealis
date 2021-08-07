Borealis provides a Read-Eval-Print-Loop functionality.

The REPL can be accessed by running borealis with the interactive flag (`-i` or `--interactive`), or by calling the standard function `debug`.

The evaluator will assign the result of the most recently evaluated expression to the special variable _ (underscore) when running the REPL with the interactive flag.

## Commands

The following commands are supported by the REPL.

* `.clear` - Clear the terminal.
* `.exit` - Exit the REPL.
* `.help` - Show help.
* `.save` - Save the REPL history to a file (`.save ./file.txt`), if no filename is given the default file will be `./repl.bor`.

## Examples

```
> any foo = 12;
> printLine(_);
12
> .exit
```

```
> any bar;
> bar = &foo;
> .save ./awesome_file.bor
> .exit
```

Running in source code file:
```borealis
printLine('hello world');
debug(); # Run REPL here
```