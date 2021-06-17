/* 
 * File:   river_entity.h
 * Author: Administrator
 */

#ifndef RIVER_ENTITY_H
#define	RIVER_ENTITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>

#include "riv_common.h"
#include "rivfs.h"

    struct fattb {
        char file_name[MAX_PATH * 3];
        time_t time_stamp; // last_write;

        union {
            __off64_t file_size; /* Size of file, in bytes.  */
            uint64 sibling;
        };
        unsigned char sha1_chks[SHA1_DIGEST_LEN];
    };

    struct ckchks {
        time_t time_stamp; // last_write;
        unsigned char md5_chks[MD5_DIGEST_LEN];
        uint64 sibling;
    };

    int entity_initial(struct riv_live *plive, char *location);
    int entity_final(struct riv_live *plive);
    //
    uint64 init_attrib(FILE *fpattb, char *file_name);
#define attrib_read(pattb, fpattb, ainde) attb_read(pattb, fpattb, ainde)
#define remove_attrib(fpattb, ainde) add_idle_attb(fpattb, ainde)
#define attrib_stamp_update(plive, attb, ainde, stamp) { \
    attb.time_stamp = stamp; \
    attb_write(plive->fpattb, &attb, ainde); \
}
#define attrib_size_update(plive, attb, ainde, size) { \
    attb.file_size = size; \
    attb_write(plive->fpattb, &attb, ainde); \
}
#define attrib_file_update(fpattb, ainde, new_name)    attb_file_modify(fpattb, ainde, new_name)
    //
    uint64 init_cchks(FILE *fpchks);
    uint64 cchks_inde_read(struct ckchks *pchks, FILE *fpchks, uint64 cuks_hind, uint32 cinde);
#define remove_ckchks(fpchks, cuks_hind) add_idle_chks(fpchks, cuks_hind)
    uint64 reset_ckchks_length(FILE *fpchks, uint64 cuks_hind, uint32 old_clen, uint32 new_clen);
    uint64 file_chks_update(FILE *fpchks, char *file_name, uint64 cuks_hind, struct stat64 *pstat);
    uint64 file_chunk_update(FILE *fpchks, char *file_name, uint64 cuks_hind, uint32 cinde, struct stat64 *pstat);
    uint64 chunk_chks_update(FILE *fpchks, char *file_name, uint64 cuks_hind, uint32 cinde, struct stat64 *pstat);
    // test 
    uint32 cchks_inde_numbe(FILE *fpchks, uint64 cuks_hind); // for test
    void attrib_idle_link(FILE *fpattb);
    void cchks_idle_link(FILE *fpchks);
    //
    uint64 repod_attrib(FILE *fpattb, uint64 ainde, char *new_name);
    uint64 repod_ckchks(FILE *fpchks, uint64 cuks_hind);
    //
    int file_sha1_chks(unsigned char *file_chks, FILE *fpchks, uint64 cuks_hind);
    int file_rive_chks(struct riv_chks *pchks, FILE *fpchks, uint64 ind_pos);
    int chunk_rive_chks(struct riv_chks *pchks, FILE *fpchks, uint64 ind_pos, uint32 cinde);

#endif	/* RIVER_ENTITY_H */

