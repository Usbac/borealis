The standard object `Json` gives you multiple methods related with json handling.

## Functions

### Convert to JSON

`stringify(any value): string`

Returns the given value as a JSON.

```borealis
Json.stringify([ 1, 2, 3 ]); # Returns '[1, 2, 3]'
```

For objects, only the public properties are taken into account and will be available in the Json string.

```borealis
Json.stringify(obj {
    pub any name = 'foo';
    pub any age = 23;
    any bar = 1;
}); # Returns '{"name":"foo","age":23}'
```

### Parse JSON

`parse(string json): any`

Parses the given JSON string and returns its value.

If the given string is not a valid JSON, `null` will be returned. To check if a string is a valid JSON use the `isValid` function.

```borealis
Json.parse('[1, 2, 3]'); # Returns [ 1, 2, 3 ]
```

```borealis
Json.parse('{"name":"foo","age":23}');
# Returns obj { pub any name = 'foo'; pub any age = 23; }
```

### Is valid

`isValid(string json): number`

Returns `true` if the given string is a valid JSON representation, `false` otherwise.

```borealis
Json.isValid('[1, 2, 3]'); # Returns true
```

```borealis
Json.isValid('{ a = 1 }'); # Returns false
```
