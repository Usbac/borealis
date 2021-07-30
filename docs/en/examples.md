There's no easier way to learn than by looking at examples.

This page includes some examples of common or simple algorithms.

Keep in mind that there is not a consistent style used across the examples since this is supposed to serve as a reference for the Borealis syntax.

## Even or odd

```borealis
any num = Io.readln('Enter a number: ');

if (num % 2 == 0) {
    printLine('The number is even');
} else {
    printLine('The number is odd');
}
```

## Find maximum value

```borealis
any findMax(arr) {
    any max = 0;

    foreach (arr, val) {
        if (val > max) {
            max = val;
        }
    }

    return max;
}

findMax([ 1, 42, 70, 3000, 200, -120, 98, ]);
```

## Fibonacci

```borealis
number fib(number n) {
    if (n <= 1) {
        return n;
    }

    return fib(n - 1) + fib(n - 2);
}

fib(9);
```

## Bubble sort

```borealis
any sort(array arr) {
    any length = Array.getSize(arr);

    foreach (Array.range(0, length - 1), i) {
        foreach (Array.range(0, length - 1), j) {
            if (arr[j + 1] !== null && arr[j] > arr[j + 1]) {
                any tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }

    return arr;
}

sort([ 12, 234, 8765, 0, -123, 23,2 ]);
```

## List files in directory (recursive)

```borealis
any listFiles(dir) {
    foreach (Os.getFiles(dir), file) {
        any sub_path = dir ^ '/' ^ file;
        printLine(Os.clearPath(sub_path));

        if (Os.isDir(sub_path)) {
            listFiles(sub_path);
        }
    }
}

any path = Io.readln('Enter a directory: ');

if (!Os.isDir(path)) {
    printLine('Path is no a valid directory!');
} else {
    listFiles(path);
}
```
