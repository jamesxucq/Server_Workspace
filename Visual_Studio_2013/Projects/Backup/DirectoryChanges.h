#pragma once
#include <afxmt.h>
#include <afxtempl.h>

#include "DirectoryHandler.h"

#define READ_DIR_CHANGE_BUFFER_SIZE 4096

class CDirectoryWatcher;
//this class is used internally by CDirectoryWatcher to help manage the watched directories
class CWatchDirectory {
public:
	CWatchDirectory(HANDLE hDirectory, const TCHAR *szDirectoryName, class CDirectoryHandler *pDirectoryHandler, DWORD dwNotifyFilter);
private:
	~CWatchDirectory();//only I can delete myself....use DeleteSelf()
public:
	void DeleteSelf();

	DWORD StartMonitor(HANDLE hCompPort);
	BOOL UnwatchDirectory( HANDLE hCompPort );
protected:
	BOOL SignalShutdown( HANDLE hCompPort );
	BOOL WaitForShutdown();			   
public:
	BOOL LockProperties() { return m_csProperties.Lock(); }
	BOOL UnlockProperties() { return m_csProperties.Unlock(); }

	class CDelayedSender *GetDelayedSender() const;
	CDirectoryHandler *GetDirectoryHandler() const;//the 'real' change handler is your CDirectoryHandler derived class.
	CDirectoryHandler *SetDirectoryHandler(CDirectoryHandler *pDirectoryHandler);
	BOOL CloseDirectoryHandle();

	//CDirectoryHandler *m_pDirectoryHandler;
	CDelayedSender *m_pDelayedSender;
	HANDLE      m_hDirectory;//handle to directory that we're watching
	DWORD		m_dwNotifyFilter;
	TCHAR		m_szDirectoryName[MAX_PATH];//name of the directory that we're watching
	// CHAR        m_Buffer[ READ_DIR_CHANGE_BUFFER_SIZE ];//buffer for ReadDirectoryChangesW
	CHAR		*m_Buffer; // modify by james 20120326
	DWORD       m_dwBufLength;//length or returned data from ReadDirectoryChangesW -- ignored?...
	OVERLAPPED  m_Overlapped;
	DWORD		m_dwReadDirError;//indicates the success of the call to ReadDirectoryChanges()
	TCHAR		szExistsFileName[MAX_PATH];
	CCriticalSection m_csProperties;
	CEvent		m_StartStopEvent;
	enum eRunningState {
		RUNNING_STATE_NOT_SET, RUNNING_STATE_START_MONITORING, RUNNING_STATE_STOP, RUNNING_STATE_STOP_STEP2,
		RUNNING_STATE_STOPPED, RUNNING_STATE_NORMAL
	};
	eRunningState m_RunningState;
};//end nested class CWatchDirectory 

class CFileNotifyInformation {
public:
	CFileNotifyInformation( BYTE * lpFileNotifyInfoBuffer, DWORD dwBuffSize)
		:m_pBuffer(lpFileNotifyInfoBuffer)
		,m_dwBufferSize(dwBuffSize) 
	{
			m_pCurrentRecord = (PFILE_NOTIFY_INFORMATION) m_pBuffer;
	}
	BOOL GetNextNotifyInformation();

	DWORD	GetAction() const;//gets the type of file change notifiation
	TCHAR *GetFileName(TCHAR *szFileName)const;//gets the file name from the FILE_NOTIFY_INFORMATION record
	TCHAR *GetFileNameWithPath(TCHAR *szFileName) const;//same as GetFileName() only it prefixes the root path into the file name	
protected:
	BYTE * m_pBuffer;//<--all of the FILE_NOTIFY_INFORMATION records 'live' in the buffer this points to...
	DWORD  m_dwBufferSize;
	PFILE_NOTIFY_INFORMATION m_pCurrentRecord;//this points to the current FILE_NOTIFY_INFORMATION record in m_pBuffer
};


class CDirectoryWatcher {
public:
	CDirectoryWatcher();
	~CDirectoryWatcher();
public:
	//	Starts a watch on a directory:
	DWORD	WatchDirectory(const TCHAR *szDirectory, DWORD dwNotifyFilter, CDirectoryHandler *pDirectoryHandler);
	BOOL	IsWatchingDirectory (const TCHAR *szDirectory)const;
	// int		NumWatchedDirectories()const; //counts # of directories being watched.
	BOOL	UnwatchDirectory(const TCHAR *szDirectory);//stops watching specified directory.
	BOOL	UnwatchAllDirectories();//stops watching ALL watched directories.
private:
	virtual void On_ThreadInitialize(){}//All file change notifications has taken place in the context of a worker thread...do any thread initialization here..
	virtual void On_ThreadExit(){}//do thread cleanup here
	BOOL UnwatchDirectory(CDirectoryHandler * pDirectoryHandler);//called in CDirectoryHandler::~CDirectoryHandler()
	UINT static MonitorDirectoryChanges(LPVOID lpvThis );//the worker thread that monitors directories.

	void ProcessChangeNotifications(CFileNotifyInformation & notify_info, CWatchDirectory * pdi);
	BOOL	UnwatchDirectoryBecauseOfError(CWatchDirectory * pWatchDirectory);//called in case of error.
	int		AddToWatchDirectory(CWatchDirectory * pWatchDirectory);

	//	functions for retrieving the directory watch info based on different parameters
	CWatchDirectory *GetWatchDirectory(const TCHAR *szDirectoryName, int &refer_index)const;
	CWatchDirectory *GetWatchDirectory(CWatchDirectory * pWatchDirectory, int &refer_index)const;
	CWatchDirectory *GetWatchDirectory(CDirectoryHandler * pChangeHandler, int & refer_index)const;

	HANDLE m_hCompPort;	//i/o completion port
	HANDLE m_hThread;	//MonitorDirectoryChanges() thread handle
	DWORD  m_dwThreadID;
	CTypedPtrArray<CPtrArray, CWatchDirectory*> m_DirectoriesToWatch; //holds info about the directories that we're watching.
	CCriticalSection m_csWatchDirectory;
};
