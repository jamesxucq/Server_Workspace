#pragma once
#include "third_party.h"
#include "ValidLayer.h"

class CFileHandler : public CDirectoryHandler {
public:
	CFileHandler(void);
	~CFileHandler(void);
private:
	CValidLayer m_objValidLayer;
public:
	void DeleteCached() { m_objValidLayer.DeleteCached(); }
protected:
	// These functions are called when the directory to watch has had a change made to it
	void On_FileAdded(const TCHAR *szFileName);
	void On_FileRemoved(const TCHAR *szFileName);
	void On_FileModified(const TCHAR *szFileName);
	void On_FileNameChanged(const TCHAR *szExistsName, const TCHAR *szNewName);
	void On_WatchStarted(DWORD dwErrorCode, const TCHAR *szDirectory);
	void On_WatchStopped(const TCHAR *szDirectory);
};

void HandleDirAdded(struct Valid *pValid);
void HandleFileAdded(struct Valid *pValid);
void HandleRemoved(const TCHAR *szFileName);
void HandleModified(const TCHAR *szFileName);
void HandleMoved(const TCHAR *szExistsName, const TCHAR *szNewName);

#define EXCEPT_RECYCLER(FILE_PATH) if(!_tcsncmp(FILE_PATH+2, _T("\\RECYCLER"), 9)) return;