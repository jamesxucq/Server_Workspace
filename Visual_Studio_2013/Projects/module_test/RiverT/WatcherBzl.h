#pragma once
#include "clientcom/clientcom.h"
// #include "ExecuteTransmitBzl.h"

class CWatcher {
public:
	CWatcher(void);
	~CWatcher(void);
public:
	DWORD Initialize(TCHAR *szWatchDirectoryPath);
	DWORD Finalize();
private:
	TCHAR m_szWatchingDirectory[MAX_PATH];
public:
	inline TCHAR *GetWatchingDirectory() { return m_szWatchingDirectory; }
public:
	class CFileHandler *m_pFileHandler;
	class CDirectoryWatcher	*m_pFileWatcher;
	///////////////////////////////////////////////////
	unordered_map<wstring, struct file_attrib *> m_hmExceptAddition;
	unordered_map<wstring, struct file_attrib *> m_hmExceptDelete;
	unordered_map<wstring, struct file_attrib *> m_hmExceptModify;
	unordered_map<wstring, struct file_attrib *> m_hmExceptMove;
public:
	DWORD SetExceptWatching(FilesVec *pExceptFiles);
	void ClearExceptWatching();
	void ExceptWatchingErase(const TCHAR *szFileName);
	///////////////////////////////////////////////////
};
extern CWatcher objWatcher;
#define WATCHER_OBJECT(member) objWatcher.member

/////////////////////////////////////////////////////////////////////////////////////
namespace NWatcherBzl {
	////////////////////////////////////////
	static DWORD StartDiresWatching();
	static DWORD StopDiresWatching();
	////////////////////////////////////////
	// return 0; succ/ return -1 error
	DWORD Initialize(const TCHAR *szWatchDirectory);
	DWORD Finalize();

	///////////////////////////////////////////////////
	inline unordered_map <wstring, struct file_attrib *> *GetExceptAdditionPoint()
	{ return &WATCHER_OBJECT(m_hmExceptAddition); }
	inline unordered_map <wstring, struct file_attrib *> *GetExceptDeletePoint()
	{ return &WATCHER_OBJECT(m_hmExceptDelete); }
	inline unordered_map <wstring, struct file_attrib *> *GetExceptModifyPoint()
	{ return &WATCHER_OBJECT(m_hmExceptModify); }
	inline unordered_map <wstring, struct file_attrib *> *GetExceptMovePoint()
	{ return &WATCHER_OBJECT(m_hmExceptMove); }

	///////////////////////////////////////////////////
	inline DWORD SetExceptWatching(FilesVec *pExceptFiles)
	{ return WATCHER_OBJECT(SetExceptWatching(pExceptFiles));}
	inline void ClearExceptWatching()
	{ WATCHER_OBJECT(ClearExceptWatching());}
	inline void ExceptWatchingErase(const TCHAR *szFileName)
	{ WATCHER_OBJECT(ExceptWatchingErase(szFileName)); }
	///////////////////////////////////////////////////
	inline void ClearWatchingCacheBzl() 
	{ WATCHER_OBJECT(m_pFileHandler->DeleteCached()); }
};
