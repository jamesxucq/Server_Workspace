#include "StdAfx.h"
#include <afxmt.h>

#include "DropArray.h"

CDropArray objDropArray;

void NDropArray::DeleteDropHmap(DropHmap *ptrDropHmap) {
	dense_hash_map <wstring, struct DropNode *>::iterator hmiter;

	// if(ptrDropHmap->empty()) return;
	for(hmiter = ptrDropHmap->begin(); hmiter != ptrDropHmap->end(); ++hmiter) 
		DeleteNode(hmiter->second);

	ptrDropHmap->clear();
}


DWORD CDropArray::SetWatchingDirectory(TCHAR *szWatchDirectory) {
	_tcscpy_s(m_szWatchingDirectory, szWatchDirectory);
	m_dwDirectoryLength = _tcslen(szWatchDirectory);
	return 0;
}

CCriticalSection gcs_DropArray;
void CDropArray::Destroy() {
	gcs_DropArray.Lock();
	NDropArray::DeleteDropHmap(&m_hmRecentDrop);
	gcs_DropArray.Unlock();
}

struct DropNode * CDropArray::InsDropPath(TCHAR *szPathName, DWORD dwDropEffect) {
	struct DropNode *node;

	if(!_tcsncmp(m_szWatchingDirectory, szPathName, m_dwDirectoryLength)) {
		node = (struct DropNode *)malloc(sizeof(struct DropNode));
		if(!node) return NULL;
		
		_tcscpy_s(node->szPathName, szPathName);
		node->dwDropEffect = dwDropEffect;
	
		gcs_DropArray.Lock();
		m_hmRecentDrop[szPathName] = node;
		gcs_DropArray.Unlock();
	}

	return node;
}

struct DropNode *CDropArray::FindDropArray(struct DropNode *node, const TCHAR *szPathName) {
	struct DropNode *drop = NULL;
	dense_hash_map <wstring, struct DropNode *>::iterator hmiter;

	gcs_DropArray.Lock();
	hmiter = m_hmRecentDrop.find(szPathName);
	if(hmiter != m_hmRecentDrop.end()) {
		drop = hmiter->second;
		m_hmRecentDrop.erase(hmiter);
	}
	gcs_DropArray.Unlock();

	if(drop) {
		memcpy(node, drop, sizeof(struct DropNode));
		NDropArray::DeleteNode(drop);
		return node;
	}

	return NULL;
}