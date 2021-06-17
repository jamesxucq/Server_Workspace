#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

#define hashinde(key, htab) ((htab->gethash)(key) % (htab->hashsize))
#define SUM2HEX(sum, htab) ((sum) % (htab->hashsize))

inline struct tab_nod *tnod_new(void *key, void *data) {
    struct tab_nod *node = malloc(sizeof (struct tab_nod));
    if(!node) return NULL;
    node->key = key;
    node->data = data;
    node->next = NULL;
    return node;
}

void hlist_append(struct tab_nod **root, void *key, void *data) {
    struct tab_nod *new, *posi;
    //
    new = tnod_new(key, data);
    if (NULL == *root) {
        *root = new;
    } else {
        for (posi = *root; NULL != posi->next; posi = posi->next);
        posi->next = new;
    }
}

int hlist_update(struct tab_nod *root, void *key, void *data, int (*compare)(void *, void *)) {
    struct tab_nod *posi;
    for (posi = root; NULL != posi; posi = posi->next) {
        if (compare(key, posi->key)) {
            posi->data = data;
            return 0;
        }
    }
    return -1;
}

inline struct tab_nod *tnod_del(struct tab_nod *nod) {
    struct tab_nod *next = nod->next;
    free(nod);
    return (next);
}

int hlist_remove(struct tab_nod **root, void *key, int (*compare)(void *, void *)) {
    struct tab_nod *posi, *prev;
    //
    if (NULL == *root) return -1;
    if (compare((*root)->key, key)) {
        *root = tnod_del(*root);
        return 0;
    }
    //
    prev = *root;
    for (posi = prev->next; NULL != posi; posi = posi->next) {
        if (compare(posi->key, key)) {
            prev->next = tnod_del(posi);
            return 0;
        }
        prev = posi;
    }
    return -1;
}

hashtable *hash_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), unsigned int size) {
    int length = sizeof (struct tab_nod *) * size;
    //
    hashtable *htab = malloc(sizeof (hashtable));
    if(!htab) return NULL;
    memset(htab, '\0', sizeof (hashtable));
    htab->gethash = keyfunc;
    htab->compare = compare;
    htab->hashsize = size;
    htab->count = 0;
//
    htab->hashlist = malloc(length);
    if (NULL == htab->hashlist) {
        free(htab);
        return NULL;
    }
    memset(htab->hashlist, '\0', length);
    //
    return htab;
}

void hash_del(hashtable *htab) {
    unsigned int inde;
    struct tab_nod *posi;
    //
    if (NULL == htab) return;
    for (inde = 0; inde < htab->hashsize; inde++)
        for (posi = htab->hashlist[inde]; NULL != posi; posi = tnod_del(posi));
    //
    free(htab->hashlist);
    free(htab);
}

void hash_insert(void *key, void *data, hashtable *htab) {
    unsigned int inde = hashinde(key, htab);
    struct tab_nod *root = htab->hashlist[inde];
    //
    if (0 != hlist_update(root, key, data, htab->compare)) { //(1)
        hlist_append(&(htab->hashlist[inde]), key, data);
        htab->count++;
    }
}

void hash_insert_adler32(void *key, void *data, unsigned int sum, hashtable *htab) {
    unsigned int inde = SUM2HEX(sum, htab);
    struct tab_nod *root = htab->hashlist[inde];
    //
    if (0 != hlist_update(root, key, data, htab->compare)) { //(1)
        hlist_append(&(htab->hashlist[inde]), key, data);
        htab->count++;
    }
}

void hash_remove(void *key, hashtable *htab) {
    unsigned int inde = hashinde(key, htab);
    if (0 == hlist_remove(&(htab->hashlist[inde]), key, htab->compare)) {
        htab->count--;
    }
}

void *hash_value(void *key, hashtable *htab) {
    struct tab_nod *posi;
    unsigned int inde = hashinde(key, htab);
    for (posi = htab->hashlist[inde]; NULL != posi; posi = posi->next) {
        if (htab->compare(key, posi->key)) {
            return (posi->data);
        }
    }
    return NULL;
}

inline struct tab_nod *hash_value_adler32(unsigned int sum, hashtable *htab) {
    unsigned int inde = SUM2HEX(sum, htab);
    return htab->hashlist[inde];
}

void *hash_value_md5(void *key, struct tab_nod *root, hashtable *htab) {
    struct tab_nod *posi;
    //
    for (posi = root; NULL != posi; posi = posi->next) {
        if (htab->compare(key, posi->key)) {
            return (posi->data);
        }
    }
    return NULL;
}

void hash_for_each_do(hashtable *htab, int(*call_back) (void *, void *)) {
    unsigned int inde = 0;
    struct tab_nod *posi;
    for (inde = 0; inde < htab->hashsize; inde++) {
        for (posi = htab->hashlist[inde]; NULL != posi; posi = posi->next) {
            call_back(posi->key, posi->data);
        }
    }
}

inline int hash_count(hashtable *htab) {
    return htab->count;
}
