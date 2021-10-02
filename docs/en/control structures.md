In control structures expressions are evaluated to their boolean value.

Keep in mind that the braces are always required for control structures, even if you've only got one line in the block.

```borealis
if (expr)
    print('done'); # Syntax error

if (expr) {
    print('done'); # Valid
}
```

## If

It allows the conditional execution of a code block.

```borealis
if (expr) {
    # Code
}
```

If expr evaluates to `true` the code block will be executed, and if it evaluates to `false` Borealis will ignore it.

```borealis
if (5 > 2) {
    print('5 is bigger than 2');
}
```

## Else

Often you'd want to execute a different statement if the condition in if is not met. This is what else is for.

```borealis
if (a > b) {
    print('a is greater than b');
} else {
    print('a is NOT greater than b');
}
```

The else statement is only executed if the if expression evaluated to `false`, and if there were any else if expressions evaluated to `false` too.

## Else if

Is a combination of if and else. It executes the code block only if the original if expression evaluates to `false`, but unlike else, it will execute the alternative expression only if the else if conditional expression evaluates to `true`.

```borealis
if (a > b) {
    print('a is greater than b');
} else if (a == b) {
    print('a is equal to b');
} else {
    print('a is less than b');
}
```

## Case

The case structure works pretty much like the common switch structure in other languages. The difference is that the case structure does not require a break to end the running piece of code.

The case structure is useful when you need to compare the same expression with many values, and execute a different piece of code depending on which value it equals to.

```borealis
case (num) {
    1: {
        print('one');
    }
    2: {
        print('two');
    }
    3: {
        print('three');
    }
}
```

## Foreach

Iterations over an iterable value (arrays) can be done with the `foreach` keyword.

```borealis
foreach (arr, value) {
    # code
}

foreach (arr, key: value) {
    # code
}
```

The first form will iterate `iterable_expr`, on each iteration the value of the current element will be stored in `value`.

The second will do the same but will additionally store the current element's key in `key`.

```borealis
array foo = [ 1, 2, 3, 4 ];

foreach (foo, val) {
    print(val);
}

# It will print 1234
```

```borealis
array foo = [ 1, 2, 3, 4 ];

foreach (foo, key: val) {
    foo[key] = val * 2;
}

# Now foo will be [ 2, 4, 6, 8 ]
```

## While

While loops are the simplest type of loop in Borealis, they behave exactly like in C.

```borealis
while (expr) {
    # code
}
```

The code block will be executed as long as the expression evaluates to `true`.
The value of the expression is checked each time only at the beginning of the loop.

```borealis
number i = 0;

while (i <= 5) {
    print(i++);
}

# It will print 012345
```

## Break

Break ends the execution of the current while and foreach structure.

```borealis
number i = 0;

while (true) {
    if (i > 3) {
        break;
    }

    print(i++);
}

# It will print 0123
```

## Continue

Is used within looping structures to skip the rest of the current loop iteration, continue execution at the condition evaluation and then the beginning of the next iteration.

```borealis
foreach ([ 1, 2, 3, 4, 5 ], val) {
    if (val == 2 || val == 4) {
        continue;
    }

    print(val);
}

# It will print 135
```

## Return

Returns program control to the calling module. Execution resumes at the expression following the called module's invocation.

If called from within a function, the return statement ends execution of the current function, and returns its argument as the value of the function call.

```borealis
any getFive() {
    return 5;
}

any num = getFive(); # 5 will be returned
```

If called from within an imported file, the return statement ends execution of the current file and returns its argument as the value of the file import.

```borealis
# a.bor
any my_array = import 'b.bor';

# b.bor
return [ 1, 2, 3 ];
```
