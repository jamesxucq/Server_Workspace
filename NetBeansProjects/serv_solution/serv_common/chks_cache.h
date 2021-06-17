/* 
 * File:   chks_cache.h
 * Author: Administrator
 */

#ifndef CHKS_CACHE_H
#define	CHKS_CACHE_H

// #ifdef	__cplusplus
// extern "C" {
// #endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"    
#include "hash_table_ext.h"    
#include "third_party.h"    

struct riv_live* new_riv_live(char *location);
void destory_riv_live(struct riv_live *plive);
void del_riv_live(void *plive);

extern hashmap *_rivlive_hm_;

// #ifdef	__cplusplus
// }
// #endif

#endif	/* CHKS_CACHE_H */

