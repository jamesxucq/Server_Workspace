#include <sys/types.h>
#include <sys/times.h>
#include <utime.h>
#include <dirent.h>
#include <limits.h>

#include "logger.h"
#include "string_utility.h"
#include "file_utility.h"

void *load_file(const char *file_path, size_t *bytes) {
    int fildes;

    if (!bytes || !file_path) return NULL;

    if ((fildes = open(file_path, O_RDONLY, 0)) < 0) return NULL;
    struct stat fs;
    if (fstat(fildes, &fs) < 0) {
        close(fildes);
        return NULL;
    }

    size_t size = fs.st_size;
    if (bytes != NULL && *bytes > 0 && *bytes < fs.st_size) size = *bytes;

    void *buf = malloc(size + 1);
    if (buf == NULL) {
        close(fildes);
        return NULL;
    }

    ssize_t count = read(fildes, buf, size);
    close(fildes);

    if (count != size) {
        free(buf);
        return NULL;
    }

    ((char*) buf)[count] = '\0';

    if (bytes != NULL) *bytes = count;
    return buf;
}

off_t file_size(const char *file_path) {
    struct stat statbuf;
    if (!file_path) return -1;
    if (stat(file_path, &statbuf) < 0) return -1;
    return statbuf.st_size;
}

off_t file_size_descrip(int fildes) {
    struct stat statbuf;
    if (fstat(fildes, &statbuf) < 0) return -1;
    return statbuf.st_size;
}

/*
off_t file_size_point(FILE *filpint) {
    int fildes;
    struct stat statbuf;

    if (!filpint) return -1;
    if ((fildes = fileno(filpint)) == -1) return -1;

    if (fstat(fildes, &statbuf) < 0) return -1;
    return statbuf.st_size;
}
 */
off_t file_size_point(FILE *filpint) {
    if (!filpint) return -1;

    if (fseek(filpint, 0, SEEK_END)) return -1;
    return ftell(filpint);
}

int file_time(time_t *last_write, char *name) {
    struct stat statbuf;

    if (!name || !last_write) return -1;
    if (stat(name, &statbuf) < 0) return -1;
    *last_write = statbuf.st_mtime;

    return 0;
}

int file_time_point(time_t *last_write, FILE *filpint) {
    int fildes;
    struct stat statbuf;

    if (!filpint || !last_write) return -1;
    if ((fildes = fileno(filpint)) == -1) return -1;

    if (fstat(fildes, &statbuf) < 0) return -1;
    *last_write = statbuf.st_mtime;

    return 0;
}

int file_size_time(size_t *file_size, time_t *last_write, char *name) {
    struct stat statbuf;

    if (stat(name, &statbuf) < 0) return -1;
    LOG_INFO("file_size:%d", statbuf.st_size);
    *file_size = statbuf.st_size;
    *last_write = statbuf.st_mtime;

    return 0;
}

int file_size_time_point(size_t *file_size, time_t *last_write, FILE *filpint) {
    int fildes;
    struct stat statbuf;

    if (!file_size || !filpint || !last_write) return -1;
    if ((fildes = fileno(filpint)) == -1) return -1;

    if (fstat(fildes, &statbuf) < 0) return -1;
    *file_size = statbuf.st_size;
    *last_write = statbuf.st_mtime;

    return 0;
}

int valid_file_path(const char *path) {
    if (path == NULL) return -1;

    int lenght = strlen(path);
    if (lenght == 0 || lenght >= PATH_MAX) return -1;
    else if (strpbrk(path, "\\:*?\"<>|") != NULL) return -1;
    return 0;
}

char *file_read_line(FILE *filpint) {
    int memsize;
    int c, c_count;
    char *string = NULL;

    if (!filpint) return NULL;
    for (memsize = 1024, c_count = 0; (c = fgetc(filpint)) != EOF;) {
        if (c_count == 0) {
            string = (char *) malloc(sizeof (char) * memsize);
            if (string == NULL) {
                //DEBUG("Memory allocation failed.");
                return NULL;
            }
        } else if (c_count == memsize - 1) {
            char *stringtmp;

            memsize *= 2;

            /* Here, we do not use realloc(). Because sometimes it is unstable. */
            stringtmp = (char *) malloc(sizeof (char) * (memsize + 1));
            if (stringtmp == NULL) {
                //DEBUG("Memory allocation failed.");
                free(string);
                return NULL;
            }
            memcpy(stringtmp, string, c_count);
            free(string);
            string = stringtmp;
        }
        string[c_count++] = (char) c;
        if ((char) c == '\n') break;
    }

    if (c_count == 0 && c == EOF) return NULL;
    string[c_count] = '\0';

    return string;
}

char *file_get_name(const char *file_path) {
    static char file_name[PATH_MAX];
    char dupli_path[PATH_MAX];

    strcpy(dupli_path, file_path);
    strcpy(file_name, basename(dupli_path));

    return file_name;
}

