#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_map.h"

#define SUM2HEX(sum, hmap) ((sum) % (hmap->hashsize))

inline struct map_nod *node_new(unsigned int key, void *data) {
    struct map_nod *node = malloc(sizeof (struct map_nod));
    if(!node) return NULL;
    node->key = key;
    node->data = data;
    node->next = NULL;
    return node;
}

void mlist_append(struct map_nod **root, unsigned int key, void *data) {
    struct map_nod *new, *posi;
    //
    new = node_new(key, data);
    if (NULL == *root) {
        *root = new;
    } else {
        for (posi = *root; NULL != posi->next; posi = posi->next);
        posi->next = new;
    }
}

int mlist_update(struct map_nod *root, unsigned int key, void *data, void (*freefunc)(void *)) {
    struct map_nod *posi;
    for (posi = root; NULL != posi; posi = posi->next) {
        if (key == posi->key) {
            freefunc(posi->data);
            posi->data = data;
            return 0;
        }
    }
    return -1;
}

inline struct map_nod *node_del(struct map_nod *nod, void (*freefunc)(void *)) {
    struct map_nod *next = nod->next;
    freefunc(nod->data);
    free(nod);
    return (next);
}

int mlist_remove(struct map_nod **root, unsigned int key, void (*freefunc)(void *)) {
    struct map_nod *posi, *prev;
    //
    if (NULL == *root) return -1;
    if ((*root)->key == key) {
        *root = node_del(*root, freefunc);
        return 0;
    }
    //
    prev = *root;
    for (posi = prev->next; NULL != posi; posi = posi->next) {
        if (posi->key == key) {
            prev->next = node_del(posi, freefunc);
            return 0;
        }
        prev = posi;
    }
    return -1;
}

hashmap *hashm_create(void (*freefunc)(void *), unsigned int size) {
    int length = sizeof (struct map_nod *) * size;
    //
    hashmap *hmap = malloc(sizeof (hashmap));
    if(!hmap)return NULL;
    memset(hmap, '\0', sizeof (hashmap));
    hmap->freefunc = freefunc;
    hmap->hashsize = size;
    hmap->count = 0;
    //
    hmap->hmaplist = malloc(length);
    if (NULL == hmap->hmaplist) {
        free(hmap);
        return NULL;
    }
    memset(hmap->hmaplist, '\0', length);
    //
    return hmap;
}

void hashm_del(hashmap *hmap) {
    unsigned int inde;
    struct map_nod *posi;
    //
    if (NULL == hmap) return;
    for (inde = 0; inde < hmap->hashsize; inde++)
        for (posi = hmap->hmaplist[inde]; NULL != posi; posi = node_del(posi, hmap->freefunc));
    //
    free(hmap->hmaplist);
    free(hmap);
}

void hashm_clear(hashmap *hmap) {
    unsigned int inde;
    struct map_nod *posi;
    //
    if (NULL == hmap) return;
    for (inde = 0; inde < hmap->hashsize; inde++)
        for (posi = hmap->hmaplist[inde]; NULL != posi; posi = node_del(posi, hmap->freefunc));
    //
    int length = sizeof (struct map_nod *) * hmap->hashsize;
    memset(hmap->hmaplist, '\0', length);
    hmap->count = 0;
}

void hashm_insert(unsigned int key, void *data, hashmap *hmap) {
    unsigned int inde = SUM2HEX(key, hmap);
    struct map_nod *root = hmap->hmaplist[inde];
    //
    if (0 != mlist_update(root, key, data, hmap->freefunc)) { //(1)
        mlist_append(&(hmap->hmaplist[inde]), key, data);
        hmap->count++;
    }
}

void hashm_remove(unsigned int key, hashmap *hmap) {
    unsigned int inde = SUM2HEX(key, hmap);
    if (0 == mlist_remove(&(hmap->hmaplist[inde]), key, hmap->freefunc)) {
        hmap->count--;
    }
}

void *hashm_value(unsigned int key, hashmap *hmap) {
    struct map_nod *posi;
    unsigned int inde = SUM2HEX(key, hmap);
    for (posi = hmap->hmaplist[inde]; NULL != posi; posi = posi->next) {
        if (key == posi->key) {
            return (posi->data);
        }
    }
    return NULL;
}

void hashm_for_each_do(hashmap *hmap, int(*call_back) (unsigned int, void *)) {
    unsigned int inde = 0;
    struct map_nod *posi;
    for (inde = 0; inde < hmap->hashsize; inde++) {
        for (posi = hmap->hmaplist[inde]; NULL != posi; posi = posi->next) {
            call_back(posi->key, posi->data);
        }
    }
}

inline int hashm_count(hashmap *hmap) {
    return hmap->count;
}
