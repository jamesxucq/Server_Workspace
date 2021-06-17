/* 
 * File:   river_cache.h
 * Author: Administrator
 */

#ifndef RIVER_CACHE_H
#define	RIVER_CACHE_H

#include "kyotocabinet/kchashdb.h"
#include "rivfs.h"
#include "riv_common.h"

int cache_initial(struct riv_live *plive, char *location);
int cache_final(struct riv_live *plive);

struct cache_index {
    uint64 ainde;
    uint64 cuks_hind;
};

#define insert_cache(rivdb, key, value) \
    rivdb.set(key, strlen(key), (char *)&value, sizeof (struct cache_index))

#define insert_cache_ext(rivdb, key, pval) \
    rivdb.set(key, strlen(key), (char *)pval, sizeof (struct cache_index))

#define cache_remove(rivdb, key) rivdb.remove(key, strlen(key))

#define cache_find(rval, rivdb, key) \
    rivdb.get(key, strlen(key), (char *)&rval, sizeof (struct cache_index))

#define cache_update(rivdb, key, value) \
    rivdb.set(key, strlen(key), (char *)&value, sizeof (struct cache_index))

#endif	/* RIVER_CACHE_H */

