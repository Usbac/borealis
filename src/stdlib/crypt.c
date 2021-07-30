#include <stdlib.h>
#include <string.h>
#include "../engine/processor_helper.h"
#include "../state.h"
#include "../../lib/sha1.h"
#include "../../lib/sha256.h"
#include "../../lib/md5.h"
#include "crypt.h"

enum HASH_TYPE {
    H_SHA1, H_SHA256, H_MD5,
};


static char *mapHash(unsigned char *str, enum HASH_TYPE type)
{
    size_t size = 0;
    unsigned char *checksum;
    char *result;

    if (type == H_SHA1) {
        sha1_context ctx;
        size = 20;
        checksum = calloc_(1, size);
        sha1_starts(&ctx);
        sha1_update(&ctx, str, strlen((char *) str));
        sha1_finish(&ctx, checksum);
    } else if (type == H_SHA256) {
        sha256_context ctx;
        size = 32;
        checksum = calloc_(1, size);
        sha256_starts(&ctx);
        sha256_update(&ctx, str, strlen((char *) str));
        sha256_finish(&ctx, checksum);
    } else if (type == H_MD5) {
        md5_context ctx;
        size = 16;
        checksum = calloc_(1, size);
        md5_starts(&ctx);
        md5_update(&ctx, str, strlen((char *) str));
        md5_finish(&ctx, checksum);
    }

    result = calloc_(1, size * 2 + 1);

    for (size_t j = 0; j < size; j++) {
        sprintf(&result[j * 2], "%02x", checksum[j]);
    }

    free(checksum);

    return result;
}


void stdSha1(struct result_list *args)
{
    unsigned char *str = (unsigned char *) getValueStr(args->first);
    pushResultStr(mapHash(str, H_SHA1));
    free(str);
}


void stdSha256(struct result_list *args)
{
    unsigned char *str = (unsigned char *) getValueStr(args->first);
    pushResultStr(mapHash(str, H_SHA256));
    free(str);
}


void stdMd5(struct result_list *args)
{
    unsigned char *str = (unsigned char *) getValueStr(args->first);
    pushResultStr(mapHash(str, H_MD5));
    free(str);
}
