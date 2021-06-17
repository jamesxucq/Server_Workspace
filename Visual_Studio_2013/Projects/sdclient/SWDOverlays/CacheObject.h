#pragma once

#include <windows.h>
#include "clientcom/ShellInterface.h"
#include "CacheUtility.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default
#include <vector>
using  std::vector;

//
class CCacheObject {
public:
	CCacheObject(void);
	~CCacheObject(void);
public:
	TCHAR m_szWatchDire[MAX_PATH];
	struct CacheResponse tReply;
private:
	unordered_map<wstring, struct dire_attrib *> m_mDiresCache;
	vector<struct path_attrib *>m_vPathsVec;
	unordered_map<wstring, DWORD> m_mFilesCache;
private:
	DWORD DireRecursErase(const TCHAR *dirpath, DWORD statcode);
	DWORD DirectoryErase(const TCHAR *dirpath, DWORD statcode);
	DWORD FileErase(const TCHAR *szFileName, DWORD statcode) ;// 0 succ; 1 not found ; -1 error
	VOID PathVecErase(const TCHAR *path);
private:
	DWORD InsertFile(const TCHAR *szFileName, DWORD statcode); // 0 succ; 1 found ; -1 error
	VOID InsertDirectory(const TCHAR *dirpath, DWORD statcode);
	VOID InsertPathVec(const TCHAR *path, DWORD statcode);
	VOID InsertDireRecurs(const TCHAR *dirpath, DWORD statcode); // increase == 0  increase the ref
public:
	struct dire_attrib *DiresCacheFind(const TCHAR *key);
	struct path_attrib *PathsVecFind(const TCHAR *key);
	DWORD FilesCacheFind(const TCHAR *key);
	DWORD FilesAttribFind(const TCHAR *key);
public:
	DWORD CacheErase(const TCHAR *path, DWORD statcode);
	DWORD CacheSeta(const TCHAR *path, DWORD statcode);
	DWORD SetCacheValue(TCHAR *path, DWORD statcode);
	VOID CacheClear(DWORD statcode);
};

// 临界区结构对象
extern CRITICAL_SECTION gcsDiresCache;
extern CRITICAL_SECTION gcsPathsVec;
extern CRITICAL_SECTION gcsFilesCache;
extern CRITICAL_SECTION gcsResponse;

extern class CCacheObject objCacheObject;
#define CACHE_OBJECT(member)	objCacheObject.member

//
#define	FILE_DIRECTORY_BYTE(FILE_STATUS)	((FILE_STATUS) & 0x00003000)
#define REALITY_BYTE(FILE_STATUS)		(FILE_STATUS) & 0x00000FFF
//
#define MODIFY_FILE_STATUS_PERMISSION(OLD_STATUS, NEW_STATUS) \
	(NEW_STATUS&0x000000F0 && OLD_STATUS&0x0000000F) \
	|| (NEW_STATUS&0x0000000F && OLD_STATUS&0x0000000F && NEW_STATUS>OLD_STATUS)

#define CHECK_APPEND_REPETITIVE(FILE_ATTRIB, NEW_STATUS) \
	(FILE_ATTRIB & (NEW_STATUS<<16))

#define ERASE_STATUS_NOT_FOUND(FILE_ATTRIB, NEW_STATUS)	\
	(FILE_ATTRIB & (NEW_STATUS<<16))

#define FILE_CURRENT_STATCODE(FILE_STATUS)		(FILE_STATUS) & 0x0000FFFF
#define FILE_PREVIOUS_STATCODE(FILE_STATUS)		(FILE_STATUS) & 0xFFFF0000

