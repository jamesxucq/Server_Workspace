#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "common_macro.h"
#include "string_utility.h"
#include "file_utility.h"
#include "iattb_type.h"
#include "recu_utility.h"

struct recu_head {
    uint64 hand_inde;
    char location[MAX_PATH * 3];
    char subdir[MAX_PATH * 3];
};

int read_head(struct recu_head *repa, FILE *recu_path) {
    if (fseeko64(recu_path, 0, SEEK_SET)) return -1;
    int rlen = fread(repa, sizeof (struct recu_head), 1, recu_path);
    if (0x01 != rlen) return -1;
    return 0;
}

int write_head(FILE *recu_path, struct recu_head *repa) {
    if (fseeko64(recu_path, 0, SEEK_SET)) return -1;
    int wlen = fwrite(repa, sizeof (struct recu_head), 0x01, recu_path);
    if (0x01 != wlen) return -1;
    return 0;
}

int read_path(char *path, FILE *recu_path, uint64 offset) {
    if (fseeko64(recu_path, offset, SEEK_SET)) return -1;
    int rlen = fread(path, MAX_PATH * 3, 0x01, recu_path);
    if (0x01 != rlen) return -1;
    return 0;
}

int write_path(FILE *recu_path, const char *path) {
    if (fseeko64(recu_path, 0, SEEK_END)) return -1;
    int wlen = fwrite(path, MAX_PATH * 3, 0x01, recu_path);
    if (0x01 != wlen) return -1;
    return 0;
}

int read_latt(struct list_attrib *latt, FILE *recu_acd, uint64 offset) {
    if (fseeko64(recu_acd, offset, SEEK_SET)) return -1;
    int rlen = fread(latt, sizeof (struct list_attrib), 1, recu_acd);
    if (0x01 != rlen) return -1;
    return 0;
}

int write_latt(FILE *recu_acd, struct list_attrib *latt) {
    if (fseeko64(recu_acd, 0, SEEK_END)) return -1;
    int wlen = fwrite(latt, sizeof (struct list_attrib), 0x01, recu_acd);
    if (0x01 != wlen) return -1;
    return 0;
}

int write_latt_ext(FILE *recu_acd, char *location, const char *file_name, unsigned int action_type) {
    struct list_attrib latt;
    memset(&latt, 0, sizeof (struct list_attrib));
    //
    latt.action_type = action_type;
    strcpy(latt.file_name, file_name);
    char abso_path[MAX_PATH * 3];
    POOL_PATH(abso_path, location, file_name);
// _LOG_DEBUG("absolu:%s location:%s file_name:%s", abso_path, location, file_name);
    FILE_SIZE_TIME(latt.file_size, latt.last_write, abso_path)
            //
    if (fseeko64(recu_acd, 0, SEEK_END)) return -1;
    int wlen = fwrite(&latt, sizeof (struct list_attrib), 0x01, recu_acd);
    if (0x01 != wlen) return -1;
    //
    return 0;
}

#define RECU_ADDI_ALIST(recu_acd, location, file_name) write_latt_ext(recu_acd, location, file_name, RECURSIV)
#define LIST_ADDI_ALIST(recu_acd, location, file_name) write_latt_ext(recu_acd, location, file_name, LIST)
//
#define RESIVE_SESSION_INTERV        32 // 32s

int reuse_recursive_seion(char *file_name) { // 0:reuse !0:new
    struct stat64 statb;
    if (stat64(file_name, &statb) == -1) {
        //_LOG_WARN("can not get info of %s/n", file1);
        return -1;
    }
    if (RESIVE_SESSION_INTERV < (time(NULL) - statb.st_mtime)) return 1;
    return 0;
}

