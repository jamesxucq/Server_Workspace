/* utility.h
 * Author: Administrator
 * Created on 2012-9-12
 */

#ifndef UTILITY_H
#define	UTILITY_H

#ifdef	__cplusplus
extern "C" {
#endif 
    
#include "macro.h"
#include <time.h>

    inline char *append_path(char *path, char *subdir);
    char *trans_auth_csum(char *csum, unsigned int uid, char *access_key);
    int valid_file_path(const char *path);
#define FILE_EXISTS(PATH_NAME) access(PATH_NAME, F_OK)
    char *strsplit(char *str, const char *delim);
    char *split_value(char *str);
    char *split_line(char *str);
    int clear_backup_file(FILE *backup_point, FILE *origin_point, size_t offset);
    int recu_dirs_size(uint64 *pool_size, char *location, char *subdir);
    int list_dirs_remove(char *dirpath);
    inline char *last_backup(char *path, char *subdir, time_t bak_time);
    char *strnrchr(char *str, char c, int num);

// inline char *utc_time_text(time_t utctime);
// Wed, 15 Nov 1995 04:58:08 GMT
#define UTC_TIME_TEXT(TIMESTR, UTCTIME) { \
    struct tm *utctm; \
    if (!UTCTIME) UTCTIME = time(NULL); \
    utctm = gmtime(&UTCTIME); \
    if (!strftime(TIMESTR, TIME_LENGTH, "%a, %d %b %Y %H:%M:%S GMT", utctm)) \
        LOG_INFO("strftime error:%d", UTCTIME); \
}

#define TIME_LENGTH             32
    
#define MD5_DIGEST_LEN 16
#define MD5_TEXT_LENGTH	33    

#ifdef	__cplusplus
}
#endif

#endif	/* UTILITY_H */

