/* 
 * File:   list_cache.h
 * Author: Administrator
 *
 * Created on
 */

#ifndef LIST_CACHE_H
#define	LIST_CACHE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RECURSIVE_FILE_DEFAULT   "~/recursive_file.xml"
#define LIST_DIRECTORY_DEFAULT   "~/list_directory.xml"
#define RECU_DIRECTORY_DEFAULT   "~/recu_directory.xml"
#define ANCHOR_FILE_DEFAULT   "~/anchor_file.xml"

    //
#define CACHE_RECURSIVE_FILE   0x00000001
#define CACHE_LIST_DIREC       0x00000002
#define CACHE_RECU_DIRECTORY   0x00000004
#define CACHE_ANCHOR_FILE      0x00000008

    void clear_list_cache(unsigned int list_cache, char *location);

#ifdef	__cplusplus
}
#endif

#endif	/* LIST_CACHE_H */

