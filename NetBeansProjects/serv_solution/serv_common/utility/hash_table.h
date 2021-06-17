#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>

struct tab_nod {
    void *key;
    void *data;
    struct tab_nod *next;
};

struct _hashtable_ {
    unsigned int (*gethash)(void *);
    int (*compare)(void *, void *);
    unsigned int hashsize;
    unsigned int count;
    struct tab_nod **hashlist;
};

typedef struct _hashtable_ hashtable;

//
hashtable *hash_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), unsigned int size);
void hash_del(hashtable *htab);
void hash_insert(void *key, void *data, hashtable *htab);
void hash_remove(void *key, hashtable *htab);
void *hash_value(void *key, hashtable *htab);
void hash_for_each_do(hashtable *htab, int (*call_back) (void *, void *));
int hash_count(hashtable *htab);

//
void hash_insert_adler32(void *key, void *data, unsigned int sum, hashtable *htab);
inline struct tab_nod *hash_value_adler32(unsigned int sum, hashtable *htab);
void *hash_value_md5(void *key, struct tab_nod *root, hashtable *htab);

#ifdef	__cplusplus
}
#endif

#endif

