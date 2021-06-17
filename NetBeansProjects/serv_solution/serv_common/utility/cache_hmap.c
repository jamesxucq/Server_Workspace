#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache_hmap.h"

#define hashinde(key, cmap) ((cmap->gethash)(key) % (cmap->hashsize))

inline struct cache_node *cnod_new(void *key, void *data) {
    struct cache_node *cnod = malloc(sizeof (struct cache_node));
    if(!cnod) return NULL;
    cnod->key = key;
    cnod->data = data;
    cnod->next = NULL;
    return cnod;
}

void clist_append(struct cache_node **root, void *key, void *data) {
    struct cache_node *new, *posi;
    //
    new = cnod_new(key, data);
    if (NULL == *root) {
        *root = new;
    } else {
        for (posi = *root; NULL != posi->next; posi = posi->next);
        posi->next = new;
    }
}

int clist_update(struct cache_node *root, void *key, void *data, int (*compare)(void *, void *)) {
    struct cache_node *posi;
    for (posi = root; NULL != posi; posi = posi->next) {
        if (compare(key, posi->key)) {
            posi->data = data;
            return 0;
        }
    }
    return -1;
}

inline struct cache_node *cnod_del(struct cache_node *nod) {
    struct cache_node *next = nod->next;
    free(nod);
    return (next);
}

int clist_remove(struct cache_node **root, void *key, int (*compare)(void *, void *)) {
    struct cache_node *posi, *prev;
    //
    if (NULL == *root) return -1;
    if (compare((*root)->key, key)) {
        *root = cnod_del(*root);
        return 0;
    }
    //
    prev = *root;
    for (posi = prev->next; NULL != posi; posi = posi->next) {
        if (compare(posi->key, key)) {
            prev->next = cnod_del(posi);
            return 0;
        }
        prev = posi;
    }
    return -1;
}

cachemap *cache_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), unsigned int size) {
    int length = sizeof (struct cache_node *) * size;
    //
    cachemap *cmap = (cachemap *) malloc(sizeof (cachemap));
    if(!cmap) return NULL;
    memset(cmap, '\0', sizeof (cachemap));
    cmap->gethash = keyfunc;
    cmap->compare = compare;
    cmap->hashsize = size;
    cmap->count = 0;
    //
    cmap->hmaplist = malloc(length);
    if (NULL == cmap->hmaplist) {
        free(cmap);
        return NULL;
    }
    memset(cmap->hmaplist, '\0', length);
    //
    return cmap;
}

void cache_del(cachemap *cmap) {
    unsigned int inde;
    struct cache_node *posi;
    //
    if (NULL == cmap) return;
    for (inde = 0; inde < cmap->hashsize; inde++)
        for (posi = cmap->hmaplist[inde]; NULL != posi; posi = cnod_del(posi));
    //
    free(cmap->hmaplist);
    free(cmap);
}

void cache_insert(void *key, void *data, cachemap *cmap) {
    unsigned int inde = hashinde(key, cmap);
    struct cache_node *root = cmap->hmaplist[inde];
    //
    if (0 != clist_update(root, key, data, cmap->compare)) { //(1)
        clist_append(&(cmap->hmaplist[inde]), key, data);
        cmap->count++;
    }
}

void cache_remove(void *key, cachemap *cmap) {
    unsigned int inde = hashinde(key, cmap);
    if (0 == clist_remove(&(cmap->hmaplist[inde]), key, cmap->compare)) {
        cmap->count--;
    }
}

void *cache_value(void *key, cachemap *cmap) {
    struct cache_node *posi;
    unsigned int inde = hashinde(key, cmap);
    for (posi = cmap->hmaplist[inde]; NULL != posi; posi = posi->next) {
        if (cmap->compare(key, posi->key)) {
            return (posi->data);
        }
    }
    return NULL;
}

void cache_for_each_do(cachemap *cmap, int (call_back) (void *, void *)) {
    unsigned int inde = 0;
    struct cache_node *posi;
    for (inde = 0; inde < cmap->hashsize; inde++) {
        for (posi = cmap->hmaplist[inde]; NULL != posi; posi = posi->next) {
            call_back(posi->key, posi->data);
        }
    }
}

inline int cache_count(cachemap *cmap) {
    return cmap->count;
}

