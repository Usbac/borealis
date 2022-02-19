#include <sys/errno.h>
#include "../state.h"
#include "error.h"

/**
 * Error standard module element.
 */
struct element *module = NULL;

/**
 * Element 'last' of the error standard module.
 */
struct element *last = NULL;


static void declareModuleConst(const char *key, int val)
{
    struct element *el = elementInit(key, NULL, 0, T_Number);
    el->constant = true;
    el->value.number = val;

    elementTablePush(&module->value.values, el);
}


static void initErrorConstants(void)
{
    declareModuleConst("ACCESS", EACCES);
    declareModuleConst("FAULT", EFAULT);
    declareModuleConst("IO", EIO);
    declareModuleConst("LOOP", ELOOP);
    declareModuleConst("NAME_TOO_LONG", ENAMETOOLONG);
    declareModuleConst("NOT_ENTRY", ENOENT);
    declareModuleConst("NOT_DIR", ENOTDIR);
    declareModuleConst("IS_DIR", EISDIR);
    declareModuleConst("NOT_EMPTY", ENOTEMPTY);
    declareModuleConst("EXIST", EEXIST);
    declareModuleConst("INVALID", EINVAL);
    declareModuleConst("PERMISSION", EPERM);
    declareModuleConst("READ_ONLY_FS", EROFS);
    declareModuleConst("CROSS_DEVICE", EXDEV);
    declareModuleConst("NO_SPACE", ENOSPC);
    declareModuleConst("QUOTA_EXCEEDED", EDQUOT);
    declareModuleConst("NOT_SUPPORTED", ENOTSUP);
    declareModuleConst("BUSY", EBUSY);
    declareModuleConst("MAX_LINK", EMLINK);
    declareModuleConst("INTERRUPTED", EINTR);
    declareModuleConst("ADDRESS_FAMILY", EAFNOSUPPORT);
    declareModuleConst("M_FILE", EMFILE);
    declareModuleConst("N_FILE", ENFILE);
    declareModuleConst("NO_BUFFER", ENOBUFS);
    declareModuleConst("NO_MEMORY", ENOMEM);
    declareModuleConst("PROTOCOL", EPROTONOSUPPORT);
    declareModuleConst("PROTOCOL_TYPE", EPROTOTYPE);
    declareModuleConst("AGAIN", EAGAIN);
    declareModuleConst("BAD_FILE", EBADF);
    declareModuleConst("CONNECTION_RESET", ECONNRESET);
    declareModuleConst("HOST", EHOSTUNREACH);
    declareModuleConst("INTERRUPTED", EINTR);
    declareModuleConst("MSG_SIZE", EMSGSIZE);
    declareModuleConst("NETWORK_DOWN", ENETDOWN);
    declareModuleConst("NETWORK_UNREACHABLE", ENETUNREACH);
    declareModuleConst("NOT_SOCKET", ENOTSOCK);
    declareModuleConst("NOT_SUPPORTED", EOPNOTSUPP);
    declareModuleConst("PIPE", EPIPE);
    declareModuleConst("ADDRESS_NOT_AVAILABLE", EADDRNOTAVAIL);
    declareModuleConst("NOT_CONNECTION", ENOTCONN);
    declareModuleConst("TIMEOUT", ETIMEDOUT);
    declareModuleConst("ADDRESS_IN_USE", EADDRINUSE);
    declareModuleConst("DESTINATION_ADDRESS", EDESTADDRREQ);
    declareModuleConst("CONNECTION_ABORTED", ECONNABORTED);
}


void initErrorModule(struct element *err)
{
    module = err;
    last = elementInit("last", NULL, 0, T_Null);
    elementTablePush(&module->value.values, last);
    initErrorConstants();
}


void setLastError(int val)
{
    elementFreeValues(&last);
    last->type = T_Number;
    last->value.number = val;
}
