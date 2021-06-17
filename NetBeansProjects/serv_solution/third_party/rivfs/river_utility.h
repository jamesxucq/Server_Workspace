/* 
 * File:   river_utility.h
 * Author: Administrator
 */

#ifndef RIVER_UTILITY_H
#define	RIVER_UTILITY_H

#include "riv_common.h"
#include "river_entry.h"
    
    //
    uint32 fill_idle_entry(FILE *fpchks, FILE *fpiden);
    //
    uint64 find_idle_iden(FILE *fpiden);
    uint32 add_idle_iden(FILE *fpiden, uint64 ainde);
    uint64 iden_read(struct file_id *pattb, FILE *fpiden, uint64 ainde);
    uint64 iden_write(FILE *fpiden, struct file_id *pattb, uint64 ainde);
    uint64 iden_sibli_modify(FILE *fpiden, uint64 ainde, uint64 sibling);
    uint64 iden_file_modify(FILE *fpiden, uint64 ainde, char *new_name);
    //
    uint64 find_sibli_chks(FILE *fpchks, uint64 cinde);
    uint64 find_idle_chks(FILE *fpchks);
    uint32 add_idle_chks(FILE *fpchks, uint64 chks_hind);
    uint64 chks_read(struct ckchks *pcks, FILE *fpchks, uint64 cinde);
    uint64 chks_write(FILE *fpchks, struct ckchks *pcks, uint64 cinde);
    uint64 chks_sibli_modify(FILE *fpchks, uint64 cinde, uint64 sibling);
    uint64 chks_stamp_modify(FILE *fpchks, time_t stamp, uint64 cinde);
    //
    void chunk_chks(unsigned char *pchks, FILE *fpchks, uint64 file_size, uint64 cinde);

#endif	/* RIVER_UTILITY_H */

