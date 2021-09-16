#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "../engine/processor_helper.h"
#include "../state.h"
#include "socket.h"

#if defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define closesocket(x) close(x)
#endif

#ifndef SOCKET
#define SOCKET int
#endif


/**
 * Used to retrieve an integer property out of a
 * socket object (returned by some functions defined here).
 */
static int getSocketProp(struct element_table *obj, char *key)
{
    struct element *prop = getTrueElement(elementGet(key, obj, NULL));
    if (prop == NULL || prop->type != T_Number) {
        errorF(state->line_n, E_INVALID_SOCKET);
    }

    return (int) prop->value.number;
}


void stdNew(struct result_list *args)
{
    int domain = (int) getValueD(args->first);
    int type = (int) getValueD(args->first->next);
    int protocol = args->first->next->next != NULL ?
        (int) getValueD(args->first->next->next) : 0;
    struct element_table *result = elementTableInit();
    struct element *prop;

#if defined(_WIN32) || defined(WIN32)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
        statePushResultD(false);
        elementsTableFree(result, 0);
        return;
    }
#endif

    prop = elementInit("_SOCK", NULL, 0, T_Number);
    prop->value.number = socket(domain, type, protocol);
    elementTablePush(&result, prop);

    prop = elementInit("_DOMAIN", NULL, 0, T_Number);
    prop->value.number = domain;
    elementTablePush(&result, prop);

    statePushResultObj(result);
}


void stdSend(struct result_list *args)
{
    SOCKET sock = (int) getSocketProp(getValueObj(args->first), "_SOCK");;
    char *msg = getValueStr(args->first->next);
    const int FLAGS = args->first->next->next != NULL ?
        (int) getValueD(args->first->next->next) : 0;

    statePushResultD((double) send(sock, msg, strlen(msg), FLAGS));
    free(msg);
}


void stdRecv(struct result_list *args)
{
    const int FLAGS = args->first->next->next != NULL ?
        (int) getValueD(args->first->next->next) : 0;
    SOCKET sock = (int) getSocketProp(getValueObj(args->first), "_SOCK");
    const int RES_SIZE = (int) getValueD(args->first->next);
    char *res = calloc_(1, RES_SIZE + 1);
    ssize_t bytes = recv(sock, res, RES_SIZE, FLAGS);

    if (bytes <= 0) {
        statePushResultD(false);
        free(res);
    } else {
        statePushResultStr(res);
    }
}


void stdBind(struct result_list *args)
{
    struct element_table *sock = getValueObj(args->first);
    char *host = getValueStr(args->first->next);
    const int SOCK = getSocketProp(sock, "_SOCK");
    struct sockaddr_in addr;

    addr.sin_family = getSocketProp(sock, "_DOMAIN");
    addr.sin_addr.s_addr = inet_addr(host);
    addr.sin_port = (int) getValueD(args->first->next->next);
    memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

    statePushResultD((int) bind(SOCK, (struct sockaddr *) &addr, sizeof(addr)));

    free(host);
}


void stdListen(struct result_list *args)
{
    const int SOCK = getSocketProp(getValueObj(args->first), "_SOCK");
    int result = listen(SOCK, (int) getValueD(args->first->next));

    if (result == -1) {
        statePushResultD(errno);
        return;
    }

    statePushResultD(result);
}


void stdAccept(struct result_list *args)
{
    struct element_table *result = elementTableInit();
    struct element_table *sock = getValueObj(args->first);
    struct element *prop;

    prop = elementInit("_SOCK", NULL, 0, T_Number);
    prop->value.number = accept(getSocketProp(sock, "_SOCK"), NULL, NULL);
    elementTablePush(&result, prop);

    prop = elementInit("_DOMAIN", NULL, 0, T_Number);
    prop->value.number = getSocketProp(sock, "_DOMAIN");
    elementTablePush(&result, prop);

    statePushResultObj(result);
}


void stdClose(struct result_list *args)
{
#if defined(_WIN32) || defined(WIN32)
    WSACleanup();
#endif
    closesocket(getSocketProp(getValueObj(args->first), "_SOCK"));
}


void stdShutdown(struct result_list *args)
{
    int how = args->first->next != NULL ? (int) getValueD(args->first->next) : 2;
    statePushResultD((double) shutdown(getSocketProp(getValueObj(args->first), "_SOCK"), how) == 0);
}


void stdHtons(struct result_list *args)
{
    statePushResultD(htons((int) getValueD(args->first)));
}


void stdHtonl(struct result_list *args)
{
    statePushResultD(htonl((int) getValueD(args->first)));
}


void stdSetTimeout(struct result_list *args)
{
    SOCKET sock = getSocketProp(getValueObj(args->first), "_SOCK");
    struct timeval timeout = {
        .tv_sec = (int) getValueD(args->first->next),
        .tv_usec = 0,
    };

    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}


void stdGetTimeout(struct result_list *args)
{
    SOCKET sock = getSocketProp(getValueObj(args->first), "_SOCK");
    int timeout = 0;
    socklen_t len = sizeof(timeout);

    getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, &len);
    statePushResultD(timeout);
}
