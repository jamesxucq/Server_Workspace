/*
int recursive_file(fatt_lsth alisth, char *location, char *subdir) {
    DIR *dires;
    struct dirent *dirp;
    char subdirex[MAX_PATH * 3], pool_path[MAX_PATH * 3];
    //
    POOL_PATH(pool_path, location, subdir);
    if (NULL == (dires = opendir(pool_path))) {
        _LOG_INFO("opendir");
        return -1;
    }
    //
    while (NULL != (dirp = readdir(dires))) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            //_LOG_INFO("dirp->d_name:%s", dirp->d_name);
            FULL_NAME_EXT(pool_path, subdir, dirp->d_name);
            // _LOG_DEBUG("POOL_PATH:%s", POOL_PATH);
            RECU_ADD_ALIST(alisth, location, pool_path);
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            sprintf(subdirex, "%s%s/", subdir, dirp->d_name);
            if (recursive_file(alisth, location, subdirex)) {
                closedir(dires);
                return -1;
            }
        }
    }
    closedir(dires);
    //
    return 0;
}
 */


/*
int list_directory(fatt_lsth alisth, char *location, char *subdir) {
    DIR *dires;
    struct dirent *dirp;
    char POOL_PATH[MAX_PATH * 3];
    //
    POOL_PATH(POOL_PATH, location, subdir);
    if (NULL == (dires = opendir(POOL_PATH))) {
        _LOG_INFO("opendir");
        return -1;
    }
    //
    while (NULL != (dirp = readdir(dires))) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            _LOG_INFO("%s", dirp->d_name);
            if (!strcmp("/", subdir)) FULL_NAME_EXT(POOL_PATH, subdir, dirp->d_name);
            else FULL_NAME(POOL_PATH, subdir, dirp->d_name);
            LIST_ADD_ALIST(alisth, location, POOL_PATH);
        }
    }
    closedir(dires);
    //
    return 0;
}
 */
/*
int recu_directory(dires_listh dlisth, char *location, char *subdir) {
    DIR *dires;
    struct dirent *dirp;
    char subdirex[MAX_PATH * 3], POOL_PATH[MAX_PATH * 3];
    //
    POOL_PATH(POOL_PATH, location, subdir);
    if (NULL == (dires = opendir(POOL_PATH))) {
        _LOG_INFO("opendir");
        return -1;
    }
    //
    while (NULL != (dirp = readdir(dires))) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            //_LOG_INFO("%s", dirp->d_name);
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            if (!strcmp("/", subdir))
                sprintf(subdirex, "%s%s", subdir, dirp->d_name);
            else sprintf(subdirex, "%s/%s", subdir, dirp->d_name);
            _LOG_INFO("%s", subdirex);
            add_new_dlist(dlisth, subdirex);
            if (recu_directory(dlisth, location, subdirex)) {
                closedir(dires);
                return -1;
            }
        }
    }
    closedir(dires);
    //
    return 0;
}
 */
/*
int recu_directory_att(FILE *listfp, char *location, char *subdir) {
    DIR *dires;
    struct dirent *dirp;
    char subdirex[MAX_PATH * 3], POOL_PATH[MAX_PATH * 3];
    //
    POOL_PATH(POOL_PATH, location, subdir);
    if (NULL == (dires = opendir(POOL_PATH))) {
        _LOG_INFO("opendir");
        return -1;
    }
    //
    while (NULL != (dirp = readdir(dires))) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            _LOG_INFO("%s", dirp->d_name);
            //RecuAddAttElem(listfp, location, fullname(subdir, dirp->d_name));
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            if (!strcmp("/", subdir))
                sprintf(subdirex, "%s%s", subdir, dirp->d_name);
            else sprintf(subdirex, "%s/%s", subdir, dirp->d_name);
            if (recu_directory_att(listfp, location, subdirex)) {
                closedir(dires);
                return -1;
            }
        }
    }
    closedir(dires);
    //
    return 0;
}
 
/*
int build_recursive_file(FILE **recu_xml, char *location, char *subdir) {
    struct fatt_list *alist = NULL;
    char file_name[MAX_PATH * 3];
    FILE *recu_xml = NULL;
    //
    LIST_CACHE(file_name, location, RECURSIVE_FILE_DEFAULT);
    struct stat64 statbuf;
    if ((!stat64(file_name, &statbuf) && (CACHE_ALIFE_TIME > (time(NULL) - statbuf.st_mtime)))) {
        _LOG_DEBUG("open file is:%s", file_name); // disable by james 20120410
        recu_xml = fopen64(file_name, "ab+");
        //
    } else {
        _LOG_DEBUG("new file is:%s", file_name); // disable by james 20120410
        if (recu_xml = fopen64(file_name, "wb+")) {
            if (!recursive_file(&alist, location, subdir)) {
                if (flush_alist_xmlfile(recu_xml, alist)) {
                    _LOG_INFO("flush alist xml file error!");
                    fclose(recu_xml);
                    unlink(file_name);
                    recu_xml = NULL;
                }
            }
            DELETE_FATT_LIST(alist)
        } else _LOG_WARN("can not new file:%s", file_name);
    }
    //
    return recu_xml;
}
 */

