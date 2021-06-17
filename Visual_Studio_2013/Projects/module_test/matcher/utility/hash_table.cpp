#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

#define hash_inde(key, tab) ((tab->get_hash)(key) % (tab->hash_size -1))
#define SUM2HEX(sum, tab) ((sum) % (tab->hash_size))

inline struct hash_entr *hash_entr_new(void *key, void *data) {
    struct hash_entr *new_entry = (struct hash_entr *)malloc(sizeof (struct hash_entr));
	if(!new_entry) return NULL;
    new_entry->key = key;
    new_entry->data = data;
    new_entry->next = NULL;
    return new_entry;
}

void hlist_append(struct hash_entr **root, void *key, void *data) {
    struct hash_entr *new_entry, *toke;

    new_entry = hash_entr_new(key, data);
    if (NULL == *root) {
        *root = new_entry;
    } else {
        for (toke = *root; NULL != toke->next; toke = toke->next);
        toke->next = new_entry;
    }
}

int hlist_update(struct hash_entr *root, void *key, void *data,
                 int (*comp_func)(void *, void *))
{
    struct hash_entr *toke;
    for (toke = root; NULL != toke; toke = toke->next) {
        if (comp_func(key, toke->key)) {
            toke->data = data;
            return 0;
        }
    }
    return -1;
}

inline struct hash_entr *hash_entr_free(struct hash_entr *h) {
    struct hash_entr *next = h->next;
    free(h);
    return (next);
}

int hlist_remove(struct hash_entr **root, void *key,
                 int (*comp_func)(void *, void *))
{
    struct hash_entr *toke, *previ;

    if (NULL == *root) return -1;
    if (comp_func((*root)->key, key)) {
        *root = hash_entr_free(*root);
        return 0;
    }

    previ = *root;
    for (toke = previ->next; NULL != toke; toke = toke->next) {
        if (comp_func(toke->key, key)) {
            previ->next = hash_entr_free(toke);
            return 0;
        }
        previ = toke;
    }
    return -1;
}

hash_table *hash_create(unsigned int (*key_func)(void *),
                       int (*comp_func)(void *, void *),
                       unsigned int size)
{
    int len = sizeof (struct hash_entr *) * size;
//
    hash_table *tab = (hash_table *)malloc(sizeof (hash_table));
	if(!tab) return NULL;
    memset(tab, '\0', sizeof (hash_table));
    tab->get_hash = key_func;
    tab->comp_func = comp_func;
    tab->hash_size = size;
    tab->count = 0;
//
    tab->hash_list = (struct hash_entr **)malloc(len);
    if (NULL == tab->hash_list) {
        free(tab);
        return NULL;
    }
    memset(tab->hash_list, '\0', len);
//
    return tab;
}

void hash_free(hash_table *tab) {
    unsigned int inde;
    struct hash_entr *toke;

    if (NULL == tab) return;
    for (inde = 0; inde < tab->hash_size; inde++)
        for (toke = tab->hash_list[inde]; NULL != toke; toke = hash_entr_free(toke));

    free(tab->hash_list);
    free(tab);
}

void hash_destroy(hash_table *tab, void (*destroy)(void *, void *)) {
    unsigned int inde;
    struct hash_entr *toke;

    if (NULL == tab) return;
    for (inde = 0; inde < tab->hash_size; inde++) {
        for (toke = tab->hash_list[inde]; NULL != toke; toke = hash_entr_free(toke))
            destroy(toke->key, toke->data);
    }

    free(tab->hash_list);
    free(tab);
}


void hash_clear(hash_table *tab) {
    unsigned int inde;
    struct hash_entr *toke;

    if (NULL == tab) return;
    for (inde = 0; inde < tab->hash_size; inde++)
        for (toke = tab->hash_list[inde]; NULL != toke; toke = hash_entr_free(toke));

    int len = sizeof (struct hash_entr *) * tab->hash_size;
    memset(tab->hash_list, '\0', len);
    tab->count = 0;
}

void hash_insert(void *key, void *data, hash_table *tab) {
    unsigned int inde = hash_inde(key, tab);
    struct hash_entr *root = tab->hash_list[inde];

    if (hlist_update(root, key, data, tab->comp_func) != 0) { //(1)
        hlist_append(&(tab->hash_list[inde]), key, data);
        tab->count++;
    }
}

void hash_insert_adler32(void *key, void *data, unsigned int sum, hash_table *tab) {
    unsigned int inde = SUM2HEX(sum, tab);
    struct hash_entr *root = tab->hash_list[inde];

    if (hlist_update(root, key, data, tab->comp_func) != 0) { //(1)
        hlist_append(&(tab->hash_list[inde]), key, data);
        tab->count++;
    }
}

void hash_remove(void *key, hash_table *tab) {
    unsigned int inde = hash_inde(key, tab);
    if (0 == hlist_remove(&(tab->hash_list[inde]), key, tab->comp_func)) {
        tab->count--;
    }
}

void *hash_value(void *key, hash_table *tab) {
    struct hash_entr *toke;
    unsigned int inde = hash_inde(key, tab);
    for (toke = tab->hash_list[inde]; NULL != toke; toke = toke->next) {
        if (tab->comp_func(key, toke->key)) {
            return (toke->data);
        }
    }
    return NULL;
}

struct hash_entr *hash_value_adler32(unsigned int sum, hash_table *tab) {
    unsigned int inde = SUM2HEX(sum, tab);
    return tab->hash_list[inde];
}

void *hash_value_md5(void *key, struct hash_entr *root, hash_table *tab) {
    struct hash_entr *toke;

    for (toke = root; NULL != toke; toke = toke->next) {
        if (tab->comp_func(key, toke->key)) {
            return (toke->data);
        }
    }
    return NULL;
}

void hash_for_each_do(hash_table *tab, int(cb) (void *, void *)) {
    unsigned int inde = 0;
    struct hash_entr *toke;
    for (inde = 0; inde < tab->hash_size; inde++) {
        for (toke = tab->hash_list[inde]; NULL != toke; toke = toke->next) {
            cb(toke->key, toke->data);
        }
    }
}

inline int hash_count(hash_table *tab) {
    return tab->count;
}
