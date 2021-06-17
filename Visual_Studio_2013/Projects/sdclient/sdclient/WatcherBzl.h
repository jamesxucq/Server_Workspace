#pragma once
#include "clientcom/clientcom.h"
#include "ExceptAction.h"

class CWatcherObj {
public:
	CWatcherObj(void);
	~CWatcherObj(void);
public:
	DWORD Initialize(TCHAR *szLocation, TCHAR *szWatchDirectory);
	DWORD Finalize();
private:
	TCHAR m_szWatchDire[MAX_PATH];
	TCHAR m_szActioCache[MAX_PATH];
	HANDLE m_hActioCache;
public:
	class CActionHandler m_obActionHandler;
	class CDirectoryWatcher	m_obFileWatcher;
	//
	BOOL IsCacheExist();
};
extern CWatcherObj objWatcher;
#define DWATCH_OBJECT(member) objWatcher.member

//
namespace NWatcherBzl {
	// return 0; succ/ return -1 error
	DWORD Initialize(TCHAR *szLocation, const TCHAR *szDriveLetter);
	DWORD Finalize();
	//
#define CACHE_ACTION_COPY(LOCK_ACTIO) \
	DWATCH_OBJECT(m_obActionHandler.CacheActioCopy(LOCK_ACTIO));
#define LAST_ACTION_COPY(FILE_NAME) \
	DWATCH_OBJECT(m_obActionHandler.LastActioCopy(FILE_NAME));
	inline VOID ClearActioCache() 
	{ DWATCH_OBJECT(m_obActionHandler.ClearActioCache()); }
	//
	inline BOOL IsActioCacheExist() // 文件存在,且有数据
	{ return DWATCH_OBJECT(IsCacheExist()); }
};
