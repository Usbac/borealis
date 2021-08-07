A function can be defined with the following syntax:

```borealis
any foo(arg1, arg2) {
    print('Example');

    return return_val;
}
```

Any valid Borealis code can be inside a function except for a function definition.

A valid function name starts with a letter or underscore, followed by any number of letters, numbers, or underscores.

Common functions are defined before executing the source code, just after the parsing stage of the Borealis interpreter, meaning that you can call a function defined below in the code.

```borealis
foo(); # valid

any foo() {
    return 1;
}
```

It's possible to call recursive functions.

```borealis
any recursion(n) {
    if (n < 10) {
        printLine(n);
        recursion(n + 1);
    }
}
```

_The maximum number of nested function calls in Borealis is 256. Any number of nested calls above it will crash the call stack and terminate the current script._

### Static functions

Borealis allows the definition of static type functions, instead of using the `any` keyword use the keyword of the type you want.

```borealis
string upper(str) {
    return String.toUpper(str);
}
```

If a value with a different type is returned an error will be thrown.

## Return values

Values are returned by using the optional return statement. Any data type can be returned.

This causes the function to end its execution immediately and pass control back to the line from which it was called.

```borealis
any square(n) {
    return n * n;
}

print(8); # It will print 32
```

## Arguments

Information can be passed to a function via the arguments list, which is a comma delimited list of expressions. The arguments are evaluated from left to right.

Borealis supports passing by value, passing by reference, default argument values and variable arguments list.

The arguments list accepts a trailing comma just like in arrays.

```borealis
any manyArgs(
    arg1,
    arg2,
    arg3,
    arg4,
    arg5, # trailing commas are valid
) {
    # code
}
```

### Static arguments

The type of a parameter can be specified.

```borealis
any square(number n) {
    return n * n;
}
```

If an argument with a different type is passed, an error will be thrown.

### Optional arguments

One or multiple optional arguments can be defined with the following syntax.

```borealis
any foo(arg1, arg2 = 'default') {}
```

_A mandatory argument cannot be defined after an optional argument._

### Passing by reference

You can use a reference as argument.

```borealis
any increment(foo) {
    foo++;
}

any num = 0;
increment(&num);

# num now is 1
```

### Variable-length arguments list

With the spread operator you can define a variable-length arguments list.

The arguments will be passed into the given variable as an array.

```borealis
any foo(arg1, ...args) {
    # Here args will be an array with the arguments
}
```

Example:

```borealis
any sum(...numbers) {
    any result = 0;

    foreach (numbers, n) {
        result += n;
    }

    return result;
}

sum(1, 2, 3, 4); # It will return 10
```

_The last parameter is the only one that can be defined with the spread operator._

### Unpacking arrays as arguments

An array can be used as arguments list by using the spread `...` operator.

```borealis
any args = [ 1, 2, 3 ];

func(...args); # Equivalent to func(1, 2, 3);
```

The array can even be after normal arguments.

```borealis
any printArgs(arg1, arg2, arg3) {
    print(arg1, arg2, arg3);
}

printArgs(1, ...[ 2, 3 ]); # It will print '1 2 3'
```

## Anonymous functions

Anonymous functions, also know as closures, allow the creation of functions without a name.

They can be defined with the `f` keyword.

```borealis
f() {
    # code
};
```

Anonymous functions will have access to elements from the scope where they are being called.

Unlike normal functions and just like variables they are defined when executing the source code.

Anonymous functions are first-class citizens, meaning that they can be stored in variables, passed as arguments, returned from a function, and more.

```borealis
any runFunc(func) {
    func();
}

runFunc(f() {
    print('svalbard');
});
```

The above example will print "svalbard".

## Built-in functions

Borealis comes with many different built-in functions, they are created when starting any script and they execute a more low-level code in C.

The built-in functions are usually faster than the user-defined functions in Borealis.

```borealis
toString([ 1,2,3 ]);
print('Hello world');
```
