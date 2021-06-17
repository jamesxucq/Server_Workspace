
#ifndef COM_H
#define	COM_H

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
    
    //
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define LINE_LENGTH                   1024

#ifndef uint64
#define uint64 unsigned long long
#endif  

#define NONE			0x00000000
    // list status
#define RECU                    0x00400000 // recursive
#define LIST                    0x00800000 // list

#define TIME_LENGTH             32


#define LIST_CACHE(LPATH, PATH, SUBDIR) \
        sprintf(LPATH, "%s/cache%s", PATH, SUBDIR + 1)

#define RECURSIVE_FILE_DEFAULT   "~/recursive_file.xml"
#define LIST_DIRECTORY_DEFAULT   "~/list_directory.xml"
#define RECU_DIRECTORY_DEFAULT   "~/recu_directory.xml"
#define ANCHOR_FILE_DEFAULT   "~/anchor_file.xml"

#define POOL_PATH(PPATH, PATH, SUBDIR) \
        sprintf(PPATH, "%s/user_pool%s", PATH, SUBDIR)

#define FILE_SIZE_TIME(FILE_SIZE, LAST_WRITE, NAME) { \
    struct stat64 statbuf; \
    if (stat64(NAME, &statbuf) < 0) printf("stat errno:%d", errno); \
    FILE_SIZE = statbuf.st_size; \
    LAST_WRITE = statbuf.st_mtime; \
}
    
#define UTC_TIME_TEXT(TIMESTR, UTCTIME) { \
    strcpy(TIMESTR, "time time time"); \
}
    
#define FULL_NAME_EXT(FNAME, PATH, FILE) \
        sprintf(FNAME, "%s%s", PATH, FILE)
    
#define FILE_EXISTS(PATH_NAME) access(PATH_NAME, F_OK)
    
    
    
    
    
    
    


#ifdef	__cplusplus
}
#endif

#endif	/* COM_H */

