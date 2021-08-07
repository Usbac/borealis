A constant is an identifier (name) for a value that cannot change during the execution of the program.

A valid constant name starts with a letter or underscore, followed by any number of letters, numbers, or underscores.

Constants are case-sensitive and have only a single scope, just like variables.

By convention, constant identifiers are always uppercase.

## Declaration

Constants can be declared by prepending the `const` keyword to a variable declaration.

```borealis
const any foo = 12;
```

Modifying a constant during runtime will result in an error being thrown.

```borealis
const any PI = 3.14159;

PI = 1; # Error.
```
