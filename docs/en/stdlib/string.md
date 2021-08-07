The standard object `String` gives you multiple methods related with strings.

## Functions

### To uppercase

`toUpper(string str): string`

Returns the given string with all alphabetic characters converted to uppercase.

```borealis
String.toUpper('I love it'); # Returns 'I LOVE IT'
```

```borealis
String.toUpper('Hello señor'); # Returns 'HELLO SEÑOR'
```

### To lowercase

`toLower(string str): string`

Returns the given string with all alphabetic characters converted to lowercase.

```borealis
String.toLower('Hello WORLD'); # Returns 'hello world'
```

```borealis
String.toLower('HEY!'); # Returns 'hey!'
```

### Get length

`getLength(string str): number`

Returns the length of the string.

```borealis
String.getLength('abcd'); # Returns 4
```

```borealis
String.getLength('汉字'); # Returns 2
```

Keep in mind that the string length is not always equal to the number of bytes in the string, since strings in Borealis are UTF-8 encoded.

### Get byte length

`getByteLength(string str): number`

Returns the byte length of the string.

```borealis
String.getByteLength('hello'); # Returns 5
```

```borealis
String.getByteLength('汉字'); # Returns 6
```

### Includes

`includes(string haystack, string needle): number`

Returns `true` if the string contains the given substring, `false` otherwise.

```borealis
String.includes('Lorem ipsum', 'ipsum'); # returns true
```

```borealis
String.includes('Lorem ipsum dolor sit amet', 'o'); # returns true
```

### Trim

`trim(string str, string substr = ' '): string`

Strips a subtring from the beginning and end of a string.

```borealis
String.trim(' hey there '); # Returns 'hey there'
```

```borealis
String.trim('000abc000', '0'); # Returns 'abc'
```

### Left trim

`leftTrim(string str, string substr = ' '): string`

Strips a subtring from the beginning of a string.

```borealis
String.leftTrim(' hey there '); # Returns 'hey there '
```

```borealis
String.leftTrim('000abc000', '0'); # Returns 'abc000'
```

### Right trim

`rightTrim(string str, string substr = ' '): string`

Strips a substring from the end of a string.

```borealis
String.rightTrim(' hey there '); # Returns ' hey there'
```

```borealis
String.rightTrim('000abc000', '0'); # Returns '000abc'
```

### Index of

`indexOf(string haystack, string needle): number`

Returns the position of the first occurrence of a substring in a string.

If the needle is not found, it will return -1.

The string position starts at 0, not 1.

```borealis
String.indexOf('hello world', 'world'); # Returns 6
```

```borealis
String.indexOf('hello world', 'h'); # Returns 0
```

### Last index of

`lastIndexOf(string haystack, string needle): number`

Returns the position of the last occurrence of a substring in a string.

If the needle is not found, it will return -1.

The string position starts at 0, not 1.

```borealis
String.lastIndexOf('hello world', 'o'); # Returns 7
```

### Split

`split(string str, string separator): array`

Returns an array of strings, each of which is a substring of string formed by splitting it on boundaries formed by the separator.

```borealis
String.split('i,love,you'); # Returns ['i', 'love', 'you']
```

```borealis
String.split('hello world how are you', ' ');
# Returns ['hello', 'world', 'how', 'are', 'you']
```

### Starts with

`startsWith(string haystack, string needle): number`

Returns `true` if the string starts with the substring, `false` otherwise.

```borealis
String.startsWith('I am from the US', 'I am'); # Returns true
```

```borealis
any str = 'The lazy fox jumped over the fence';

if (String.startsWith(str, 'The')) {
    print('It ends with The');
}
```

### Ends with

`endsWith(string haystack, string needle): number`

Returns `true` if the string ends with the substring, `false` otherwise.

```borealis
String.endsWith('I am from the US', 'US'); # Returns true
```

```borealis
any str = 'The lazy fox jumped over the fence';

if (String.endsWith(str, 'fence')) {
    print('It ends with fence');
}
```

### Get char

`getChar(string str, number index): string`

Returns the character at the specified position in the string.

The string position starts at 0, not 1.

```borealis
String.getChar('abcdef', 3); # Returns 'd'
```

### Slice

`slice(string str, number start, number end = -1): string`

Returns a portion of the string from the specified start to the specified end.

The end parameter will be ignored if it's equal to -1.

```borealis
String.slice("1234567890", 4); # Returns '567890'
```

```borealis
String.slice("1234567890", 4, 7); # Returns '567'
```

### Repeat

`repeat(string str, number times): string`

Returns the string repeated the given number of times.

If the times parameter is equal or less than zero, an empty string will be returned.

```borealis
String.repeat('hey', 3); # Returns 'heyheyhey'
```

### Is whitespace

`isWhitespace(string str): number`

Returns `true` if the string only contains whitespace characters, `false` otherwise.

Whitespace characters are described in this table:

| Character  | Description     |
|------------|-----------------|
| ' '        | Space           |
| '\n'       | New line        |
| '\t'       | Horizontal tab  |
| '\r'       | Carriage return |
| '\0'       | Null            |

