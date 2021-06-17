#ifndef _SHA_HTABLE_H_
#define _SHA_HTABLE_H_

//
#include "LockedAction.h"

//
#define SHA1_BACKET_SIZE		0x8000 // 32K
unsigned int BKDR_SHA_hash(void *dat);
int equal_sha(void *k1, void *k2);

//
#define SHAHTabCreate(hsize)		hash_create(BKDR_SHA_hash, equal_sha, hsize)

//
namespace NSHAHmap {
	hash_table *BuildSHAHTab(LkAiVec *pLkAiVec);
	void DeleSHAHTab(hash_table *htab);
};


#endif /* _SHA_HTABLE_H_ */