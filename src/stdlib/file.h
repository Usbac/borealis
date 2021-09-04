#ifndef FILE_H
#define FILE_H

/**
 * Creates a new file with the given path.
 */
void stdCreateFile(struct result_list *args);

/**
 * Returns the content of a file.
 */
void stdReadFile(struct result_list *args);

/**
 * Writes content to a file.
 */
void stdWriteFile(struct result_list *args);

/**
 * Appends content to a file.
 */
void stdAppendFile(struct result_list *args);

/**
 * Copies a file.
 */
void stdCopy(struct result_list *args);

/**
 * Returns the file type.
 * file: regular file
 * dir: directory
 * char: character special
 * link: symbolic link
 * fifo: FIFO
 * block: block special
 * unknown: unknown type
 */
void stdGetType(struct result_list *args);

/**
 * Returns the file size or null if it cannot be accessed.
 */
void stdGetFileSize(struct result_list *args);

#endif /* FILE_H */
