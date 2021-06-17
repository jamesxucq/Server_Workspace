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

using namespace kyotocabinet;

struct riv_live {
    char location[MAX_PATH * 3];
    TreeDB rivdb;
    FILE *fpattb;
    FILE *fpchks;
};

int riv_initial(struct riv_live *plive, char *location);
int riv_final(struct riv_live *plive);

int riv_file_sha1(unsigned char *pchks, char *file_name, struct riv_live *plive);
int riv_file_chks(struct riv_chks *pchks, char *file_name, uint32 cinde, struct riv_live *plive);
int riv_cuk_chks(struct riv_chks *pchks, char *file_name, uint32 cinde, struct riv_live *plive);
int river_insert(char *file_name, struct riv_live *plive);
int river_copy(char *new_name, char *exis_name, struct riv_live *plive);
int river_move(char *new_name, char *exis_name, struct riv_live *plive);
int river_remove(char *file_name, struct riv_live *plive);

#endif	/* RIVFS_H */

