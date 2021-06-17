#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <string.h>

struct hash_entr {
	void *key;
	void *data;
	struct hash_entr *next;
};

typedef struct _hash_table_ {
	unsigned int (*get_hash)(void *);
	int (*comp_func)(void *, void *);
	unsigned int hash_size;
	unsigned int count;
	struct hash_entr **hash_list;
} hash_table;

hash_table *hash_create(unsigned int (*key_func)(void *),
					   int (*comp_func)(void *, void *),
					   unsigned int size);
void hash_free(hash_table *tab);
void hash_destroy(hash_table *tab, void (*destroy)(void *, void *));
void hash_clear(hash_table *tab);
void hash_insert(void *key, void *data, hash_table *tab);
void hash_remove(void *key, hash_table *tab);
void *hash_value(void *key, hash_table *tab);
void hash_for_each_do(hash_table *tab, int (cb) (void *, void *));
int hash_count(hash_table *tab);

//
void hash_insert_adler32(void *key, void *data, unsigned int sum, hash_table *tab);
struct hash_entr *hash_value_adler32(unsigned int sum, hash_table *tab);
void *hash_value_md5(void *key, struct hash_entr *root, hash_table *tab);

#endif

