#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/times.h>


// #include "common_macro.h"
// #include "string_utility.h"
// #include "file_utility.h"
// #include "fatt_type.h"
#include "recu_utility.h"

struct recu_head {
    uint64 hand_inde;
    char location[MAX_PATH * 3];
    char subdir[MAX_PATH * 3];
};

int read_head(struct recu_head *repa, FILE *recu_path) {
    if (fseeko64(recu_path, 0, SEEK_SET)) return -1;
    int rlen = fread(repa, sizeof (struct recu_head), 0x01, recu_path);
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

int write_path(FILE *recu_path, char *path) {
    if (fseeko64(recu_path, 0, SEEK_END)) return -1;
    int wlen = fwrite(path, MAX_PATH * 3, 0x01, recu_path);
    if (0x01 != wlen) return -1;
    return 0;
}

int read_latt(struct list_attrib *latt, FILE *recu_acd, uint64 offset) {
    if (fseeko64(recu_acd, offset, SEEK_SET)) return -1;
    int rlen = fread(latt, sizeof (struct list_attrib), 0x01, recu_acd);
    if (0x01 != rlen) return -1;
    return 0;
}

int write_latt(FILE *recu_acd, struct list_attrib *latt) {
    if (fseeko64(recu_acd, 0, SEEK_END)) return -1;
    int wlen = fwrite(latt, sizeof (struct list_attrib), 0x01, recu_acd);
    if (0x01 != wlen) return -1;
    return 0;
}

int write_latt_ext(FILE *recu_acd, char *location, char *file_name, unsigned int action_type) {
    struct list_attrib latt;
    memset(&latt, 0, sizeof (struct list_attrib));
    //
    latt.action_type = action_type;
    strcpy(latt.file_name, file_name);
    char absolu_path[MAX_PATH * 3];
    POOL_PATH(absolu_path, location, file_name);
    FILE_SIZE_TIME(latt.file_size, latt.last_write, absolu_path)
            //
    if (fseeko64(recu_acd, 0, SEEK_END)) return -1;
    int wlen = fwrite(&latt, sizeof (struct list_attrib), 0x01, recu_acd);
    if (0x01 != wlen) return -1;
    //
    return 0;
}

#define RECU_ADD_ALIST(recu_acd, location, file_name) write_latt_ext(recu_acd, location, file_name, RECU)
#define LIST_ADD_ALIST(recu_acd, location, file_name) write_latt_ext(recu_acd, location, file_name, LIST)
//

int creat_recuf_enviro(FILE **recu_path, FILE **recu_acd, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
    struct recu_head repa;
    //
    printf("create recu file env.\n");
    LIST_CACHE(file_name, location, RECURSIVE_FILE_PATH);
    printf("open file:%s\n", file_name);
    FILE *path_stre = fopen64(file_name, "rb+");
    if (!path_stre) {
        printf("new file:%s\n", file_name);
        if (!(path_stre = fopen64(file_name, "wb+"))) return -1;
        printf("write head.\n");
        repa.hand_inde = sizeof (struct recu_head);
        strcpy(repa.location, location);
        strcpy(repa.subdir, subdir);
        write_head(path_stre, &repa);
        write_path(path_stre, subdir);
    }
    *recu_path = path_stre;
    //
    LIST_CACHE(file_name, location, RECURSIVE_FILE_ENTRY);
    FILE *recu_stre = fopen64(file_name, "ab+");
    if (!recu_stre) return -1;
    *recu_acd = recu_stre;
    printf("new file:%s\n", file_name);
    //
    return 0x00;
}

int hand_recursive_file(FILE *recu_path, FILE *recu_acd, int max_recu) { // 0 OK; 1 PROCESSING; 2 NOT_FOUND; 3 INTERNAL_SERVER_ERROR; 
    if (fseeko64(recu_path, 0, SEEK_SET)) return 0x03;
    if (fseeko64(recu_acd, 0, SEEK_END)) return 0x03;
    //
    struct recu_head repa;
    read_head(&repa, recu_path);
    //
    printf("hand recu file.\n");
    DIR *dires;
    struct dirent *dirp;
    char subdir[MAX_PATH * 3], subdirex[MAX_PATH * 3], pool_path[MAX_PATH * 3];
    int recu_tally = 0;
    int haval = 0x01;
    //
    for (; max_recu > recu_tally; repa.hand_inde += MAX_PATH * 3) {
        if (read_path(subdir, recu_path, repa.hand_inde)) {
            haval = 0x00;
            break;
        }
        POOL_PATH(pool_path, repa.location, subdir);
        if (NULL == (dires = opendir(pool_path))) {
            printf("opendir\n");
            return 0x02;
        }
        printf("--- hand dir:%s\n", pool_path);
        while (NULL != (dirp = readdir(dires))) {
            if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
                //_LOG_INFO("dirp->d_name:%s", dirp->d_name);
                FULL_NAME_EXT(pool_path, subdir, dirp->d_name);
                // _LOG_DEBUG("POOL_PATH:%s", POOL_PATH);
                recu_tally++;
                RECU_ADD_ALIST(recu_acd, repa.location, pool_path);
                printf("--- add file:%s\n", pool_path);
            } else if (DT_DIR == dirp->d_type) {
                if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                    continue;
                sprintf(subdirex, "%s%s/", subdir, dirp->d_name);
                write_path(recu_path, subdirex);
                printf("--- add dir:%s\n", subdirex);
            }
        }
        closedir(dires);
    }
    //
    write_head(recu_path, &repa);
    printf("recu_tally:%d haval:%d\n", recu_tally, haval);
    return haval;
}

