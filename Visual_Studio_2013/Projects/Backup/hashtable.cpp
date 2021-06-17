#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define hashindex(key, tab) ((tab->gethash)(key) % (tab->hashsize -1))
#define SUM2HEX(sum) ((sum) % (tab->hashsize))

inline struct hashentry *hashentry_new(void *key, void *data) {
    struct hashentry *new_entry = (struct hashentry *)malloc(sizeof (struct hashentry));
    new_entry->key = key;
    new_entry->data = data;
    new_entry->next = NULL;
    return new_entry;
}

void hlist_append(struct hashentry **root, void *key, void *data) {
    struct hashentry *new_entry, *pos;

    new_entry = hashentry_new(key, data);
    if (*root == NULL) {
        *root = new_entry;
    } else {
        for (pos = *root; pos->next != NULL; pos = pos->next);
        pos->next = new_entry;
    }
}

int hlist_update(struct hashentry *root, void *key, void *data,
        int (*compare)(void *, void *)) {
    struct hashentry *pos;
    for (pos = root; pos != NULL; pos = pos->next) {
        if (compare(key, pos->key)) {
            //free(pos->data);
            pos->data = data;
            free(key);
            return 0;
        }
    }
    return -1;
}

inline struct hashentry *hashentry_free(struct hashentry *h) {
    struct hashentry *next = h->next;
    free(h->key);
    free(h->data);
    free(h);
    h = NULL;
    return (next);
}

int hlist_remove(struct hashentry **root, void *key,
        int (*compare)(void *, void *)) {
    struct hashentry *pos, *prev;

    if (NULL == *root) return -1;
    if (compare((*root)->key, key)) {
        *root = hashentry_free(*root);
        return 0;
    }

    prev = *root;
    for (pos = prev->next; NULL != pos; pos = pos->next) {
        if (compare(pos->key, key)) {
            prev->next = hashentry_free(pos);
            return 0;
        }
        prev = pos;
    }
    return -1;
}

hashtable *hash_create(unsigned int (*keyfunc)(void *),
        int (*comparefunc)(void *, void *),
        int size) {
    int len = sizeof (struct hashentry *) * size;

    hashtable *tab = (hashtable *)malloc(sizeof (hashtable));
    memset(tab, '\0', sizeof (hashtable));

    tab->hashlist = (struct hashentry **)malloc(len);
    if (tab->hashlist == NULL) {
        free(tab);
        return NULL;
    }
    memset(tab->hashlist, '\0', len);

    tab->compare = comparefunc;
    tab->gethash = keyfunc;
    tab->hashsize = size;
    tab->count = 0;
    return tab;
}

void hash_free(hashtable *tab) {
    int i;
    struct hashentry *pos;

    if (NULL == tab) return;
    for (i = 0; i < tab->hashsize; i++)
        for (pos = tab->hashlist[i]; NULL != pos; pos = hashentry_free(pos));

    free(tab->hashlist);
    free(tab);
    tab = NULL;
}

void hash_insert(void *key, void *data, hashtable *tab) {
    unsigned int index = hashindex(key, tab);
    struct hashentry *root = tab->hashlist[index];

    if (hlist_update(root, key, data, tab->compare) != 0) { //(1)
        hlist_append(&(tab->hashlist[index]), key, data);
        tab->count++;
    }
}

void hash_insert_adler32(void *key, void *data, unsigned int sum, hashtable *tab) {
    unsigned int index = SUM2HEX(sum);
    struct hashentry *root = tab->hashlist[index];

    if (hlist_update(root, key, data, tab->compare) != 0) { //(1)
        hlist_append(&(tab->hashlist[index]), key, data);
        tab->count++;
    }
}

void hash_remove(void *key, hashtable *tab) {
    unsigned int index = hashindex(key, tab);
    if (hlist_remove(&(tab->hashlist[index]), key, tab->compare) == 0) {
        tab->count--;
    }
}

void *hash_value(void *key, hashtable *tab) {
    struct hashentry *pos;
    unsigned int index = hashindex(key, tab);
    for (pos = tab->hashlist[index]; NULL != pos; pos = pos->next) {
        if (tab->compare(key, pos->key)) {
            return (pos->data);
        }
    }
    return NULL;
}

struct hashentry *hash_value_adler32(unsigned int sum, hashtable *tab) {
    unsigned int index = SUM2HEX(sum);
    return tab->hashlist[index];
}

void *hash_value_md5(void *key, struct hashentry *root, hashtable *tab) {
    struct hashentry *pos;

    for (pos = root; NULL != pos; pos = pos->next) {
        if (tab->compare(key, pos->key)) {
            return (pos->data);
        }
    }
    return NULL;
}

void hash_for_each_do(hashtable *tab, int(cb) (void *, void *)) {
    int i = 0;
    struct hashentry *pos;
    for (i = 0; i < tab->hashsize; i++) {
        for (pos = tab->hashlist[i]; NULL != pos; pos = pos->next) {
            cb(pos->key, pos->data);
        }
    }
}

inline int hash_count(hashtable *tab) {
    return tab->count;
}