#define ADJUST_FILE_STATUS_BYTE(FILE_ATTRIB) { \
	(FILE_ATTRIB) = FILE_PREVIOUS_STATCODE((FILE_ATTRIB)); \
	if((FILE_ATTRIB) & (FILE_STATUS_SYNCING<<16)) \
		(FILE_ATTRIB) |= FILE_STATUS_SYNCING; \
	else if((FILE_ATTRIB) & (FILE_STATUS_CONFLICT<<16)) \
		(FILE_ATTRIB) |= FILE_STATUS_CONFLICT; \
	else if((FILE_ATTRIB) & (FILE_STATUS_ADDED<<16)) \
		(FILE_ATTRIB) |= FILE_STATUS_ADDED; \
	else if((FILE_ATTRIB) & (FILE_STATUS_DELETE<<16)) \
		(FILE_ATTRIB) |= FILE_STATUS_DELETE; \
	else if((FILE_ATTRIB) & (FILE_STATUS_MODIFIED<<16)) \
		(FILE_ATTRIB) |= FILE_STATUS_MODIFIED; \
}

#define APPEND_FILE_STATUS(FILE_ATTRIB, NEW_STATUS)		(FILE_ATTRIB) |= NEW_STATUS << 16
#define ERASE_FILE_STATUS(FILE_ATTRIB, NEW_STATUS)		(FILE_ATTRIB) &= ~(NEW_STATUS<<16)
		
// At dir cache 
// 0xFFFFF000 is index
// 0x00000FFF is folder attribute
// INCREASE_REFERENCE DECREASE_REFERENCE		

#define DIRE_CACHE_REFERENCE_INCREASE(DIRE_ATTRIB, FILE_STATUS) { \
	switch(FILE_STATUS) { \
	case FILE_STATUS_SYNCING: \
		(DIRE_ATTRIB)->syncing ++; \
		break; \
	case FILE_STATUS_ADDED: \
		(DIRE_ATTRIB)->added ++; \
		break; \
	case FILE_STATUS_DELETE: \
		(DIRE_ATTRIB)->deleted ++; \
		break; \
	case FILE_STATUS_MODIFIED: \
		(DIRE_ATTRIB)->modified ++; \
		break; \
	case FILE_STATUS_CONFLICT: \
		(DIRE_ATTRIB)->conflict ++; \
		break; \
	} \
}

#define DIRE_CACHE_REFERENCE_DECREASE(DIRE_ATTRIB, FILE_STATUS) { \
	switch(FILE_STATUS) { \
	case FILE_STATUS_SYNCING: \
		(DIRE_ATTRIB)->syncing --; \
		break; \
	case FILE_STATUS_ADDED: \
		(DIRE_ATTRIB)->added --; \
		break; \
	case FILE_STATUS_DELETE: \
		(DIRE_ATTRIB)->deleted --; \
		break; \
	case FILE_STATUS_MODIFIED: \
		(DIRE_ATTRIB)->modified --; \
		break; \
	case FILE_STATUS_CONFLICT: \
		(DIRE_ATTRIB)->conflict --; \
		break; \
	} \
}

// can delete 0; can not delete 1
#define DIRE_CANT_DELETE(DIRE_ATTRIB) \
	((DIRE_ATTRIB)->syncing || (DIRE_ATTRIB)->modified || (DIRE_ATTRIB)->added || (DIRE_ATTRIB)->deleted || (DIRE_ATTRIB)->conflict)

#define ADJUST_DIRE_STATCODE(DIRE_ATTRIB) { \
	if((DIRE_ATTRIB)->syncing) \
		(DIRE_ATTRIB)->dispstat = FILE_STATUS_SYNCING; \
	else if((DIRE_ATTRIB)->conflict) \
		(DIRE_ATTRIB)->dispstat = FILE_STATUS_CONFLICT;	\
	else if((DIRE_ATTRIB)->added) \
		(DIRE_ATTRIB)->dispstat = FILE_STATUS_ADDED; \
	else if((DIRE_ATTRIB)->deleted) \
		(DIRE_ATTRIB)->dispstat = FILE_STATUS_DELETE; \
	else if((DIRE_ATTRIB)->modified) \
		(DIRE_ATTRIB)->dispstat = FILE_STATUS_MODIFIED;	\
}
