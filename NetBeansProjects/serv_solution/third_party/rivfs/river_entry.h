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

//
#define INVA_TIME_VALU -1
//

struct file_id {
    union {
        uint32 chklen; /* chunk length */
        uint64 sibling;
    };
    char file_name[MAX_PATH * 3];
    time_t time_stamp; // last_write;
    unsigned char sha1_chks[SHA1_DIGEST_LEN];
};

struct ckchks {
    uint64 sibling;
    time_t time_stamp; // last_write;
    unsigned char md5_chks[MD5_DIGEST_LEN];
};

int entry_initial(struct riv_live *plive);
int entry_final(struct riv_live *plive);
//
uint64 init_identi(FILE *fpiden, char *file_name);
#define identi_read(pattb, fpiden, ainde) iden_read(pattb, fpiden, ainde)
#define remove_identi(fpiden, ainde) add_idle_iden(fpiden, ainde)
#define identi_stamp_update(plive, ident, ainde, stamp) { \
    ident.time_stamp = stamp; \
    iden_write(plive->fpiden, &ident, ainde); \
}
#define identi_clen_update(plive, ident, ainde, clen) { \
    ident.chklen = clen; \
    iden_write(plive->fpiden, &ident, ainde); \
}
#define identi_file_update(fpiden, ainde, new_name)    iden_file_modify(fpiden, ainde, new_name)
//
uint64 init_cchks(FILE *fpchks);
uint64 cchks_inde_read(struct ckchks *pchks, FILE *fpchks, uint64 chks_hind, uint32 cinde);
#define remove_ckchks(fpchks, chks_hind) add_idle_chks(fpchks, chks_hind)
uint64 reset_ckchks_length(FILE *fpchks, uint64 chks_hind, uint32 old_clen, uint32 new_clen);
uint32 file_chks_update(FILE *fpchks, char *file_name, uint64 chks_hind, struct stat64 *pstat);
uint32 file_chks_verify(FILE *fpchks, char *file_name, uint64 chks_hind, struct stat64 *pstat);
// 0:pass 0x01:not found 0x02:retry
uint32 file_chunk_update(FILE *fpchks, uint64 *ind_pos, char *file_name, uint64 chks_hind, uint32 cinde, struct stat64 *pstat);
uint32 file_chunk_verify(FILE *fpchks, uint64 *ind_pos, char *file_name, uint64 chks_hind, uint32 cinde, struct stat64 *pstat);
uint64 chunk_chks_update(FILE *fpchks, char *file_name, uint64 chks_hind, uint32 cinde, struct stat64 *pstat);
uint64 chunk_chks_verify(FILE *fpchks, char *file_name, uint64 chks_hind, uint32 cinde, struct stat64 *pstat);
// test 
uint32 cchks_inde_numbe(FILE *fpchks, uint64 chks_hind); // for test
void identi_idle_link(FILE *fpiden);
void cchks_idle_link(FILE *fpchks);
//
uint64 repod_identi(FILE *fpiden, uint64 ainde, char *new_name);
uint64 repod_ckchks(FILE *fpchks, uint64 chks_hind);
//
uint32 file_sha1_chks(unsigned char *file_chks, FILE *fpchks, uint64 chks_hind);
uint32 file_rive_sha1(FILE *fpriv, unsigned char *file_chks);
uint32 file_rive_chks(FILE *fpriv, FILE *fpchks, uint64 ind_pos, uint32 cinde);
uint32 chunk_rive_chks(FILE *fpriv, FILE *fpchks, uint64 ind_pos, uint32 cinde);

#endif	/* RIVER_ENTITY_H */

