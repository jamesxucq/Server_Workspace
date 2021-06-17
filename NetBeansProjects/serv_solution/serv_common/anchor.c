/*
 * Anch.c
 *
 *  Created on: 2010-3-10
 *      Author: Divad
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "common_macro.h"
#include "logger.h"
#include "string_utility.h"
#include "file_utility.h"
#include "anchor.h"
#include "ope_list.h"

//
anchor_files _anchor_files_;

//
inline int set_anchor_path(char *anch_idx, char *anch_acd) {
    if (!anch_idx || !anch_acd) return -1;
    strcpy(_anchor_files_.anch_idx, anch_idx);
    strcpy(_anchor_files_.anch_acd, anch_acd);
    return 0;
}

#define FAIL_EXIT(DATA, RENO) { \
    _LOG_ERROR("anchor file error!"); \
    fclose(DATA); return RENO; }

uint32 load_last_anchor(char *location) {
    FILE *anchor_idxfp = NULL;
    char anch_idx[MAX_PATH * 3];
    struct idxent last_inde;
    //
    memset(&last_inde, '\0', sizeof (struct idxent));
    APPEND_PATH(anch_idx, location, _anchor_files_.anch_idx);
    anchor_idxfp = fopen64(anch_idx, "ab+");
    if (!anchor_idxfp) {
        _LOG_ERROR("open file error! %s", anch_idx);
        return 0;
    }
    //
    uint64 idx_size;
    FILE_SIZE_STREAM(idx_size, anchor_idxfp)
    if (0 < idx_size) {
        if (fseeko64(anchor_idxfp, -(__off64_t)sizeof (struct idxent), SEEK_END)) FAIL_EXIT(anchor_idxfp, 0)
            if (0 >= fread(&last_inde, sizeof (struct idxent), 1, anchor_idxfp)) FAIL_EXIT(anchor_idxfp, 0)
    }
    fclose(anchor_idxfp);
    return last_inde.anchor_number;
}

uint32 addi_anchor(char *location) {
    FILE *anchor_idxfp = NULL;
    char idxdat[MAX_PATH * 3];
    struct idxent ins_inde, last_inde;
    //
    memset(&ins_inde, '\0', sizeof (struct idxent));
    memset(&last_inde, '\0', sizeof (struct idxent));
    APPEND_PATH(idxdat, location, _anchor_files_.anch_idx);
    anchor_idxfp = fopen64(idxdat, "ab+");
    if (!anchor_idxfp) {
        _LOG_ERROR("open file error! %s", idxdat);
        return 0;
    }
    //
    uint64 idx_size;
    FILE_SIZE_STREAM(idx_size, anchor_idxfp)
    if (0 < idx_size) {
        if (fseeko64(anchor_idxfp, -(__off64_t)sizeof (struct idxent), SEEK_END)) FAIL_EXIT(anchor_idxfp, 0)
            if (0 >= fread(&last_inde, sizeof (struct idxent), 0x01, anchor_idxfp)) FAIL_EXIT(anchor_idxfp, 0)
    }
    ins_inde.anchor_number = last_inde.anchor_number + 0x01;
    APPEND_PATH(idxdat, location, _anchor_files_.anch_acd);
    FILE_SIZE(ins_inde.data_oset, idxdat)
    ins_inde.time_stamp = time(NULL);
    //
    if (fseeko64(anchor_idxfp, 0, SEEK_END)) FAIL_EXIT(anchor_idxfp, 0) {
        if (0 >= fwrite(&ins_inde, sizeof (struct idxent), 1, anchor_idxfp)) FAIL_EXIT(anchor_idxfp, 0)
            // _LOG_DEBUG("add new anchor:%d file offset:%d", ins_inde.anchor_number, ins_inde.data_oset); // disable by james 20130409
    }
    fclose(anchor_idxfp);
    return ins_inde.anchor_number;
}

#define OPERA_FSDO(FSDO, AOPE) { \
    FSDO.action_type = AOPE->action_type; \
    strcpy(FSDO.file_name, AOPE->file_name); \
    FSDO.reserved = AOPE->reserved; \
}

int flush_anch_list(char *location, struct ope_list *action_list) {
    struct fatent faen;
    struct ope_list *olist;
    struct openod *opesdo;
    // if NULL == action_list ,means not change
    char anch_acd[MAX_PATH * 3];
    APPEND_PATH(anch_acd, location, _anchor_files_.anch_acd);
    FILE *anchor_datfp = fopen64(anch_acd, "ab+");
    if (!anchor_datfp) {
        _LOG_ERROR("open file error! %s", anch_acd);
        return -1;
    }
    if (fseeko64(anchor_datfp, 0, SEEK_END)) FAIL_EXIT(anchor_datfp, -1)
    // #define ALIST_FOREACH(head,el) for(el=head;el;el=el->next)
    OLIST_FOREACH(action_list, olist) {
        opesdo = &olist->opesdo;
        OPERA_FSDO(faen, opesdo)
        if (0 >= fwrite(&faen, sizeof (struct fatent), 1, anchor_datfp))
            FAIL_EXIT(anchor_datfp, -1)
    }
    //
    fclose(anchor_datfp);
    return 0;
}

int search_anchor(struct idxent *anch_idx, char *idxname, uint32 anchor) {
    struct idxent *ndxent;
    struct idxent *mid_toke, *min_toke, *max_toke;
    int anchor_found = SEARCH_NOFOND;
    //
    int anchor_idxfd = open64(idxname, O_RDWR | O_CREAT, 0777);
    if (!anchor_idxfd) {
        _LOG_ERROR("open file error! %s", idxname);
        return SEARCH_NOFOND;
    }
    uint64 file_size;
    FILE_SIZE_DESCRIP(file_size, anchor_idxfd)
    if (0x00 == file_size) {
        memset(anch_idx, '\0', sizeof (struct idxent));
        close(anchor_idxfd);
_LOG_DEBUG("file is zero, anchor:%u", anchor);
        if (0x00 == anchor) return SEARCH_FOUND;
        else return SEARCH_NOFOND;
    }
    //
    ndxent = (struct idxent *) mmap64(0, file_size, PROT_READ, MAP_SHARED, anchor_idxfd, 0);
    if (ndxent == MAP_FAILED) {
        close(anchor_idxfd);
_LOG_DEBUG("ndxent is failed, anchor:%u", anchor);
        return SEARCH_FAILED;
    }
    //
    int inde_numbe = file_size / sizeof (struct idxent);
    max_toke = ndxent + inde_numbe - 1;
    min_toke = ndxent;
    mid_toke = min_toke + (inde_numbe >> 1);
    for (;;) {
        if (anchor > mid_toke->anchor_number) {
            min_toke = mid_toke;
            inde_numbe = ((max_toke - min_toke) / sizeof (struct idxent)) >> 1;
            mid_toke = min_toke + (inde_numbe ? inde_numbe : 1);
        } else if (anchor < mid_toke->anchor_number) {
            max_toke = mid_toke;
            inde_numbe = ((max_toke - min_toke) / sizeof (struct idxent)) >> 1;
            mid_toke = min_toke + inde_numbe;
        } else if (anchor == mid_toke->anchor_number) {
            memcpy(anch_idx, mid_toke, sizeof (struct idxent));
            anchor_found = SEARCH_FOUND;
            break;
        }
        if (min_toke == max_toke) {
            anchor_found = SEARCH_NOFOND;
            break;
        }
    }
    munmap((void*) ndxent, file_size);
    close(anchor_idxfd);
    //
_LOG_DEBUG("search anchor, anchor:%u anchor_found:%d", anchor, anchor_found);
    return anchor_found;
}

int get_alist_by_idxent(fsdo_lsth idx_lsth, char *anch_acd, struct idxent *ndxent) {
    struct fatent faen;
    //
    FILE *anchor_datfp = fopen64(anch_acd, "ab+");
    if (!anchor_datfp) {
        _LOG_ERROR("open file error! %s", anch_acd);
        return -1;
    }
    if (fseeko64(anchor_datfp, ndxent->data_oset, SEEK_SET)) FAIL_EXIT(anchor_datfp, -1)
    memset(&faen, '\0', sizeof (struct fatent));
    while (!feof(anchor_datfp)) {
        if (0x01 == fread(&faen, sizeof (struct fatent), 1, anchor_datfp)) { //do something
// _LOG_DEBUG("faen action_type:%x file_name:%s", faen.action_type, faen.file_name);
            if (!append_alist_entry(idx_lsth, &faen)) {
                delete_alist(*idx_lsth);
                FAIL_EXIT(anchor_datfp, -1)
            }
        }
    }
    //
    fclose(anchor_datfp);
    return 0;
}

