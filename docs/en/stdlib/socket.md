The standard object `Socket` gives you multiple methods related with socket networking.

## Constants

The object also has some useful constants.

| Name         | Type           | Description                                                                                           |
|--------------|----------------|-------------------------------------------------------------------------------------------------------|
| AF_INET      | Socket domain  | IPv4 protocol.                                                                                        |
| AF_INET6     | Socket domain  | IPv6 protocol.                                                                                        |
| AF_UNIX      | Socket domain  | UNIX socket.                                                                                          |
| SOCK_STREAM  | Socket type    | A stream socket provides a sequenced, reliable two-way connection for a byte stream.                  |
| SOCK_DGRAM   | Socket type    | A datagram socket provides connectionless, unreliable messaging.                                      |
| SOCK_RAW     | Socket type    | A raw socket provides low-level access for direct access/implementing network protocols.              |
| SOCK_RDM     | Socket type    | A reliable datagram socket provides reliable delivery of messages.                                    |
| SO_RCVTIMEO  | Socket option  | The timeout value for input operations (in milliseconds).                                             |
| SO_SNDTIMEO  | Socket option  | The timeout value specifying the amount of time that an output function blocks (in milliseconds).     |
| SO_RCVLOWAT  | Socket option  | The minimum number of bytes to process for socket input operations.                                   |
| SO_SNDLOWAT  | Socket option  | The minimum number of bytes to process for socket output operations.                                  |
| SO_DONTROUTE | Socket option  | Reports whether outgoing messages bypass the standard routing facilities (This is boolean).           |
| SO_KEEPALIVE | Socket option  | Reports whether connections are kept active with periodic transmission of messages (This is boolean). |
| SO_BROADCAST | Socket option  | Permit sending of broadcast messages, if this is supported by the protocol (This is boolean).         |

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

Returns `true` on success, `false` otherwise.

```borealis
Socket.listen(sock, 10);
```

```borealis
Socket.listen(socket_in_use, 10); # Returns false
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

Returns `true` on success, `false` otherwise.

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

### Set option

`setOption(object socket, number option, number value): number`

Sets a socket option for the given socket.

Returns `true` on success, `false` otherwise.

The option parameter must be one of the socket option constants defined in the `Socket` object.

```borealis
object server = Socket.new(Socket.AF_INET, Socket.SOCK_STREAM);

# Set the timeout value for input operations to 1200 milliseconds.
Socket.setOption(server, Socket.SO_RCVTIMEO, 1200);
```

```borealis
object server = Socket.new(Socket.AF_INET, Socket.SOCK_STREAM);

# Permit sending of broadcast messages.
Socket.setOption(server, Socket.SO_BROADCAST, true);
```

### Get option

`getOption(object socket, number option): number`

Returns the specified socket option of the given socket.

```borealis
# Will return true if the socket permits sending of broadcast messages.
Socket.getOption(server, Socket.SO_BROADCAST);
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
