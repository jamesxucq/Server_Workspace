#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <string.h>

struct hashentry {
	void *key;
	void *data;
	struct hashentry *next;
};

typedef struct _hashtable_ {
	unsigned int (*gethash)(void *);
	int (*compare)(void *, void *);
	int hashsize;
	int count;
	struct hashentry **hashlist;
} hashtable;

hashtable *hash_create(unsigned int (*keyfunc)(void *),
					   int (*comparefunc)(void *, void *),
					   int size);
void hash_free(hashtable *tab);
void hash_destroy(hashtable *tab, void (*destroy)(void *));
void hash_clear(hashtable *tab);
void hash_insert(void *key, void *data, hashtable *tab);
void hash_remove(void *key, hashtable *tab);
void *hash_value(void *key, hashtable *tab);
void hash_for_each_do(hashtable *tab, int (cb) (void *, void *));
int hash_count(hashtable *tab);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void hash_insert_adler32(void *key, void *data, unsigned int sum, hashtable *tab);
struct hashentry *hash_value_adler32(unsigned int sum, hashtable *tab);
void *hash_value_md5(void *key, struct hashentry *root, hashtable *tab);

#endif

