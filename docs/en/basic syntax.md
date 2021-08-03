
## Instructions separation

As in C, statements in Borealis must end with a semicolon `;` no matter if they are in the same line or in different lines.

```borealis
any a = 1; # Valid

any b = 1 # Syntax error
```

## Expressions

Expressions are one of the most important blocks of Borealis.

In Borealis, almost anything is an expression, from a function call (`foo()`) to an assignment (`a = 5`), if it's a value or returns a value, probably it's an expression.

```borealis
any a = 0;
any b = a = 8; # Assign the value 8 to a and b

any c = a++; # Assign original value of a to c and then increment it.

c = foo(); # Assign to c the return value of the function foo.
```

## Comments

Borealis has two type of comments:

* Single line comments (starts with the character `#` and ends at the end of line).

* Multi line comments (starts with `/*` and ends at the first `*/`).

```borealis
print('Hello world'); # This is commented.

/*
 * Comment that goes
 * across multiple
 * lines
 */
```

## White spaces

White spaces in Borealis usually do not affect the code, unless they are inside a string.

```borealis
# Both are valid
a = 1 + 2;
a=1+2;
```

```borealis
any msg = 'hello
world';

# Now msg is "Hello\nWorld"
```
