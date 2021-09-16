The standard object `Object` gives you multiple methods related with object.

## Functions

### Get keys

`getKeys(object o): array`

Returns all the object properties keys.

Only public properties are taken into account.

```borealis
any foo = obj {
    pub any name = 'alex';
    pub any age = 23;
    any password = '1234';
};

Object.getKeys(foo); # Returns [ 'name', 'age' ]
```

### Get values

`getValues(object o): array`

Returns all the object properties values.

Only public properties are taken into account.

```borealis
any foo = obj {
    pub any name = 'alex';
    pub any age = 23;
    any password = '1234';
};

Object.getValues(foo); # Returns [ 'alex', 23 ]
```
