#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>

#include "utility.h"
#include "macro.h"
#include "md5.h"

inline char *append_path(char *path, char *subdir) {
    static char fullpath[MAX_PATH * 3];
    sprintf(fullpath, "%s%s", path, subdir + 1);
    return fullpath;
}

void get_md5sum_text(char *buf, int len, char *sum) {
    MD5_CTX md;

    MD5Init(&md);
    MD5Update(&md, (unsigned char *) buf, len);
    MD5Final(&md);

    int index;
    for (index = 0; index < MD5_DIGEST_LEN; index++)
        sprintf(sum + index * 2, "%02x", md.digest[index]);
    sum[32] = '\0';
}

char *trans_auth_csum(char *csum, unsigned int uid, char *access_key) {
    char buffer[MID_TEXT_LEN];
    char sum[MD5_TEXT_LENGTH];

    if (!access_key || !csum) return NULL;
    sprintf(buffer, "%lu:%s", uid, access_key);
    get_md5sum_text(buffer, strlen(buffer), sum);

    strcpy(csum, sum);
    return csum;
}

int valid_file_path(const char *path) {
    if (path == NULL) return -1;

    int lenght = strlen(path);
    if (lenght == 0 || lenght >= PATH_MAX) return -1;
    else if (strpbrk(path, "\\:*?\"<>|") != NULL) return -1;
    return 0;
}

char *strsplit(char *str, const char *delim) {
    char *tokensp;
    static char *tokenep;

    if (!str) {
        if (!tokenep) return NULL;
        tokensp = tokenep + strlen(delim);
    } else tokensp = str;

    tokenep = strstr(tokensp, delim);
    if (tokenep) *tokenep = '\0';

    return tokensp;
}

char *split_value(char *str) {
    char *tokensp, *tokenep;

    tokensp = strchr(str, ' ');
    if (tokensp) *tokensp = '\0';

    ++tokensp;
    tokenep = strrchr(tokensp, ' ');
    if (tokenep) *tokenep = '\0';

    return tokensp;
}

char *split_line(char *str) {
    char *tokensp;

    tokensp = strstr(str, ": ");
    if (tokensp) *tokensp = '\0';

    return tokensp + 2;
}

#define BUFF_SIZE                        0x2000

int clear_backup_file(FILE *backup_point, FILE *origin_point, size_t offset) {
    char buffer[BUFF_SIZE];
    int rlen, wlen;

    if (fseek(origin_point, offset, SEEK_SET)) return -1;
    if (fseek(backup_point, 0, SEEK_SET)) return -1;

    do {
        rlen = fread(buffer, 1, BUFF_SIZE, origin_point);
        if (0 >= rlen) continue;

        if (0 >= (wlen = fwrite(buffer, 1, rlen, backup_point))) continue;
        if (wlen != rlen) return -1;
    } while (0 < rlen && 0 < wlen);

    return 0;
}

inline char *full_name_ex(char *path, char *subdir, char *file) {
    static char fullpath[MAX_PATH * 3];
    sprintf(fullpath, "%s/user_pool%s%s", path, subdir, file);
    return fullpath;
}

inline char *full_path(char *path, char *subdir) {
    static char fullpath[MAX_PATH * 3];
    sprintf(fullpath, "%s/user_pool%s", path, subdir);
    return fullpath;
}

off_t file_size(const char *file_path) {
    struct stat statbuf;
    if (!file_path) return -1;
    if (stat(file_path, &statbuf) < 0) return -1;
    return statbuf.st_size;
}

int recu_dirs_size(uint64 *pool_size, char *location, char *subdir) {
    DIR *dirs;
    struct dirent *dirp;
    char subdirex[MAX_PATH * 3];

    if ((dirs = opendir(full_path(location, subdir))) == NULL)
        return -1;

    while ((dirp = readdir(dirs)) != NULL) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            //LOG_INFO("dirp->d_name:%s", dirp->d_name);
            *pool_size += file_size(full_name_ex(location, subdir, dirp->d_name));
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            sprintf(subdirex, "%s%s/", subdir, dirp->d_name);
            if (recu_dirs_size(pool_size, location, subdirex)) {
                closedir(dirs);
                return -1;
            }
        }
    }
    closedir(dirs);

    return 0;
}

int list_dirs_remove(char *dirpath) {
    DIR *dirs;
    struct dirent *dirp;
    char file_name[MAX_PATH * 3];

    if ((dirs = opendir(dirpath)) == NULL) return -1;

    while ((dirp = readdir(dirs)) != NULL) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            sprintf(file_name, "%s/%s", dirpath, dirp->d_name);
            if (unlink(file_name) < 0) return -1;
        }
    }
    closedir(dirs);

    return 0;
}

inline char *last_backup(char *path, char *subdir, time_t bak_time) {
    static char bakpath[MAX_PATH * 3];
    sprintf(bakpath, "%s/backup%s_%d", path, strrchr(subdir, '/'), bak_time);
    return bakpath;
}

char *strnrchr(char *str, char c, int num) {
    int count = 0;
    char *p = str;
    while ('\0' != *p) {
        while ('\0' != *p)
            if (c == *p++) {
                count++;
                break;
            }
    }
    if (num > count) return NULL;
    count = 0;
    while (num != count++) {
        while (c != *p) p--;
        if (num != count) p--;
    }
    return p;
}