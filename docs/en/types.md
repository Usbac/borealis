Borealis supports the following data types.

Scalar types:

* number
* string

Compound types:

* array
* object
* function

And the special types:

* null

A variable type can be dynamic or static.

## Boolean

As you may notice, a boolean type does not exists in Borealis, but the constants `true` and `false` are available, they represent the number `1` and `0` respectively.

The following values are considered `false` or `0`:

* The number zero (`0`).

* An empty string (`''`).

* Null (`null`).

Every other value is considered `true` or `1`.

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

## Array

Arrays are actually an ordered map. A map associates keys to values. A borealis array can be treated as an array, a hashmap, a dictionary and more.

An array can be created with the `[` and `]` tags. Any number or comma-separated `key = value` or `value` can be declared inside.

If multiple elements in the array use the same key, only the last one will be used and all the others will be overwritten.

The comma after the last element is optional.

```borealis
[
    'key' = 'value',
    'key2' = 'value2',
    'key3' = 'value3',
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

An existing array can be modified by explicitly setting values in it with the key inside brackets.

```borealis
any foo = [];
foo['name'] = 'alex';
```

If there's nothing inside the brackets, the internal next index of the array will be used. It will work like a push.

```borealis
any foo = [ 'a', 'b' ];
foo[] = 'c';
```

The above example will leave the foo array as `'a', 'b', 'c'`.

### Expanding arrays

With the spread operator `...` you can expand an array into another one.

```borealis
any new_data = [
    'name' = 'juan',
    'age' = 23,
];

any user = [ 'name' = 'alex', ...new_data ]; # Will return [ 'name' = 'juan', 'age' = 23 ]
```

## Object

Objects work pretty much like arrays but can have common declarations and statements in its body, and its properties can be private.

They can be created with the syntax `obj { }`.

```borealis
any customer = obj {
    any name = 'alex';
    any age = 23;
    public any foo = 'bar';
};
```

Object properties are private by default, they can be made public by adding the `public` keyword before the declaration.

```borealis
obj {
    public any foo;
};
```

The `this` keyword can be used to refer to the object in the current context.

```borealis
obj {
    any name = 'alex';

    print(this.name); # This will print 'alex'
}
```

The dot syntax can be used to access to an object property.

```borealis
customer.name
```

If the object property doesn't exists or if it's private, an error will be thrown.

That can be avoided with the safe dot syntax.

```borealis
customer?.name
```

Object properties can be accessed with the brackets syntax too.

```borealis
customer['name']
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

The special type `null` represents something without a value. The only valid "value" of null is, in fact, null.

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

ref === null; # true since the value of ref is now null.
```
