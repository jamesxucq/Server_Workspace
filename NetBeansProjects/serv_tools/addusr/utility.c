#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "macro.h"
#include "utility.h"

int valid_file_path(const char *path) {
    if (path == NULL) return -1;

    int lenght = strlen(path);
    if (lenght == 0 || lenght >= PATH_MAX) return -1;
    else if (strpbrk(path, "\\:*?\"<>|") != NULL) return -1;
    return 0;
}
//
#define POOL_PATH(PPATH, LOCATION, USER_NAME, SUBDIR) \
        sprintf(PPATH, "%s/%s/%s", LOCATION, USER_NAME, SUBDIR)
#define USER_LOCATION(LPATH, LOCATION, USER_NAME) \
        sprintf(LPATH, "%s/%s", LOCATION, USER_NAME)

int recu_mkdir(char *location, char *user_name) {
    char make_path[MAX_PATH * 3];
    //
    USER_LOCATION(make_path, location, user_name);
    if (mkdir(make_path, 0777)) printf("mkdir errno:%d", errno);
    else printf("mkdir user location:%s\n", make_path);
//
    POOL_PATH(make_path, location, user_name, "backup");
    if (mkdir(make_path, 0777)) printf("mkdir errno:%d", errno);
    POOL_PATH(make_path, location, user_name, "cache");
    if (mkdir(make_path, 0777)) printf("mkdir errno:%d", errno);
    POOL_PATH(make_path, location, user_name, "user_pool");
    if (mkdir(make_path, 0777)) printf("mkdir errno:%d", errno);
        //
    return 0;
}
