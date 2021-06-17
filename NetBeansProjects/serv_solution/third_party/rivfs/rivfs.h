/* 
 * File:   rivfs.h
 * Author: Administrator
 */

#ifndef RIVFS_H
#define	RIVFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kyotocabinet/kchashdb.h"
#include "riv_common.h"
//
using namespace kyotocabinet;

//
struct riv_live {
    char location[MAX_PATH * 3];
    TreeDB *rivdb;
    FILE *fpiden;
    FILE *fpchks;
};
//
uint32 riv_initial(struct riv_live *plive, char *location);
uint32 riv_final(struct riv_live *plive);
//
uint32 riv_file_sha1(FILE *fpriv, char *file_name, struct riv_live *plive);
uint32 ren_file_sha1(FILE *fpriv, char *file_name, struct riv_live *plive);

uint32 riv_file_chks(FILE *fpriv, char *file_name, uint32 cinde, struct riv_live *plive);
uint32 ren_file_chks(FILE *fpriv, char *file_name, uint32 cinde, struct riv_live *plive);

uint32 riv_cuk_chks(FILE *fpriv, char *file_name, uint32 cinde, struct riv_live *plive);
uint32 ren_cuk_chks(FILE *fpriv, char *file_name, uint32 cinde, struct riv_live *plive);
#define _RIV_CLOSE_(fptr) if(fptr) fclose(fptr);
//
uint32 river_insert(char *file_name, struct riv_live *plive);
uint32 river_copy(char *new_name, char *exis_name, struct riv_live *plive);
uint32 river_move(char *new_name, char *exis_name, struct riv_live *plive);
uint32 river_remove(char *file_name, struct riv_live *plive);
//
#endif	/* RIVFS_H */

