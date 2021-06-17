#pragma once


#include "FilesVec.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <google/sparse_hash_map>
using google::sparse_hash_map;      // namespace where class lives by default
#include <google/dense_hash_map>
using google::dense_hash_map;      // namespace where class lives by default
#include <vector>
using  std::vector;


class CAnchorCache
{
public:
	CAnchorCache(void);
	~CAnchorCache(void);
public:
	bool Create();
	bool Destroy();
// anchor logic operate;
private:
	bool m_bSyncingLocked; ////0 no 1 yes
public:
	void SetAnchorLockStatus(bool bSyncingLocked);
	bool GetAnchorLockStatus();
public:
	DWORD m_uiLastAnchor;
private:
	FilesAttribVec m_vFilesAttCache; //after last anchor modify files;
	dense_hash_map <wstring, struct FileAttrib *> m_htFilesAttNormal; //for add modify delete cache
	dense_hash_map <wstring, struct FileAttrib *> m_htFilesAttExistNew; //for exist new cache
public:
	inline FilesAttribVec *GetFilesVecCachePointer() {return &m_vFilesAttCache;}
	inline FilesAttribVec *GetLastFilesVecPointer() {return &m_vFilesAttCache;}
public:
	DWORD FilesCacheInsert(wchar_t *sChangeName, char file_status);
	DWORD FilesCacheInsert(wchar_t *sExistName, wchar_t *sNewName);
public:
	int AddNewAnchor(DWORD uiAnchor);
};


extern CAnchorCache OAnchorCache;