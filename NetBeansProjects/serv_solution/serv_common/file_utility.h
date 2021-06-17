#ifndef FILEUTILITY_H_
#define FILEUTILITY_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/times.h>

#include "common_macro.h"
#include "logger.h"
#include "string_utility.h"

    //

#define FILE_SIZE(SIZE, PATH) { \
    struct stat64 statb; \
    if (stat64(PATH, &statb) < 0) \
        _LOG_WARN("stat errno:%d", errno); \
    SIZE = statb.st_size; \
}

#define FILE_SIZE_PATH(SIZE, PATH, SUBDIR) { \
    char path[MAX_PATH * 3]; \
    sprintf(path, "%s/user_pool%s", PATH, SUBDIR); \
    struct stat64 statb; \
    if (stat64(path, &statb) < 0) \
        _LOG_WARN("stat errno:%d", errno); \
    SIZE = statb.st_size; \
}
    
#define FIDI_SIZE_PATH(IS_DIRE, SIZE, PATH, SUBDIR) { \
    char path[MAX_PATH * 3]; \
    sprintf(path, "%s/user_pool%s", PATH, SUBDIR); \
    struct stat64 statb; \
    if (stat64(path, &statb) < 0) \
        _LOG_WARN("stat errno:%d", errno); \
    IS_DIRE = S_ISDIR(statb.st_mode); \
    SIZE = statb.st_size; \
}

#define FILE_SIZE_TIME_PATH(SIZE, LAST_WRITE, PATH, SUBDIR) { \
    char path[MAX_PATH * 3]; \
    sprintf(path, "%s/user_pool%s", PATH, SUBDIR); \
    struct stat64 statb; \
    if (stat64(path, &statb) < 0) \
        _LOG_WARN("stat errno:%d", errno); \
    SIZE = statb.st_size; \
    LAST_WRITE = statb.st_mtime; \
}

    // off_t file_size_descrip(int fildes);
#define FILE_SIZE_DESCRIP(SIZE, FILDES) { \
    struct stat64 statb; \
    if (fstat64(FILDES, &statb) < 0) \
        _LOG_WARN("fstat errno:%d", errno); \
    SIZE = statb.st_size; \
}
    //off_t file_size_stre(FILE *stre);
#define FILE_SIZE_STREAM(SIZE, STREAM) { \
    if (fseeko64(STREAM, 0, SEEK_END)) \
        _LOG_WARN("fseeko64 errno:%d", errno); \
    SIZE = ftello64(STREAM); \
}
    // rewind(STREAM);
#define FILE_SIZE_STREAM_EXT(SIZE, CHKS, STREAM) { \
    if (fseeko64(STREAM, 0, SEEK_END)) \
        _LOG_WARN("fseeko64 errno:%d", errno); \
    SIZE = ftello64(STREAM); \
    chks_text_stre(CHKS, STREAM, SIZE); \
}

    /*Get file_name from file_path*/
    // char *file_get_name(const char *file_path);
#define FILE_GET_NAME(FILE_NAME, FILE_PATH) { \
    char path[PATH_MAX]; \
    strcpy(path, FILE_PATH); \
    strcpy(FILE_NAME, basename(path)); \
}
    /* Get directory suffix from file_path */
    // char *file_get_dir(char *filedir, const char *file_path);
#define FILE_GET_DIRE(FILE_DIRE, FILE_PATH) { \
    char path[PATH_MAX]; \
    strcpy(path, FILE_PATH); \
    strcpy(FILE_DIRE, dirname(path)); \
}

    //
    // int file_time(time_t *last_write, char *name);
#define FILE_TIME(LAST_WRITE, NAME) { \
    struct stat64 statb; \
    if (stat64(NAME, &statb) < 0) \
        _LOG_WARN("stat errno:%d", errno); \
    LAST_WRITE = statb.st_mtime; \
}
    // int file_time_stre(time_t *last_write, FILE *stre);
#define FILE_TIME_STREAM(LAST_WRITE, STREAM) { \
    int fildes; \
    struct stat64 statb; \
    if ((fildes = fileno(STREAM)) == -1) \
        _LOG_WARN("fileno errno:%d", errno); \
    if (fstat64(fildes, &statb) < 0) \
        _LOG_WARN("fstat errno:%d", errno); \
    LAST_WRITE = statb.st_mtime; \
}
    // int file_size_time(size_t *file_size, time_t *last_write, char *name);
#define FILE_SIZE_TIME(FILE_SIZE, LAST_WRITE, NAME) { \
    struct stat64 statb; \
    if (stat64(NAME, &statb) < 0) \
        _LOG_WARN("stat errno:%d", errno); \
    FILE_SIZE = statb.st_size; \
    LAST_WRITE = statb.st_mtime; \
}
    // int file_size_time_stre(size_t *file_size, time_t *last_write, FILE *stre);
