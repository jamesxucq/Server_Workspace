#include "StdAfx.h"
#include "DirectoryChanges.h"


static BOOL	EnablePrivilege(LPCTSTR pszPrivName, BOOL fEnable) {    
	BOOL fOk = FALSE;      
	HANDLE hToken;    

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {            
		TOKEN_PRIVILEGES tp = { 1 };        

		if( LookupPrivilegeValue(NULL, pszPrivName, &tp.Privileges[0].Luid)) {
			tp.Privileges[0].Attributes = fEnable ?  SE_PRIVILEGE_ENABLED : 0;
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
			fOk = (GetLastError() == ERROR_SUCCESS);		
		}
		CloseHandle(hToken);	
	}	
	return(fOk);
}

//	these two are required for the FILE_FLAG_BACKUP_SEMANTICS flag used in the call to //  CreateFile() to open the directory handle for ReadDirectoryChangesW
//just to make sure...it's on by default for all users.//<others here as needed>
static int invoke_count = 0;
void PrivilegeEnabler() {
	if(invoke_count++) return;
	/////////////////////////////////////////////////////////////////////////////////////////
	LPCTSTR arPrivelegeNames[] = { SE_BACKUP_NAME, SE_RESTORE_NAME, SE_CHANGE_NOTIFY_NAME };
	for(int index = 0; index < sizeof(arPrivelegeNames) / sizeof(arPrivelegeNames[0]); ++index) {
		if( !EnablePrivilege(arPrivelegeNames[index], TRUE) ) {
			TRACE(_T("Unable to enable privilege: %s	--	GetLastError(): %d\n"), arPrivelegeNames[index], GetLastError());
			TRACE(_T("CDirectoryWatcher notifications may not work as intended due to insufficient access rights/process privileges.\n"));
			TRACE(_T("File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		}
	}
}

static bool IsDirectory(const CString & strPath) {
	DWORD dwAttrib	= GetFileAttributes( strPath );
	return static_cast<bool>( ( dwAttrib != 0xffffffff && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) );
}

CWatchDirectory::CWatchDirectory(HANDLE hDirectory, const TCHAR *szDirectoryName, class CDirectoryHandler *pDirectoryHandler, DWORD dwNotifyFilter)
:m_pDelayedSender( NULL )
,m_hDirectory(hDirectory)
,m_dwNotifyFilter(dwNotifyFilter)
,m_Buffer(NULL)
,m_dwBufLength(0)
,m_dwReadDirError(ERROR_SUCCESS)
,m_StartStopEvent(FALSE, TRUE) //NOT SIGNALLED, MANUAL RESET
,m_RunningState( RUNNING_STATE_NOT_SET )
{	
	//	This object 'decorates' the pChangeHandler passed in so that notifications fire in the context a thread other than
	//	CDirectoryWatcher::MonitorDirectoryChanges()
	//	Supports the include and exclude filters
	_tcscpy_s(m_szDirectoryName, szDirectoryName);

	m_pDelayedSender = new CDelayedSender( pDirectoryHandler );
	if( GetDirectoryHandler() ) GetDirectoryHandler()->AddReference();

	memset(&m_Overlapped, 0, sizeof(OVERLAPPED));
	memset(szExistsFileName, '\0', MAX_PATH);
}

CWatchDirectory::~CWatchDirectory() {
	if( GetDelayedSender() ) {//If this call to CDirectoryChangeHandler::Release() causes m_pChangeHandler to delete itself, 
		//the dtor for CDirectoryChangeHandler will call CDirectoryWatcher::UnwatchDirectory( CDirectoryChangeHandler * ),
		//which will make try to delete this object again. if m_pChangeHandler is NULL, it won't try to delete this object again...
		CDirectoryHandler * pDirectoryHandler = SetDirectoryHandler( NULL );
		if( pDirectoryHandler ) pDirectoryHandler->Release();
	}
	CloseDirectoryHandle();

	delete m_pDelayedSender;
	m_pDelayedSender = NULL;
}

void CWatchDirectory::DeleteSelf() {
	delete this;
}

class CDelayedSender* CWatchDirectory::GetDelayedSender() const { 
	return m_pDelayedSender; 
}

//	The 'real' change handler is the CDirectoryChangeHandler object passed to CDirectoryWatcher::WatchDirectory() -- this is the object that really handles the changes.
CDirectoryHandler *CWatchDirectory::GetDirectoryHandler() const {	
	return m_pDelayedSender->GetChangeHandler(); 
}

//	Allows you to switch out, at run time, which object really handles the change notifications.
CDirectoryHandler * CWatchDirectory::SetDirectoryHandler(CDirectoryHandler * pDirectoryHandler) {
	CDirectoryHandler * pExistsHandler = m_pDelayedSender->GetChangeHandler(); 
	m_pDelayedSender->SetChangeHandler(pDirectoryHandler); 
	return pExistsHandler;
}

//	Closes the directory handle that was opened in CDirectoryWatcher::WatchDirecotry()
BOOL CWatchDirectory::CloseDirectoryHandle()
{
	BOOL b = TRUE;
	if( m_hDirectory != INVALID_HANDLE_VALUE ) {
		b = CloseHandle(m_hDirectory);
		m_hDirectory = INVALID_HANDLE_VALUE;
	}
	return b;
}

/*********************************************
The return value is either ERROR_SUCCESS if ReadDirectoryChangesW is successfull,
or is the value of GetLastError() for when ReadDirectoryChangesW() failed.
**********************************************/
DWORD CWatchDirectory::StartMonitor(HANDLE hCompPort)
{

	//Guard the properties of this object 
	VERIFY( LockProperties() );

	m_RunningState = RUNNING_STATE_START_MONITORING;//set the state member to indicate that the object is to START monitoring the specified directory
	PostQueuedCompletionStatus(hCompPort, sizeof(CWatchDirectory *), (DWORD)this, &m_Overlapped);//make the thread waiting on GetQueuedCompletionStatus() wake up

	VERIFY( UnlockProperties() );//unlock this object so that the thread can get at them...

	//wait for signal that the initial call 
	//to ReadDirectoryChanges has been made
	DWORD dwWait = 0;
	do {
		dwWait = WaitForSingleObject(m_StartStopEvent, 10 * 1000);
		if( dwWait != WAIT_OBJECT_0 ) {
			//	shouldn't ever see this one....but just in case you do, notify me of the problem wesj@hotmail.com.
			TRACE(_T("WARNING! Possible lockup detected. FILE: %s Line: %d\n"), _T( __FILE__ ), __LINE__);
		}
	} while( dwWait != WAIT_OBJECT_0 );

	ASSERT( dwWait == WAIT_OBJECT_0 );
	m_StartStopEvent.ResetEvent();

	return m_dwReadDirError;//This value is set in the worker thread when it first calls ReadDirectoryChangesW().
}

/*******************************************
Sets the running state of the object to stop monitoring a directory,
Causes the worker thread to wake up and to stop monitoring the dierctory
********************************************/
BOOL CWatchDirectory::UnwatchDirectory(HANDLE hCompPort)
{
	// Signal that the worker thread is to stop watching the directory
	if( SignalShutdown(hCompPort) ) {
		//and wait for the thread to signal that it has shutdown
		return WaitForShutdown();
	}
	return FALSE;
}

//	Signals the worker thread(via the I/O completion port) that it is to stop watching the 
//	directory for this object, and then returns.
BOOL CWatchDirectory::SignalShutdown( HANDLE hCompPort )
{
	BOOL bRetVal = FALSE;
	//Lock the properties so that they aren't modified by another thread
	VERIFY( LockProperties() ); //unlikey to fail...

	//set the state member to indicate that the object is to stop monitoring the 
	//directory that this CWatchDirectory is responsible for...
	m_RunningState = CWatchDirectory::RUNNING_STATE_STOP;
	//put this object in the I/O completion port... GetQueuedCompletionStatus() will return it inside the worker thread.
	bRetVal = PostQueuedCompletionStatus(hCompPort, sizeof(CWatchDirectory *), (DWORD)this, &m_Overlapped);
	if( !bRetVal ) {
		TRACE(_T("PostQueuedCompletionStatus() failed! GetLastError(): %d\n"), GetLastError());
	}

	VERIFY( UnlockProperties() );

	return bRetVal;
}

//	This is to be called some time after SignalShutdown().
//Wait for the Worker thread to indicate that the watch has been stopped
BOOL CWatchDirectory::WaitForShutdown() {
	DWORD dwWait;
	bool bWMQuitReceived = false;

	do{
		dwWait	= MsgWaitForMultipleObjects(1, &m_StartStopEvent.m_hObject, FALSE, 5000, QS_ALLINPUT);//wait five seconds
		switch( dwWait ) {
		case WAIT_OBJECT_0:
			//handle became signalled!
			break;
		case WAIT_OBJECT_0 + 1:
			{
				MSG msg;
				while( PeekMessage(&msg, NULL, 0,0, PM_REMOVE ) ) {
					if( msg.message != WM_QUIT) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					} else break;
				}
			}break;
		case WAIT_TIMEOUT:
			{
				TRACE(_T("WARNING: Possible Deadlock detected! ThreadID: %d File: %s Line: %d\n"), GetCurrentThreadId(), _T(__FILE__), __LINE__);
			}break;
		}//end switch(dwWait)
	}while( dwWait != WAIT_OBJECT_0 && !bWMQuitReceived );
	m_StartStopEvent.ResetEvent();

	return (BOOL) (dwWait == WAIT_OBJECT_0 || bWMQuitReceived);
}

/***************
Sets the m_pCurrentRecord to the next FILE_NOTIFY_INFORMATION record.
Even if this return FALSE, (unless m_pCurrentRecord is NULL)
m_pCurrentRecord will still point to the last record in the buffer.
****************/
BOOL CFileNotifyInformation::GetNextNotifyInformation() {
	//is there another record after this one?
	if( m_pCurrentRecord &&	m_pCurrentRecord->NextEntryOffset!=0UL) {
		//set the current record to point to the 'next' record
		PFILE_NOTIFY_INFORMATION pExistsRecord = m_pCurrentRecord;
		m_pCurrentRecord = (PFILE_NOTIFY_INFORMATION) ((LPBYTE)m_pCurrentRecord + m_pCurrentRecord->NextEntryOffset);
		if( (DWORD)((BYTE*)m_pCurrentRecord - m_pBuffer) > m_dwBufferSize ) {
			//we've gone too far.... this data is hosed.
			// This sometimes happens if the watched directory becomes deleted... remove the FILE_SHARE_DELETE flag when using CreateFile() to get the handle to the directory...
			m_pCurrentRecord = pExistsRecord;
		}					
		return (BOOL)(m_pCurrentRecord != pExistsRecord);
	}
	return FALSE;
}

DWORD CFileNotifyInformation::GetAction() const { 
	if( m_pCurrentRecord ) return m_pCurrentRecord->Action;
	return 0UL;
}


TCHAR *CFileNotifyInformation::GetFileName(TCHAR *szFileName) const {
	if( m_pCurrentRecord ) {
		memset(szFileName, '\0', MAX_PATH);
		memcpy(szFileName, m_pCurrentRecord->FileName, min((MAX_PATH * sizeof(WCHAR)), m_pCurrentRecord->FileNameLength));
		return szFileName;
	}
	return NULL;
}		

TCHAR *CFileNotifyInformation::GetFileNameWithPath(IN TCHAR *szFileName) const {
	if( m_pCurrentRecord ) {
		int length = min((MAX_PATH << 1), m_pCurrentRecord->FileNameLength);
		memcpy(szFileName, m_pCurrentRecord->FileName, length);
		szFileName[length >> 1] = '\0';
		return szFileName;
	}
	return NULL;
}

CDirectoryWatcher::CDirectoryWatcher()
:m_hCompPort(NULL)
,m_hThread(NULL)
,m_dwThreadID(0UL) {
}

CDirectoryWatcher::~CDirectoryWatcher() {
	UnwatchAllDirectories();
	if(m_hCompPort) {
		CloseHandle( m_hCompPort );
		m_hCompPort = NULL;
	}
}

DWORD CDirectoryWatcher::WatchDirectory(const TCHAR *szDirectory, DWORD dwNotifyFilter, CDirectoryHandler *pDirectoryHandler) {
	if(!szDirectory || *szDirectory==0 || dwNotifyFilter==0 || pDirectoryHandler == NULL) {
		TRACE(_T("ERROR: You've passed invalid parameters to CDirectoryWatcher::WatchDirectory()\n"));
		::SetLastError(ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}
	//double check that it's really a directory
	if(!IsDirectory(szDirectory)) {
		TRACE(_T("ERROR: CDirectoryWatcher::WatchDirectory() -- %s is not a directory!\n"), szDirectory);
		::SetLastError(ERROR_BAD_PATHNAME);
		return ERROR_BAD_PATHNAME;
	}
	//double check that this directory is not already being watched....
	//if it is, then unwatch it before restarting it...
	if(IsWatchingDirectory(szDirectory)) {
		VERIFY(UnwatchDirectory(szDirectory));
	}
	//	Reference this singleton so that privileges for this process are enabled 
	//	so that it has required permissions to use the ReadDirectoryChangesW API, etc.
	PrivilegeEnabler();
	//open the directory to watch....
	HANDLE hDirectory = CreateFile(szDirectory, 
		FILE_LIST_DIRECTORY, 
		FILE_SHARE_READ | FILE_SHARE_WRITE ,//| FILE_SHARE_DELETE, <-- removing FILE_SHARE_DELETE prevents the user or someone else from renaming or deleting the watched directory. This is a good thing to prevent.
		NULL, //security attributes
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | //<- the required priviliges for this flag are: SE_BACKUP_NAME and SE_RESTORE_NAME.  CPrivilegeEnabler takes care of that.
		FILE_FLAG_OVERLAPPED, //OVERLAPPED!
		NULL);
	if( hDirectory == INVALID_HANDLE_VALUE ) {
		DWORD dwError = GetLastError();
		TRACE(_T("CDirectoryWatcher::WatchDirectory() -- Couldn't open directory for monitoring. %d\n"), dwError);
		::SetLastError(dwError);//who knows if TRACE will cause GetLastError() to return success...probably won't, but set it manually just for fun.
		return dwError;
	}
	//opened the dir!
	CWatchDirectory *pWatchDirectory = new CWatchDirectory( hDirectory, szDirectory, pDirectoryHandler, dwNotifyFilter);
	if( !pWatchDirectory ) {
		TRACE(_T("WARNING: Couldn't allocate a new CWatchDirectory() object --- File: %s Line: %d\n"), _T( __FILE__ ), __LINE__);
		CloseHandle( hDirectory );
		::SetLastError(ERROR_OUTOFMEMORY);
		return ERROR_OUTOFMEMORY;
	}
	//create a IO completion port/or associate this key with
	//the existing IO completion port
	m_hCompPort = CreateIoCompletionPort(hDirectory, m_hCompPort, (DWORD)pWatchDirectory, 0);
	if(m_hCompPort == NULL) {
		TRACE(_T("ERROR -- Unable to create I/O Completion port! GetLastError(): %d File: %s Line: %d"), GetLastError(), _T( __FILE__ ), __LINE__ );
		DWORD dwError = GetLastError();
		pWatchDirectory->DeleteSelf( );
		::SetLastError(dwError);//who knows what the last error will be after index call pWatchDirectory->DeleteSelf(), so set it just to make sure
		return dwError;
	} else {//completion port created/directory associated w/ it successfully
		if( m_hThread == NULL ) {
			//start the thread
			CWinThread * pThread = AfxBeginThread(MonitorDirectoryChanges, this);
			if( !pThread ) {//couldn't create the thread!
				TRACE(_T("CDirectoryWatcher::WatchDirectory()-- AfxBeginThread failed!\n"));
				pWatchDirectory->DeleteSelf( );
				return (GetLastError() == ERROR_SUCCESS)? ERROR_MAX_THRDS_REACHED : GetLastError();
			} else {
				m_hThread	 = pThread->m_hThread;
				m_dwThreadID = pThread->m_nThreadID;
				pThread->m_bAutoDelete = TRUE;//pThread is deleted when thread ends....it's TRUE by default(for CWinThread ptrs returned by AfxBeginThread(threadproc, void*)), but just makin sure.
			}
		}
		if( m_hThread != NULL ) {//thread is running, 
			DWORD dwStarted = pWatchDirectory->StartMonitor( m_hCompPort );
			if( dwStarted != ERROR_SUCCESS ) {//there was a problem!
				TRACE(_T("Unable to watch directory: %s -- GetLastError(): %d\n"), dwStarted);
				pWatchDirectory->DeleteSelf( );
				::SetLastError(dwStarted);//I think this'll set the Err object in a VB app.....
				return dwStarted;
			} else {//ReadDirectoryChangesW was successfull!
				// pDirectoryHandler->ReferencesWatcher( this );//reference is removed when directory is unwatched.
				VERIFY( AddToWatchDirectory( pWatchDirectory ) );
				SetLastError(dwStarted);
				return dwStarted;
			}
		} else {
			::SetLastError(ERROR_MAX_THRDS_REACHED);
			return ERROR_MAX_THRDS_REACHED;
		}
	}
}

/*********************************************
Determines whether or not a directory is being watched
be carefull that you have the same name path name, including the backslash
as was used in the call to WatchDirectory(). ie: "C:\\Temp" is different than "C:\\Temp\\"
**********************************************/
BOOL CDirectoryWatcher::IsWatchingDirectory(const TCHAR *szDirectory) const {
	CSingleLock lock( const_cast<CCriticalSection*>(&m_csWatchDirectory), TRUE);
	int index;
	if( GetWatchDirectory(szDirectory, index) ) return TRUE;
	return FALSE;
}

/********************************************
The worker thread function which monitors directory changes....
********************************************/
UINT CDirectoryWatcher::MonitorDirectoryChanges(LPVOID lpvThisWatcher) {
	DWORD numBytes;
	CWatchDirectory * pdi;
	LPOVERLAPPED lpOverlapped;

	CDirectoryWatcher * pThisWatcher = reinterpret_cast<CDirectoryWatcher*>(lpvThisWatcher);
	pThisWatcher->On_ThreadInitialize();

	do {
		// Retrieve the directory info for this directory
		// through the io port's completion key
		if( !GetQueuedCompletionStatus( pThisWatcher->m_hCompPort, &numBytes, (LPDWORD) &pdi, &lpOverlapped, INFINITE) ) {
			if( !pdi || ( pdi && AfxIsValidAddress(pdi, sizeof(CWatchDirectory))) &&  pdi->m_hDirectory != INVALID_HANDLE_VALUE ) {
				//the directory handle is still open! (we expect this when after we close the directory handle )
#ifdef _DEBUG
				TRACE(_T("GetQueuedCompletionStatus() returned FALSE\nGetLastError(): %d Completion Key: %p lpOverlapped: %p\n"), GetLastError(), pdi, lpOverlapped);
				MessageBeep( static_cast<UINT>(-1) );
#endif
			}
		}

		if ( pdi ) { //pdi will be null if I call PostQueuedCompletionStatus(m_hCompPort, 0,0,NULL);
			bool bObjectShouldBeOk = true;
			try {
				VERIFY( pdi->LockProperties() );//don't give the main thread a chance to change this object
			} catch(...) {
				TRACE(_T("CDirectoryWatcher::MonitorDirectoryChanges() -- pdi->LockProperties() raised an exception!\n"));
				bObjectShouldBeOk = false;
			}
			if( bObjectShouldBeOk ) {
				//while we're working with this object...
				CWatchDirectory::eRunningState Run_State = pdi->m_RunningState ;
				VERIFY( pdi->UnlockProperties() );//let another thread back at the properties...
				switch( Run_State ) 
				{
				case CWatchDirectory::RUNNING_STATE_START_MONITORING:
					{
						if(!pdi->m_Buffer) { // add by james 20120326
							pdi->m_Buffer = (char *)malloc(READ_DIR_CHANGE_BUFFER_SIZE);
							if(!pdi->m_Buffer) {
								if( pdi->GetDelayedSender() )
									pdi->GetDelayedSender()->On_WatchStarted(ERROR_OUTOFMEMORY, pdi->m_szDirectoryName);
							}
						}
						if( !ReadDirectoryChangesW( pdi->m_hDirectory,
							pdi->m_Buffer,//<--FILE_NOTIFY_INFORMATION records are put into this buffer
							READ_DIR_CHANGE_BUFFER_SIZE,
							TRUE,
							pdi->m_dwNotifyFilter,
							&pdi->m_dwBufLength,//this var not set when using asynchronous mechanisms...
							&pdi->m_Overlapped,
							NULL) )//no completion routine!
						{
							pdi->m_dwReadDirError = GetLastError();
							if( pdi->GetDelayedSender() )
								pdi->GetDelayedSender()->On_WatchStarted(pdi->m_dwReadDirError, pdi->m_szDirectoryName);
						} else { //read directory changes was successful!
							//allow it to run normally
							pdi->m_RunningState = CWatchDirectory::RUNNING_STATE_NORMAL;
							pdi->m_dwReadDirError = ERROR_SUCCESS;
							if( pdi->GetDelayedSender() )
								pdi->GetDelayedSender()->On_WatchStarted(ERROR_SUCCESS, pdi->m_szDirectoryName );
						}
						pdi->m_StartStopEvent.SetEvent();//signall that the ReadDirectoryChangesW has been called
						//check CWatchDirectory::m_dwReadDirError to see whether or not ReadDirectoryChangesW succeeded...
					}break;
				case CWatchDirectory::RUNNING_STATE_STOP:
					{
						//We want to shut down the monitoring of the directory
						//that pdi is managing...
						if( pdi->m_hDirectory != INVALID_HANDLE_VALUE ) {
							pdi->CloseDirectoryHandle();
							pdi->m_RunningState = CWatchDirectory::RUNNING_STATE_STOP_STEP2;//back up step...GetQueuedCompletionStatus() will still need to return from the last time that ReadDirectoryChangesW() was called.....
							pdi->GetDelayedSender()->On_WatchStopped( pdi->m_szDirectoryName );
						} else {
							pdi->m_StartStopEvent.SetEvent();//set the event that ReadDirectoryChangesW has returned and no further calls to it will be made...
						}
						if( pdi->m_Buffer ) { // add by james 20120326
							free(pdi->m_Buffer);
							pdi->m_Buffer = NULL; 
						}
					}break;
				case CWatchDirectory::RUNNING_STATE_STOP_STEP2:
					{
						if( pdi->m_hDirectory == INVALID_HANDLE_VALUE )
							pdi->m_StartStopEvent.SetEvent();//signal that no further calls to ReadDirectoryChangesW will be made
						else {//for some reason, the handle is still open..		
							pdi->CloseDirectoryHandle();
						}
					}break;
				case CWatchDirectory::RUNNING_STATE_NORMAL:
					{
						//process the FILE_NOTIFY_INFORMATION records:
						CFileNotifyInformation notify_info( (LPBYTE)pdi->m_Buffer, READ_DIR_CHANGE_BUFFER_SIZE);

						pThisWatcher->ProcessChangeNotifications(notify_info, pdi);
						if( !ReadDirectoryChangesW( pdi->m_hDirectory,
							pdi->m_Buffer,//<--FILE_NOTIFY_INFORMATION records are put into this buffer 
							READ_DIR_CHANGE_BUFFER_SIZE,
							TRUE,
							pdi->m_dwNotifyFilter,
							&pdi->m_dwBufLength,//this var not set when using asynchronous mechanisms...
							&pdi->m_Overlapped,
							NULL) )//no completion routine!
						{
							TRACE(_T("WARNING: ReadDirectoryChangesW has failed during normal operations...failed on directory: %s\n"), pdi->m_szDirectoryName);
							int nOldThreadPriority = GetThreadPriority( GetCurrentThread() );
							SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
							try {
								pdi->m_dwReadDirError = GetLastError();
								pdi->GetDelayedSender()->On_ReadDirectoryChangesError( pdi->m_dwReadDirError, pdi->m_szDirectoryName );
								//Do the shutdown
								pThisWatcher->UnwatchDirectoryBecauseOfError( pdi );
							} catch(...) {
								//just in case of exception, this thread will be set back to 
								//normal priority.
							}
							//	Set the thread priority back to normal.
							SetThreadPriority(GetCurrentThread(), nOldThreadPriority);

						} else {//success, continue as normal
							pdi->m_dwReadDirError = ERROR_SUCCESS;
						}
					}break;
				default:
					TRACE(_T("MonitorDirectoryChanges() -- how did I get here?\n"));
					break;//how did I get here?
				}//end switch( pdi->m_RunningState )
			}//end if( bObjectShouldBeOk )
		}//end if( pdi )
	} while( pdi );

	pThisWatcher->On_ThreadExit();
	return 0; //thread is ending
}

/////////////////////////////////////////////////////////////
//	Processes the change notifications and dispatches the handling of the 
//	notifications to the CDirectoryChangeHandler object passed to WatchDirectory()
void CDirectoryWatcher::ProcessChangeNotifications(IN CFileNotifyInformation & notify_info, IN CWatchDirectory * pdi) {
	TCHAR szFileName[MAX_PATH] = {0};

	if( !pdi || !AfxIsValidAddress(pdi, sizeof(CWatchDirectory)) ) {
		TRACE(_T("Invalid arguments to CDirectoryWatcher::ProcessChangeNotifications() -- pdi is invalid!\n"));
		TRACE(_T("File: %s Line: %d"), _T( __FILE__ ), __LINE__ );
		return;
	}

	CDelayedSender *pDelayedSender = pdi->GetDelayedSender();
	if( !pDelayedSender ) {
		TRACE(_T("CDirectoryWatcher::ProcessChangeNotifications() Unable to continue, pdi->GetChangeHandler() returned NULL!\n"));
		TRACE(_T("File: %s  Line: %d\n"), _T( __FILE__ ), __LINE__ );
		return;
	}

	_tcscpy_s(szFileName, pdi->m_szDirectoryName);
	TCHAR *token = szFileName + _tcslen(pdi->m_szDirectoryName);
	do {
		notify_info.GetFileNameWithPath( token );
		switch( notify_info.GetAction() ) {
		case FILE_ACTION_ADDED:		// a file was added!
			pDelayedSender->On_FileAdded( szFileName ); 
			break;
		case FILE_ACTION_REMOVED:	//a file was removed
			pDelayedSender->On_FileRemoved( szFileName ); 
			break;
		case FILE_ACTION_MODIFIED:
			//a file was changed
			pDelayedSender->On_FileModified( szFileName ); 
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
			_tcscpy_s(pdi->szExistsFileName, szFileName );
			break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			if(*(pdi->szExistsFileName) != '\0')
				pDelayedSender->On_FileNameChanged(pdi->szExistsFileName, szFileName );
			break;
		default:
			TRACE(_T("CDirectoryWatcher::ProcessChangeNotifications() -- unknown FILE_ACTION_ value! : %d\n"), notify_info.GetAction() );
			break;//unknown action
		}
	} while( notify_info.GetNextNotifyInformation() );
}


//unwatch all of the watched directories delete all of the CWatchDirectory objects, kill off the worker thread
BOOL CDirectoryWatcher::UnwatchAllDirectories() {
	if( m_hThread != NULL ) {
		CSingleLock lock( &m_csWatchDirectory, TRUE);

		CWatchDirectory * pWatchDirectory;
		//Unwatch each of the watched directories and delete the CWatchDirectory associated w/ that directory...
		int max = m_DirectoriesToWatch.GetSize();
		for(int index = 0; index < max; ++index) {
			if( (pWatchDirectory = m_DirectoriesToWatch[index]) != NULL ) {
				VERIFY( pWatchDirectory->UnwatchDirectory( m_hCompPort ) );

				m_DirectoriesToWatch.SetAt(index, NULL)	;
				pWatchDirectory->DeleteSelf();
			}
		}
		m_DirectoriesToWatch.RemoveAll();
		//kill off the thread
		PostQueuedCompletionStatus(m_hCompPort, 0, 0, NULL);//The thread will catch this and exit the thread
		//wait for it to exit
		WaitForSingleObject(m_hThread, INFINITE);
		//CloseHandle( m_hThread );//Since thread was started w/ AfxBeginThread() this handle is closed automatically, closing it again will raise an exception
		m_hThread = NULL;
		m_dwThreadID = 0UL;		
		//close the completion port...
		CloseHandle( m_hCompPort );
		m_hCompPort = NULL;

		return TRUE;
	}

	return FALSE;
}

// FUNCTION:UnwatchDirectory(const CString & strDirToStopWatching -- if this directory is being watched, the watch is stopped
BOOL CDirectoryWatcher::UnwatchDirectory(const TCHAR *szDirectory) {
	BOOL bRetVal = FALSE;

	//The io completion port must be open
	if( m_hCompPort != NULL ) {
		CSingleLock lock(&m_csWatchDirectory, TRUE);
		int nIndex = -1;
		CWatchDirectory * pWatchDirectory = GetWatchDirectory(szDirectory, nIndex);
		if( pWatchDirectory != NULL &&	nIndex != -1 ) {
			//stop watching this directory
			VERIFY( pWatchDirectory->UnwatchDirectory( m_hCompPort ) );
			//cleanup the object used to watch the directory
			m_DirectoriesToWatch.SetAt(nIndex, NULL);
			pWatchDirectory->DeleteSelf();
			bRetVal = TRUE;
		}
	}

	return bRetVal;
}

/************************************
This function is called from the dtor of CDirectoryChangeHandler automatically,
but may also be called by a programmer because it's public...
A single CDirectoryChangeHandler may be used for any number of watched directories.
Unwatch any directories that may be using this 
CDirectoryChangeHandler * pChangeHandler to handle changes to a watched directory...
The CWatchDirectory::m_pChangeHandler member of objects in the m_DirectoriesToWatch
array will == pChangeHandler if that handler is being used to handle changes to a directory....
************************************/
BOOL CDirectoryWatcher::UnwatchDirectory(CDirectoryHandler * pChangeHandler) {
	CSingleLock lock(&m_csWatchDirectory, TRUE);

	int nUnwatched = 0;
	int nIndex = -1;
	CWatchDirectory * pWatchDirectory;
	//	go through and unwatch any directory that is 
	//	that is using this pChangeHandler as it's file change notification handler.
	while( (pWatchDirectory = GetWatchDirectory( pChangeHandler, nIndex )) != NULL ) {
		VERIFY( pWatchDirectory->UnwatchDirectory( m_hCompPort ) );
		nUnwatched++;
		m_DirectoriesToWatch.SetAt(nIndex, NULL);
		pWatchDirectory->DeleteSelf( );	
	}
	return (BOOL)(nUnwatched != 0);
}

//	Called in the worker thread in the case that ReadDirectoryChangesW() fails
//	during normal operation. One way to force this to happen is to watch a folder
//	using a UNC path and changing that computer's IP address.
BOOL CDirectoryWatcher::UnwatchDirectoryBecauseOfError(CWatchDirectory * pWatchDirectory) {
	BOOL bRetVal = FALSE;
	if( pWatchDirectory ) {
		CSingleLock lock(&m_csWatchDirectory, TRUE);

		int nIndex = -1;
		if( GetWatchDirectory(pWatchDirectory, nIndex) == pWatchDirectory ) {
			// we are actually watching this....
			//	Remove this CWatchDirectory object from the list of watched directories.
			m_DirectoriesToWatch.SetAt(nIndex, NULL);//mark the space as free for the next watch...
			//	and delete it...
			pWatchDirectory->DeleteSelf( );
		}
	}
	return bRetVal;
}

//	To add the CWatchDirectory  * to an array. The array is used to keep track of which directories are being watched.
//	Add the ptr to the first non-null slot in the array.
int	CDirectoryWatcher::AddToWatchDirectory(CWatchDirectory * pWatchDirectory) {
	CSingleLock lock( &m_csWatchDirectory, TRUE);
	ASSERT( lock.IsLocked() );

	//first try to add it to the first empty slot in m_DirectoriesToWatch
	int max = m_DirectoriesToWatch.GetSize();
	int index;
	for(index = 0; index < max; ++index) {
		if( m_DirectoriesToWatch[index] == NULL ) {
			m_DirectoriesToWatch[index] = pWatchDirectory;
			break;
		}
	}
	if( index == max ) { // there where no empty slots, add it to the end of the array
		try{
			index = m_DirectoriesToWatch.Add( pWatchDirectory );
		} catch(CMemoryException * e) {
			e->ReportError();
			e->Delete();//??? delete this? I thought CMemoryException objects where pre allocated in mfc? -- sample code in msdn does, so will index
			index = -1;
		}
	}

	return (BOOL)(index != -1);
}

//	functions for retrieving the directory watch info based on different parameters
CWatchDirectory * CDirectoryWatcher::GetWatchDirectory(IN const TCHAR *szDirectoryName, OUT int &refer_index)const {
	// can't be watching a directory if it's you don't pass in the name of it...
	if( *szDirectoryName == 0 ) return FALSE;		  //
	CSingleLock lock(const_cast<CCriticalSection*>(&m_csWatchDirectory), TRUE);

	int max = m_DirectoriesToWatch.GetSize();
	CWatchDirectory * pWatchDirectory = NULL;
	for(int index = 0; index < max; ++index ) {
		if( (pWatchDirectory=m_DirectoriesToWatch[index])!=NULL && _tcsicmp(pWatchDirectory->m_szDirectoryName, szDirectoryName)==0 ) {
			refer_index = index;
			return pWatchDirectory;
		}
	}

	return NULL;//NOT FOUND
}

CWatchDirectory * CDirectoryWatcher::GetWatchDirectory(CWatchDirectory * pWatchDirectory, int & refer_index)const {
	CSingleLock lock( const_cast<CCriticalSection*>(&m_csWatchDirectory), TRUE);

	int max = m_DirectoriesToWatch.GetSize();
	CWatchDirectory * pdirectory;
	for(int index = 0; index < max; ++index) {
		if( (pdirectory=m_DirectoriesToWatch[index])!=NULL && pdirectory==pWatchDirectory ) {
			refer_index = index;
			return pdirectory;
		}
	}
	return NULL;//NOT FOUND
}

CWatchDirectory * CDirectoryWatcher::GetWatchDirectory(CDirectoryHandler * pChangeHandler, int &refer_index)const {
	CSingleLock lock( const_cast<CCriticalSection*>(&m_csWatchDirectory), TRUE);

	int max = m_DirectoriesToWatch.GetSize();
	CWatchDirectory * pdirectory;
	for(int index = 0; index < max; ++index) {
		if( (pdirectory=m_DirectoriesToWatch[index])!=NULL && pdirectory->GetDirectoryHandler()==pChangeHandler ) {
			refer_index = index;
			return pdirectory;
		}
	}
	return NULL;//NOT FOUND
}
