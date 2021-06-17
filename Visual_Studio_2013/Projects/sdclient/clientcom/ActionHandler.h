#pragma once

#include "ActionUtility.h"

class CActionHandler : public CDirectoryHandler {
public:
    CActionHandler(void);
    ~CActionHandler(void);
private:
    CRITICAL_SECTION m_csValidLink;
    struct Action m_tActioAdd;
    HANDLE m_hActioCache;
    DWORD m_dwLastActio;
public:
    VOID Initialize(HANDLE hActioCache);
    int CacheActioCopy(HANDLE hLockActio);
	void LastActioCopy(TCHAR *szFileName);
	void ClearActioCache();
protected:
    // These functions are called when the directory to watch has had a change made to it
    void On_FileAdded(const TCHAR *szFileName, DWORD ulTimestamp);
    void On_FileRemoved(const TCHAR *szFileName, DWORD ulTimestamp);
    void On_FileModified(const TCHAR *szFileName, DWORD ulTimestamp);
    void On_FileNameChanged(const TCHAR *szExistName, const TCHAR *szNewName, DWORD ulTimestamp);
    void On_WatchStarted(DWORD dwErrorCode, const TCHAR *szDirectory);
    void On_WatchStopped(const TCHAR *szDirectory);
};
