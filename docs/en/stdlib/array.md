The standard object `Array` gives you multiple methods related with arrays.

## Functions

### Get

`get(array arr, string key): any`

Returns the element in the array with the given key.

```borealis
any arr = [ 'a' = '1', 'b' = '2' ];

Array.get(arr, 'a'); # Returns '1'
```

### Get size

`getSize(array arr): number`

Returns the size of the array.

```borealis
Array.getSize([ 1, 2, 3, 4 ]); # Returns 4
```

### Push

`push(array arr, any value)`

Pushes a new element at the end of the array.

```borealis
any arr = [];
Array.push(arr, 1);
Array.push(arr, 'hello');

# Here arr will be [ 1, 'hello' ]
```

### Prepend

`prepend(array arr, any value)`

Inserts a new element at the start of the array.

```borealis
any arr = [ 2 ];
Array.prepend(arr, 1);

# Here arr will be [ 1, 2 ]
```

### Pop

`pop(array arr): any`

Pops a element off the end of the array and returns it.

```borealis
any arr = [ 1, 2, 3 ];

any last = Array.pop(arr); # Returns 3
# Here arr will be [ 1, 2 ]
```

If the array is empty the function will return `null`.

### Shift

`shift(array arr): any`

Shifts a element off the start of the array and returns it.

```borealis
any arr = [ 1, 2, 3 ];

any first = Array.shift(arr); # Returns 1
# Here arr will be [ 2, 3 ]
```

### Get keys

`getKeys(array arr): array`

Returns all the keys of an array.

```borealis
any arr = [
    'os' = 'linux'
    'version' = '5.1',
];

Array.getKeys(arr); # Returns [ 'os', 'version' ]
```

### Has key

`hasKey(array arr, string key): number`

Returns `true` if the array has the specified key, `false` otherwise.

```borealis
any arr = [ 'a' = 1, 'b' = 2, 'c' = 3 ];
Array.hasKey(arr, 'a'); # Returns true
```

```borealis
Array.hasKey([ 'name' = 1, 'city' = 2 ], 'country'); # Returns false
```

### Join

`join(array arr, string delimiter = ''): string`

Joins all the elements in the array with a string.

```borealis
Array.join([ 'a', 'b', 'c' ]); # Returns 'abc'
```

```borealis
Array.join([ 1, 2, 3 ], '-'); # Returns '1-2-3'
```

### Has value

`has(array arr, any value): number`

Returns `true` if the given value exists in the array, `false` otherwise.

A strict comparison is used by the function.

```borealis
Array.has([ 1, 2, 3 ], 3); # Returns true
```

```borealis
Array.has([ 'a', [ 'foo' ], null ], [ 'foo' ]); # Returns true
```

Strict comparison example:

```borealis
Array.has([ '1', '2', '3' ], 3); # Returns false
```

### Reverse

`reverse(array arr): array`

Returns the array reversed.

```borealis
Array.reverse([ 1, 2, 3 ]); # Returns [ 3, 2, 1 ]
```

### Range

`range(number start, number end, number steps = 1): array`

Returns an array containing a range of elements.

```borealis
Array.range(0, 3); # Returns [ 1, 2, 3 ]
```

```borealis
Array.range(0, 5, 2); # Returns [ 0, 2, 4 ]
```

### Sort

`sort(array arr)`

Sorts the given array.

Elements will be arranged from lowest to highest using their numeric representation.

The [quicksort](https://www.google.com/search?q=quicksort) algorithm is used by this function.

```borealis
any arr = [ 4, 2, 8, 0 ];
Array.sort(arr);

# Here arr will be [ 0, 2, 4, 8 ]
```

### Column

`column(array arr, string column): array`

Returns the values from a single column in the array.

```borealis
any users = [
    [
        'name' = 'alex',
        'age' = 23,
    ],
    [
        'name' = 'evan',
        'age' = 29,
    ],
    [
        'name' = 'carlos',
        'age' = 32,
    ],
];

Array.column(users, 'name'); # Returns [ 'alex', 'evan', 'carlos' ]
```

### Map

`map(array arr, function func): array`

Applies the closure to the elements of the given array.

Closure format:

`func(element[, index])`

The closure must take at least one parameter which is the element in the current iteration.

Array keys are preserved.

```borealis
Array.map([ 1, 2, 3 ], f(e) {
    return e + 1;
}); # Returns [ 2, 3, 4 ]
```

```borealis
Array.map([ 'a', 'b', 'c' ], String.toUpper); # Returns [ 'A', 'B', 'C' ]
```

An optional second parameter can be given to the closure which will be used as the iteration index.

```borealis
Array.map([ 'a', 'b', 'c' ], f(el, index) {
    print(el ^ index);
}); # Will print 'a0b1c2'
```

### Filter

`filter(array arr, function func): array`

Filters elements of an array using a closure.

Closure format:

`func(element[, index])`

The closure must take at least one parameter which is the element in the current iteration.

Array keys are preserved.

```borealis
Array.filter([ 1, 2, 3, 4, 5, 6 ], f(e) {
    return e % 2 == 0;
}); # Returns [ 2, 4, 6 ]
```

An optional second parameter can be given to the closure which will be used as the iteration index.

```borealis
Array.filter([ 'a', 'b', 'c' ], f(el, index) {
    print(el ^ index);
    return true;
}); # Will print 'a0b1c2'
```

### Reduce

`reduce(array arr, function func, any initial_val = 0): any`

Iteratively reduces the array to a single value using a closure.

Closure format:

`func(accumulator, element[, index])`

The closure must take at least two parameters which are the accumulator and the element in the current iteration.

Array keys are preserved.

```borealis
Array.reduce([ 1, 2, 3 ], f(accumulator, el) {
    return accumulator + el;
}); # Returns 6
```

An initial value can be given as third parameter.

```borealis
Array.reduce([ 1, 2, 3 ], f(accumulator, el) {
    return accumulator + el;
}, 10); # Returns 60
```

An optional third parameter can be given to the closure which will be used as the iteration index.

```borealis
Array.reduce([ 1, 2, 3 ], f(accumulator, el, index) {
    return accumulator + el;
});
```

### Some

`some(array arr, function func): bool`

Returns `true` whether at least one element in the array passes the test implemented by the given closure, `false` otherwise.

Closure format:

`func(element[, index])`

The closure must take at least one parameter which is the element in the current iteration.

```borealis
Array.some([ 1, 2, 3, 4 ], f(el) {
    return el > 0;
}); # Returns true
```

```borealis
Array.some([ 0, -3, -10, -2 ], f(el) {
    return el > 0;
}); # Returns false
```

An optional second parameter can be given to the closure which will be used as the iteration index.

```borealis
Array.some([ 1, 2, 3, 4 ], f(el, index) {
    return el > 0;
});
```

### Merge

`merge(array arr, ...values): array`

Returns a merge of all the given arrays.

If the input arrays have the same keys, then the later value for that key will overwrite the previous one.

```borealis
any arr1 = [
    'name' = 'alex',
    'age' = 23,
];

any arr2 = [
    'name' = 'juan',
    'city' = 'NY',
];

Array.merge(arr1, arr2); # Returns [ 'name' = 'juan', 'age' = 23, 'city' = 'NY' ]
```
