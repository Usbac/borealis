The standard object `Object` gives you multiple methods related with object.

## Functions

### Get keys

`getKeys(object o): array`

Returns all the object properties keys.

Only public properties are taken into account.

```borealis
any foo = obj {
    public any name = 'alex';
    public any age = 23;
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
    public any name = 'alex';
    public any age = 23;
    any password = '1234';
};

Object.getValues(foo); # Returns [ 'alex', 23 ]
```
