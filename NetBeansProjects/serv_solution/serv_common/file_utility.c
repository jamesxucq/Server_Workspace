
#include "file_utility.h"

int recu_mkdir(const char *dirpath, mode_t mode) {
    char parent[PATH_MAX];
    if (!mkdir(dirpath, mode)) return 0;
    if (ENOENT == errno) {
        FILE_GET_DIRE(parent, dirpath)
        if (recu_mkdir(parent, mode) || mkdir(dirpath, mode))
            return -1;
    }
    return 0;
}

/*
inline int cmp_time(time_t timea, time_t timeb) {
    return timea > timeb ? -1 : timea == timeb ? 0 : 1;
}
 */
#define BUFF_SIZE                        0x2000
#define BUILD_CHUNK_SIZE		((int)1 << 22) // 4M

FILE *creat_bakup_file(FILE *local_stre, uint64 offset) {
    unsigned char buffer[BUFF_SIZE];
    FILE *bakup_stre = NULL;
    int rlen, wlen;
    //
    if (!(bakup_stre = tmpfile64())) return NULL;
    if (fseeko64(local_stre, offset, SEEK_SET)) return NULL;
    rewind(bakup_stre);
    //
    do {
        rlen = fread(buffer, 1, BUFF_SIZE, local_stre);
        if (0 >= rlen) continue;
        //
        if (0 >= (wlen = fwrite(buffer, 1, rlen, bakup_stre))) continue;
        if (wlen != rlen) return NULL;
    } while (0 < rlen && 0 < wlen);
    //
    return bakup_stre;
}

FILE *creat_bakup_chunk(FILE *local_stre, uint64 offset) {
    unsigned char buffer[BUFF_SIZE];
    FILE *bakup_stre = NULL;
    int rlen, wlen;
    //
    if (!(bakup_stre = tmpfile64())) return NULL;
    if (fseeko64(local_stre, offset, SEEK_SET)) return NULL;
    rewind(bakup_stre);
    //
    size_t bakup_tatol = 0;
    do {
        rlen = fread(buffer, 1, BUFF_SIZE, local_stre);
        if (0 >= rlen) continue;
        //
        if (0 >= (wlen = fwrite(buffer, 1, rlen, bakup_stre))) continue;
        if (wlen != rlen) return NULL;
        bakup_tatol += wlen;
        if (BUILD_CHUNK_SIZE == bakup_tatol) break;
    } while (0 < rlen && 0 < wlen);
    //
    return bakup_stre;
}

#define FAIL_EXIT(DATA, RENO) { fclose(DATA); return RENO; }
#define FAULT_EXIT(INDE, DATA, RENO) { fclose(INDE); fclose(DATA); return RENO; }

int creat_action_bakup(char *location, char *file_name, time_t act_time) {
    unsigned char buffer[BUFF_SIZE];
    int rlen, wlen;
    //        
    char abso_path[MAX_PATH * 3];
    POOL_PATH(abso_path, location, file_name);
    FILE *action = fopen64(abso_path, "rb");
    if (!action) {
        _LOG_ERROR("not open file %s", abso_path);
        return -1;
    }
    //
    LAST_BAKUP(abso_path, location, file_name, act_time);
    FILE *bakup = fopen64(abso_path, "wb+");
    if (!bakup) {
        _LOG_ERROR("not open file %s", abso_path);
        FAIL_EXIT(action, -1)
    }
        //        
        do {
            rlen = fread(buffer, 1, BUFF_SIZE, action);
            if (0 >= rlen) continue;
            //
            if (0 >= (wlen = fwrite(buffer, 1, rlen, bakup))) continue;
            if (wlen != rlen) FAULT_EXIT(bakup, action, -1)
            } while (0 < rlen && 0 < wlen);
    //        
    fclose(bakup);
    fclose(action);
    return 0;
}

