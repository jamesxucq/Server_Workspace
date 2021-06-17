#ifndef _HASHTABLEEX_H_
#define _HASHTABLEEX_H_ 

#include "common_macro.h"
#include "utility/hashtable.h"

#define create_hashtable(hsize)		hash_create(lh_strhash, equal_str, hsize)
#define CreateHashTable(hsize)		hash_create(NULL_Hash, equal_md5, hsize)

unsigned int NULL_Hash(void *src);

int equal_md5(void *k1, void *k2);
unsigned int lh_strhash(void *src);
int equal_str(void *k1, void *k2);

int BuildHashTable(hashtable *htable, HANDLE sums_stream, DWORD file_size);
int BuildHashTable(hashtable *htable, struct complex_sum *psums_complex, int block_count);

#define FIRST_HASH_CHECK			0
#define ROLLING_HASH_CHECK			1
// bFine :>=0 found <0  not found  mode==0 first /not 
int HashTableSearch(BYTE *DatPtr, hashtable *htable, BYTE mode);

#endif /* _HASHTABLEEX_H_ */