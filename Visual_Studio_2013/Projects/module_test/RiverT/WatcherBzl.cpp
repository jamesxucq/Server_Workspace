#include "StdAfx.h"
#include "WatcherBzl.h"


CWatcher::CWatcher(void):
m_pFileWatcher(NULL),
m_pFileHandler(NULL)
{
}

CWatcher::~CWatcher(void) {
}
CWatcher objWatcher;

DWORD CWatcher::Initialize(TCHAR *szWatchDirectoryPath) {
	_tcscpy_s(m_szWatchingDirectory, szWatchDirectoryPath);

	m_pFileHandler = new CFileHandler();
	if(!m_pFileHandler) return -1;

	m_pFileWatcher = new CDirectoryWatcher();
	if(!m_pFileWatcher) return -1;

	return 0;
}

DWORD CWatcher::Finalize() {
	if(m_pFileWatcher) delete m_pFileWatcher;
	m_pFileWatcher = NULL;

	if(m_pFileHandler) delete m_pFileHandler;
	m_pFileHandler = NULL;

	return 0;
}


DWORD CWatcher::SetExceptWatching(FilesVec *pExceptFiles) {
	vector<struct file_attrib *>::iterator iter;	
	m_hmExceptAddition.clear(); m_hmExceptDelete.clear();
	m_hmExceptModify.clear(); m_hmExceptMove.clear();

	for(iter=pExceptFiles->begin(); iter!=pExceptFiles->end(); iter++) {
		switch((*iter)->modify_type) {
		case ADD:
			m_hmExceptAddition[(*iter)->file_name] = *iter;
			break;
		case DEL:
			m_hmExceptDelete[(*iter)->file_name] = *iter;
			break;
		case MODIFY:
			m_hmExceptModify[(*iter)->file_name] = *iter;
			break;
		case EXIST:
			m_hmExceptMove[(*iter)->file_name] = *iter;
			break;
		}
	}

	return 0;
}

void CWatcher::ClearExceptWatching() {
	m_hmExceptAddition.clear();
	m_hmExceptDelete.clear();
	m_hmExceptModify.clear();
	m_hmExceptMove.clear();
}

void CWatcher::ExceptWatchingErase(const TCHAR *szFileName) {
	unordered_map<wstring, struct file_attrib *>::iterator	file_iter;
	///////////////////////////////////////////////////////
	if(!szFileName) return;
	file_iter = m_hmExceptAddition.find(NO_ROOT(szFileName));
	if(file_iter != m_hmExceptAddition.end())
		m_hmExceptAddition.erase(file_iter);
	///////////////////////////////////////////////////////
	file_iter = m_hmExceptDelete.find(NO_ROOT(szFileName));
	if(file_iter != m_hmExceptDelete.end())
		m_hmExceptDelete.erase(file_iter);
	///////////////////////////////////////////////////////
	file_iter = m_hmExceptModify.find(NO_ROOT(szFileName));
	if(file_iter != m_hmExceptModify.end())
		m_hmExceptModify.erase(file_iter);
	///////////////////////////////////////////////////////
	file_iter = m_hmExceptMove.find(NO_ROOT(szFileName));
	if(file_iter != m_hmExceptMove.end())
		m_hmExceptMove.erase(file_iter);
}


// static int watching_reference = 0; // just for test 20120307 by james
DWORD NWatcherBzl::Initialize(const TCHAR *szWatchDirectory) {
	TCHAR szWatchDirectoryPath[MAX_PATH];

	// LOG_DEBUG(_T("--------------------------- create watching_reference:%d directory:%s"), ++watching_reference, szWatchDirectory);
	if (!szWatchDirectory) return -1;

	_tcscpy_s(szWatchDirectoryPath, szWatchDirectory); _tcscat(szWatchDirectoryPath, _T("\\"));
	if(WATCHER_OBJECT(Initialize(szWatchDirectoryPath))) return -1;
	// Set up change notify register
	if(NWatcherBzl::StartDiresWatching()) return -1;

	///////////////////////////////////////////////////////////////
	return 0;
}

DWORD NWatcherBzl::Finalize() {
	// LOG_DEBUG(_T("--------------------------- destroy watching_reference:%d"), --watching_reference);
	///////////////////////////////////////////////////////////////
	if(NWatcherBzl::StopDiresWatching()) return -1;
	WATCHER_OBJECT(Finalize());

	return 0;
}

DWORD NWatcherBzl::StartDiresWatching() {
	class CDirectoryWatcher *pFileWatcher;
	pFileWatcher = WATCHER_OBJECT(m_pFileWatcher);

	DWORD dwChangeFilter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE;
	DWORD dwWatchValue = pFileWatcher->WatchDirectory(WATCHER_OBJECT(GetWatchingDirectory()), dwChangeFilter, WATCHER_OBJECT(m_pFileHandler));
	if(ERROR_SUCCESS != dwWatchValue) return -1;
	//MessageBox(_T("Failed to start watch:\n") + GetLastErrorMessageString( dwWatch ) );
	LOG_DEBUG(_T("----------- dires watching start ok -----------"));

	return 0;
}

DWORD NWatcherBzl::StopDiresWatching() {
	class CDirectoryWatcher *pFileWatcher;
	pFileWatcher = WATCHER_OBJECT(m_pFileWatcher);

	if(!pFileWatcher->UnwatchAllDirectories()) return -1;
	LOG_DEBUG(_T("----------- dires watching stop ok -----------"));

	return 0;
}

//PFILE_NOTIFY_INFORMATION