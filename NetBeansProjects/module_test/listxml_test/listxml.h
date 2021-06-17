/* 
 * File:   listxml.h
 * Author: Administrator
 *
 * Created on
 */

#ifndef LISTXML_H
#define	LISTXML_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "common_header.h"
// #include "server_macro.h"

    // 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 
    int build_recursive_file(FILE **recu_xml, char *location, char *subdir);
    // 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 
    int build_list_directory(FILE **list_xml, char *location, char *subdir);
    // 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 
    int build_recu_directory(FILE **recu_xml, char *location, char *subdir);

#define CLEAR_RECURSIVE_FILE(FILE_NAME, LOCATION) {\
    LIST_CACHE(FILE_NAME, LOCATION, RECURSIVE_FILE_DEFAULT); \
_LOG_DEBUG("delete file is:%s", FILE_NAME); \
    unlink(FILE_NAME); \
}

#define CLEAR_LIST_DIREC(FILE_NAME, LOCATION) {\
    LIST_CACHE(FILE_NAME, LOCATION, LIST_DIRECTORY_DEFAULT); \
_LOG_DEBUG("delete file is:%s", FILE_NAME); \
    unlink(FILE_NAME); \
}

#define CLEAR_RECU_DIRECTORY(FILE_NAME, LOCATION) {\
    LIST_CACHE(FILE_NAME, LOCATION, RECU_DIRECTORY_DEFAULT); \
_LOG_DEBUG("delete file is:%s", FILE_NAME); \
    unlink(FILE_NAME); \
}

#define LISTX_CLOSE(fptr) if(fptr) fclose(fptr);

#ifdef	__cplusplus
}
#endif

#endif	/* LISTXML_H */

