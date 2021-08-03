The import statement includes and evaluates the specified file.

```borealis
import "file.bor";
```

If the file cannot be accessed an error will be thrown.

The source code of a file will only be evaluated the first time it's imported.

The given path can be either relative or absolute.

_The standard function Os.isAbsolute is used internally to check if the path is absolute or not._

## File scope

Elements in Borealis like constants and variables have a scope limited to the file where they have been declared. Meaning that you can import a file `a.bor` and a file `b.bor` with variables declarations sharing the same name without any problem.

## Return value

The import statement holds the value returned by the file. This is useful for getting elements from another file.

number.bor
```borealis
return 42;
```

main.bor
```borealis
any num = import "number.bor";

print(num); # It will print '42'
```

The value returned by an import is actually a reference so you can modify its state in multiple imports across multiple files.

number.bor
```borealis
return 0;
```

main.bor
```borealis
any num = import "number.bor";
num++;

any another = import "number.bor";
another++;

print(num); # It will print '2'
```
