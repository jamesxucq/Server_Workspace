#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "action_hmap.h"

#define hashinde(key, amap) ((amap->gethash)(key) % (amap->hashsize))

inline struct acti_node *anod_new(void *key, void *data) {
    struct acti_node *anod = malloc(sizeof (struct acti_node));
    if(!anod) return NULL;
    anod->key = key;
    anod->data = data;
    anod->next = NULL;
    return anod;
}

void alist_append(struct acti_node **root, void *key, void *data) {
    struct acti_node *new, *posi;
    //
    new = anod_new(key, data);
    if (NULL == *root) {
        *root = new;
    } else {
        for (posi = *root; NULL != posi->next; posi = posi->next);
        posi->next = new;
    }
}

int alist_update(struct acti_node *root, void *key, void *data, int (*compare)(void *, void *)) {
    struct acti_node *posi;
    for (posi = root; NULL != posi; posi = posi->next) {
        if (compare(key, posi->key)) {
            posi->data = data;
            return 0;
        }
    }
    return -1;
}

inline struct acti_node *anod_del(struct acti_node *nod, void (*freefunc)(void *)) {
    struct acti_node *next = nod->next;
    freefunc(nod->data);
    free(nod);
    return (next);
}

int alist_remove(struct acti_node **root, void *key, int (*compare)(void *, void *), void (*freefunc)(void *)) {
    struct acti_node *posi, *prev;
    if (NULL == *root) return -1;
    if (compare((*root)->key, key)) {
        *root = anod_del(*root, freefunc);
        return 0;
    }
    //
    prev = *root;
    for (posi = prev->next; NULL != posi; posi = posi->next) {
        if (compare(posi->key, key)) {
            prev->next = anod_del(posi, freefunc);
            return 0;
        }
        prev = posi;
    }
    return -1;
}

actionmap *action_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), void (*freefunc)(void *), unsigned int size) {
    int length = sizeof (struct acti_node *) * size;
    //
    actionmap *amap = (actionmap *) malloc(sizeof (actionmap));
    if(!amap) return NULL;
    memset(amap, '\0', sizeof (actionmap));
    amap->gethash = keyfunc;
    amap->compare = compare;
    amap->freefunc = freefunc;
    amap->hashsize = size;
    amap->count = 0;
    //
    amap->hmaplist = malloc(length);
    if (NULL == amap->hmaplist) {
        free(amap);
        return NULL;
    }
    memset(amap->hmaplist, '\0', length);
    //
    return amap;
}

void action_del(actionmap *amap) {
    unsigned int inde;
    struct acti_node *posi;
    //
    if (NULL == amap) return;
    for (inde = 0; inde < amap->hashsize; inde++)
        for (posi = amap->hmaplist[inde]; NULL != posi; posi = anod_del(posi, amap->freefunc));
    //
    free(amap->hmaplist);
    free(amap);
}

void action_insert(void *key, void *data, actionmap *amap) {
    unsigned int inde = hashinde(key, amap);
    struct acti_node *root = amap->hmaplist[inde];
    //
    if (0 != alist_update(root, key, data, amap->compare)) { //(1)
        alist_append(&(amap->hmaplist[inde]), key, data);
        amap->count++;
    }
}

void action_remove(void *key, actionmap *amap) {
    unsigned int inde = hashinde(key, amap);
    if (0 == alist_remove(&(amap->hmaplist[inde]), key, amap->compare, amap->freefunc)) {
        amap->count--;
    }
}

void *action_value(void *key, actionmap *amap) {
    struct acti_node *posi;
    unsigned int inde = hashinde(key, amap);
    for (posi = amap->hmaplist[inde]; NULL != posi; posi = posi->next) {
        if (amap->compare(key, posi->key)) {
            return (posi->data);
        }
    }
    return NULL;
}

void action_for_each_do(actionmap *amap, int (call_back) (void *, void *)) {
    unsigned int inde = 0;
    struct acti_node *posi;
    for (inde = 0; inde < amap->hashsize; inde++) {
        for (posi = amap->hmaplist[inde]; NULL != posi; posi = posi->next) {
            call_back(posi->key, posi->data);
        }
    }
}

inline int action_count(actionmap *amap) {
    return amap->count;
}

