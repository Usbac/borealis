The standard table `Table` gives you multiple methods related with tables.

## Functions

### Get

`get(table tab, string key): any`

Returns the element in the table with the given key.

```borealis
any tab = [ 'a' = '1', 'b' = '2' ];

Table.get(tab, 'a'); # Returns '1'
```

### Get size

`getSize(table tab): number`

Returns the size of the table.

```borealis
Table.getSize([ 1, 2, 3, 4 ]); # Returns 4
```

### Push

`push(table tab, any value)`

Pushes a new element at the end of the table.

```borealis
any tab = [];
Table.push(tab, 1);
Table.push(tab, 'hello');

# Here tab will be [ 1, 'hello' ]
```

### Prepend

`prepend(table tab, any value)`

Inserts a new element at the start of the table.

```borealis
any tab = [ 2 ];
Table.prepend(tab, 1);

# Here tab will be [ 1, 2 ]
```

### Pop

`pop(table tab): any`

Pops an element off the end of the table and returns it.

```borealis
any tab = [ 1, 2, 3 ];

any last = Table.pop(tab); # Returns 3
# Here tab will be [ 1, 2 ]
```

If the table is empty the function will return `null`.

### Shift

`shift(table tab): any`

Shifts an element off the start of the table and returns it.

```borealis
any tab = [ 1, 2, 3 ];

any first = Table.shift(tab); # Returns 1
# Here tab will be [ 2, 3 ]
```

### Get keys

`getKeys(table tab): table`

Returns all the keys of a table.

```borealis
any tab = [
    'os' = 'linux'
    'version' = '5.1',
];

Table.getKeys(tab); # Returns [ 'os', 'version' ]
```

### Has key

`hasKey(table tab, string key): number`

Returns `true` if the table has the specified key, `false` otherwise.

```borealis
any tab = [ 'a' = 1, 'b' = 2, 'c' = 3 ];
Table.hasKey(tab, 'a'); # Returns true
```

```borealis
Table.hasKey([ 'name' = 1, 'city' = 2 ], 'country'); # Returns false
```

### Join

`join(table tab, string delimiter = ''): string`

Joins all the elements in the table with a string.

```borealis
Table.join([ 'a', 'b', 'c' ]); # Returns 'abc'
```

```borealis
Table.join([ 1, 2, 3 ], '-'); # Returns '1-2-3'
```

### Has value

`has(table tab, any value): number`

Returns `true` if the given value exists in the table, `false` otherwise.

A strict comparison is used by the function.

```borealis
Table.has([ 1, 2, 3 ], 3); # Returns true
```

```borealis
Table.has([ 'a', [ 'foo' ], null ], [ 'foo' ]); # Returns true
```

Strict comparison example:

```borealis
Table.has([ '1', '2', '3' ], 3); # Returns false
```

### Reverse

`reverse(table tab): table`

Returns the table reversed.

```borealis
Table.reverse([ 1, 2, 3 ]); # Returns [ 3, 2, 1 ]
```

### Range

`range(number start, number end, number steps = true): table`

Returns a table containing a range of elements.

```borealis
Table.range(0, 3); # Returns [ 1, 2, 3 ]
```

With steps:
```borealis
Table.range(0, 5, 2); # Returns [ 0, 2, 4 ]
```

### Sort

`sort(table tab, function func = f() {})`

Sorts the given table.

Elements will be arranged from lowest to highest using their numeric representation.

