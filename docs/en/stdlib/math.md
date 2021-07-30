The standard object `Math` gives you multiple methods related with math.

## Constants

The object also has some useful constants.

| Name   | Description                     |
|--------|---------------------------------|
| PI     | PI. (3.14159...)                |
| E      | Euler. (2.71828...)             |
| LN2    | Natural Logarithm. (0.69314...) |
| SQRT2  | Square root of 2. (1.41421...)  |

### Examples

```borealis
Math.PI; # Returns 3.14159...
```

```borealis
Math.E; # Returns 2.71828...
```

## Functions

### Square root

`sqrt(number n): number`

Returns the square root of the given number.

```borealis
Math.sqrt(841); # Returns 29
```

### Natural logarithm

`log(number n): number`

Returns the natural logarithm of the given number.

```borealis
Math.log(4.6); # Returns 1.5260563034950492
```

### Absolute

`abs(number n): number`

Returns the absolute value of the given number.

```borealis
Math.abs(-55); # Returns 55
```

### Floor

`floor(number n): number`

Returns the nearest integer less than the given number.

```borealis
Math.floor(1.234); # Returns 1
```

### Ceil

`ceil(number n): number`

Returns the nearest integer greater than the given number.

```borealis
Math.ceil(1.234); # Returns 2
```

### Round

`round(number n, number decimals = 0): number`

Returns the number rounded with the given number of decimals.

```borealis
Math.round(1.52); # Returns 2
```

```borealis
Math.round(1.5234567, 2); # Returns 1.52
```

### Cosine

`cos(number n): number`

Returns the cosine of the given number.

```borealis
Math.cos(1.4); # Returns 0.16996714290024104
```

### Arc cosine

`acos(number n): number`

Returns the arc cosine of the given number.

```borealis
Math.acos(0.4); # Returns 1.1592794807274085
```

### Hyperbolic cosine

`cosh(number n): number`

Returns the hyperbolic cosine of the given number.

```borealis
Math.cosh(1.7); # Returns 2.8283154578899672
```

### Arc hyperbolic cosine

`acosh(number n): number`

Returns the inverse hyperbolic cosine of the given number.

```borealis
Math.acosh(1.7); # Returns 1.1232309825872959
```

### Sine

`sin(number n): number`

Returns the sine of the given number.

```borealis
Math.sin(1.4); # Returns 0.98544972998846014
```

### Arc sine

`asin(number n): number`

Returns the arc sine of the given number.

```borealis
Math.asin(0.4); # Returns 0.41151684606748806
```

### Hyperbolic sine

`sinh(number n): number`

Returns the hyperbolic sine of the given number.

```borealis
Math.sinh(1.7); # Returns 2.6456319338372323
```

### Arc hyperbolic sine

`asinh(number n): number`

Returns the inverse hyperbolic sine of the given number.

```borealis
Math.asinh(1.7); # Returns 1.3008204268406467
```

### Tangent

`tan(number n): number`

Returns the tangent of the given number.

```borealis
Math.tan(1.4); # Returns 5.7978837154828868
```

### Arc tangent

`atan(number n): number`

Returns the arc tangent of the given number.

```borealis
Math.atan(2.4); # Returns 1.176005207095135
```

### Hyperbolic tangent

`tanh(number n): number`

Returns the hyperbolic tangent of the given number.

```borealis
Math.tanh(2.4); # Returns 0.98367485769368024
```

### Arc hyperbolic tangent

`atanh(number n): number`

Returns the inverse hyperbolic tangent of the given number.

```borealis
Math.atanh(0.45); # Returns 0.48470027859405174
```