//
#define  HAVE_DIRENT_DTYPE    0x01
#ifdef HAVE_DIRENT_DTYPE
int recu_rmdir(char *dirpath) {
    DIR *dires;
    struct dirent *dirp;
    char subdir[MAX_PATH * 3];
    //
    if (NULL == (dires = opendir(dirpath))) return -1;
    sprintf(subdir, "%s/", dirpath);
    const char *pdir = strlast(subdir);
    while (NULL != (dirp = readdir(dires))) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            strcpy((char *)pdir, dirp->d_name);
            if (unlink(subdir) < 0) {
                closedir(dires);
                return -1;
            }
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            strcpy((char *)pdir, dirp->d_name);
            if (recu_rmdir(subdir)) {
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
#else
int recu_rmdir(char *dirpath) {
    DIR *dires;
    struct dirent *dirp;
    struct stat64 statb;
    char subdir[MAX_PATH * 3];
    //
    if (NULL == (dires = opendir(dirpath))) return -1;
    sprintf(subdir, "%s/", dirpath);
    char *pdir = strlast(subdir);
    while (NULL != (dirp = readdir(dires))) {
        strcpy(pdir, dirp->d_name);
        if (stat64(subdir, &statb) < 0) {
            closedir(dires);
            _LOG_WARN("stat errno:%d", errno);
            return -1;
        }
        if (S_ISLNK(statb.st_mode) || S_ISREG(statb.st_mode)) {
            if (unlink(subdir) < 0) {
                closedir(dires);
                return -1;
            }
        } else if (S_ISDIR(statb.st_mode)) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            if (recu_rmdir(subdir)) {
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
#endif

int file_copy(char *from, char *to) {
    int fd1, fd2;
    struct stat64 statb;
    if (stat64(from, &statb) == -1) {
        _LOG_WARN("can not get info of %s", from);
        return -1;
    }
    if ((fd1 = open64(from, O_RDONLY)) == -1) {
        _LOG_WARN("can not open file %s", from);
        return -1;
    }
    if ((fd2 = creat64(to, statb.st_mode)) == -1) {
        _LOG_WARN("can not open file %s", to);
        return -1;
    }
    //
    int size;
    unsigned char buffer[0x1000] = "";
    while (0 != (size = read(fd1, buffer, 0x1000))) {
        if (write(fd2, buffer, size) != size)
            _LOG_WARN("write error!");
    }
    //
    close(fd1);
    close(fd2);
    return 0;
}

#define MAX_COPY_SIZ  0x20000 // 1GB
// #define MAX_COPY_SIZ  1024 // 8MB

int file_copy_ext(char *from, char *to) { // 0:ok 1:processing -1:error
    struct stat64 statbuf1, statbuf2;
    if (stat64(from, &statbuf1) == -1) {
        _LOG_WARN("can not get info of %s", from);
        return -1;
    }
    if (stat64(to, &statbuf2) == -1)
        statbuf2.st_size = 0;
    //
    int fd1, fd2;
    if ((fd1 = open64(from, O_RDONLY)) == -1) {
        _LOG_WARN("can not open file %s", from);
        return -1;
    }
    if ((fd2 = open64(to, O_WRONLY | O_CREAT | O_APPEND, statbuf1.st_mode)) == -1) {
        close(fd1);
        _LOG_WARN("can not open file %s", to);
        return -1;
    }
    //
    int size, inde;
    unsigned char buffer[0x2000] = ""; //8K
    int cpval = 0x01;
    lseek64(fd1, statbuf2.st_size, SEEK_SET);
    for (inde = 0; MAX_COPY_SIZ > inde; inde++) {
        if ((size = read(fd1, buffer, 0x2000))) {
            if (write(fd2, buffer, size) != size) {
                cpval = -1;
                _LOG_WARN("write error!");
                break;
            }
        } else {
            cpval = 0x00;
            break;
        }
    }
    //
    close(fd1);
    close(fd2);
    return cpval;
}

void copy_regular_file(char from[], char to[]) {
    struct stat64 statb;
    if (stat64(from, &statb) == -1) {
        _LOG_WARN("can not get info of %s", from);
        return;
    }
    int fd1, fd2;
    if ((fd1 = open64(from, O_RDONLY)) == -1) {
        _LOG_WARN("can not open file %s", from);
        return;
    }
    if ((fd2 = creat64(to, statb.st_mode)) == -1) {
        _LOG_WARN("can not open file %s", to);
        return;
    }
    //
    unsigned char buffer[0x1000] = "";
    int size;
    while (0 != (size = read(fd1, buffer, 0x1000))) {
        if (write(fd2, buffer, size) != size) {
            _LOG_WARN("write error!");
        }
    }
    close(fd1);
    close(fd2);
}

int recu_dires_copy(char *dire_from, char *dire_to) {
    char path1[MAX_PATH * 3] = "";
    char path2[MAX_PATH * 3] = "";
    DIR *dires;
    struct dirent *dirp;
    struct stat64 statb;
    //
    if (stat64(dire_from, &statb) != -1) {
        if (S_ISDIR(statb.st_mode)) {
            if (mkdir(dire_to, 0777) == -1) {
                _LOG_WARN("can not creat dir %s", dire_to);
                return -1;
            }
            if (NULL == (dires = opendir(dire_from))) {
                _LOG_WARN("can not open dir %s", dire_from);
                return -1;
            }
            while (NULL != (dirp = readdir(dires))) {
                if (0 == strcmp(dirp->d_name, ".") || 0 == strcmp(dirp->d_name, ".."))
                    continue;
                sprintf(path1, "%s/%s", dire_from, dirp->d_name);
                sprintf(path2, "%s/%s", dire_to, dirp->d_name);
                recu_dires_copy(path1, path2); //
            }
        } else copy_regular_file(dire_from, dire_to);
    }
    closedir(dires);
    //
    return 0;
}

// for debut
void plawi(char *base_name, char *file_name){
    char path[MAX_PATH * 3];
    sprintf(path, "%s/user_pool%s", base_name, file_name);
    time_t last_write;
    FILE_TIME(last_write, path)
    _LOG_DEBUG("path:%s last_write:%s", path, utc_time_text(last_write));
}

//

char *get_executable_path(char* epath) {
    char exec_path[PATH_MAX];
    int len = readlink("/proc/self/exe", exec_path, PATH_MAX);
    if (0 > len) return NULL;
    exec_path[len] = '\0';
    //
    strcpy(epath, exec_path);
    return epath;
}

char *get_appli_path() {
    static char appli_path[1024];
    if ('\0' == appli_path[0]) {
        if (!get_executable_path(appli_path)) return NULL;
        char *tokep = strrchr(appli_path, '/');
        if (!tokep) return NULL;
        tokep[0] = '\0';
        tokep = strrchr(appli_path, '/');
        if (!tokep) return NULL;
        (tokep)[0] = '\0';
    }
    return appli_path;
}