The [quicksort](https://www.google.com/search?q=quicksort) algorithm is used by this function.

_The table keys are preserved._

```borealis
any tab = [ 4, 2, 8, 0 ];
Table.sort(tab);

# Here tab will be [ 0, 2, 4, 8 ]
```

This function accepts an optional user-supplied comparison function to determine the order.

Closure format:

`number func(a, b)`

The comparison function must return a number less than, equal to, or greater than zero if the first argument is considered to be respectively less than, equal to, or greater than the second.

```borealis
any people = [
    [
        'name' = 'Steve',
        'age' = 23,
    ],
    [
        'name' = 'Bill',
        'age' = 20,
    ],
    [
        'name' = 'Linus',
        'age' = 15,
    ],
];

Table.sort(people, f(a, b) {
    return a['age'] - b['age'];
});

/**
 * Here the people table will be like this:
 * [
 *     [
 *         'name' = 'Linus',
 *         'age' = 15,
 *     ],
 *     [
 *         'name' = 'Bill',
 *         'age' = 20,
 *     ],
 *     [
 *         'name' = 'Steve',
 *         'age' = 23,
 *     ],
 * ]
 */
```

### Column

`column(table tab, string column): table`

Returns the values from a single column in the table.

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

Table.column(users, 'name'); # Returns [ 'alex', 'evan', 'carlos' ]
```

### Map

`map(table tab, function func): table`

Applies the closure to the elements of the given table.

Closure format:

`func(element[, index])`

The closure must take at least one parameter which is the element in the current iteration.

Array keys are preserved.

```borealis
Table.map([ 1, 2, 3 ], f(e) {
    return e + 1;
}); # Returns [ 2, 3, 4 ]
```

```borealis
Table.map([ 'a', 'b', 'c' ], String.toUpper); # Returns [ 'A', 'B', 'C' ]
```

An optional second parameter can be given to the closure which will be used as the iteration index.

```borealis
Table.map([ 'a', 'b', 'c' ], f(el, index) {
    print(el ^ index);
}); # It will print 'a0b1c2'
```

### Filter

`filter(table tab, function func): table`

Filters elements of a table using a closure.

Closure format:

`func(element[, index])`

The closure must take at least one parameter which is the element in the current iteration.

Array keys are preserved.

```borealis
Table.filter([ 1, 2, 3, 4, 5, 6 ], f(e) {
    return e % 2 == 0;
}); # Returns [ 2, 4, 6 ]
```

An optional second parameter can be given to the closure which will be used as the iteration index.

```borealis
Table.filter([ 'a', 'b', 'c' ], f(el, index) {
    print(el ^ index);
    return true;
}); # It will print 'a0b1c2'
```

### Reduce

`reduce(table tab, function func, any initial_val = 0): any`

Iteratively reduces the table to a single value using a closure.

Closure format:

`func(accumulator, element[, index])`

The closure must take at least two parameters which are the accumulator and the element in the current iteration.

Array keys are preserved.

```borealis
Table.reduce([ 1, 2, 3 ], f(accumulator, el) {
    return accumulator + el;
}); # Returns 6
```

An initial value can be given as third parameter.

```borealis
Table.reduce([ 1, 2, 3 ], f(accumulator, el) {
    return accumulator + el;
}, 10); # Returns 60
```

An optional third parameter can be given to the closure which will be used as the iteration index.

```borealis
Table.reduce([ 1, 2, 3 ], f(accumulator, el, index) {
    return accumulator + el;
});
```

### Some

`some(table tab, function func): bool`

Returns `true` whether at least one element in the table passes the test implemented by the given closure, `false` otherwise.

Closure format:

`func(element[, index])`

The closure must take at least one parameter which is the element in the current iteration.

```borealis
Table.some([ 1, 2, 3, 4 ], f(el) {
    return el > 0;
}); # Returns true
```

```borealis
Table.some([ 0, -3, -10, -2 ], f(el) {
    return el > 0;
}); # Returns false
```

An optional second parameter can be given to the closure which will be used as the iteration index.

```borealis
Table.some([ 1, 2, 3, 4 ], f(el, index) {
    return el > 0;
});
```

### Merge

`merge(table tab, ...values): table`

Returns a merge of all the given tables.

If the input tables have the same keys, then the later value for that key will overwrite the previous one.

```borealis
any tab1 = [
    'name' = 'alex',
    'age' = 23,
];

any tab2 = [
    'name' = 'juan',
    'city' = 'NY',
];

Table.merge(tab1, tab2); # Returns [ 'name' = 'juan', 'age' = 23, 'city' = 'NY' ]
```

### Fill

`fill(number start_index, number length, any value): table`

Returns a table filled with the given value.

The table keys will start at the specified start_index parameter.

```borealis
Table.fill(0, 3, 1912); # Returns [ 1912, 1912, 1912 ]
```

```borealis
Table.fill(5, 6, 'apple'); # Returns [ '5' = 'apple', '6' = 'apple', '7' = 'apple', '8' = 'apple', '9' = 'apple', '10' = 'apple' ]
```
