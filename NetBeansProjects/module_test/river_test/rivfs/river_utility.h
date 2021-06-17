/* 
 * File:   river_utility.h
 * Author: Administrator
 */

#ifndef RIVER_UTILITY_H
#define	RIVER_UTILITY_H

#include "riv_common.h"
#include "river_entity.h"
    
    //
    int fill_idle_entity(FILE *fpchks, FILE *fpattb);
    //
    uint64 find_idle_attb(FILE *fpattb);
    int add_idle_attb(FILE *fpattb, uint64 ainde);
    uint64 attb_read(struct fattb *pattb, FILE *fpattb, uint64 ainde);
    uint64 attb_write(FILE *fpattb, struct fattb *pattb, uint64 ainde);
    uint64 attb_sibli_modify(FILE *fpattb, uint64 ainde, uint64 sibling);
    uint64 attb_file_modify(FILE *fpattb, uint64 ainde, char *new_name);
    //
    uint64 find_sibli_chks(FILE *fpchks, uint64 cinde);
    uint64 find_idle_chks(FILE *fpchks);
    uint64 chks_sibli_modify(FILE *fpchks, uint64 cinde, uint64 sibling);
    int add_idle_chks(FILE *fpchks, uint64 cuks_hind);
    uint64 chks_read(struct ckchks *pcks, FILE *fpchks, uint64 cinde);
    uint64 chks_write(FILE *fpchks, struct ckchks *pcks, uint64 cinde);
    int chks_md5_modify(FILE *fpchks, unsigned char *md5_chks, uint64 cinde);
    //
    void chunk_chks(unsigned char *pchks, FILE *fpchks, uint64 file_size, uint64 cinde);

#endif	/* RIVER_UTILITY_H */

