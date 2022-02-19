#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
#if !defined(WIN32) && !defined(_WIN32)
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#endif
#include "../element.h"
#include "../state.h"
#include "../engine/processor_helper.h"
#include "error.h"
#include "os.h"


void stdGetCwd(struct result_list *args)
{
    statePushResultStr(getcwd_());
}


void stdClearPath(struct result_list *args)
{
    char *path = getValueStr(args->first);
    statePushResultStr(strJoinPaths("", path));
    free(path);
}


void stdIsFile(struct result_list *args)
{
    struct stat info;
    char *path = getValueStr(args->first);
    if (stat(path, &info) < 0) {
        statePushResultD(false);
        setLastError(errno);
    } else {
        statePushResultD(S_ISREG(info.st_mode) && !S_ISDIR(info.st_mode));
    }

    free(path);
}


void stdIsDir(struct result_list *args)
{
    struct stat info;
    char *path = getValueStr(args->first);
    if (stat(path, &info) < 0) {
        statePushResultD(false);
        setLastError(errno);
    } else {
        statePushResultD(S_ISDIR(info.st_mode));
    }

    free(path);
}


void stdExists(struct result_list *args)
{
    char *path = getValueStr(args->first);
    struct stat info;

    if (stat(path, &info) < 0) {
        statePushResultD(false);
        setLastError(errno);
    } else {
        statePushResultD(S_ISREG(info.st_mode) || S_ISDIR(info.st_mode));
    }

    free(path);
}


void stdRename(struct result_list *args)
{
    char *ori = getValueStr(args->first);
    char *new = getValueStr(args->first->next);
    int result = rename(ori, new) == 0;

    if (!result) {
        setLastError(errno);
    }

    statePushResultD(result);
    free(ori);
    free(new);
}


void stdUnlink(struct result_list *args)
{
    char *path = getValueStr(args->first);
    int result = remove(path) == 0;

    if (!result) {
        setLastError(errno);
    }

    statePushResultD(result);
    free(path);
}


void stdMakeDir(struct result_list *args)
{
    char *path = getValueStr(args->first);
    int result = mkdir_(path, 0777) == 0;

    if (!result) {
        setLastError(errno);
    }

    statePushResultD(result);
    free(path);
}


void stdGetFiles(struct result_list *args)
{
    struct element_table *arr;
    char *path = getValueStr(args->first);
    DIR *dir = opendir(path);
    struct dirent *ent;

    if (!dir) {
        statePushResultD(false);
        goto end;
    }

    arr = elementTableInit();
    while((ent = readdir(dir))) {
        struct element *el;
        char *key;

        /* Avoid current and previous folders */
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
            continue;
        }

        key = strFromSizet(arr->next_index);
        el = elementInit(key, NULL, 0, T_String);
        el->value.string = strDup(ent->d_name);
        elementTablePush(&arr, el);
        free(key);
    }

    statePushResultArr(arr);
    closedir(dir);
    end: free(path);
}


void stdChmod(struct result_list *args)
{
    char *path = getValueStr(args->first);
    char *mode_str = getValueStr(args->first->next);
    long mode = strtol(mode_str, NULL, 8);
    int result = chmod(path, mode) == 0;

    if (!result) {
        setLastError(errno);
    }

    statePushResultD(result);

    free(path);
    free(mode_str);
}


void stdChown(struct result_list *args)
{
#if defined(WIN32) || defined(_WIN32)
    statePushResultD(false);
#else
    char *path_str = getValueStr(args->first);
    char *user_str = getValueStr(args->first->next);
    struct passwd *user = getpwnam(user_str);
    int result;

    if (user == NULL) {
        statePushResultD(false);
        goto end;
    }

    if (args->first->next->next != NULL) {
        char *group_str = getValueStr(args->first->next->next);
        struct group *group = getgrnam(group_str);
        result = chown(path_str, user->pw_uid, group != NULL ? group->gr_gid : 0);
        free(group_str);
    } else {
        result = chown(path_str, user->pw_uid, 0);
    }

    if (result == -1) {
        setLastError(errno);
    }

    statePushResultD(result != -1);

    end:
        free(path_str);
        free(user_str);
#endif
}


