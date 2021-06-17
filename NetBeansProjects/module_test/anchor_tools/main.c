/* 
 * File:   main.c
 * Author: Administrator
 *
 * Created on 2011年7月13日, 上午11:11
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <utime.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

/*
 * 
 */

#ifndef int32
#define int32 int
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

typedef unsigned long DWORD;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

struct fattent {
    unsigned int action_type; /* add mod del list recu */
    char file_name[MAX_PATH * 3];
    size_t file_size;
    time_t last_write; /* When the item was last modified */
    unsigned int reserved; /* filehigh */
};

// #pragma pack(1)

struct idxent {
    uint32 anchor_number;
    off_t data_offset;
    time_t time_stamp;
};
// #pragma pack()

#define ANCHOR_FILE     "./AnchorData.dat"
#define INDEX_FILE      "./AnchorIndex.ndx"
#define LOG_INFO        printf

#define APPEND_PATH(FUPATH, PATH, SUBDIR) \
        sprintf(FUPATH, "%s%s", PATH, SUBDIR + 1)
//off_t file_size_stream(FILE *stream);
#define FILE_SIZE_STREAM(SIZE, STREAM) { \
    if (fseek(STREAM, 0, SEEK_END)) LOG_INFO("fseek errno:%d\n", errno); \
    SIZE = ftell(STREAM); \
}
// off_t file_size(const char *file_path);
#define FILE_SIZE(SIZE, PATH) { \
    struct stat statbuf; \
    if (stat(PATH, &statbuf) < 0) LOG_INFO("stat errno:%d\n", errno); \
    SIZE = statbuf.st_size; \
}
#define FAIL_EXIT(DATFP, RNO) { fclose(DATFP); return RNO; }

uint32 add_anchor(time_t time_stamp) {
    FILE *anchor_idxfp = NULL;
    struct idxent ins_index, last_index;

    memset(&ins_index, '\0', sizeof (struct idxent));
    memset(&last_index, '\0', sizeof (struct idxent));
    anchor_idxfp = fopen(INDEX_FILE, "ab+");
    if (!anchor_idxfp) return 0;

    size_t idx_size;
    FILE_SIZE_STREAM(idx_size, anchor_idxfp)
    if (0 < idx_size) {
        if (fseek(anchor_idxfp, -sizeof (struct idxent), SEEK_END)) FAIL_EXIT(anchor_idxfp, 0)
            if (0 >= fread(&last_index, sizeof (struct idxent), 1, anchor_idxfp)) FAIL_EXIT(anchor_idxfp, 0)
            }
    ins_index.anchor_number = last_index.anchor_number + 1;
    FILE_SIZE(ins_index.data_offset, INDEX_FILE)
    ins_index.time_stamp = time_stamp;

    if (fseek(anchor_idxfp, 0, SEEK_END)) FAIL_EXIT(anchor_idxfp, 0)
        if (0 >= fwrite(&ins_index, sizeof (struct idxent), 1, anchor_idxfp)) FAIL_EXIT(anchor_idxfp, 0)
            LOG_INFO("add new anchor:%d file offset:%d\n", ins_index.anchor_number, ins_index.data_offset);
    fclose(anchor_idxfp);

    return ins_index.anchor_number;
}

#define OPERA_FATT(FATT, OPE) { \
    FATT.action_type = OPE->action_type; \
    strcpy(FATT.file_name, OPE->file_name); \
    FATT.file_size = OPE->file_size; \
    FATT.last_write = OPE->last_write; \
    FATT.reserved = OPE->reserved; \
}

int add_action(struct fattent *fatt) {
    FILE *anchor_datfp = fopen(ANCHOR_FILE, "ab+");
    if (!anchor_datfp) return -1;
    if (fseek(anchor_datfp, 0, SEEK_END)) return -1;

    if (0 >= fwrite(fatt, sizeof (struct fattent), 1, anchor_datfp))
        return -1;
    fclose(anchor_datfp);

    return 0;
}

#define UNKNOWN			0x00000000
////////////////////////////////////////////////////////////////////////////////
// file real status
#define ADD                     0x00010000 // addition
#define MODIFY                  0x00020000 // modify
#define DEL                     0x00040000 // delete
#define EXISTS                  0x00080000 // exists
#define COPY                    0x00100000 // copy
#define MOVE                    0x00200000 // move

#define SECONDS_ONEDAY           86400

int main(int argc, char** argv) {
    struct fattent fatt;
    time_t time_stamp;

    fatt.action_type = ADD;
    strcpy(fatt.file_name, "/afa/eee.txt");
    fatt.file_size = 1024;
    fatt.last_write = time(NULL);  
    ////////////////////////////////////////////////////////// -7 
     add_action(&fatt);
     add_action(&fatt);
     add_action(&fatt);
     add_action(&fatt);
    
     time_stamp = time(NULL) - 7*SECONDS_ONEDAY;
     add_anchor(time_stamp);  
    ////////////////////////////////////////////////////////// -6
     add_action(&fatt);
     add_action(&fatt);
     add_action(&fatt);
     add_action(&fatt);
    
     time_stamp = time(NULL) - 6*SECONDS_ONEDAY;
     add_anchor(time_stamp);   
    ////////////////////////////////////////////////////////// today
     add_action(&fatt);
     add_action(&fatt);
     add_action(&fatt);
     add_action(&fatt);

     time_stamp = time(NULL);
     add_anchor(time(NULL));
    //////////////////////////////////////////////////////////
    
    return (EXIT_SUCCESS);
}