```borealis
String.isWhitespace(' '); # Returns true
```

```borealis
String.isWhitespace("\n\r\t"); # Returns true
```

### Replace

`replace(string str, string search, string replace): string`

Returns the string with all the occurrences of the search string replaced with the replace string.

```borealis
String.replace('hello world', 'o', 'A'); # Returns 'hellA wArld'
```

```borealis
String.replace('hello there and hello again', 'hello', 'hey');
# Returns 'hey there and hey again'
```

### Remove

`remove(string str, string search): string`

Returns the string with all the occurrences of the search string removed.

```borealis
String.remove('hello world', 'o'); # Returns 'hell wrld'
```

```borealis
String.remove('hello there and hello again', 'hello');
# Returns ' there and  again'
```

### Compare

`compare(string a, string b): number`

Returns `true` if both strings are equal in a binary level, `false` otherwise.

```borealis
String.compare('Lorem ipsum', 'Lorem ipsum'); # Returns true
```

```borealis
String.compare('Lorem ipsum', 'lorem ipsum'); # Returns false
```

### Compare case insensitive

`compareI(string a, string b): number`

Returns `true` if both strings are equal in a case-insensitive way, `false` otherwise.

```borealis
String.compareI('Lorem', 'Lorem'); # Returns true
```

```borealis
String.compareI('LOREm', 'lorem'); # Returns true
```

### Contains chars

`containsChars(string str, string chars): number`

Returns `true` if the string contains all the characters in the substring, `false` otherwise.

```borealis
String.containsChars('longyearbyen', 'aeyo'); # Returns true
```

```borealis
String.containsChars('longyearbyen', 'aeui'); # Returns false
```

### Interpolate

`interpolate(string str, array values): string`

Returns the string with all the values in the array interpolated.

The keys must have the `{key}` format.

```borealis
String.interpolate('{name} is {age} yo', [ 'name' = 'alex', 'age' = 23 ]);
# Returns 'alex is 23 yo'
```

### Sprintf

`sprintf(string str, ...values): string`

Returns a formatted string.

The valid formats are:

* `%s` - String.
* `%d` - Floating point number.
* `%i` - Integer.
* `%e` - Scientific notation.

```borealis
String.sprintf('There are %i unicorns in the %s', 5, 'country');
# Returns 'There are 5 unicorns in the country'
```

If the number of arguments is less than the number of formats in the string, the returned string will be cut off just before mapping the last format that was not found.

```borealis
String.sprintf('There are %i unicorns in the %s', 5);
# Returns 'There are 5 unicorns in the '
```

### Pad start

`padStart(string str, number pad, string pad_str = ' '): string`

Returns the string with a starting pad with a certain length.

```borealis
String.padStart('hello', 10, ' '); # Returns '     hello'
```

```borealis
String.padStart('hello', 10, '+='); # Returns '+=+=+hello'
```

### Pad end

`padEnd(string str, number pad, string pad_str = ' '): string`

Returns the string with a ending pad with a certain length.

```borealis
String.padEnd('hello', 10, ' '); # Returns 'hello     '
```

```borealis
String.padEnd('hello', 10, '+='); # Returns 'hello+=+=+'
```

### Reverse

`reverse(string str): string`

Returns the string reversed.

```borealis
String.reverse('abcd'); # Returns 'dcba'
```

### Encode

`encode(string str, string encoding): string`

Returns the string with the specified encoding.

Valid encodings are 'base64' and 'hex'.

```borealis
String.encode('Hello', 'base64'); # Returns 'SGVsbG8A'
```

### Decode

`decode(string str, string encoding): string`

Returns the string decoded with the specified encoding.

Valid encodings are 'base64' and 'hex'.

```borealis
String.decode('SGVsbG8A', 'base64'); # Returns 'Hello'
```

### Count

`count(string haystack, string needle): string`

Returns the number of times a substring appears in a string.

```borealis
String.count('hello world', 'o'); # Returns 2
```

```borealis
String.count('hey hey hey', 'hey'); # Returns 3
```

### Is uppercase

`isUpper(string str): number`

Returns `true` if all the letters in the string are uppercase letters, `false` otherwise.

```borealis
String.isUpper('YOU AND ME'); # Returns true
```

```borealis
String.isUpper('YOU AND mE'); # Returns false
```

### Is lowercase

`isLower(string str): number`

Returns `true` if all the letters in the string are lowercase letters, `false` otherwise.

```borealis
String.isLower('you and me'); # Returns true
```

```borealis
String.isLower('you and mE'); # Returns false
```

### Capitalize

`capitalize(string str): string`

Returns the string with the first character in uppercase.

```borealis
String.capitalize('welcome'); # Returns 'Welcome'
```

This method, does NOT ignore whitespaces.

```borealis
String.capitalize(' welcome'); # Returns ' welcome'
```

### From char codes

`fromCharCodes(...values): string`

Returns a string created from the specified sequence of UTF-8 code units.

The arguments are treated as numbers.

```borealis
String.fromCharCodes(65, 66, 67); # Returns 'ABC'
```

```borealis
String.fromCharCodes(2023); # Returns 'ߧ'
```
