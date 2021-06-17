#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define hashindex(key, tab) ((tab->gethash)(key) % (tab->hashsize -1))
#define SUM2HEX(sum, tab) ((sum) % (tab->hashsize))

inline struct hashentry *hashentry_new(void *key, void *data) {
	struct hashentry *new_entry = (struct hashentry *)malloc(sizeof (struct hashentry));
	new_entry->key = key;
	new_entry->data = data;
	new_entry->next = NULL;
	return new_entry;
}

void hlist_append(struct hashentry **root, void *key, void *data) {
	struct hashentry *new_entry, *position;

	new_entry = hashentry_new(key, data);
	if (*root == NULL) {
		*root = new_entry;
	} else {
		for (position = *root; position->next != NULL; position = position->next);
		position->next = new_entry;
	}
}

int hlist_update(struct hashentry *root, void *key, void *data,
				 int (*compare)(void *, void *)) {
					 struct hashentry *position;
					 for (position = root; position != NULL; position = position->next) {
						 if (compare(key, position->key)) {
							 position->data = data;
							 return 0;
						 }
					 }
					 return -1;
}

inline struct hashentry *hashentry_free(struct hashentry *h) {
	struct hashentry *next = h->next;
	free(h);
	h = NULL;
	return (next);
}

int hlist_remove(struct hashentry **root, void *key,
				 int (*compare)(void *, void *)) {
					 struct hashentry *position, *previous;

					 if (NULL == *root) return -1;
					 if (compare((*root)->key, key)) {
						 *root = hashentry_free(*root);
						 return 0;
					 }

					 previous = *root;
					 for (position = previous->next; NULL != position; position = position->next) {
						 if (compare(position->key, key)) {
							 previous->next = hashentry_free(position);
							 return 0;
						 }
						 previous = position;
					 }
					 return -1;
}

hashtable *hash_create(unsigned int (*keyfunc)(void *),
					   int (*comparefunc)(void *, void *),
					   /*void (*freefunc)(void *),*/
					   int size) {
						   int len = sizeof (struct hashentry *) * size;

						   hashtable *tab = (hashtable *)malloc(sizeof (hashtable));
						   memset(tab, '\0', sizeof (hashtable));
						   tab->gethash = keyfunc;
						   tab->compare = comparefunc;
						   tab->hashsize = size;
						   tab->count = 0;

						   tab->hashlist = (struct hashentry **)malloc(len);
						   if (tab->hashlist == NULL) {
							   free(tab);
							   return NULL;
						   }
						   memset(tab->hashlist, '\0', len);

						   return tab;
}

void hash_free(hashtable *tab) {
	int i;
	struct hashentry *position;

	if (NULL == tab) return;
	for (i = 0; i < tab->hashsize; i++)
		for (position = tab->hashlist[i]; NULL != position; position = hashentry_free(position));

	free(tab->hashlist);
	free(tab);
	tab = NULL;
}

void hash_destroy(hashtable *tab, void (*destroy)(void *)) {
	int i;
	struct hashentry *position;

	if (NULL == tab) return;
	for (i = 0; i < tab->hashsize; i++) {
		for (position = tab->hashlist[i]; NULL != position; position = hashentry_free(position))
			destroy(position->data);
	}

	free(tab->hashlist);
	free(tab);
	tab = NULL;
}


void hash_clear(hashtable *tab) {
	int i;
	struct hashentry *position;

	if (NULL == tab) return;
	for (i = 0; i < tab->hashsize; i++)
		for (position = tab->hashlist[i]; NULL != position; position = hashentry_free(position));

	int len = sizeof (struct hashentry *) * tab->hashsize;
	memset(tab->hashlist, '\0', len);
	tab->count = 0;
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
	unsigned int index = SUM2HEX(sum, tab);
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
	struct hashentry *position;
	unsigned int index = hashindex(key, tab);
	for (position = tab->hashlist[index]; NULL != position; position = position->next) {
		if (tab->compare(key, position->key)) {
			return (position->data);
		}
	}
	return NULL;
}

struct hashentry *hash_value_adler32(unsigned int sum, hashtable *tab) {
	unsigned int index = SUM2HEX(sum, tab);
	return tab->hashlist[index];
}

void *hash_value_md5(void *key, struct hashentry *root, hashtable *tab) {
	struct hashentry *position;

	for (position = root; NULL != position; position = position->next) {
		if (tab->compare(key, position->key)) {
			return (position->data);
		}
	}
	return NULL;
}

void hash_for_each_do(hashtable *tab, int(cb) (void *, void *)) {
	int i = 0;
	struct hashentry *position;
	for (i = 0; i < tab->hashsize; i++) {
		for (position = tab->hashlist[i]; NULL != position; position = position->next) {
			cb(position->key, position->data);
		}
	}
}

inline int hash_count(hashtable *tab) {
	return tab->count;
}
