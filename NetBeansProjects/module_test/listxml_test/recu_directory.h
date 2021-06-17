/* 
 * File:   recu_directory.h
 * Author: David
 *
 * Created on 2010
 */

#ifndef RECU_DIRECTORY_H
#define	RECU_DIRECTORY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "recu_utility.h"
    //
    int recursive_file(char *location, char *subdir);
#define clear_recursive_file(location) clear_recuf_entry(location)
    //
    int list_directory(char *location, char *subdir);
#define clear_list_directory(location) clear_listd_entry(location)
    //
    int recu_directory(char *location, char *subdir);
#define clear_recu_directory(location) clear_recud_entry(location)
    //
#ifdef	__cplusplus
}
#endif

#endif	/* RECU_DIRECTORY_H */