void clear_recuf_enviro(FILE *recu_path, char *location, int clear_type, FILE *recu_acd) {
    char file_name[MAX_PATH * 3];
    fclose(recu_path);
    printf("clear recu file env.\n");
    if (0x01 != clear_type) {
        LIST_CACHE(file_name, location, RECURSIVE_FILE_PATH);
        unlink(file_name);
        printf("clear: %s\n", file_name);
    }
    fclose(recu_acd);
}

void clear_recuf_entry(char *location) {
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, RECURSIVE_FILE_ENTRY);
    unlink(file_name);
    printf("clear: %s\n", file_name);
}

//

int creat_listd_enviro(FILE **recu_acd, char *location) {
    char file_name[MAX_PATH * 3];
    //
    printf("create list dir env.\n");
    LIST_CACHE(file_name, location, LIST_DIRECTORY_ENTRY);
    FILE *recu_stre = fopen64(file_name, "wb+");
    if (!recu_stre) return -1;
    *recu_acd = recu_stre;
    printf("new file:%s\n", file_name);
    //
    return 0x00;
}

int hand_list_directory(FILE *recu_acd, char *location, char *subdir) {
    if (fseeko64(recu_acd, 0, SEEK_SET)) return 0x03;
    //
    DIR *dires;
    struct dirent *dirp;
    char pool_path[MAX_PATH * 3];
    //
    POOL_PATH(pool_path, location, subdir);
    if (NULL == (dires = opendir(pool_path))) {
        printf("opendir\n");
        return 0x02;
    }
    //
    printf("hand list dir.\n");
    printf("--- hand dir:%s\n", pool_path);
    while (NULL != (dirp = readdir(dires))) {
        if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
            printf("%s", dirp->d_name);
            FULL_NAME_EXT(pool_path, subdir, dirp->d_name);
            LIST_ADD_ALIST(recu_acd, location, pool_path);
            printf("--- add file:%s\n", pool_path);
        }
    }
    closedir(dires);
    //
    return 0x00;
}

void clear_listd_enviro(FILE *recu_acd) {
    printf("clear list dir env.\n");
    fclose(recu_acd);
}

