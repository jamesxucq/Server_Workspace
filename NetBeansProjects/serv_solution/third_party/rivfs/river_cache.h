/* 
 * File:   river_cache.h
 * Author: Administrator
 */

#ifndef RIVER_CACHE_H
#define	RIVER_CACHE_H

#include "kyotocabinet/kchashdb.h"
#include "rivfs.h"
#include "riv_common.h"

uint32 cache_initial(struct riv_live *plive);
uint32 cache_final(struct riv_live *plive);

struct cache_inde {
    uint64 ainde;
    uint64 chks_hind;
};

#define insert_cache(rivdb, key, value) \
    rivdb->set(key, strlen(key), (char *)&value, sizeof (struct cache_inde))

#define insert_cache_ext(rivdb, key, pval) \
    rivdb->set(key, strlen(key), (char *)pval, sizeof (struct cache_inde))

#define cache_remove(rivdb, key) rivdb->remove(key, strlen(key))

#define cache_find(cvalu, rivdb, key) \
    rivdb->get(key, strlen(key), (char *)&cvalu, sizeof (struct cache_inde))

#define cache_update(rivdb, key, value) \
    rivdb->set(key, strlen(key), (char *)&value, sizeof (struct cache_inde))

#endif	/* RIVER_CACHE_H */