/*
int build_list_directory(FILE **list_xml, char *location, char *subdir) {
    struct fatt_list *alist = NULL;
    char file_name[MAX_PATH * 3];
    FILE *list_xml = NULL;
    //
    LIST_CACHE(file_name, location, LIST_DIRECTORY_DEFAULT);
    struct stat64 statbuf;
    if ((!stat64(file_name, &statbuf) && (CACHE_ALIFE_TIME > (time(NULL) - statbuf.st_mtime)))) {
        _LOG_DEBUG("open file is:%s", file_name); // disable by james 20120410
        list_xml = fopen64(file_name, "ab+");
        //
    } else {
        _LOG_DEBUG("new file is:%s", file_name); // disable by james 20120410
        if (list_xml = fopen64(file_name, "wb+")) {
            if (!list_directory(&alist, location, subdir)) {
                if (flush_alist_xmlfile(list_xml, alist)) {
                    fclose(list_xml);
                    unlink(file_name);
                    list_xml = NULL;
                }
            }
            DELETE_FATT_LIST(alist)
        } else _LOG_WARN("can not new file:%s", file_name);
    }
    //
    return list_xml;
}
 */

/*
int build_recu_directory(FILE **recu_xml, char *location, char *subdir) {
    struct dires_list *dlist = NULL;
    char file_name[MAX_PATH * 3];
    FILE *recu_xml = NULL;
    //
    LIST_CACHE(file_name, location, RECU_DIRECTORY_DEFAULT);
    struct stat64 statbuf;
    if ((!stat64(file_name, &statbuf) && (CACHE_ALIFE_TIME > (time(NULL) - statbuf.st_mtime)))) {
        _LOG_DEBUG("open file is:%s", file_name); // disable by james 20120410
        recu_xml = fopen64(file_name, "ab+");
        //
    } else {
        _LOG_DEBUG("new file is:%s", file_name); // disable by james 20120410
        if (recu_xml = fopen64(file_name, "wb+")) {
            if (!recu_directory(&dlist, location, subdir)) {
                if (flush_dlist_xmlfile(recu_xml, dlist)) {
                    fclose(recu_xml);
                    unlink(file_name);
                    recu_xml = NULL;
                }
            }
            DELETE_DIRES_LIST(dlist)
        } else _LOG_WARN("can not new file:%s", file_name);
    }
    //
    return recu_xml;
}
 */

 
/*
int flush_alist_xmlfile(FILE *alist_xml, FILE *alist) {
    char line_buff[LINE_LENGTH];
    struct fatt_list *alisttmp;
//
    rewind(alist_xml);
    rewind(alist);
//
    ADD_XML_HEAD(line_buff);
    fputs(line_buff, alist_xml);
    fputs("<files_attrib>", alist_xml);
    if (alist) {
        char *text_ptr, last_write[TIME_LENGTH];
        struct fattsdo *fatt;
        // for(el=head;el;el=el->next)
//
        ALIST_FOREACH(alist, alisttmp) {
            fatt = &alisttmp->fatt;
            ADD_ATTRIB_LINE(line_buff, text_ptr, last_write, fatt);
            fputs(line_buff, alist_xml);
        }
    }
    fputs("</files_attrib>", alist_xml);
//
    return 0;
}
 */

 
/*
int flush_dlist_xmlfile(FILE *dlist_xml, FILE *dlist) {
    char line_txt[LINE_LENGTH];
    struct dires_list *dlisttmp;
//
    rewind(dlist_xml);
    rewind(dlist);
//
    ADD_XML_HEAD(line_txt);
    fputs(line_txt, dlist_xml);
    fputs("<directories>", dlist_xml);
    if (dlist) {
//
        DLIST_FOREACH(dlist, dlisttmp) {
            ADD_DIR_LINE(line_txt, dlisttmp->dir_name);
            fputs(line_txt, dlist_xml);
        }
    }
    fputs("</directories>", dlist_xml);
//
    return 0;
}
 */

 
// struct fatt_list *add_alist_newsdo(fatt_lsth alisth, char *location, char *file_name, unsigned int action_type);
// #define RECU_ADD_ALIST(alisth, location, file_name) add_alist_newsdo(alisth, location, file_name, RECU)
// #define LIST_ADD_ALIST(alisth, location, file_name) add_alist_newsdo(alisth, location, file_name, LIST)


/*
struct fatt_list *add_alist_newsdo(fatt_lsth alisth, char *location, char *file_name, unsigned int action_type) {
    struct fatt_list *alisttmp = (struct fatt_list *) malloc(sizeof (struct fatt_list));
    if (!alisttmp) return NULL;
    memset(alisttmp, '\0', sizeof (struct fatt_list));
    //
    // _LOG_INFO("location:%s file_name:%s", location, file_name);
    alisttmp->fatt.file_name = strdup(file_name);
    alisttmp->fatt.action_type = action_type;
    char full_name[MAX_PATH * 3];
    POOL_PATH(full_name, location, file_name);
    FILE_SIZE_TIME(alisttmp->fatt.file_size, alisttmp->fatt.last_write, full_name)
    //
    ALIST_PREPEND(*alisth, alisttmp);
    return alisttmp;
}
*/