#define FILE_SIZE_TIME_STREAM(FILE_SIZE, LAST_WRITE, STREAM) { \
    int fildes; \
    struct stat64 statb; \
    if ((fildes = fileno(STREAM)) == -1) \
        _LOG_WARN("fileno errno:%d", errno); \
    if (fstat64(fildes, &statb) < 0) \
        _LOG_WARN("fstat errno:%d", errno); \
    FILE_SIZE = statb.st_size; \
    LAST_WRITE = statb.st_mtime; \
}
    //
    /* Determine if two time_t values are equivalent*/
    FILE *creat_bakup_file(FILE *local_stre, uint64 offset);
    FILE *creat_bakup_chunk(FILE *local_stre, uint64 offset);
    int creat_action_bakup(char *location, char *file_name, time_t act_time);

    //
#define SET_FILE_MODTIME(FILE_NAME, MODTIME) { \
    struct utimbuf utb; \
    memset(&utb, '\0', sizeof (struct utimbuf)); \
    utb.modtime = MODTIME; \
    if (utime(FILE_NAME, &utb)) \
        _LOG_WARN("utime errno:%d", errno); \
_LOG_DEBUG("path:%s mtim:%s", FILE_NAME, utc_time_text(MODTIME)); \
}

#define SET_FILE_MODTIME_EXT(BASE_NAME, FILE_NAME, MODTIME) { \
    char path[MAX_PATH * 3]; \
    sprintf(path, "%s/user_pool%s", BASE_NAME, FILE_NAME); \
    struct utimbuf utb; \
    memset(&utb, '\0', sizeof (struct utimbuf)); \
    utb.modtime = MODTIME; \
    if (utime(path, &utb)) \
        _LOG_WARN("utime errno:%d", errno); \
_LOG_DEBUG("path:%s mtim:%s", path, utc_time_text(MODTIME)); \
}
    
    //
    // inline int directory_exist(char *dirpath);
#define DIRECTORY_EXISTS(DIRPATH) \
    struct stat64 statb; \
    statb.st_mode = 0; \
    lstat64(DIRPATH, &statb); \
    // S_ISDIR(statb.st_mode);

#define FILE_EXIST(PATH_NAME) access(PATH_NAME, F_OK)
    //inline int is_directory(char *path);
#define IS_DIRECTORY(PATH) \
    struct stat64 statb; \
    if (lstat64(PATH, &statb) < 0) \
        _LOG_WARN("lstat errno:%d", errno); \
    // S_ISDIR(statb.st_mode);

    //
#define CREAT_BAKUP_DIRPATH(BAKUP_PATH, MODE) { \
    char filedir[PATH_MAX]; \
    FILE_GET_DIRE(filedir, BAKUP_PATH) \
    if (mkdir(filedir, MODE)) \
        _LOG_WARN("mkdir errno:%d", errno); \
    _LOG_WARN("mkdir bakup path:%s", filedir); \
}

    //
#define CREAT_FILE_DIRPATH(LOCATION, FILE_NAME, MODE) { \
    char filedir[PATH_MAX]; \
    FILE_GET_DIRE(filedir, FILE_NAME) \
    char *dirpath = next_dirpath(LOCATION, filedir); \
    while (dirpath) { \
        DIRECTORY_EXISTS(dirpath) \
        if (TRUE != S_ISDIR(statb.st_mode)) { \
            if (mkdir(dirpath, MODE)) \
                _LOG_WARN("mkdir errno:%d", errno); \
            _LOG_WARN("mkdir:%s", dirpath); \
        } \
        dirpath = next_dirpath(LOCATION, NULL); \
    } \
}
    /*Attempts to create a directory recursively.*/
    int recu_mkdir(const char *dirpath, mode_t mode);
#define CREAT_FILE_DIRPATH_EXT(LOCATION, FILE_NAME, MODE) { \
    char filedir[MAX_PATH * 3]; \
    char dirpath[PATH_MAX]; \
    FILE_GET_DIRE(filedir, FILE_NAME) \
    POOL_PATH(dirpath, LOCATION, filedir); \
    recu_mkdir(dirpath, MODE); \
}
    
#define CREAT_DIRE_PATH(LOCATION, DIR_NAME, MODE) { \
    char dirpath[PATH_MAX]; \
    POOL_PATH(dirpath, LOCATION, DIR_NAME); \
    recu_mkdir(dirpath, MODE); \
}

    //
    int recu_rmdir(char *dirpath);
    int file_copy(char *from, char *to);
    int file_copy_ext(char *from, char *to);
    int recu_dires_copy(char *dire_from, char *dire_to);

// for debut
    void plawi(char *base_name, char *file_name);
    
//
char *get_appli_path();
#define CREAT_CONF_PATH(PATH_NAME, FILE_NAME) \
    sprintf(PATH_NAME, "%s/conf/%s", get_appli_path(), FILE_NAME);
#define CREAT_LOGS_PATH(PATH_NAME, FILE_NAME) \
    sprintf(PATH_NAME, "%s/logs/%s", get_appli_path(), FILE_NAME);
    
    //
#define GetTickCount time(NULL);

#ifdef	__cplusplus
}
#endif

#endif /* FILEUTILITY_H_ */
