#pragma once

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <google/dense_hash_map>
using google::dense_hash_map;      // namespace where class lives by default

struct DropNode {
	DWORD	dwDropEffect;
	TCHAR	szPathName[MAX_PATH+1];
};
typedef dense_hash_map <wstring, struct DropNode *> DropHmap;

namespace NDropArray {
	inline void DeleteNode(struct DropNode *node) 
	{ if(node) free(node); }
	void DeleteDropHmap(DropHmap *ptrDropHmap);
}

class CDropArray {
private:
	TCHAR m_szWatchingDirectory[MAX_PATH+1];
	DWORD m_dwDirectoryLength;
	dense_hash_map <wstring, struct DropNode *>m_hmRecentDrop;
public:
	DWORD SetWatchingDirectory(TCHAR *szWatchDirectory);
	void Destroy();
public:
	struct DropNode *InsDropPath(TCHAR *szPathName, DWORD dwDropEffect);
	struct DropNode *FindDropArray(struct DropNode *node, const TCHAR *szPathName);
};
extern CDropArray objDropArray;
