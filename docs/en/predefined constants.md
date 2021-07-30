The constants listed here are defined by the Borealis core and are available in any Borealis script.

| Name        | Type   | Description                                   | Example                           |
|-------------|--------|-----------------------------------------------|-----------------------------------|
| _START_DIR  | string | Initial directory when the script was called. | /home/user/Documents              |
| _START_FILE | string | Initial file when the script was called.      | /home/user/Documents/main.bor     |
| _FILE       | string | File name of the current file script.         | /home/user/Documents/imported.bor |
| _VERSION    | string | Borealis version.                             | 1.0                               |
| _ARGS       | array  | List of arguments given to the script.        | [ './borealis', '-f', 'main.bor' ]|