Variable names in Borealis are case sensitive.

A valid variable name starts with a letter or underscore, followed by any number of letters, numbers, or underscores.

```borealis
any valid_name;
any _valid2;
```

All variables are assigned by value. This means, that after assigning one variable's value to another, changing one of those variables will have no effect on the other.

## References

Variables can be assigned by reference.

To assign by reference, simply prepend an ampersand `&` to the beginning of the variable you want to reference.

```borealis
any foo = 10;
any bar = &foo;

foo++;

print(bar); # It will print 11
```

References work with elements, object properties and array indices.

## Declaration

There are two types of declarations in Borealis, dynamic and static.

### Dynamic

Dynamic declarations are declarations made with the `any` keyword, variables defined this way can hold any type of value.

```borealis
any foo = 1;
foo = 'abc';
foo = f() { };
```

### Static

Static declarations are declarations made with one of the existing data type keywords (`number`, `string`, `array`, `object` or `function`).

Variables defined this way only can hold values of the specified data type.

```borealis
string name = 'alex';
number age = 23;
array ids = [ 1, 2, 3 ];
object foo = obj {};
```

Trying to assign a different type to a static variable will result in an error being thrown.

```borealis
number age = 23;
age = 'hello' # This will result in an error.
```

A static variable can only reference static variables of the same type.

```borealis
number foo = 12;
number bar = &foo; # Valid

any another = 12;
bar = &another; # Not valid
```

## Scope

The scope of a variable is the context within which it is defined.

```borealis
if (true) {
    any foo = 12;
}

foo; # The variable does not exists here.
```
