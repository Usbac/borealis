The standard table `Bit` gives you multiple methods related with bit manipulation.

## Functions

### And

`and(number a, number b): number`

Returns the bitwise operation and.

```borealis
Bit.and(2, 3); # Returns 2
```

### Or

`or(number a, number b): number`

Returns the bitwise operation or.

```borealis
Bit.or(3, 6); # Returns 7
```

### Left shift

`leftShift(number a, number b): number`

Returns the bitwise operation left shift.

```borealis
Bit.leftShift(-3, 2); # Returns -12
```

### Right shift

`rightShift(number a, number b): number`

Returns the bitwise operation right shift.

```borealis
Bit.rightShift(8, 2); # Returns 2
```

### Not

`not(number a, number b): number`

Returns the bitwise operation not.

```borealis
Bit.not(-1); # Returns 0
```
