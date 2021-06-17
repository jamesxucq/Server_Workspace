// #include "opera_bzl.h"
// #include "list_cache.h"
// #include "fatt_list.h"
#include "recu_directory.h"
#include "listxml.h"
#include "com.h"

// 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 

int build_recursive_file(FILE **recu_xml, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
    int bival = 0;
    //
    LIST_CACHE(file_name, location, RECURSIVE_FILE_DEFAULT);
    if (!FILE_EXISTS(file_name)) {
        printf("open recursive file is:%s\n", file_name); // disable by james 20120410
        *recu_xml = fopen64(file_name, "ab+");
    } else {
        printf("new recursive file is:%s\n", file_name); // disable by james 20120410
        if (!(bival = recursive_file(location, subdir))) {
            if (*recu_xml = fopen64(file_name, "wb+")) {
                if (flush_elist_xmlfile(*recu_xml, location, RECURSIVE_FILE_ENTRY)) {
                    printf("flush alist xml file error!\n");
                    fclose(*recu_xml);
                    unlink(file_name);
                }
                clear_recursive_file(location);
            } else printf("can not new file:%s\n", file_name);
        }
    }
    //
    return bival;
}

// 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 

int build_list_directory(FILE **list_xml, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
    int bival = 0;
    //
    LIST_CACHE(file_name, location, LIST_DIRECTORY_DEFAULT);
    if (!FILE_EXISTS(file_name)) {
        printf("open list file is:%s\n", file_name); // disable by james 20120410
        *list_xml = fopen64(file_name, "ab+");
    } else {
        printf("new list file is:%s\n", file_name); // disable by james 20120410
        if (!(bival = list_directory(location, subdir))) {
            if (*list_xml = fopen64(file_name, "wb+")) {
                if (flush_elist_xmlfile(*list_xml, location, LIST_DIRECTORY_ENTRY)) {
                    printf("flush alist xml file error!\n");
                    fclose(*list_xml);
                    unlink(file_name);
                }
                clear_list_directory(location);
            } else printf("can not new file:%s\n", file_name);
        }
    }
    //
    return bival;
}

// 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 

int build_recu_directory(FILE **recu_xml, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
    int bival = 0;
    //
    LIST_CACHE(file_name, location, RECU_DIRECTORY_DEFAULT);
    if (!FILE_EXISTS(file_name)) {
        printf("open recu file is:%s\n", file_name); // disable by james 20120410
        *recu_xml = fopen64(file_name, "ab+");
    } else {
        printf("new recu file is:%s\n", file_name); // disable by james 20120410
        if (!(bival = recu_directory(location, subdir))) {
            if (*recu_xml = fopen64(file_name, "wb+")) {
                if (flush_dlist_xmlfile(*recu_xml, location, RECU_DIRECTORY_ENTRY)) {
                    printf("flush alist xml file error!\n");
                    fclose(*recu_xml);
                    unlink(file_name);
                }
                clear_recu_directory(location);
            } else printf("can not new file:%s\n", file_name);
        }
    }
    //
    return bival;
}
