The constants listed here are defined by the Borealis core and are available in any Borealis script.

| Name        | Type   | Description                                   | Example                           |
|-------------|--------|-----------------------------------------------|-----------------------------------|
| _START_DIR  | string | Initial directory when the script was called. | /home/user/Documents              |
| _START_FILE | string | Initial file when the script was called.      | /home/user/Documents/main.bor     |
| _FILE       | string | File name of the current file script.         | /home/user/Documents/imported.bor |
| _VERSION    | string | Borealis version.                             | 1.0                               |
| _ARGS       | table  | List of arguments given to the script.        | [ './borealis', '-f', 'main.bor' ]|
| _GLOBALS    | table  | Table that can be used to store globals.      | []                                |

## Examples

```borealis
print('The current script file is: ' ^ _START_FILE);
```

```borealis
print('Running Borealis version ' ^ _VERSION);
```

```borealis
any printScriptArgs() {
    foreach (_ARGS, arg) {
        print(arg);
    }
}

printScriptArgs();
```