int creat_recuf_enviro(FILE **recu_path, FILE **recu_acd, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
// fprintf(stderr, "create recu file env.\n");
    struct recu_head repa;
    FILE *path_stre;
    LIST_CACHE(file_name, location, RECURSIVE_FILE_PATH);
// fprintf(stderr, "open file:%s\n", file_name);
    if (reuse_recursive_seion(file_name)) {
// fprintf(stderr, "new file:%s\n", file_name);
        if (!(path_stre = fopen64(file_name, "wb+"))) {
            _LOG_ERROR("not open file:%s", file_name);
            return -1;
        }
// fprintf(stderr, "write head.\n");
        repa.hand_inde = sizeof (struct recu_head);
        strcpy(repa.location, location);
        strcpy(repa.subdir, subdir);
        write_head(path_stre, &repa);
        write_path(path_stre, subdir);
    } else {
        path_stre = fopen64(file_name, "rb+");
    }
    *recu_path = path_stre;
    //
    LIST_CACHE(file_name, location, RECURSIVE_FILE_ENTRY);
    FILE *recu_stre = fopen64(file_name, "ab+");
    if (!recu_stre) {
        _LOG_ERROR("not open file:%s", file_name);
        return -1;
    }
    *recu_acd = recu_stre;
// fprintf(stderr, "new file:%s\n", file_name);
    //
    return 0x00;
}
//
#define  HAVE_DIRENT_DTYPE    0x01
#ifdef HAVE_DIRENT_DTYPE
int hand_recursive_file(FILE *recu_path, FILE *recu_acd, int max_recu) { // 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 
    if (fseeko64(recu_path, 0, SEEK_SET)) return 0x03;
    if (fseeko64(recu_acd, 0, SEEK_END)) return 0x03;
    //
    struct recu_head repa;
    read_head(&repa, recu_path);
//
    DIR *dires;
    struct dirent *dirp;
    char subdir[MAX_PATH * 3], inte_path[MAX_PATH * 3];
    const char *ploca, *psdir;
    int recu_tatol = 0;
    int haval = 0x01;
    //
    POOL_PATH_EXA(inte_path, ploca, repa.location)
    for (; max_recu > recu_tatol; repa.hand_inde += MAX_PATH * 3) {
        if (read_path(subdir, recu_path, repa.hand_inde)) {
            haval = 0x00;
            break;
        }
        POOL_PATH_EXB(ploca, psdir, subdir)
        if (NULL == (dires = opendir(inte_path))) return 0x02;
//
        while (NULL != (dirp = readdir(dires))) {
            if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
                strcpy((char *)psdir, dirp->d_name);
                recu_tatol++;
// _LOG_DEBUG("f inte_path:%s", inte_path);
                RECU_ADDI_ALIST(recu_acd, repa.location, ploca);
            } else if (DT_DIR == dirp->d_type) {
                if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                    continue;
                sprintf((char *)psdir, "%s/", dirp->d_name);
// _LOG_DEBUG("d inte_path:%s", inte_path);
                RECU_ADDI_ALIST(recu_acd, repa.location, ploca);
                write_path(recu_path, ploca);
            }
        }
        closedir(dires);
    }
    //
    write_head(recu_path, &repa);
    return haval;
}
#else
int hand_recursive_file(FILE *recu_path, FILE *recu_acd, int max_recu) { // 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 
    if (fseeko64(recu_path, 0, SEEK_SET)) return 0x03;
    if (fseeko64(recu_acd, 0, SEEK_END)) return 0x03;
    //
    struct recu_head repa;
    read_head(&repa, recu_path);
//
    DIR *dires;
    struct dirent *dirp;
    struct stat64 statb;
    char subdir[MAX_PATH * 3], inte_path[MAX_PATH * 3];
    const char *ploca, *psdir;
    int recu_tatol = 0;
    int haval = 0x01;
    //
    POOL_PATH_EXA(inte_path, ploca, repa.location)
    for (; max_recu > recu_tatol; repa.hand_inde += MAX_PATH * 3) {
        if (read_path(subdir, recu_path, repa.hand_inde)) {
            haval = 0x00;
            break;
        }
        POOL_PATH_EXB(ploca, psdir, subdir)
        if (NULL == (dires = opendir(inte_path))) return 0x02;
//
        while (NULL != (dirp = readdir(dires))) {
            strcpy(psdir, dirp->d_name);
            if (stat64(inte_path, &statb) < 0) {
                closedir(dires);
                return 0x03;
            }
            if (S_ISLNK(statb.st_mode) || S_ISREG(statb.st_mode)) {
                recu_tatol++;
                RECU_ADDI_ALIST(recu_acd, repa.location, ploca);
            } else if (S_ISDIR(statb.st_mode)) {
                if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                    continue;
                strcat(psdir, "/");
                RECU_ADDI_ALIST(recu_acd, repa.location, ploca);
                write_path(recu_path, ploca);
            }
        }
        closedir(dires);
    }
    //
    write_head(recu_path, &repa);
    return haval;
}
#endif