char *file_get_dir(char *filedir, const char *file_path) {
    char dupli_path[PATH_MAX];

    strcpy(dupli_path, file_path);
    strcpy(filedir, dirname(dupli_path));

    return filedir;
}

int file_make_dir(const char *dirpath, mode_t mode, unsigned int recursive) {
    char file_dir[PATH_MAX];

    if (mkdir(dirpath, mode) == 0) return 0;
    if (!recursive && errno == ENOENT) {
        char *parent_path = file_get_dir(file_dir, dirpath);
        if (!file_make_dir(parent_path, mode, recursive) && !file_make_dir(dirpath, mode, recursive)) {
            return 0;
        }
    }

    return -1;
}

int creat_file_dirpath(char *location, char *file_name, mode_t mode) {
    char filedir[PATH_MAX];
    int ret_value = 0;

    char *file_path = file_get_dir(filedir, file_name);
    //LOG_INFO("file_path:%s\n", file_path);

    char *dirpath = next_dirpath(location, file_path);
    while (dirpath) {
        if (TRUE != directory_exists(dirpath)) {
            ret_value = mkdir(dirpath, mode);
            if (ret_value) return -1;
            LOG_INFO("mkdir:%s", dirpath);
        }
        dirpath = next_dirpath(location, NULL);
    }

    return 0;
}

int creat_file_dirpath_ext(char *location, char *file_name, mode_t mode) {
    char filedir[PATH_MAX];
    char file_location[PATH_MAX];
    int ret_value = 0;

    char *file_path = file_get_dir(filedir, file_name);
    sprintf(file_location, "%s/user_pool", location);
    // LOG_INFO("file_path:%s\n", file_path);

    char *dirpath = next_dirpath(file_location, file_path);
    while (dirpath) {
        if (TRUE != directory_exists(dirpath)) {
            ret_value = mkdir(dirpath, mode);
            if (ret_value) return -1;
            LOG_INFO("mkdir:%s", dirpath);
        }
        dirpath = next_dirpath(file_location, NULL);
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

FILE *creat_backup_file(FILE *local_point, size_t offset) {
    char buffer[BUFF_SIZE];
    FILE *backup_point = NULL;
    int rlen, wlen;

    if (!(backup_point = tmpfile())) return NULL;
    if (fseek(local_point, offset, SEEK_SET)) return NULL;
    if (fseek(backup_point, 0, SEEK_SET)) return NULL;

    do {
        rlen = fread(buffer, 1, BUFF_SIZE, local_point);
        if (0 >= rlen) continue;

        if (0 >= (wlen = fwrite(buffer, 1, rlen, backup_point))) continue;
        if (wlen != rlen) return NULL;
    } while (0 < rlen && 0 < wlen);

    return backup_point;
}

FILE *creat_backup_chunk(FILE *local_point, size_t offset) {
    char buffer[BUFF_SIZE];
    FILE *backup_point = NULL;
    int rlen, wlen;

    if (!(backup_point = tmpfile())) return NULL;
    if (fseek(local_point, offset, SEEK_SET)) return NULL;
    if (fseek(backup_point, 0, SEEK_SET)) return NULL;

    size_t backup_count = 0;
    do {
        rlen = fread(buffer, 1, BUFF_SIZE, local_point);
        if (0 >= rlen) continue;

        if (0 >= (wlen = fwrite(buffer, 1, rlen, backup_point))) continue;
        if (wlen != rlen) return NULL;
        backup_count += wlen;
        if (BUILD_CHUNK_SIZE == backup_count) break;
    } while (0 < rlen && 0 < wlen);

    return backup_point;
}

#define FAIL_EXIT(DATFP, RNO) { fclose(DATFP); return RNO; }
#define FAULT_EXIT(IDXFP, DATFP, RNO) { fclose(IDXFP); fclose(DATFP); return RNO; }

int creat_action_backup(char *location, char *file_name, time_t act_time) {
    char buffer[BUFF_SIZE];
    int rlen, wlen;

    FILE *action = fopen(full_path(location, file_name), "rb");
    if (!action) return -1;
    FILE *backup = fopen(last_backup(location, file_name, act_time), "wb+");
    if (!backup) FAIL_EXIT(action, -1)
        do {
            rlen = fread(buffer, 1, BUFF_SIZE, action);
            if (0 >= rlen) continue;

            if (0 >= (wlen = fwrite(buffer, 1, rlen, backup))) continue;
            if (wlen != rlen) FAULT_EXIT(backup, action, -1)
            } while (0 < rlen && 0 < wlen);

    fclose(backup);
    fclose(action);

    return 0;
}

/*
int ftruncate_pointer(FILE *filpint, size_t length) {
    int fildes;

    if (!filpint) return -1;
    if ((fildes = fileno(filpint)) == -1) return -1;

    return ftruncate(fildes, length);
}
 */

int set_file_modtime(char *name, time_t modtime) {
    if (!name) return -1;

    struct utimbuf utb;
    memset(&utb, '\0', sizeof (struct utimbuf));
    utb.modtime = modtime;
    if (utime(name, &utb)) return -1;

    return 0;
}

/*
 inline int directory_exists(char *dirpath) {
    DIR *dirs;
    int ret_value = 0;

    dirs = opendir(dirpath);
    if (dirs) ret_value = 1;
    closedir(dirs);

    return ret_value;
}
 */

inline int directory_exists(char *dirpath) {
    struct stat statbuf;
    if (lstat(dirpath, &statbuf) < 0) return -1;
    return S_ISDIR(statbuf.st_mode);
}

inline int is_directory(char *path) {
    struct stat statbuf;
    if (lstat(path, &statbuf) < 0) return -1;
    return S_ISDIR(statbuf.st_mode);
}

int recu_dirs_remove(char *dirpath) {
    DIR *dirs;
    struct dirent *dirp;
    char subdirex[MAX_PATH * 3];
    char file_name[MAX_PATH * 3];

    if ((dirs = opendir(dirpath)) == NULL) return -1;

    while ((dirp = readdir(dirs)) != NULL) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            sprintf(file_name, "%s/%s", dirpath, dirp->d_name);
            if (unlink(file_name) < 0) return -1;
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            sprintf(subdirex, "%s/%s", dirpath, dirp->d_name);
            if (recu_dirs_remove(subdirex)) {
                closedir(dirs);
                return -1;
            }
        }
    }
    closedir(dirs);
    if (rmdir(dirpath) < 0) return -1;

    return 0;
}

