#include "opera_bzl.h"
#include "list_cache.h"
#include "fasd_list.h"
#include "listxml.h"

//
#define CACHE_INTERVAL_TIME   45 // 45s
static int cache_exist(char *file_name) {
    time_t last_write;
    int reval = -1;
    //
    if(!FILE_EXIST(file_name)) {
        FILE_TIME(last_write, file_name)
        if(CACHE_INTERVAL_TIME > (time (NULL) - last_write)) {
            reval = 0x00;
// _LOG_DEBUG("*********** exist cache file:%s\n", file_name);
        } else {
// _LOG_DEBUG("*********** cache file timeout:%s\n", file_name);
            unlink(file_name);
        }
    }
    //
    return reval;
}

// 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 

int build_recursive_file(FILE **recu_xml, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
    int ibval = 0;
    //
    LIST_CACHE(file_name, location, RECURSIVE_FILE_DEFAULT);
    if (!cache_exist(file_name)) {
// _LOG_DEBUG("*********** open recursive file is:%s\n", file_name); // disable by james 20120410
        *recu_xml = fopen64(file_name, "ab+");
    } else {
// _LOG_DEBUG("*********** new recursive file is:%s\n", file_name); // disable by james 20120410
        if (!(ibval = recursive_file(location, subdir))) {
            if ((*recu_xml = fopen64(file_name, "wb+"))) {
                if (flush_elist_xmlfile(*recu_xml, location, RECURSIVE_FILE_ENTRY)) {
// _LOG_DEBUG("flush alist xml file error!\n");
                    fclose(*recu_xml);
                    unlink(file_name);
                }
                clear_recursive_file(location);
            } else { _LOG_ERROR("can not new file:%s\n", file_name); }
        }
    }
    //
// _LOG_DEBUG("ibval:%d\n", ibval);
    return ibval;
}

// 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 

int build_list_directory(FILE **list_xml, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
    int ibval = 0;
    //
    LIST_CACHE(file_name, location, LIST_DIRECTORY_DEFAULT);
    if (!cache_exist(file_name)) {
// _LOG_DEBUG("*********** open list file is:%s\n", file_name); // disable by james 20120410
        *list_xml = fopen64(file_name, "ab+");
    } else {
// _LOG_DEBUG("*********** new list file is:%s\n", file_name); // disable by james 20120410
        if (!(ibval = list_directory(location, subdir))) {
            if ((*list_xml = fopen64(file_name, "wb+"))) {
                if (flush_elist_xmlfile(*list_xml, location, LIST_DIRECTORY_ENTRY)) {
// _LOG_DEBUG("flush alist xml file error!\n");
                    fclose(*list_xml);
                    unlink(file_name);
                }
                clear_list_directory(location);
            } else { _LOG_ERROR("can not new file:%s\n", file_name); }
        }
    }
    //
    return ibval;
}

// 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 

int build_recu_directory(FILE **recu_xml, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
    int ibval = 0;
    //
    LIST_CACHE(file_name, location, RECU_DIRECTORY_DEFAULT);
    if (!cache_exist(file_name)) {
// _LOG_DEBUG("*********** open recu file is:%s\n", file_name); // disable by james 20120410
        *recu_xml = fopen64(file_name, "ab+");
    } else {
// _LOG_DEBUG("*********** new recu file is:%s\n", file_name); // disable by james 20120410
        if (!(ibval = recu_directory(location, subdir))) {
            if ((*recu_xml = fopen64(file_name, "wb+"))) {
                if (flush_dlist_xmlfile(*recu_xml, location, RECU_DIRECTORY_ENTRY)) {
// _LOG_DEBUG("flush alist xml file error!\n");
                    fclose(*recu_xml);
                    unlink(file_name);
                }
                clear_recu_directory(location);
            } else { _LOG_ERROR("can not new file:%s\n", file_name); }
        }
    }
    //
    return ibval;
}

// 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 

int build_anchor_file(FILE **anch_xml, char *location, unsigned int anchor) {
    struct fsdo_list *alist = NULL;
    char file_name[MAX_PATH * 3];
    int ibval = 0;
    //
    LIST_CACHE(file_name, location, ANCHOR_FILE_DEFAULT);
    if (!cache_exist(file_name)) {
// _LOG_DEBUG("*********** open anchor file is:%s", file_name); // disable by james 20120410
        *anch_xml = fopen64(file_name, "ab+");
    } else {
// _LOG_DEBUG("*********** new anchor file is:%s", file_name); // disable by james 20120410
        int iaval = GET_ALIST_BY_ANCHOR_BZL(&alist, location, anchor);
        if (iaval) {
            if (ERR_FAULT == iaval || SEARCH_ANCHOR == iaval || SEARCH_ANCHOR == ALIST_ERROR) 
                ibval = 3;
            else if (ANCHOR_NOFOND == iaval) ibval = 2;
        } else {
            if ((*anch_xml = fopen64(file_name, "wb+"))) {
                if (flush_alist_xmlfile(*anch_xml, alist)) ibval = 3;
            } else {
                _LOG_ERROR("can not new file:%s", file_name);
            }
        }
        DELETE_FSDO_LIST(alist)
    }
    //
// _LOG_DEBUG("open anchor ibval:%d", ibval);
    return ibval;
}