void clear_listd_entry(char *location) {
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, LIST_DIRECTORY_ENTRY);
    unlink(file_name);
    printf("clear: %s\n", file_name);
}
//

int creat_recud_enviro(FILE **recu_path, FILE **recu_acd, char *location, char *subdir) {
    char file_name[MAX_PATH * 3];
    //
    printf("create recu dir env.\n");
    struct recu_head repa;
    LIST_CACHE(file_name, location, RECU_DIRECTORY_PATH);
    printf("open file:%s\n", file_name);
    FILE *path_stre = fopen64(file_name, "rb+");
    if (!path_stre) {
        printf("new file:%s\n", file_name);
        if (!(path_stre = fopen64(file_name, "wb+"))) return -1;
        printf("write head.\n");
        repa.hand_inde = sizeof (struct recu_head);
        strcpy(repa.location, location);
        strcpy(repa.subdir, subdir);
        write_head(path_stre, &repa);
        write_path(path_stre, subdir);
    }
    *recu_path = path_stre;
    //
    LIST_CACHE(file_name, location, RECU_DIRECTORY_ENTRY);
    FILE *recu_stre = fopen64(file_name, "ab+");
    if (!recu_stre) return -1;
    *recu_acd = recu_stre;
    printf("new file:%s\n", file_name);
    //
    return 0x00;
}

int hand_recu_directory(FILE *recu_path, FILE *recu_acd, int max_recu) {
    if (fseeko64(recu_path, 0, SEEK_SET)) return 0x03;
    if (fseeko64(recu_acd, 0, SEEK_END)) return 0x03;
    //
    struct recu_head repa;
    read_head(&repa, recu_path);
    //
    DIR *dires;
    struct dirent *dirp;
    char subdir[MAX_PATH * 3], subdirex[MAX_PATH * 3], pool_path[MAX_PATH * 3];
    int recu_tally = 0;
    int haval = 0x01;
    //
    printf("hand recu dir.\n");
    for (; max_recu > recu_tally; repa.hand_inde += MAX_PATH * 3) {
        if (read_path(subdir, recu_path, repa.hand_inde)) {
            haval = 0x00;
            break;
        }
        POOL_PATH(pool_path, repa.location, subdir);
        if (NULL == (dires = opendir(pool_path))) {
            printf("opendir\n");
            return 0x02;
        }
        printf("--- hand dir:%s\n", pool_path);
        while (NULL != (dirp = readdir(dires))) {
            if (DT_UNKNOWN == dirp->d_type || DT_REG == dirp->d_type) {
                //_LOG_INFO("dirp->d_name:%s", dirp->d_name);
                recu_tally++;
            } else if (DT_DIR == dirp->d_type) {
                if (!strcmp(".", dirp->d_name) || !strcmp("..", dirp->d_name))
                    continue;
                sprintf(subdirex, "%s%s/", subdir, dirp->d_name);
                write_path(recu_path, subdirex);
                write_path(recu_acd, subdirex);
                printf("--- add dir:%s\n", subdirex);
            }
        }
        closedir(dires);
    }
    //
    write_head(recu_path, &repa);
    printf("recu_tally:%d haval:%d\n", recu_tally, haval);
    return haval;
}

void clear_recud_enviro(FILE *recu_path, char *location, int clear_type, FILE *recu_acd) {
    char file_name[MAX_PATH * 3];
    fclose(recu_path);
    printf("clear recu dir env.\n");
    if (0x01 != clear_type) {
        LIST_CACHE(file_name, location, RECU_DIRECTORY_PATH);
        unlink(file_name);
        printf("clear: %s\n", file_name);
    }
    fclose(recu_acd);
}

void clear_recud_entry(char *location) {
    char file_name[MAX_PATH * 3];
    LIST_CACHE(file_name, location, RECU_DIRECTORY_ENTRY);
    unlink(file_name);
    printf("clear: %s\n", file_name);
}








