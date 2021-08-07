An operator is something that takes one or more values and yields another value.

Borealis has two types of operators:

* Unary operators, which take only one value (such as `!` or `++`).
* Binary operators, which take two values (such as the familiar arithmetical operators `+` and `-`).

## Precedence

The precedence of an operator specifies how "tightly" it binds two expressions together. Parentheses have the highest precedence, so they can be used to force precedence over others operators.

```borealis
2 + 5 * 4
# Result is 22 since multiplication has a higher precedence than addition
(2 + 5) * 4
# Result is 32 since parentheses has been used to force precedence.
```

## Table

The operators table is ordered from higher to lower precedence.

|  Operators                         | Associativity | Information                                                                  |
|----------------------------------- |---------------|------------------------------------------------------------------------------|
| `(` `)`                            | None          | parentheses                                                                  |
| `**`                               | Left          | exponentiation                                                               |
| `.` `?.` `++` `--` `:`             | Left          | property access, safe property access, pos increment, pos decrement, colon   |
| `obj`                              | Left          | new object                                                                   |
| `+` `-` `++` `--` `!` `&`          | Right         | positive, negative, pre increment, pre decrement, negation, reference        |
| `*` `/` `%`                        | Left          | multiplication, division, modulus                                            |
| `+` `-`                            | Left          | addition, subtraction                                                        |
| `^`                                | Left          | concatenation                                                                |
| `<` `>` `<=` `>=`                  | Left          | lower, higher, lower or equal, higher or equal                               |
| `==` `!=` `eq` `ne` `===`          | Left          | compare, different, compare string, different string, compare strict         |
| `&&`                               | Left          | and                                                                          |
| `||`                               | Left          | or                                                                           |
| `??`                               | Right         | null coalesce                                                                |
| `=` `+=` `-=` `/=` `*=` `^=`, `%=` | Right         | assignation, addition/subtraction/division/concatenation/modulus assignation |
| `...` `<<`                         | Left          | spread, bang                                                                 |

_Borealis doesn't have bitwise operators, for manipulating bits use the functions of the standard object `Bit`._

## Arithmetic operators

All the arithmetic operators return a value of type `number`.

```borealis
2 + 5  # 7   Addition
2 - 5  # -3  Substraction
2 / 5  # 0.4 Division
2 * 5  # 10  Multiplication
4 % 2  # 0   Modulus
2 ** 5 # 32  Exponentiation
```

## Assignment operators

The basic assigment operator is `=`. It sets the left operand to the value of the right operand.

It can be combined with the arithmetic and string operators, this allows you to use a value in an expression and then set its value to the result of that expression.

```borealis
any a = 5;
a += 3;
# a is now 8

any b = 'hello';
b ^= ' world';
# b is now 'hello world'
```

### Bang operator

The bang operator simplifies the syntax for setting a variable value to the result of a function call using the same variable as argument.

```borealis
any foo = 'hello';
foo << String.toUpper;

# foo is now 'HELLO'
```

It means that both examples do exactly the same:

```borealis
foo << String.toUpper;
foo = String.toUpper(foo);
```

The operator can be chained, it's executed from left to right.

```borealis
any foo = 'hello';

foo << String.toUpper << String.reverse;

# foo is now 'OLLEH'
```

## Comparison operators

As their name implies, they allow you to compare two values.


| Name                    | Compared as | Example | Result                                          |
|-------------------------|-------------|---------|-------------------------------------------------|
| Equal                   | `number`    | a == b  | true if both values are equal.                  |
| Not equal               | `number`    | a != b  | true if both values are not equal.              |
| Less than               | `number`    | a < b   | true if a is less than b.                       |
| Greater than            | `number`    | a > b   | true if a is greater than b.                    |
| Less than or equal      | `number`    | a <= b  | true if a is less than or equal to b.           |
| Greater than or equal   | `number`    | a >= b  | true if a is greater than or equal to b.        |
| Equal string            | `string`    | a eq b  | true if both values are equal.                  |
| Not equal string        | `string`    | a ne b  | true if both values are not equal.              |
| Equal strict            | _strict_    | a === b | true if a and b have the same value and type.   |

_Keep in mind that the common comparison operators such as `==` and `!=` will treat both values as numeric._

```borealis
1 == 1 # true
1 == '1.0' # true
1 != '1.0' # false
```

```borealis
1 eq 1 # true
1 eq '1.0' # false
'one' eq 'one' # true
'one' eq 'two' # false
'one' ne 'two' # true
```

## Logical operators

| Name  | Example   | Result                                     |
|-------|-----------|--------------------------------------------|
| And   | a && b    | true if both values are true               |
| Or    | a \|\| b  | true if at least one of the values is true |

```borealis
1 && 1 # true
1 && 0 # false
1 || 1 # true
1 || 0 # true
0 || 0 # false
```

## String operators

There are two string operators:

* The concatenation operator `^` which returns the concatenation of the left and right operands.
* The concatenation assignment operator `^=` which appends the right operand to the left operand.

```borealis
print('hello ' ^ 'world');
```

```borealis
any msg = 'hello ';
msg ^= 'world';
```

## Increment/decrement operators

| Name            | Example | Result                              |
|-----------------|---------|-------------------------------------|
| Pre-increment   | ++a     | increments a by one, then returns a |
| Pos-increment   | a++     | returns a, then increments a by one |
| Pre-decrement   | --a     | decrements a by one, then returns a |
| Pos-decrement   | a--     | returns a, then decrements a by one |

_The increment/decrement operators convert the value to its numeric representation before incrementing or decrementing it._

```borealis
any a;

a = 5;
print('Should be 5: ' ^ a++);
print('Should be 6: ' ^ a);

a = 5;
print('Should be 6: ' ^ ++a);
print('Should be 6: ' ^ a);

a = 5;
print('Should be 5: ' ^ a--);
print('Should be 4: ' ^ a);

a = 5;
print('Should be 4: ' ^ --a);
print('Should be 4: ' ^ a);
```
