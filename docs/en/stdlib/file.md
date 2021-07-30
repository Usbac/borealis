The standard object `File` gives you multiple methods related with files handling.

## Functions

### Create

`create(string filename): number`

Creates a file with the given filename.

Returns -1 if the file already exists, 1 in case of success or 0 in case of errors.

```borealis
File.exists('./file.txt');
```

### Read

`read(string filename, string encoding = 'utf8'): any`

Returns the content of the file. If the file does not exists or an error has ocurred while reading it, it will return `null`.

Valid values for the encoding parameter are: 'utf8' (UTF-8), 'hex' (Hexadecimal) and 'base64' (Base64).

The function will read the file in binary mode when using the 'hex' and 'base64' encodings.

```borealis
File.read('./has_hello.txt'); # In this case it returns 'hello'
```

```borealis
File.read('./has_hello.txt', 'hex'); # Returns '68656C6C6F0A'
```

### Write

`write(string filename, string content, number override = 1): number`

Writes text to a file. If the file does not exists, it will be created.

Returns `true` in case of success, `false` otherwise.

If the override parameter is true, the function will override.

```borealis
File.write('./hello.txt', 'hello world'); # Returns true if 'hello world' has been written successfully
```

```borealis
File.write('file.txt', 'awesome text', false);
# Will write 'awesome text' only if the file does not exists
```

### Append

`append(string filename, string content): number`

Appends text to the file.

Returns `true` in case of success, `false` otherwise.

```borealis
File.append('file.txt', ' world'); # Will append ' world' to file.txt
```

```borealis
File.write('new.txt', 'hello ');
File.append('new.txt', 'world');

# The file new.txt should have 'hello world'
```

### Copy

`copy(string source_path, string dest_path): number`

Copies the file with the source path to the destination path.

Returns `true` in case of success, `false` otherwise.

_The destination path needs to contain the actual file name, not just the path to the folder you wish to copy._

```borealis
File.copy('./original.txt', './new.txt'); # new.txt should be a copy of original.txt
```

### Get modification time

`getModTime(string filename): any`

Returns the time of the last modification to the file as an unix timestamp (in seconds).

If the file does not exists or is not accesible, `null` will be returned.

```borealis
File.getModTime('file.txt'); # Can return something like 1620226469
```

### Get type

`getType(string filename): any`

Returns the type of the file as a string. If the file does not exists or is not accesible `null` is returned.

Return values are:

| Name             | Description               |
|------------------|---------------------------|
| directory        | Directory.                |
| file             | File.                     |
| char             | Character special device. |
| link             | Symbolic link.            |
| fifo             | FIFO (named pipe).        |
| block            | Block special device.     |
| unknown          | Unknown.                  |

```borealis
File.getType('text_file.txt'); # Returns 'file'
```

```borealis
File.getType('../folder'); # Returns 'directory'
```

### Get size

`getSize(string filename): any`

Returns the size of the file in bytes. If the file does not exists or is not accesible `null` is returned.

```borealis
File.getSize('file.txt'); # Returns 471056 if the file size is '471,1 kB'
```
