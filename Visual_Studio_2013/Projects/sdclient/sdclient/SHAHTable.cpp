#include "stdafx.h"

#include "clientcom/clientcom.h"
#include "FastUtility.h"
#include "ChksAddi.h"

#include "SHAHTable.h"

/* BKDR Hash Function */
unsigned int BKDR_SHA_hash(void *dat) {
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;
	char *str = (char *)dat;
	int inde;

	for (inde = 0; SHA1_DIGEST_LEN > inde; inde++) {
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

//
int equal_sha(void *k1, void *k2) {
	return (0 == memcmp((char *) k1, (char *) k2, SHA1_DIGEST_LEN));
}

static struct FastZNode *add_new_znode(hash_table *htab, struct LockActio *lock_acti, char *file_chks) {
// _LOG_DEBUG(_T("add new node! %08x %s"), atte->action_type, atte->file_name);
	char *hash_key = (char *)malloc(SHA1_DIGEST_LEN);
	if(!hash_key) return NULL;
	memcpy(hash_key, file_chks, SHA1_DIGEST_LEN);
	//
    struct FastZNode *ast_new = (struct FastZNode *)malloc(sizeof(struct FastZNode));
	if(!ast_new) {
		free(hash_key);
		return NULL;
	}
    memset(ast_new, '\0', sizeof(struct FastZNode));
	ast_new->dwActioType = lock_acti->dwActioType;
	ast_new->lock_acti = lock_acti;
	//
	hash_insert(hash_key, ast_new, htab);
    return ast_new;
}

hash_table *NSHAHmap::BuildSHAHTab(LkAiVec *pLkAiVec) {
    LkAiVec::iterator iter;
	char szSha1Chks[SHA1_DIGEST_LEN];
	struct FastZNode *fast_znode;
	hash_table *htab = SHAHTabCreate(SHA1_BACKET_SIZE);
	//
	struct LockActio *lock_acti;
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
		lock_acti = (*iter);
		if(ADDI&lock_acti->dwActioType && !lock_acti->coupleID) {
			objChksAddi.GetFileChks(szSha1Chks, lock_acti->ulFileInde);
			fast_znode = (struct FastZNode *)hash_value(szSha1Chks, htab);
			if(fast_znode) {
				NFastUtili::AppNewZNode(fast_znode, lock_acti);
			} else {
				add_new_znode(htab, lock_acti, szSha1Chks);
			}		
		}
    }
	//
	return htab;
}

static void dele_znode_link(void *key, void *data) {
	struct FastZNode *fast_znode = (struct FastZNode *)data;
	struct FastZNode *prev_znode;
	while(fast_znode) {
		prev_znode = fast_znode;
		fast_znode = fast_znode->pNext;
		free(prev_znode);
	}
	free(key);
}

void NSHAHmap::DeleSHAHTab(hash_table *htab) {
	hash_destroy(htab, dele_znode_link);
}