void clear_recuf_enviro(FILE *recu_path, char *location, int clear_type, FILE *recu_acd) {
    char file_name[MAX_PATH * 3];
    fclose(recu_path);
// fprintf(stderr, "clear recu file env.\n");
    if (0x01 != clear_type) {
        LIST_CACHE(file_name, location, RECURSIVE_FILE_PATH);
        unlink(file_name);
// fprintf(stderr, "clear: %s\n", file_name);
    }
    fclose(recu_acd);
}

void clear_recuf_entry(char *location) {
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, RECURSIVE_FILE_ENTRY);
    unlink(file_name);
// fprintf(stderr, "clear: %s\n", file_name);
}

//
int creat_listd_enviro(FILE **recu_acd, char *location) {
    char file_name[MAX_PATH * 3];
// fprintf(stderr, "create list dir env.\n");
    LIST_CACHE(file_name, location, LIST_DIRECTORY_ENTRY);
    FILE *recu_stre = fopen64(file_name, "wb+");
    if (!recu_stre) {
        _LOG_ERROR("not open file:%s", file_name);
        return -1;
    }
    *recu_acd = recu_stre;
// fprintf(stderr, "new file:%s\n", file_name);
    return 0x00;
}

#ifdef HAVE_DIRENT_DTYPE
int hand_list_directory(FILE *recu_acd, char *location, char *subdir) {
    if (fseeko64(recu_acd, 0, SEEK_SET)) return 0x03;
    //
    DIR *dires;
    struct dirent *dirp;
    char inte_path[MAX_PATH * 3];
    const char *ploca, *psdir;
    //
    POOL_PATH_EXA(inte_path, ploca, location)
    POOL_PATH_EXB(ploca, psdir, subdir)
    if (NULL == (dires = opendir(inte_path))) return 0x02;
    //
    while (NULL != (dirp = readdir(dires))) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            strcpy((char *)psdir, dirp->d_name);
            LIST_ADDI_ALIST(recu_acd, location, ploca);
        } else if (DT_DIR == dirp->d_type) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            sprintf((char *)psdir, "/%s/", dirp->d_name);
            LIST_ADDI_ALIST(recu_acd, location, ploca);
        }
    }
    closedir(dires);
    //
    return 0x00;
}
#else
int hand_list_directory(FILE *recu_acd, char *location, char *subdir) {
    if (fseeko64(recu_acd, 0, SEEK_SET)) return 0x03;
    //
    DIR *dires;
    struct dirent *dirp;
    struct stat64 statb;
    char inte_path[MAX_PATH * 3];
    const char *ploca, *psdir;
    //
    POOL_PATH_EXA(inte_path, ploca, location)
    POOL_PATH_EXB(ploca, psdir, subdir)     
    if (NULL == (dires = opendir(inte_path))) return 0x02;
    //
    while (NULL != (dirp = readdir(dires))) {
        strcpy(psdir, dirp->d_name);
        if (stat64(inte_path, &statb) < 0) {
            closedir(dires);
            return 0x03;
        }
        if (S_ISLNK(statb.st_mode) || S_ISREG(statb.st_mode)) {
            LIST_ADDI_ALIST(recu_acd, location, ploca);
        } else if (S_ISDIR(statb.st_mode)) {
            if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                continue;
            strcat(psdir, "/");
            LIST_ADDI_ALIST(recu_acd, location, ploca);
        }
    }
    closedir(dires);
    //
    return 0x00;
}
#endif


void clear_listd_enviro(FILE *recu_acd) {
// fprintf(stderr, "clear list dir env.\n");
    fclose(recu_acd);
}

void clear_listd_entry(char *location) {
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, LIST_DIRECTORY_ENTRY);
    unlink(file_name);
// fprintf(stderr, "clear: %s\n", file_name);
}

int creat_recud_enviro(FILE **recu_path, FILE **recu_acd, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
// fprintf(stderr, "create recu dir env.\n");
    struct recu_head repa;
    FILE *path_stre;
    LIST_CACHE(file_name, location, RECU_DIRECTORY_PATH);
// fprintf(stderr, "open file:%s\n", file_name);
    if (reuse_recursive_seion(file_name)) {
// fprintf(stderr, "new file:%s\n", file_name);
        if (!(path_stre = fopen64(file_name, "wb+"))) {
            _LOG_ERROR("not open file:%s", file_name);
            return -1;
        }
// fprintf(stderr, "write head.\n");
        repa.hand_inde = sizeof (struct recu_head);
        strcpy(repa.location, location);
        strcpy(repa.subdir, subdir);
        write_head(path_stre, &repa);
        write_path(path_stre, subdir);
    } else path_stre = fopen64(file_name, "rb+");
    *recu_path = path_stre;
    //
    LIST_CACHE(file_name, location, RECU_DIRECTORY_ENTRY);
    FILE *recu_stre = fopen64(file_name, "ab+");
    if (!recu_stre) {
        _LOG_ERROR("not open file:%s", file_name);
        return -1;
    }
    *recu_acd = recu_stre;
// fprintf(stderr, "new file:%s\n", file_name);
    //
    return 0x00;
}

