#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>
#include <math.h>
#include "../element.h"
#include "../state.h"
#include "../engine/processor_helper.h"
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
    char *path = getValueStr(args->first);
    statePushResultD(isFilePath(path));
    free(path);
}


void stdIsDir(struct result_list *args)
{
    struct stat info;
    char *path = getValueStr(args->first);
    statePushResultD(stat(path, &info) < 0 ? false : S_ISDIR(info.st_mode));
    free(path);
}


void stdExists(struct result_list *args)
{
    char *path = getValueStr(args->first);
    struct stat info;
    bool result = stat(path, &info) < 0 ?
        false :
        (S_ISREG(info.st_mode) || S_ISDIR(info.st_mode));

    statePushResultD(result);
    free(path);
}


void stdRename(struct result_list *args)
{
    char *ori = getValueStr(args->first);
    char *new = getValueStr(args->first->next);
    statePushResultD(rename(ori, new) == 0);
    free(ori);
    free(new);
}


void stdUnlink(struct result_list *args)
{
    char *path = getValueStr(args->first);
    statePushResultD(remove(path) == 0);
    free(path);
}


void stdMakeDir(struct result_list *args)
{
    char *path = getValueStr(args->first);
    statePushResultD(mkdir_(path, 0777) == 0);
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
        el->public = true;
        el->value.string = strDup(ent->d_name);
        elementTablePush(&arr, el);
        free(key);
    }

    StatePushResultArr(arr);
    closedir(dir);
    end: free(path);
}


void stdChmod(struct result_list *args)
{
    char *path = getValueStr(args->first);
    char *mode_str = getValueStr(args->first->next);
    long mode = strtol(mode_str, NULL, 8);
    statePushResultD(chmod(path, mode) == 0);

    free(path);
    free(mode_str);
}


void stdGetPermissions(struct result_list *args)
{
    char *filename = getValueStr(args->first);
    char *result = malloc_(BUFFER);
    struct stat info;

    if (stat(filename, &info) < 0) {
        statePushResultD(false);
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
