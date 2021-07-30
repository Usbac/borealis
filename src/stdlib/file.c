#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "../element.h"
#include "../state.h"
#include "../engine/processor_helper.h"
#include "../../lib/base64.h"
#include "file.h"


void stdCreateFile(struct result_list *args)
{
    char *path = getValueStr(args->first);
    FILE *fp = fopen(path, "r");
    int result;

    if (fp != NULL) {
        result = -1;
    } else {
        fp = fopen(path, "w");
        result = fp != NULL;
    }

    if (fclose(fp) != 0) {
        result = 0;
    }

    pushResultD(result);
    free(path);
}


void stdReadFile(struct result_list *args)
{
    char *path = getValueStr(args->first);
    char *encoding = args->first->next != NULL ?
        getValueStr(args->first->next) :
        strDup("utf8");
    FILE *fp = fopen(path, !strcmp(encoding, "utf8") ? "r" : "rb");
    char *content = NULL;
    long len;

    if (!fp) {
        pushResultNull();
        goto end;
    }

    len = getFileSize(fp);

    if (!strcmp(encoding, "hex")) {
        int c;
        size_t i = 0;
        content = calloc_(1, (len * 2) + 1);

        while ((c = fgetc(fp)) != EOF) {
            snprintf(&content[i++ * 2], 3, "%02X", c);
        }
    } else if (!strcmp(encoding, "base64")) {
        char *aux = calloc_(1, len + 1);
        size_t base64_len = base64_encoded_length(len);
        content = malloc_(base64_len);
        fread(aux, len, 1, fp);

        base64_encode(content, base64_len, aux, len);
        free(aux);
    } else {
        content = calloc_(1, len + 1);
        fread(content, len, 1, fp);
    }

    fclose(fp);

    if (content != NULL) {
        pushResultStr(content);
    } else {
        pushResultNull();
    }

    end:
        free(path);
        free(encoding);
}


void stdWriteFile(struct result_list *args)
{
    char *path = getValueStr(args->first);
    char *content = getValueStr(args->first->next);
    FILE *fp = fopen(path, "r");
    bool override = args->first->next->next != NULL ?
        isTrue(args->first->next->next) :
        true;
    bool result = fp == NULL || override;

    if (result) {
        fp = fopen(path, "w");
        result = fprintf(fp, "%s", content) >= 0;
    }

    if (fclose(fp) != 0) {
        result = false;
    }

    pushResultD(result);
    free(path);
    free(content);
}


void stdAppendFile(struct result_list *args)
{
    char *path = getValueStr(args->first);
    char *content = getValueStr(args->first->next);
    FILE *fp = fopen(path, "a");
    bool result = fprintf(fp, "%s", content) >= 0;

    if (fclose(fp) != 0) {
        result = false;
    }

    pushResultD(result);
    free(path);
    free(content);
}


void stdCopy(struct result_list *args)
{
    char *ori = getValueStr(args->first);
    char *dest = getValueStr(args->first->next);
    FILE *fp_ori = fopen(ori, "rb");
    FILE *fp_dest = fopen(dest, "wb");
    unsigned char buff[8192];
    size_t n, m;
    bool result = true;

    if (fp_ori == NULL || fp_dest == NULL) {
        result = false;
        goto end;
    }

    do {
        n = fread(buff, 1, sizeof(buff), fp_ori);
        m = n ? fwrite(buff, 1, n, fp_dest) : 0;
    } while (n > 0 && n == m);

    if (fclose(fp_ori) != 0 || fclose(fp_dest) != 0) {
        result = false;
    }

    end:
        pushResultD(result);
        free(ori);
        free(dest);
}


void stdGetModTime(struct result_list *args)
{
    char *path = getValueStr(args->first);
    struct stat info;

    if (stat(path, &info) < 0) {
        pushResultNull();
    } else {
        pushResultD((double) info.st_mtime);
    }

    free(path);
}


void stdGetType(struct result_list *args)
{
    char *path = getValueStr(args->first);
    char *result;

    struct stat info;
    if (stat(path, &info) < 0) {
        pushResultNull();
        goto end;
    }

    if (S_ISDIR(info.st_mode)) {
        result = "directory";
    } else if (S_ISREG(info.st_mode)) {
        result = "file";
    } else if (S_ISCHR(info.st_mode)) {
        result = "char";
    } else if (S_ISLNK(info.st_mode)) {
        result = "link";
    } else if (S_ISFIFO(info.st_mode)) {
        result = "fifo";
    } else if (S_ISBLK(info.st_mode)) {
        result = "block";
    } else {
        result = "unknown";
    }

    pushResultStr(strDup(result));

    end: free(path);
}


void stdGetFileSize(struct result_list *args)
{
    char *path = getValueStr(args->first);
    FILE *fp = fopen(path, "rb");
    long size;

    if (!fp || (size = getFileSize(fp)) == 0) {
        pushResultNull();
    } else {
        pushResultD((double) size);
    }

    fclose(fp);
    free(path);
}