void stdGetUser(struct result_list *args)
{
#if defined(WIN32) || defined(_WIN32)
    statePushResultNull();
#else
    char *user_str = getValueStr(args->first);
    struct passwd *user = getpwnam(user_str);

    if (user == NULL) {
        statePushResultNull();
    } else {
        struct element_table *result = elementTableInit();
        struct element *el = elementInit("name", NULL, 0, T_String);
        el->value.string = strDup(user->pw_name);
        elementTablePush(&result, el);

        el = elementInit("password", NULL, 0, T_String);
        el->value.string = strDup(user->pw_passwd);
        elementTablePush(&result, el);

        el = elementInit("directory", NULL, 0, T_String);
        el->value.string = strDup(user->pw_dir);
        elementTablePush(&result, el);

        el = elementInit("id", NULL, 0, T_Number);
        el->value.number = user->pw_uid;
        elementTablePush(&result, el);

        el = elementInit("group_id", NULL, 0, T_Number);
        el->value.number = user->pw_gid;
        elementTablePush(&result, el);

        el = elementInit("shell", NULL, 0, T_String);
        el->value.string = strDup(user->pw_shell);
        elementTablePush(&result, el);

        statePushResultArr(result);
    }

    free(user_str);
#endif
}


void stdGetGroup(struct result_list *args)
{
#if defined(WIN32) || defined(_WIN32)
    statePushResultNull();
#else
    char *group_str = getValueStr(args->first);
    struct group *group = getgrnam(group_str);

    if (group == NULL) {
        statePushResultNull();
    } else {
        struct element_table *result = elementTableInit();
        struct element *el = elementInit("name", NULL, 0, T_String);
        el->value.string = strDup(group->gr_name);
        elementTablePush(&result, el);

        el = elementInit("password", NULL, 0, T_String);
        el->value.string = strDup(group->gr_passwd);
        elementTablePush(&result, el);

        el = elementInit("id", NULL, 0, T_Number);
        el->value.number = group->gr_gid;
        elementTablePush(&result, el);

        el = elementInit("members", NULL, 0, T_Array);
        elementTablePush(&result, el);
        for (size_t i = 0; group->gr_mem[i] != NULL; i++) {
            char *user_name = strFromSizet(i);
            struct element *user = elementInit(user_name, NULL, 0, T_String);
            user->value.string = strDup(group->gr_mem[i]);

            elementTablePush(&el->value.values, user);
            free(user_name);
        }

        statePushResultArr(result);
    }

    free(group_str);
#endif
}


void stdGetPermissions(struct result_list *args)
{
    char *filename = getValueStr(args->first);
    char *result = malloc_(BUFFER);
    struct stat info;

    if (stat(filename, &info) == 0) {
        statePushResultD(false);
        setLastError(errno);
        goto end;
    }

    stat(filename, &info);
#if defined(_WIN32) || defined(WIN32)
    snprintf(result, BUFFER, "%o", info.st_mode & S_IRWXU);
#else
    snprintf(result, BUFFER, "%o", info.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
#endif

    statePushResultStr(result);
    end: free(filename);
}


void stdIsAbsolute(struct result_list *args)
{
    char *path = getValueStr(args->first);
    statePushResultD(isAbsolutePath(path));
    free(path);
}


void stdGetEnv(struct result_list *args)
{
    char *key = getValueStr(args->first);
    char *env = getenv(key);
    if (env != NULL) {
        statePushResultStr(strDup(env));
    } else {
        statePushResultNull();
    }

    free(key);
}


void stdGetTime(struct result_list *args)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    statePushResultD(floor((double) tv.tv_sec * 1000 + (double) tv.tv_usec / 1000));
}


void stdExec(struct result_list *args)
{
    char *str = getValueStr(args->first);
    statePushResultD(system(str));
    free(str);
}


void stdGetModTime(struct result_list *args)
{
    char *path = getValueStr(args->first);
    struct stat info;

    if (stat(path, &info) != 0) {
        statePushResultNull();
        setLastError(errno);
    } else {
        statePushResultD((double) info.st_mtime);
    }

    free(path);
}


void stdGetChangeTime(struct result_list *args)
{
    char *path = getValueStr(args->first);
    struct stat info;

    if (stat(path, &info) != 0) {
        statePushResultNull();
        setLastError(errno);
    } else {
        statePushResultD((double) info.st_ctime);
    }

    free(path);
}


void stdGetAccessTime(struct result_list *args)
{
    char *path = getValueStr(args->first);
    struct stat info;

    if (stat(path, &info) < 0) {
        statePushResultNull();
        setLastError(errno);
    } else {
        statePushResultD((double) info.st_atime);
    }

    free(path);
}
