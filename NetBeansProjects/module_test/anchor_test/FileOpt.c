#include <sys/types.h>
#include <utime.h>
#include <dirent.h>
#include <limits.h>

#include "Logger.h"
#include "StringOpt.h"
#include "FileOpt.h"

void *LoadFile(const char *filepath, size_t *nbytes) {
    int fd;
    if ((fd = open(filepath, O_RDONLY, 0)) < 0) return NULL;

    struct stat fs;
    if (fstat(fd, &fs) < 0) {
        close(fd);
        return NULL;
    }

    size_t size = fs.st_size;
    if (nbytes != NULL && *nbytes > 0 && *nbytes < fs.st_size) size = *nbytes;

    void *buf = malloc(size + 1);
    if (buf == NULL) {
        close(fd);
        return NULL;
    }

    ssize_t count = read(fd, buf, size);
    close(fd);

    if (count != size) {
        free(buf);
        return NULL;
    }

    ((char*) buf)[count] = '\0';

    if (nbytes != NULL) *nbytes = count;
    return buf;
}

off_t FileSizeN(const char *filepath) {
    struct stat finfo;
    if (stat(filepath, &finfo) < 0) return -1;
    return finfo.st_size;
}

off_t FileSizeD(int fd) {
    struct stat finfo;
    if (fstat(fd, &finfo) < 0) return -1;
    return finfo.st_size;
}

/*
off_t FileSizeP(FILE *fp) {
    int fd;
    struct stat finfo;

    if (!fp) return -1;
    if ((fd = fileno(fp)) == -1) return -1;

    if (fstat(fd, &finfo) < 0) return -1;
    return finfo.st_size;
}
 */
off_t FileSizeP(FILE *fp) {
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    return ftell(fp);
}

int FileTimeN(time_t *modify_time, char *name) {
    struct stat finfo;

    if (stat(name, &finfo) < 0) return -1;
    *modify_time = finfo.st_mtime;

    return 0;
}

int FileSizeTimeN(off_t *filesize, time_t *modify_time, char *name) {
    struct stat finfo;

    if (stat(name, &finfo) < 0) return -1;
    *filesize = finfo.st_size;
    *modify_time = finfo.st_mtime;

    return 0;
}

int FileSizeTimeP(off_t *filesize, time_t *modify_time, FILE *fp) {
    int fd;
    struct stat finfo;

    if (!fp) return -1;
    if ((fd = fileno(fp)) == -1) return -1;

    if (fstat(fd, &finfo) < 0) return -1;
    *filesize = finfo.st_size;
    *modify_time = finfo.st_mtime;

    return 0;
}

bool FileCheckPath(const char *path) {
    if (path == NULL) return false;

    int nLen = strlen(path);
    if (nLen == 0 || nLen >= PATH_MAX) return false;
    else if (strpbrk(path, "\\:*?\"<>|") != NULL) return false;
    return true;
}

char *FileReadLine(FILE *fp) {
    int memsize;
    int c, c_count;
    char *string = NULL;

    for (memsize = 1024, c_count = 0; (c = fgetc(fp)) != EOF;) {
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

char *FileGetName(const char *filepath) {
    char *path = strdup(filepath);
    char *bname = basename(path);
    char *filename = strdup(bname);
    free(path);

    return filename;
}

char *FileGetDir(const char *filepath) {
    char *path = strdup(filepath);
    char *dname = dirname(path);
    char *dir = strdup(dname);
    free(path);

    return dir;
}

char *FileGetDirEx(char *filedir, const char *filepath) {
    char *path = strdup(filepath);
    char *dname = dirname(path);
    strcpy(filedir, dname);
    free(path);

    return filedir;
}

bool FileMkdir(const char *dirpath, mode_t mode, bool recursive) {
    if (mkdir(dirpath, mode) == 0) return true;

    if (recursive == true && errno == ENOENT) {
        char *parentpath = FileGetDir(dirpath);
        if (FileMkdir(parentpath, mode, recursive) == true
                && FileMkdir(dirpath, mode, recursive) == true) {
            free(parentpath);
            return true;
        }
        free(parentpath);
    }

    return false;
}

bool CreatFileDirPath(char *sHomePath, char *FileName, mode_t mode) {
    char filedir[(PATH_MAX + 1)*2];
    int retValue = 0;

    char *filepath = FileGetDirEx(filedir, FileName);
    //LOG_INFO("filepath:%s\n", filepath);
    char *dirpath = NextDirPath(sHomePath, filepath);

    while (dirpath) {
        if (!CheckDirExists(dirpath)) {
            retValue = mkdir(dirpath, mode);
            if (retValue) {
                //free(filepath);
                return false;
            } else
                LOG_INFO("mkdir:%s", dirpath);
        }
        dirpath = NextDirPath(sHomePath, NULL);
    }
    //free(filepath);

    return true;
}

/*
inline int cmp_time(time_t timea, time_t timeb) {
    return timea > timeb ? -1 : timea == timeb ? 0 : 1;
}
 */
#define BUF_SIZ                         0x1000	// 4K

FILE *BackupFile(FILE *fpm) {
    char fbuf[BUF_SIZ];
    FILE *fpt = NULL;
    int rlen, wlen;

    if (!(fpt = tmpfile())) return NULL;
    fseek(fpt, 0, SEEK_SET);
    fseek(fpm, 0, SEEK_SET);

    do {
        rlen = fread(fbuf, 1, BUF_SIZ, fpm);
        if (0 >= rlen) continue;

        wlen = fwrite(fbuf, 1, rlen, fpt);
        if (wlen != rlen) return NULL;
        if (0 >= wlen) continue;

    } while (0 < rlen && 0 < wlen);

    return fpt;
}

int FtruncateP(FILE *fp, off_t length) {
    int fd;

    if (!fp) return -1;
    if ((fd = fileno(fp)) == -1) return -1;

    return ftruncate(fd, length);
}

int SetFileModtime(char *name, time_t modtime) {
    if (!name) return -1;

    struct utimbuf utb;
    memset(&utb, '\0', sizeof (struct utimbuf));
    utb.modtime = modtime;
    if (utime(name, &utb)) return -1;

    return 0;
}

inline int CheckDirExists(char *dirpath) {
    DIR *dp;
    int retValue = 0;

    dp = opendir(dirpath);
    if (dp) retValue = 1;
    closedir(dp);

    return retValue;
}

inline int CheckFileExists(char *filepath) {
    return access(filepath, R_OK | W_OK);
}

inline int CheckFileOrDir(char *path) {
    struct stat finfo;
    if (lstat(path, &finfo) < 0) return -1;
    return finfo.st_mode;
}

int RecuDirRemove(char *dirpath) {
    DIR *dp;
    struct dirent *dirp;
    char subdirex[MAX_PATH + 1];
    char filename[MAX_PATH + 1];

    if ((dp = opendir(dirpath)) == NULL) return -1;

    while ((dirp = readdir(dp)) != NULL) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            sprintf(filename, "%s/%s", dirpath, dirp->d_name);
            if(unlink(filename)<0) return -1;
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            sprintf(subdirex, "%s/%s", dirpath, dirp->d_name);
            if (RecuDirRemove(subdirex)) {
                closedir(dp);
                return -1;
            }
        }
    }
    closedir(dp);
    if (rmdir(dirpath) < 0) return -1;

    return 0;
}
