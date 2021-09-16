<h1 align="center">
  <img src="https://image.usbac.com.ve/Borealis.png" alt="Borealis logo" width="200">
  <br>
  Borealis
  <br>
</h1>

<h4 align="center">The elegant and consistent programming language.</h4>

<p align="center">
<img src="https://app.travis-ci.com/Usbac/borealis.svg">
<img src="https://img.shields.io/badge/stable-0.2.0-blue.svg">
<img src="https://img.shields.io/badge/license-MIT-orange.svg">
</p>

Borealis is a small, elegant and consistent scripting language. Includes a C-like syntax that's easy to implement.

## Features

* **Consistent**: Borealis offers a simple syntax and a consistent standard library. Forget about things like strange results when comparing different data types, standard functions with unexpected names or operators with weird behaviour.
* **Easy to use**: Borealis tries to keep things simple and minimal, from its standard library and data types to its deployment and compilation. Even its source code has been meticulously written, following the [ANSI C99](https://wikipedia.org/wiki/C99) standard.
* **Comprehensive**: The language offers multiple features useful for reducing common development headaches, like a built-in REPL debugger, statically typed variables, private scope by default, different operators for strings and numbers, first-class functions and more.

## Install

1. Compile the software with the `make` command or download it for your OS in the releases page.

2. Run the following command where the Borealis executable is located: `sudo cp ./borealis /usr/local/bin`

    Or move to your Borealis folder and run the command: `sudo make install`

Now you should be able to access Borealis by running `borealis` in your terminal.

## Examples

Operators:
```borealis
foo == bar; # Compare as numbers
foo eq bar; # Compare as strings
foo + bar; # Addition
foo ^ bar; # Concatenation
foo << bar; # Function call and assignation
```

General use and debugging:
```borealis
any lib = import 'lib.bor';

debug(); # Start REPL debugger at this line

Io.printLine('Goodbye!');
exit(0);
```

Functions:
```borealis
number fact(number n) {
    if (n == 0) {
        return 1;
    }

    return n * fact(n - 1);
}

fact(7); # 5040
```

Objects:
```borealis
object user = obj {
    pub string name = 'alex';
    pub number age = 23;
};

Io.printLine('Name: ' ^ user.name ^ '. Age: ' ^ user.age);
```

## Testing

Just run the command: `make test`

Or run the following command in your Borealis repository folder: `borealis -f ./tests/main.bor`

## Documentation

First time using it? Read the [Documentation](docs/en).

## Contributing

Any contribution or support to this project in the form of a pull request or message will be highly appreciated. ❤️

You can read more about the contribution process [right here](CONTRIBUTING.md). Don't be shy. :)

## License

Borealis is open-source software licensed under the [MIT license](LICENSE).
