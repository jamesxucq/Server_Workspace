/* 
 * File:   utility.h
 * Author: Administrator
 *
 * Created on 2015年12月19日, 上午9:22
 */

#ifndef UTILITY_H
#define	UTILITY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
    //
int valid_file_path(const char *path);
int recu_mkdir(char *location, char *user_name);
#define FILE_EXISTS(PATH_NAME) access(PATH_NAME, F_OK)

#define POOL_PATH(PPATH, LOCATION, USER_NAME, SUBDIR) \
        sprintf(PPATH, "%s/%s/%s", LOCATION, USER_NAME, SUBDIR)
#define USER_LOCATION(LPATH, LOCATION, USER_NAME) \
        sprintf(LPATH, "%s/%s", LOCATION, USER_NAME)

#ifdef	__cplusplus
}
#endif

#endif	/* UTILITY_H */

