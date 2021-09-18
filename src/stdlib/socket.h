#ifndef BOREALIS_SOCKET_H
#define BOREALIS_SOCKET_H

#include "../error.h"
#define E_INVALID_SOCKET RUNTIME_ERROR("Invalid socket object given")

/**
 * Returns a new socket.
 */
void stdNew(struct result_list *args);

/**
 * Sends data to the given socket.
 */
void stdSend(struct result_list *args);

/**
 * Receives data from the given socket.
 */
void stdRecv(struct result_list *args);

/**
 * Binds a name to a socket.
 */
void stdBind(struct result_list *args);

/**
 * Listens for a connection on the given socket.
 */
void stdListen(struct result_list *args);

/**
 * Accepts a connection on the given socket.
 */
void stdAccept(struct result_list *args);

/**
 * Closes the given socket.
 */
void stdClose(struct result_list *args);

/**
 * Shutdowns the given socket.
 */
void stdShutdown(struct result_list *args);

/**
 * Translates a short integer from host byte order to network byte order.
 */
void stdHtons(struct result_list *args);

/**
 * Translates a long integer from host byte order to network byte order.
 */
void stdHtonl(struct result_list *args);

/**
 * Sets a socket option.
 */
void stdSetOption(struct result_list *args);

/**
 * Returns a socket option.
 */
void stdGetOption(struct result_list *args);

#endif /* BOREALIS_SOCKET_H */