#ifdef HAVE_DIRENT_DTYPE
int hand_recu_directory(FILE *recu_path, FILE *recu_acd, int max_recu) {
    if (fseeko64(recu_path, 0, SEEK_SET)) return 0x03;
    if (fseeko64(recu_acd, 0, SEEK_END)) return 0x03;
    //
    struct recu_head repa;
    read_head(&repa, recu_path);
    //
    DIR *dires;
    struct dirent *dirp;
    char subdir[MAX_PATH * 3], inte_path[MAX_PATH * 3];
    const char *ploca, *psdir;
    int recu_tatol = 0;
    int haval = 0x01;
    //
    POOL_PATH_EXA(inte_path, ploca, repa.location)
    for (; max_recu > recu_tatol; repa.hand_inde += MAX_PATH * 3) {
        if (read_path(subdir, recu_path, repa.hand_inde)) {
            haval = 0x00;
            break;
        }
        POOL_PATH_EXB(ploca, psdir, subdir)
        if (NULL == (dires = opendir(inte_path))) return 0x02;
//
        while (NULL != (dirp = readdir(dires))) {
            if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
                recu_tatol++;
            } else if (DT_DIR == dirp->d_type) {
                if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                    continue;
                sprintf((char *)psdir, "%s/", dirp->d_name);
                write_path(recu_path, ploca);
                write_path(recu_acd, ploca);
            }
        }
        closedir(dires);
    }
    //
    write_head(recu_path, &repa);
    return haval;
}
#else
int hand_recu_directory(FILE *recu_path, FILE *recu_acd, int max_recu) {
    if (fseeko64(recu_path, 0, SEEK_SET)) return 0x03;
    if (fseeko64(recu_acd, 0, SEEK_END)) return 0x03;
    //
    struct recu_head repa;
    read_head(&repa, recu_path);
    //
    DIR *dires;
    struct dirent *dirp;
    struct stat64 statb;
    char subdir[MAX_PATH * 3], inte_path[MAX_PATH * 3];
    const char *ploca, *psdir;
    int recu_tatol = 0;
    int haval = 0x01;
    //
    POOL_PATH_EXA(inte_path, ploca, repa.location)
    for (; max_recu > recu_tatol; repa.hand_inde += MAX_PATH * 3) {
        if (read_path(subdir, recu_path, repa.hand_inde)) {
            haval = 0x00;
            break;
        }
        POOL_PATH_EXB(ploca, psdir, subdir)
        if (NULL == (dires = opendir(inte_path))) return 0x02;
//
        while (NULL != (dirp = readdir(dires))) {
            strcpy(psdir, dirp->d_name);
            if (stat64(inte_path, &statb) < 0) {
                closedir(dires);
                return 0x03;
            } 
            if (S_ISLNK(statb.st_mode) || S_ISREG(statb.st_mode)) {
                recu_tatol++;
            } else if (S_ISDIR(statb.st_mode)) {
                if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name)) 
                    continue;
                strcat(psdir, "/");
                write_path(recu_path, ploca);
                write_path(recu_acd, ploca);
            }
        }
        closedir(dires);
    }
    //
    write_head(recu_path, &repa);
    return haval;
}
#endif

void clear_recud_enviro(FILE *recu_path, char *location, int clear_type, FILE *recu_acd) {
    char file_name[MAX_PATH * 3];
    fclose(recu_path);
// fprintf(stderr, "clear recu dir env.\n");
    if (0x01 != clear_type) {
        LIST_CACHE(file_name, location, RECU_DIRECTORY_PATH);
        unlink(file_name);
// fprintf(stderr, "clear: %s\n", file_name);
    }
    fclose(recu_acd);
}

void clear_recud_entry(char *location) {
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, RECU_DIRECTORY_ENTRY);
    unlink(file_name);
// fprintf(stderr, "clear: %s\n", file_name);
}