int file_copy(char *file_from, char *file_to) {
    int fd1, fd2, size;
    char buf[0x1000] = "";
    char file1[MAX_PATH * 3] = "";
    char file2[MAX_PATH * 3] = "";

    struct stat statbuf;
    sprintf(file1, "%s", file_from);
    sprintf(file2, "%s", file_to);

    if (stat(file1, &statbuf) == -1) {
        printf("Can not get info of %s/n", file1);
        return -1;
    }
    if ((fd1 = open(file1, O_RDONLY)) == -1) {
        printf("Can not open file %s/n", file1);
        return -1;
    }
    if ((fd2 = creat(file2, statbuf.st_mode)) == -1) {
        printf("Can not open file %s/n", file2);
        return -1;
    }
    while ((size = read(fd1, buf, 0x1000)) != 0) {
        if (write(fd2, buf, size) != size)
            printf("Write error!/n");
    }
    close(fd1);
    close(fd2);

    return 0;
}

void copy_regular_file(char dirpath_from[], char dirpath_to[]) {
    int fd1, fd2, size;
    char buf[0x1000] = "";
    char dir1[MAX_PATH * 3] = "";
    char dir2[MAX_PATH * 3] = "";

    struct stat statbuf;
    sprintf(dir1, "%s", dirpath_from);
    sprintf(dir2, "%s", dirpath_to);
    sprintf(dir2, "%s/%s", dir2, dir1);

    if (stat(dir1, &statbuf) == -1) {
        printf("Can not get info of %s/n", dir1);
        return;
    }
    if ((fd1 = open(dir1, O_RDONLY)) == -1) {
        printf("Can not open file %s/n", dir1);
        return;
    }
    if ((fd2 = creat(dir2, statbuf.st_mode)) == -1) {
        printf("Can not open file %s/n", dir2);
        return;
    }
    while ((size = read(fd1, buf, 0x1000)) != 0) {
        if (write(fd2, buf, size) != size)
            printf("Write error!/n");
    }
    close(fd1);
    close(fd2);
}

int recu_dirs_copy(char *dirpath_from, char *dirpath_to) {
    DIR *dirpointer;
    char path1[MAX_PATH * 3] = "";
    char path2[MAX_PATH * 3] = "";
    char path3[MAX_PATH * 3] = "";
    struct dirent *direntp;
    struct stat info;

    strcpy(path1, dirpath_from);
    strcpy(path2, dirpath_to);
    if (stat(dirpath_from, &info) != -1) {
        if ((info.st_mode & 0170000) == 0040000) {
            sprintf(path3, "%s/%s", dirpath_to, dirpath_from);
            if (mkdir(path3, 0777) == -1) {
                printf("can not creat dir %s/n", path3);
                return -1;
            }
            if ((dirpointer = opendir(dirpath_from)) == NULL) {
                printf("Can not open dir %s/n", dirpath_from);
                return -1;
            }
            while ((direntp = readdir(dirpointer)) != NULL) {
                if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
                    continue;
                sprintf(dirpath_from, "%s/%s", dirpath_from, direntp->d_name);
                recu_dirs_copy(dirpath_from, dirpath_to); //閫掑綊鍏ョ偣銆傘�銆�
                sprintf(dirpath_from, "%s", path1);
            }
        } else {
            copy_regular_file(dirpath_from, dirpath_to);
        }
    }
    strcpy(dirpath_from, path1);
    strcpy(dirpath_to, path2);
    closedir(dirpointer);

    return 0;
}