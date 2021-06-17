/* 
 * File:   recu_utility.h
 * Author: Administrator
 */

#ifndef RECU_UTILITY_H
#define	RECU_UTILITY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_macro.h"
    
    struct list_attrib {
        unsigned int action_type; /* add mod del list recu */
        char file_name[MAX_PATH * 3];
        uint64 file_size;
        time_t last_write; /* When the item was last modified */
        unsigned int reserved; /* reserved */
    };

#define RECURSIVE_FILE_PATH   "~/recursive_file.path"
#define RECURSIVE_FILE_ENTRY   "~/recursive_file.acd"
#define LIST_DIRECTORY_ENTRY   "~/list_directory.acd"
#define RECU_DIRECTORY_PATH   "~/recu_directory.path"
#define RECU_DIRECTORY_ENTRY   "~/recu_directory.acd"
    //
    int creat_recuf_enviro(FILE **recu_path, FILE **recu_acd, char *location, char *subdir);
    int hand_recursive_file(FILE *recu_path, FILE *recu_acd, int max_recu);
    void clear_recuf_enviro(FILE *recu_path, char *location, int clear_type, FILE *recu_acd);
    void clear_recuf_entry(char *location);
    //
    int creat_listd_enviro(FILE **recu_acd, char *location);
    int hand_list_directory(FILE *recu_acd, char *location, char *subdir);
    void clear_listd_enviro(FILE *recu_acd);
    void clear_listd_entry(char *location);
    //
    int creat_recud_enviro(FILE **recu_path, FILE **recu_acd, char *location, char *subdir);
    int hand_recu_directory(FILE *recu_path, FILE *recu_acd, int max_recu);
    void clear_recud_enviro(FILE *recu_path, char *location, int clear_type, FILE *recu_acd);
    void clear_recud_entry(char *location);
    //

#ifdef	__cplusplus
}
#endif

#endif	/* RECU_UTILITY_H */

