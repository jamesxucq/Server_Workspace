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
int recu_rmdir(char *dirpath);
#define FILE_EXISTS(PATH_NAME) access(PATH_NAME, F_OK)


#ifdef	__cplusplus
}
#endif

#endif	/* UTILITY_H */

