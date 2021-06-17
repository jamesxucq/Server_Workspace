#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <utime.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "macro.h"
#include "utility.h"

int valid_file_path(const char *path) {
    if (path == NULL) return -1;

    int lenght = strlen(path);
    if (lenght == 0 || lenght >= PATH_MAX) return -1;
    else if (strpbrk(path, "\\:*?\"<>|") != NULL) return -1;
    return 0;
}

int recu_rmdir(char *dirpath) {
    DIR *dires;
    struct dirent *dirp;
    char subdirex[MAX_PATH * 3];
    char file_name[MAX_PATH * 3];
    //
    if (NULL == (dires = opendir(dirpath))) return -1;
    while (NULL != (dirp = readdir(dires))) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            sprintf(file_name, "%s/%s", dirpath, dirp->d_name);
            if (unlink(file_name) < 0) {
                closedir(dires);
                return -1;
            }
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            sprintf(subdirex, "%s/%s", dirpath, dirp->d_name);
            if (recu_rmdir(subdirex)) {
                closedir(dires);
                return -1;
            }
        }
    }
    closedir(dires);
    if (rmdir(dirpath) < 0) return -1;
    //
    return 0;
}

