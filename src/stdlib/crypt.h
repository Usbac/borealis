#ifndef CRYPT_H
#define CRYPT_H

/**
 * Returns the SHA1 hash of the given string.
 */
void stdSha1(struct result_list *args);

/**
 * Returns the SHA256 hash of the given string.
 */
void stdSha256(struct result_list *args);

/**
 * Returns the MD5 hash of the given string.
 */
void stdMd5(struct result_list *args);

#endif /* CRYPT_H */
