#ifndef _HASH_MAP_H
#define _HASH_MAP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>

struct map_nod {
    unsigned int key;
    void *data;
    struct map_nod *next;
};

struct _hashmap_ {
    void (*freefunc)(void *);
    unsigned int hashsize;
    unsigned int count;
    struct map_nod **hmaplist;
};

typedef struct _hashmap_ hashmap;

hashmap *hashm_create(void (*freefunc)(void *), unsigned int size);
void hashm_del(hashmap *hmap);
void hashm_clear(hashmap *hmap);
void hashm_insert(unsigned int key, void *data, hashmap *hmap);
void hashm_remove(unsigned int key, hashmap *hmap);
void *hashm_value(unsigned int key, hashmap *hmap);
void hashm_for_each_do(hashmap *hmap, int (*call_back) (unsigned int, void *));
int hashm_count(hashmap *hmap);

#ifdef	__cplusplus
}
#endif

#endif

