#ifndef OS_H
#define OS_H

/**
 * Returns the current working directory.
 */
void stdGetCwd(struct result_list *args);

/**
 * Returns the given path cleared.
 */
void stdClearPath(struct result_list *args);

/**
 * Returns true if the given string is the path of an existing file.
 */
void stdIsFile(struct result_list *args);

/**
 * Returns true if the given string is the path of an existing directory.
 */
void stdIsDir(struct result_list *args);

/**
 * Returns true if the given string is an existing path, false otherwise.
 */
void stdExists(struct result_list *args);

/**
 * Renames the file/directory with the given path.
 */
void stdRename(struct result_list *args);

/**
 * Deletes the file/directory with the given path.
 */
void stdUnlink(struct result_list *args);

/**
 * Makes a directory with the given path.
 */
void stdMakeDir(struct result_list *args);

/**
 * Returns an array with all the files in the given directory.
 */
void stdGetFiles(struct result_list *args);

/**
 * Changes the permission of the given path.
 */
void stdChmod(struct result_list *args);

/**
 * Returns the given file permissions.
 */
void stdGetPermissions(struct result_list *args);

/**
 * Returns true if the given path is absolute, false otherwise.
 */
void stdIsAbsolute(struct result_list *args);

/**
 * Returns the value of the specified environment variable.
 */
void stdGetEnv(struct result_list *args);

/**
 * Returns the current system time.
 */
void stdGetTime(struct result_list *args);

/**
 * Executes the given command in the host environment.
 */
void stdExec(struct result_list *args);

/**
 * Returns the modification time of a file in seconds.
 */
void stdGetModTime(struct result_list *args);

/**
 * Returns the change time of a file in seconds.
 */
void stdGetChangeTime(struct result_list *args);

/**
 * Returns the access time of a file in seconds.
 */
void stdGetAccessTime(struct result_list *args);

#endif /* OS_H */
