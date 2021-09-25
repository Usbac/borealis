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
    el->public = true;
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
    declareModuleConst("FAULT", EFAULT);
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
}


void initErrorModule(struct element *err)
{
    module = err;
    last = elementInit("last", NULL, 0, T_Null);
    last->public = true;
    elementTablePush(&module->value.values, last);
    initErrorConstants();
}


void setLastError(int val)
{
    elementFreeValues(&last);
    last->type = T_Number;
    last->value.number = val;
}
