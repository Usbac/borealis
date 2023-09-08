Borealis supports the following data types.

Scalar types:

* number
* string

Compound types:

* table
* function

And the special types:

* null

A variable type can be dynamic or static.

## Boolean

As you may notice, a boolean type does not exist in Borealis, but the constants `true` and `false` are available, they represent the number `1` and `0` respectively.

The following values are considered `false` or `0`:

* The number zero (`0`).
* An empty string (`''`).
* Null (`null`).

Every other value is considered `true` or `1`.

_The `true` or `false` constants are only referenced in the Borealis documentation when dealing with a boolean context._

## Number

Represents a double. The size is platform-dependent, although roughly 14 decimal digits is a common value (64 bit IEEE format).

They can be declared with the following syntax:

```borealis
any a = 23;
any b = 2.71828182845;
any c = 1.2e3;
```

## String

Represents a series of characters, each character is not necessary a byte since strings are `UTF-8` encoded.

Strings literals can be specified with single or double quotes.

```borealis
"hello world"
'hello world'
```

Only if the string is enclosed in double-quotes ("), the following escape sequences for special characters will be interpreted:

| Sequence | Meaning                                      |
|----------|----------------------------------------------|
| \n       | Linefeed                                     |
| \t       | Horizontal tab                               |
| \r       | Carriage return                              |
| \0       | Null                                         |
| \nnn     | Byte whose numerical value is nnn (as octal) |
| \xhh     | Character whose hexadecimal code is hh       |
| \uhhhh   | Unicode character whose code point is hhhh   |

## Table

Tables are actually an ordered map. A map associates a key to a value.

A borealis table can be treated as an array, a hashmap, a dictionary, an object and more.

A table can be created with the `[` and `]` tags. Any number or comma-separated `key = value` or `value` can be declared inside.

If multiple elements in the table use the same key, only the last one will be used and all the others will be overwritten.

The comma after the last element is optional.

```borealis
[
    'key' = 'value',
    'key2' = 'value2',
    'key3' = 'value3',
]
```

The `this` keyword can be used to refer to the table in the current context.

```borealis
[
    'name' = 'alex',
    'printName' = f() {
        print(this.name); # This will print 'alex'
    },
]
```

### Examples

```borealis
any user = [
    'name' = 'alex',
    'age'  = 23,
    'city' = 'longyearbyen',
];
```

It is possible to specify the key only for some elements:

```borealis
any foo = [
    'a',
    'b',
    'key' = 'c',
];
```

An existing table can be modified by explicitly setting values in it with the key inside brackets or after a dot.

```borealis
any foo = [];
foo['name'] = 'alex';
foo.name = 'alex'; # The same as above
```

If there's nothing inside the brackets, the internal next index of the table will be used. It will work like a push.

```borealis
any foo = [ 'a', 'b' ];
foo[] = 'c';
```

The above example will leave the foo table as `'a', 'b', 'c'`.

### Optional chaining

The safe dot operator (?.) allows you to read the value of a property located deep within a chain of tables without having to check that each reference in the chain is valid.

The safe dot operator is like the dot operator, except that instead of causing an error if a reference is null, the expression short-circuits with a return value of null.

```borealis
customer?.doesnotexists?.another;
```

### Expanding tables

With the spread operator `...` you can expand a table into another one.

```borealis
any new_data = [
    'name' = 'juan',
    'age' = 23,
];

any user = [ 'name' = 'alex', ...new_data ]; # Will return [ 'name' = 'juan', 'age' = 23 ]
```

## Function

Anonymous functions or closures, allow the creation of functions without a name.

They can be created with the syntax `f() { }`.

```borealis
any foo = f() {
    return 'bar';
};
```

More information about anonymous functions can be read in the `functions` page.

### Null

The special type null represents something without a value. The only valid "value" of null is, in fact, null.

```borealis
any foo = null;
```

A variable can be null if:

* It has been assigned to null.
* It has not been set to any value.
* It has been a reference to another variable that has been declared in a higher scope and can not be accessed anymore.

```borealis
any ref = 1;

if (true) {
    any foo = 12;
    ref = &foo;
}

ref === null; # true since the value of ref is now null
```
