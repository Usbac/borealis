The standard object `Crypt` gives you multiple methods related with cryptographic.

## Functions

### SHA1

`sha1(string str): string`

Returns the sha1 hash of the given string.

```borealis
Crypt.sha1('hello'); # Returns 'aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d'
```

### SHA256

`sha256(string str): string`

Returns the sha256 hash of the given string.

```borealis
Crypt.sha256('hello'); # Returns '2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824'
```

### MD5

`md5(string str): string`

Returns the md5 hash of the given string.

```borealis
Crypt.md5('hello'); # Returns '5d41402abc4b2a76b9719d911017c592'
```
