/* 
 * File:   cache_hmap.h
 * Author: Administrator
 */

#ifndef CACHEHMAP_H
#define	CACHEHMAP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>

struct cache_node {
    void *key;
    void *data;
    struct cache_node *next;
};

struct _cachemap_ {
    unsigned int (*gethash)(void *);
    int (*compare)(void *, void *);
    unsigned int hashsize;
    unsigned int count;
    struct cache_node **hmaplist;
};

typedef struct _cachemap_ cachemap;

cachemap *cache_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), unsigned int size);
void cache_del(cachemap *cmap);
void cache_insert(void *key, void *data, cachemap *cmap);
void cache_remove(void *key, cachemap *cmap);
void *cache_value(void *key, cachemap *cmap);
void cache_for_each_do(cachemap *cmap, int (call_back) (void *, void *));
int cache_count(cachemap *cmap);

#ifdef	__cplusplus
}
#endif

#endif	/* CACHEHMAP_H */

