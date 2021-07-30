The standard object `Io` gives you multiple methods related with input and output operations.

## Functions

### Print

`print(...args)`

Prints all the given arguments in the standard output.

```borealis
Io.print('hello world'); # It will print 'Hello world'
```

```borealis
Io.print('one', 'two', 'three'); # It will print 'onetwothree'
```

### Print line

`printLine(...args)`

Prints all the given arguments with a new line after every one.

```borealis
Io.printLine('hello'); # It will print 'hello\n'
```

```borealis
Io.printLine('one', 'two', 'three'); # It will print 'one\ntwo\nthree\n'
```

### Print verbose

`printV(any value)`

Prints the given value in verbose mode.

Usually useful for printing non-scalar values like arrays and objects.

```borealis
Io.printV([ 1,2,3 ]); # It will print '(array) [0:(number) 1,1:(number) 2,2:(number) 3]'
```

```borealis
Io.printV(obj {
    public any foo = 'a';
    any bar = 'b';
});

# It will print '(object) {foo:(string) "a",bar:(string) "b"}'
```

### Read char

`readChar(): string`

Reads a single character from the standard output.

```borealis
Io.printLine('Gender ? M/F');
any gender = Io.readChar();
```

### Read line

`readLine(string message = '', number use_mask = false): string`

Reads a single line from the standard output.

If the message argument is given, it will be printed just before reading the input.

```borealis
Io.readLine('Text: ');
```

```borealis
any name = Io.readLine('Your name: ');
```

An optional second parameter can be given which indicates if the given input will be masked (will just replace the characters the user is typing with * characters).

```borealis
any password = Io.readLine('Your password: ', true);
```

### Flush

`flush()`

Flushes the system output buffer.

```borealis
Io.flush();
```

### Add history

`historyAdd(string text)`

Adds a text to the top of the input history.

For this function to work, a history size first must be set with the `historySetSize` function.

When reading a line or character from the standard output, the top of the history is what is displayed to the user when he presses the up arrow key.

```borealis
Io.historyAdd('first command');
```

```borealis
Io.historyAdd('> prev command');
Io.readln('> '); # If the user presses the up arrow key, '> prev command' will appear
```

It's pretty much what happens when you enter multiple commands in your terminal and then press the up arrow key.

### Set history size

`historySetSize(number size)`

Sets the maximum size of the history.

If the history is full and you add a new element with the `historyAdd` function, the last function in the history will be shifted and then the new element will be pushed.

```borealis
Io.historySetSize(5);
```

### Clear

`clear()`

Clears the console if the environment allows it.

```borealis
clear();
```
