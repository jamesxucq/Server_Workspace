#ifndef FILEUTILITY_H_
#define FILEUTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "common_macro.h"

////////////////////////////////////////////////////////////////////////////////
/**
 * Load file into memory.
 * This method actually allocates memory more than 1 bytes than file_size then append
 * NULL character at the end. For example, when the file size is 10 bytes long, 10+1
 * bytes will allocated and the last byte is always NULL character. So you can load
 * text file and use without appending NULL character. By the way, *size still will
 * be returned the actual file size of 10.
 */
void *load_file(const char *file_path, size_t *bytes);
/*Get file size.*/
off_t file_size(const char *file_path);
off_t file_size_descrip(int fildes);
off_t file_size_point(FILE *filpint);
/*Check path string contains invalid characters.*/
int valid_file_path(const char *path);
/*Read one line from file.
 *Basically it's same as fgets() but can read line without length limitation.*/
char *file_read_line(FILE *filpint);
/*Get file_name from file_path*/
char *file_get_name(const char *file_path);
/* Get directory suffix from file_path */
char *file_get_dir(char *filedir, const char *file_path);

////////////////////////////////////////////////////////////////////////////////
/*Attempts to create a directory recursively.*/
int file_make_dir(const char *dirpath, mode_t mode, unsigned int recursive);
int creat_file_dirpath(char *location, char *file_name, mode_t mode);
int creat_file_dirpath_ext(char *location, char *file_name, mode_t mode);

////////////////////////////////////////////////////////////////////////////////
int file_time(time_t *last_write, char *name);
int file_time_point(time_t *last_write, FILE *filpint);
int file_size_time(size_t *file_size, time_t *last_write, char *name);
int file_size_time_point(size_t *file_size, time_t *last_write, FILE *filpint);

////////////////////////////////////////////////////////////////////////////////
/* Determine if two time_t values are equivalent*/
//inline int cmp_time(time_t timea, time_t timeb);
FILE *creat_backup_file(FILE *local_point, size_t offset);
FILE *creat_backup_chunk(FILE *local_point, size_t offset);
int creat_action_backup(char *location, char *file_name, time_t act_time);

////////////////////////////////////////////////////////////////////////////////
//int ftruncate_pointer(FILE *filpint, size_t length);
#define ftruncate_name(PATH, LENGTH)  truncate(PATH, LENGTH)
int set_file_modtime(char *name, time_t modtime);
#define set_file_modtime_ext(basename, file_name, modtime) \
    set_file_modtime(full_path(basename, file_name), modtime)

////////////////////////////////////////////////////////////////////////////////
inline int directory_exists(char *dirpath);
#define file_exists(PATH_NAME) access(PATH_NAME, F_OK)
inline int is_directory(char *path);

////////////////////////////////////////////////////////////////////////////////
int recu_dirs_remove(char *dirpath);
int file_copy(char *file_from, char *file_to);
int recu_dirs_copy(char *dirpath_from, char *dirpath_to);

////////////////////////////////////////////////////////////////////////////////
#define GetTickCount time(NULL);

#endif /* FILEUTILITY_H_ */
