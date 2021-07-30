The standard object `Socket` gives you multiple methods related with socket networking.

## Constants

The object also has some useful constants.

| Name         | Type           | Description                                                                              |
|--------------|----------------|------------------------------------------------------------------------------------------|
| AF_INET      | Socket domain  | IPv4 protocol.                                                                           |
| AF_INET6     | Socket domain  | IPv6 protocol.                                                                           |
| AF_UNIX      | Socket domain  | UNIX socket.                                                                             |
| SOCK_STREAM  | Socket type    | A stream socket provides a sequenced, reliable two-way connection for a byte stream.     |
| SOCK_DGRAM   | Socket type    | A datagram socket provides connectionless, unreliable messaging.                         |
| SOCK_RAW     | Socket type    | A raw socket provides low-level access for direct access/implementing network protocols. |
| SOCK_RDM     | Socket type    | A reliable datagram socket provides reliable delivery of messages.                       |
| EADDRINUSE   | Socket error   | Another socket is already listening on the same port.                                    |
| EBADF        | Socket error   | The socket is not a valid file descriptor.                                               |
| ENOTSOCK     | Socket error   | The socket file descriptor does not refer to a real socket.                              |
| EOPNOTSUPP   | Socket error   | The socket is not of a type that supports the listen operation.                          |
| MSG_OOB      |                | TODO |
| MSG_PEEK     |                | TODO |
| MSG_WAITALL  |                | TODO |
| MSG_DONTWAIT |                | TODO |

## Functions

### New

`new(number domain, number type, number protocol = 0): object`

Creates a new socket and returns it.

```borealis
Socket.new(Socket.AF_INET, Socket.SOCK_STREAM);
```

### Bind

`bind(object socket, string address, number port = 0): number`

Binds a name to the given socket.

This function returns 0 on success, otherwise -1 is returned, and
errno is set to indicate the error.

```borealis
Socket.bind(sock, '127.0.0.1', 8080);
```

### Listen

`listen(object socket, number backlog): number`

Listens for a connection on the given socket.

This function returns 0 on success, otherwise one of the following error codes will be returned:

* EADDRINUSE
* EBADF
* ENOTSOCK
* EOPNOTSUPP

```borealis
Socket.listen(sock, 10);
```

```borealis
Socket.listen(socket_in_use, 10); # Returns Socket.EADDRINUSE
```

### Accept

`accept(object socket): object`

Accepts a connection on the given socket.

This function returns a new socket object on success, otherwise `false` will be returned.

```borealis
Socket.accept(sock);
```

### Recv

`recv(object socket, number length, number flags = 0): any`

Receives data from a connected socket, the length of the data to receive must be specified.

This function returns the data received on success, otherwise `false` will be returned.

```borealis
any data = Socket.recv(sock, 1024);
```

### Send

`send(object socket, string data, number flags = 0): any`

Sends data to a connected socket.

Returns the number of bytes sended or `-1` in case of errors.

```borealis
Socket.send(sock, "HTTP/1.1 200 OK\r\n\r\n <h1>Hello world </h1>");
```

### Close

`close(object socket)`

Closes the given socket.

```borealis
Socket.close(sock);
```

### Shutdown

`shutdown(object socket, number how = 2)`

Shuts down a socket for receiving, sending, or both.

The value of the parameter `how` can be one of the following:

| Value | Description                          |
|-------|--------------------------------------|
| `0`   | Shutdown socket reading.             |
| `1`   | Shutdown socket writing.             |
| `2`   | Shutdown socket reading and writing. |

The function returns `true` on success, `false` otherwise.

```borealis
Socket.shutdown(sock);
```

```borealis
Socket.shutdown(sock, 0); # Shutting down only for reading
```

### Htons

`htons(number n): number`

Converts the given number (as unsigned short integer) from a host byte order to a network byte order.

```borealis
Socket.htons(8080); # Returns 36895
```

### Htonl

`htonl(number n): number`

Converts the given number (as unsigned long integer) from a host byte order to a network byte order.

```borealis
Socket.htonl(8080); # Returns 2417950720
```

### Set timeout

`setTimeout(object sock, number time)`

Sets the socket timeout in seconds of inactivity.

```borealis
Socket.setTimeout(sock, 10); # Timeout after 10 second of inactivity
```

### Get timeout

`getTimeout(object sock): number`

Returns the socket timeout.

```borealis
Socket.getTimeout(sock); # Can return something like 10
```

## Example

This example creates a simple web server that prints in the standard output the received request and returns a valid HTTP response with the "hello world" message.

```borealis
object server = Socket.new(Socket.AF_INET, Socket.SOCK_STREAM);
Socket.bind(server, '127.0.0.1', Socket.htons(3000));
Socket.listen(server, 10);

object client = Socket.accept(server);
printLine(Socket.recv(client, 1024));
Socket.send(client, "HTTP/1.1 200 OK\r\n\r\n Hello world");

Socket.close(client);
Socket.close(server);
```

If you run this code and open `127.0.0.1:3000` in your browser you should see the "Hello world" text.
