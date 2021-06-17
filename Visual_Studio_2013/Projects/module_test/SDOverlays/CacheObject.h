#pragma once

#include <windows.h>
#include "clientcom/CacheInterface.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::tr1::unordered_map;      // namespace where class lives by default
#include <vector>
using  std::vector;


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
struct path_attrib {
	TCHAR path[MAX_PATH];
	DWORD shell_status;
};

struct DirAttrib {
	DWORD dwSyncing;
	DWORD dwDelete;
	DWORD dwModified;
	DWORD dwAdded;
	DWORD dwConflict;
	DWORD dirstatus;
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
class CCacheObject {
public:
	CCacheObject(void);
	~CCacheObject(void);
public:
	TCHAR m_wsWatchingDirectory[MAX_PATH];
public:
	struct CacheResponse tReply;
private:
	unordered_map<wstring, struct DirAttrib> m_hmDirectoriesCache;
	vector<struct path_attrib *>m_vPathsCacheVec;
	unordered_map<wstring, unsigned int> m_hmFilesCache;
private:
	DWORD DirectoryRecuErase(const TCHAR *dirpath, unsigned int statcode);
	DWORD DirectoryErase(const TCHAR *dirpath, unsigned int statcode);
	DWORD FileErase(const TCHAR *szFileName, unsigned int statcode) ;//0 succ; 1 not found ; -1 error
	void PathVecErase(const TCHAR *path);
private:
	DWORD InsertFile(const TCHAR *szFileName, unsigned int statcode); //0 succ; 1 found ; -1 error
	void InsertDirectory(TCHAR *dirpath, unsigned int statcode);
	void InsertPathVec(const TCHAR *path, unsigned int statcode);
	void InsertDirectoryRecu(const TCHAR *dirpath, unsigned int statcode); //increase == 0  increase the ref
public:
	struct DirAttrib *DirectoryCacheFind(const TCHAR *key);
	struct path_attrib *PathCacheFind(const TCHAR *key);
	DWORD FileCacheFind(const TCHAR *key);
	DWORD FileCacheFindExt(const TCHAR *key);
public:
	DWORD CacheErase(const TCHAR *key, unsigned int statcode);
	DWORD CacheInsert(TCHAR *path, unsigned int statcode);
	//////////////////////////////////////////////////////////
	DWORD SetCacheValue(TCHAR *path, unsigned int statcode);
	void CacheClear(unsigned int statcode);
};

// 临界区结构对象
extern CRITICAL_SECTION gcs_DirectoriesCache;
extern CRITICAL_SECTION gcs_PathsCacheVec;
extern CRITICAL_SECTION gcs_FilesCache;
extern CRITICAL_SECTION gcs_ResponseCache;

extern class CCacheObject objCacheObject;
#define CACHE_OBJECT(member)	objCacheObject.member

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#define MODIFY_FILE_STATUS_PERMISSION(OLD_STATUS, NEW_STATUS)					\
	(NEW_STATUS&0x000000F0 && OLD_STATUS&0x0000000F)							\
	|| (NEW_STATUS&0x0000000F && OLD_STATUS&0x0000000F && NEW_STATUS>OLD_STATUS)

#define CHECK_APPEND_REPETITIVE(FILE_ATTRIB, NEW_STATUS)						\
	(FILE_ATTRIB & (NEW_STATUS<<16))

#define ERASE_STATUS_NOT_FOUND(FILE_ATTRIB, NEW_STATUS)							\
	(FILE_ATTRIB & (NEW_STATUS<<16))

#define FILE_CURRENT_STATCODE(FILE_STATUS)				(FILE_STATUS) & 0x0000FFFF
#define FILE_PREVIOUS_STATCODE(FILE_STATUS)				(FILE_STATUS) & 0xFFFF0000
#define ADJUST_FILE_STATUS_CODE(FILE_ATTRIB) {				\
	(FILE_ATTRIB) = FILE_PREVIOUS_STATCODE((FILE_ATTRIB));		\
	if((FILE_ATTRIB) & (FILE_STATUS_CONFLICT<<16))				\
	(FILE_ATTRIB) |= FILE_STATUS_CONFLICT;					\
	else if((FILE_ATTRIB) & (FILE_STATUS_SYNCING<<16))			\
	(FILE_ATTRIB) |= FILE_STATUS_SYNCING;					\
	else if((FILE_ATTRIB) & (FILE_STATUS_ADDED<<16))			\
	(FILE_ATTRIB) |= FILE_STATUS_ADDED;						\
	else if((FILE_ATTRIB) & (FILE_STATUS_MODIFIED<<16))			\
	(FILE_ATTRIB) |= FILE_STATUS_MODIFIED;					\
	else if((FILE_ATTRIB) & (FILE_STATUS_DELETE<<16))			\
	(FILE_ATTRIB) |= FILE_STATUS_DELETE;					\
}

