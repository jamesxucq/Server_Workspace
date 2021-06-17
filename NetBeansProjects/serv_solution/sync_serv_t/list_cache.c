#include "list_cache.h"
#include "listxml.h"

void clear_list_cache(unsigned int list_cache, char *location) {
    char file_name[MAX_PATH * 3];
    if(CACHE_RECURSIVE_FILE & list_cache) CLEAR_RECURSIVE_FILE(file_name, location)
    if(CACHE_LIST_DIREC & list_cache) CLEAR_LIST_DIREC(file_name, location)
    if(CACHE_RECU_DIRECTORY & list_cache) CLEAR_RECU_DIRECTORY(file_name, location)
    if(CACHE_ANCHOR_FILE & list_cache) CLEAR_ANCHOR_FILE(file_name, location)
}