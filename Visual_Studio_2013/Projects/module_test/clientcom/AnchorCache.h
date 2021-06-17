#pragma once
#include <afxmt.h>

#include "FilesVec.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

// #include <google/sparse_hash_map>
// using google::sparse_hash_map;      // namespace where class lives by default
#include <unordered_map>
using std::tr1::unordered_map;      // namespace where class lives by default
#include <vector>
using  std::vector;


class CAnchorCache {
public:
	CAnchorCache(void);
	~CAnchorCache(void);
public:
	DWORD Initialize();
	DWORD Finalize();
	// anchor logic operate;
private:
	volatile DWORD m_dwAnchorLocked; // 0 no, 1 yes
public:
	inline void SetAnchorLocked(DWORD dwAnchorLocked) 
	{ InterlockedExchange((LONG*)&m_dwAnchorLocked, dwAnchorLocked); }
	inline DWORD GetAnchorLocked() 
	{ DWORD dwAnchorLocked; InterlockedExchange((LONG*)&dwAnchorLocked, m_dwAnchorLocked); return dwAnchorLocked; }
private:
	volatile DWORD m_dwLastAnchor;
public:
	inline void SetLastAnchor(DWORD last_anchor) 
	{ InterlockedExchange((LONG*)&m_dwLastAnchor, last_anchor); }
	inline DWORD GetLastAnchor() 
	{ DWORD last_anchor; InterlockedExchange((LONG*)&last_anchor, m_dwLastAnchor); return last_anchor; }
private:
	volatile DWORD m_dwLastTickCount;
public:
	void SetLastTickCount() 
	{ InterlockedExchange((LONG*)&m_dwLastTickCount, GetTickCount()); }
	DWORD GetLastTickCount() 
	{ DWORD dwLastTickCount; InterlockedExchange((LONG*)&dwLastTickCount, m_dwLastTickCount); return dwLastTickCount; }
private:
	FilesVec m_vFilesCache; //after last anchor modify files;
	unordered_map <wstring, struct file_attrib *> m_hmFilesSingle; //for add modify delete cache
	unordered_map <wstring, struct file_attrib *> m_hmFilesDual; //for exist new cache
public:
	inline FilesVec *GetFilesCachePoint() {return &m_vFilesCache;}
	inline FilesVec *GetLastFilesPoint() {return &m_vFilesCache;}
public:
#define CONTROL_TYPE_ERASE		0x0001
#define CONTROL_TYPE_INSERT		0x0002
	static DWORD statcode_conv(DWORD control_type, DWORD modify_type);
public:
	void FilesCacheAppend(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szFileName, const DWORD modify_type);
	void FilesCacheCopy(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szExistsName, const TCHAR *szName);
	void FilesCacheMove(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szExistsName, const TCHAR *szNewName);
	void FolderCacheAppend(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szDirectory);
	void FolderCacheCopy(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szExistsName, const TCHAR *szNewName);
	void FolderCacheMove(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szExistsName, const TCHAR *szNewName);
public:
	DWORD AddNewAnchor(DWORD dwNewAnchor);
};

extern CAnchorCache objAnchorCache;

/////////////////////////////////////////////////////////////////////////
