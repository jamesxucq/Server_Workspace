/* 
 * File:   action_hmap.h
 * Author: Administrator
 */

#ifndef ACTIONHMAP_H
#define	ACTIONHMAP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>

struct acti_node {
    void *key;
    void *data;
    struct acti_node *next;
};

struct _actionmap_ {
    unsigned int (*gethash)(void *);
    int (*compare)(void *, void *);
    void (*freefunc)(void *);
    unsigned int hashsize;
    unsigned int count;
    struct acti_node **hmaplist;
};

typedef struct _actionmap_ actionmap;

actionmap *action_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), void (*freefunc)(void *), unsigned int size);
void action_del(actionmap *amap);
void action_insert(void *key, void *data, actionmap *amap);
void action_remove(void *key, actionmap *amap);
void *action_value(void *key, actionmap *amap);
void action_for_each_do(actionmap *amap, int (call_back) (void *, void *));
int action_count(actionmap *amap);

#ifdef	__cplusplus
}
#endif

#endif	/* ACTIONHMAP_H */

