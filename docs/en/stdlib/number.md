The standard table `Number` gives you multiple methods related with numbers.

## Constants

The table also has some useful constants.

| Name   | Description                          |
|--------|--------------------------------------|
| NAN    | Not a number.                        |
| MIN    | Minimum representable finite number. |
| MAX    | Maximum representable finite number. |

### Examples

```borealis
Number.NAN;
```

```borealis
Number.MAX; # Returns the maximum number
```

## Functions

### Minimum

`min(...values): number`

Returns the minimum value of all the arguments.

The arguments are treated as numbers.

```borealis
Number.min(5, 8, 2, -4); # Returns -4
```

### Maximum

`max(...values): number`

Returns the maximum value of all the arguments.

The arguments are treated as numbers.

```borealis
Number.max(5, 8, 2, -4); # Returns 8
```

### Is NAN

`isNan(any value): number`

Returns `true` if the given value is NaN, `false` otherwise.

```borealis
Number.isNan(0/0); # Returns true
```

```borealis
Number.isNan('7'); # Returns false
```

### Is finite

`isFinite(any value): number`

Returns `true` if the given value is finite, `false` otherwise.

```borealis
Number.isFinite(5); # Returns true
```

```borealis
Number.isFinite(1/0); # Returns false
```

### Random

`rand(number max): number`

Returns a pseudo random number between 0 and the given value, or 1 if no value is supplied.

```borealis
Number.rand(); # Returns 0.13
```

```borealis
Number.rand(100); # Returns 36
```

### Seed random

`sRand(number seed)`

Seeds the pseudo random number generator used by the `rand` function.

```borealis
Number.sRand(0.5);
```

### Format

`format(number n, string thousands_sep = '', string decimal_sep = '.'): string`

Returns the given number formatted with the specified thousands and decimal separators.

```borealis
Number.format(1800300.45, '.', ','); # Returns '1.800.300,45'
```
