### Print

`print(...args)`

Prints all the given arguments in the standard output.

This is a copy of `Io.print`.

```borealis
print('hello world'); # It will print 'Hello world'
```

### Print line

`printLine(...args)`

Prints all the given arguments with a new line after every one.

This is a copy of `Io.println`.

```borealis
printLine('hello'); # It will print 'hello\n'
```

### Ternary operator

`t(any cond, any left, any right): any`

Returns the second parameter if the first parameter evaluates to true, otherwise returns the third paremeter.

This function simulates the ternary operator functionality.

```borealis
any age_text = t(age > 17, 'Adult', 'Teen');
```

### Debug

`debug(any cond = true)`

Runs the REPL at the specified position, useful for debugging purposes.

For continuing with the program flow just call the exit function in the REPL.

```borealis
any foo = 0;

debug(); # Program flow will stop here and you'll be able to modify foo

printLine(foo);
```

An optional argument can be given which is the condition required to run the REPL, meaning that the REPL will run only if the value evaluates to true.

```borealis
any foo = Io.readln();

debug(foo eq 'yes'); # The REPL will run only if the user entered 'yes'
```

### Is empty

`isEmpty(any value): number`

Returns true if the given value is considered empty, false otherwise.

A value is considered empty if it's:

* An empty string (`''`).

* The number zero (`0`).

* An empty array (`[]`).

* An empty object (`obj {}`).

* Null (`null`).

```borealis
isEmpty([]); # Returns true
isEmpty(0); # Returns true

isEmpty(' '); # Returns false
isEmpty([ 1 ]); # Returns false
```

### To string

`toString(any value): string`

Returns the string representation of a value.

#### Examples

```borealis
toString(20) # Returns '20'
```

```borealis
toString([ 1,2,3 ]); # Returns '[array]'
```

### To number

`toNumber(any value): number`

Returns the numeric representation of a value.

#### Examples

```borealis
toNumber('1.5') # Returns 1.5
```

```borealis
toNumber([ 1,2,3 ]); # Returns 1
```

### To boolean

`toBool(any value): number`

Returns the boolean representation (`0` or `1`) of a value.

#### Examples

```borealis
toBool('hello'); # Returns 1
```

```borealis
toBool(''); # Returns 0
```

### To array

`toArray(any value): array`

Returns the array representation of a value.

#### Examples

```borealis
toArray(obj {
    public any name = 'alex';
    public any age = 23;
    any another = 1;
});
# Returns [ 'name' = 'alex', 'age' = 23, 'another' = 1 ]
```

### To object

`toObject(any value): object`

Returns the object representation of a value.

#### Examples

```borealis
toObject([ 'name' = 'alex', 'age' = 23 ]);
# Returns obj { public any name = 'alex'; public any age = 23; }
```

When converting an array to an object, all the properties will be public.

### Sleep

`sleep(number time)`

Sleeps the given time in milliseconds.

```borealis
sleep(1000) # Sleep one second
```

### Assert

`assert(any value[, string message])`

Asserts that the given value is `true`.

If the given value evaluates to `false`, the script will end, and if the message parameter is given, it will be printed.

```borealis
assert(1 == 1);
```

```borealis
assert(2 <= 1, 'Two is less than one');
# It will terminate the script and display the message.
```

### Exit

`exit(number status = 0)`

Terminates the current script with the given status code.

```borealis
exit();
```

```borealis
exit(0);
```

### Evaluate

`eval(string code)`

Evaluates the code in the given string.

_Keep in mind that if an error happens while evaluating the given code string, the current script execution will end._

```borealis
eval('any foo = true;');

print(foo); # Valid
```

### Type of

`typeof(any value): string`

Returns the type of the given value.

The returned string can be `string`, `number`, `array`, `object`, `function` or `null`.

```borealis
typeof('hello'); # Returns 'string'
typeof(42); # Returns 'number'
```
