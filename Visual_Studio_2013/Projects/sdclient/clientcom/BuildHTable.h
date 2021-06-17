#ifndef _BUILD_HTABLE_H_
#define _BUILD_HTABLE_H_

#include "common_macro.h"
#include "utility/hash_table.h"

#define MATCH_BACKET_SIZE		0x80000 // 512K
//
#define create_htable(hsize)		hash_create(lh_strhash, equal_str, hsize)
#define CreateHTable(hsize)		hash_create(NULL_Hash, equal_md5, hsize)
//
unsigned int NULL_Hash(void *dat);
//
int equal_md5(void *k1, void *k2);
unsigned int lh_strhash(void *dat);
int equal_str(void *k1, void *k2);
//

typedef struct _sea_env_ {
	BYTE mode;
	unsigned int adler32_chk;
	unsigned char c1;
} sea_env;

namespace NBdHTab {
	int BuildHTable(hash_table *htab, HANDLE sums_stre, DWORD file_size, int file_chunk);
	int BuildHTable(hash_table *htab, struct complex_chks *pcchks, int block_count);
	void DeleBHTab(hash_table *htab);
	//
#define INITI_ADLER_CHECK			0
#define ROLLING_ADLER_CHECK			1
	// bFine :>=0 found <0  not found  mode==0 first /not
	int HashSearch(hash_table *htab, BYTE *byte_ptr, sea_env *senv);
	int HashSearch(hash_table *htab, BYTE *byte_ptr, sea_env *senv, int selen);
};

#endif /* _BUILD_HTABLE_H_ */