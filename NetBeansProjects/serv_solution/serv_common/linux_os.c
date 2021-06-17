/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdlib.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include "common_header.h"

int find_user_group(const char *user, const char *group, uid_t *uid, gid_t *gid) {
    struct passwd *my_pwd = getpwnam(user);
    if (NULL == my_pwd) {
        _LOG_ERROR("getpwnam(\"%s\") failed", user);
        return -1;
    }
    struct group *my_grp = getgrnam(group);
    if (NULL == my_grp) {
        _LOG_ERROR("can't find group name %s\n", group);
        return -1;
    }
    if (0 == my_grp->gr_gid) {
        _LOG_WARN("I will not set gid to 0\n");
        return 0;
    }
    //
    *uid = my_pwd->pw_uid;
    *gid = my_grp->gr_gid;
    return 0;
}

