The standard table `Os` gives you multiple methods related with the operating system.

## Constants

The table also has some useful constants.

| Name     | Type   | Description                                          | Example |
|----------|--------|------------------------------------------------------|---------|
| EOL      | String | End of line used by the system.                      | \r\n    |
| PLATFORM | String | Platform name (windows, linux, mac, bsd or unknown). | windows |

### Examples

```borealis
Os.PLATFORM; # Returns the platform name
```

```borealis
Os.EOL; # In Windows it will return "\r\n"
```

## Functions

### Get current working directory

`getCWD(): string`

Returns the current working directory.

```borealis
Os.getCWD();
# If the script is running at /home/user/Documents it will return '/home/user/Documents'
```

### Clear path

`clearPath(string path): string`

Returns the given path resolved.

```borealis
Os.clearPath('/home/./user/Documents/../Downloads'); # Returns '/home/user/Downloads'
```

```borealis
Os.clearPath('./borealis/../main.bor'); # Returns '/borealis/main.bor'
```

### Is file

`isFile(string path): number`

Returns `true` if the given path is for an existing file, `false` otherwise.

```borealis
Os.isFile('./main.bor'); # If './main.bor' is a file it will return true
```

### Is directory

`isDir(string path): number`

Returns `true` if the given path is for an existing directory, `false` otherwise.

```borealis
Os.isDir('./pictures'); # If './pictures' is a directory it will return true
```

### Exists

`exists(string path): number`

Returns `true` if the given path exists, `false` otherwise.

An existing path could refer to an existing directory or file.

```borealis
Os.exists('./pictures'); # If './pictures' exists it will return true
```

```borealis
Os.exists('./users.txt'); # If './users.txt' exists it will return true
```

### Rename

`rename(string source_path, string destination_path): number`

Renames the file referred by the source path to the destination path.

Returns `true` if the file has been renamed, `false` otherwise.

```borealis
Os.rename('./users_1.txt', './users.txt');
```

### Remove

`remove(string path)`

Deletes the file with the given path.

Returns `true` if the file has been deleted, `false` otherwise.

```borealis
Os.remove('./users_1.txt');
```

### Make directory

`makeDir(string path): number`

Creates a directory with the given path.

Returns `true` if the directory has been created, `false` otherwise.

The directory is created with 0777 permissions.

```borealis
Os.makeDir('./pictures');
```

### Get files

`getFiles(string path): table`

Returns a table with all the directories and files in the given path.

If the directory path is invalid, `false` is returned.

```borealis
Os.getFiles('./folder'); # Can return something like [ '.gitignore', 'src', 'Makefile' ]
```

### Get permissions

`getPermissions(string path): string`

Returns the permissions of the given path.

If the path does not exist, `false` is returned.

```borealis
Os.getPermissions('./my_file.txt'); # Can return '0755'
```

### Is absolute path

`isAbsolute(string path): number`

Returns `true` if the given path is absolute, `false` otherwise.

An absolute path starts with either '~' or '/' in all systems except for Windows. In Windows an absolute path is a path that starts with an uppercase letter followed by a colon (like 'C:' or 'D:').

```borealis
Os.isAbsolute('~/Documents/file.txt'); # Returns true in every system but Windows
```

```borealis
Os.isAbsolute('D:\\file.txt'); # Returns true only in Windows
```

### Change permissions

`chmod(string path, string permissions): number`

Changes the permissions of the given path.

Returns `true` on success, `false` otherwise.

```borealis
Os.chmod('./file.txt', '0755');
```

### Change owner

`chmod(string path, string user, string group = ''): number`

Changes the owner of the given path.

Returns `true` on success, `false` otherwise.

_On windows this function always returns `false`._

```borealis
Os.chown('./file.txt', 'alex');
```

### Get user

`getUser(string name): any`

Returns a table with information about a user with the given name.

If the given user is invalid, `null` is returned.

_On Windows this function always returns `null`._

The table has the following properties:

| Name      | Type   | Description                                                                                                                        |
|-----------|--------|------------------------------------------------------------------------------------------------------------------------------------|
| name      | string | The username of the user.                                                                                                          |
| password  | string | The user's password in an encrypted format (for example, on a system employing shadow passwords, an asterisk is returned instead). |
| directory | string | The absolute path to the home directory of the user.                                                                               |
| id        | number | The id of the user.                                                                                                                |
| group_id  | number | The group id of the user.                                                                                                          |
| shell     | string | The absolute path to the executable of the user's default shell.                                                                   |

```borealis
Os.getUser('root');

/* The above code can return something like this:
 * [
 *    'name' = 'root',
 *    'password' = 'x',
 *    'directory' = '/root',
 *    'id' = 0,
 *    'group_id' = 0,
 *    'shell' = '/bin/bash',
 * ]
 */
```

### Get group

`getGroup(string name): any`

Returns a table with information about a group with the given name.

If the given group is invalid, `null` is returned.

_On Windows this function always returns `null`._

The table has the following properties:

| Name      | Type   | Description                                  |
|-----------|--------|----------------------------------------------|
| name      | string | The group's name.                            |
| password  | string | The group's password in an encrypted format. |
| id        | number | The id of the group.                         |
| members   | table  | The list of users in the group.              |

```borealis
Os.getGroup('sudo');

/* The above code can return something like this:
 * [
 *     'name' = 'sudo',
 *     'password' = 'x',
 *     'id' = 27,
 *     'members' = [ 'alejandro' ],
 * ]
 */
```

### Get environment

`getEnv(string key): any`

Returns the value of an environment variable.

If the environment variable does not exist, `null` is returned.

```borealis
Os.getEnv('LANG');
```

### Get system time

`getTime(): number`

Returns the current system time as a Unix timestamp (in milliseconds).

```borealis
Os.getTime(); # Can return something like 1620149607020
```

### Get modification time

`getModTime(string filename): any`

Returns the time of the last modification to the file as a unix timestamp (in seconds).

If the file does not exist or is not accessible, `null` will be returned.

```borealis
Os.getModTime('file.txt'); # Can return something like 1620226469
```

### Get change time

`getChangeTime(string filename): any`

Returns the time of the last change to the file as a unix timestamp (in seconds).

If the file does not exist or is not accessible, `null` will be returned.

```borealis
Os.getChangeTime('file.txt'); # Can return something like 1620226460
```

### Get access time

`getAccessTime(string filename): any`

Returns the time of the last access to the file as a unix timestamp (in seconds).

If the file does not exist or is not accessible, `null` will be returned.

```borealis
Os.getAccessTime('file.txt'); # Can return something like 1620226400
```

### Execute

`exec(string command)`

Executes the given command.

Returns -1 on error, and the return status of the command otherwise.

```borealis
Os.exec('ls');
```