#define APPEND_FILE_STATUS(FILE_ATTRIB, NEW_STATUS)				(FILE_ATTRIB) |= NEW_STATUS << 16
#define ERASE_FILE_STATUS(FILE_ATTRIB, NEW_STATUS)				(FILE_ATTRIB) &= ~(NEW_STATUS<<16)



//At dir cache 
//0xFFFFF000 is index
//0x00000FFF is folder attribute
//#define ROOT_DRIVE_PATH		_T("ROOT_DRIVE_PATH")

//INCREASE_REFERENCE(Command)
//DECREASE_REFERENCE(Command)
#define DIR_CACHE_REFERENCE_INCREASE(DIR_ATTRIB, FILE_STATUS) {	\
	switch(FILE_STATUS) {										\
	case FILE_STATUS_SYNCING:									\
	(DIR_ATTRIB).dwSyncing ++;								\
	break;														\
	case FILE_STATUS_DELETE:									\
	(DIR_ATTRIB).dwDelete ++;								\
	break;														\
	case FILE_STATUS_MODIFIED:									\
	(DIR_ATTRIB).dwModified ++;								\
	break;														\
	case FILE_STATUS_ADDED:										\
	(DIR_ATTRIB).dwAdded ++;								\
	break;														\
	case FILE_STATUS_CONFLICT:									\
	(DIR_ATTRIB).dwConflict ++;								\
	break;														\
}															\
}												

#define DIR_CACHE_REFERENCE_DECREASE(DIR_ATTRIB, FILE_STATUS) {	\
	switch(FILE_STATUS) {										\
	case FILE_STATUS_SYNCING:									\
	(DIR_ATTRIB).dwSyncing --;								\
	break;														\
	case FILE_STATUS_DELETE:									\
	(DIR_ATTRIB).dwDelete --;								\
	break;														\
	case FILE_STATUS_MODIFIED:									\
	(DIR_ATTRIB).dwModified --;								\
	break;														\
	case FILE_STATUS_ADDED:										\
	(DIR_ATTRIB).dwAdded --;								\
	break;														\
	case FILE_STATUS_CONFLICT:									\
	(DIR_ATTRIB).dwConflict --;								\
	break;														\
}															\
}

// can delete 0; can not delete 1
#define DIR_CANT_DELETE(DIR_ATTRIB)								\
	((DIR_ATTRIB).dwSyncing || (DIR_ATTRIB).dwDelete || (DIR_ATTRIB).dwModified || (DIR_ATTRIB).dwAdded || (DIR_ATTRIB).dwConflict)

#define ADJUST_DIR_STATUS_CODE(DIR_ATTRIB) {				\
	if((DIR_ATTRIB).dwConflict)									\
	(DIR_ATTRIB).dirstatus = FILE_STATUS_CONFLICT;			\
	else if((DIR_ATTRIB).dwSyncing)								\
	(DIR_ATTRIB).dirstatus = FILE_STATUS_SYNCING;			\
	else if((DIR_ATTRIB).dwAdded)								\
	(DIR_ATTRIB).dirstatus = FILE_STATUS_ADDED;				\
	else if( (DIR_ATTRIB).dwModified)							\
	(DIR_ATTRIB).dirstatus = FILE_STATUS_MODIFIED;			\
	else if((DIR_ATTRIB).dwDelete)								\
	(DIR_ATTRIB).dirstatus = FILE_STATUS_DELETE;			\
}