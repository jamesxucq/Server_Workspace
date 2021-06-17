#include "StdAfx.h"
#include "CacheObject.h"

CCacheObject::CCacheObject(void) {
}

CCacheObject::~CCacheObject(void) {
}
class CCacheObject objCacheObject;

// 临界区类对象
CRITICAL_SECTION gcs_DirectoriesCache;
CRITICAL_SECTION gcs_PathsCacheVec;
CRITICAL_SECTION gcs_FilesCache;
CRITICAL_SECTION gcs_ResponseCache;

const TCHAR *GetSubdirNext(const TCHAR *dirpath, TCHAR **strend);
const TCHAR *GetFileDirectoryPath(const TCHAR *szFileName);

DWORD CCacheObject::DirectoryRecuErase(const TCHAR *dirpath, unsigned int statcode) {
	unordered_map<wstring, struct DirAttrib>::iterator dir_iter;
	TCHAR *strend;

	strend = NULL;
	while(GetSubdirNext(dirpath, &strend)) {
		dir_iter = m_hmDirectoriesCache.find(dirpath);
		if(dir_iter != m_hmDirectoriesCache.end()) {
			//struct DirAttrib *ptr;
			//ptr = &(iter->second);
			DIR_CACHE_REFERENCE_DECREASE(dir_iter->second, statcode);
			if(!DIR_CANT_DELETE(dir_iter->second)) {
				// gcs_DirectoriesCache.Lock();
				m_hmDirectoriesCache.erase(dir_iter);
				// gcs_DirectoriesCache.Unlock();
			} else {//modify the dir status
				ADJUST_DIR_STATUS_CODE(dir_iter->second);
			}
		} else return -1;
	}

	return 0;
}

DWORD CCacheObject::DirectoryErase(const TCHAR *dirpath, unsigned int statcode) {		 //0 succ; 1 not found ; -1 error
	unordered_map<wstring, struct DirAttrib>::iterator dir_iter;

	dir_iter = m_hmDirectoriesCache.find(dirpath);
	if(dir_iter == m_hmDirectoriesCache.end()) 
		return 1;

	DIR_CACHE_REFERENCE_DECREASE(dir_iter->second, statcode);
	if(!DIR_CANT_DELETE(dir_iter->second)) 
		m_hmDirectoriesCache.erase(dir_iter);
	else {//modify the dir status
		ADJUST_DIR_STATUS_CODE(dir_iter->second);
	}

	return 0; 
}

DWORD CCacheObject::FileErase(const TCHAR *szFileName, unsigned int statcode) {		 //< 0  succ; 0 not found ; -1 error
	unordered_map<wstring, unsigned int>::iterator file_iter;
	unsigned int erase_status =  FILE_STATUS_UNKNOWN;

	file_iter = m_hmFilesCache.find(szFileName);
	if(file_iter == m_hmFilesCache.end()) 
		erase_status =  FILE_STATUS_UNKNOWN;
	else { //file_status = file_iter->second;
		if(ERASE_STATUS_NOT_FOUND(file_iter->second, statcode)) {
			erase_status = statcode;
			ERASE_FILE_STATUS(file_iter->second, statcode);
			ADJUST_FILE_STATUS_CODE(file_iter->second);
		} else erase_status = 0;
		if(FILE_STATUS_UNKNOWN == file_iter->second) {
			// gcs_FilesCache.Lock();
			m_hmFilesCache.erase(file_iter);
			// gcs_FilesCache.Unlock();
		}
	}

	return erase_status; 
}


void CCacheObject::PathVecErase(const TCHAR *path) {
	vector<struct path_attrib *>::iterator iter;

	for(iter=m_vPathsCacheVec.begin(); iter !=m_vPathsCacheVec.end(); ) { //iter++
		if(!_tcscmp((*iter)->path, path)) {
			//pPathAttrib = *iter;
			free(*iter); 
			// gcs_PathsCacheVec.Lock();
			iter = m_vPathsCacheVec.erase(iter);
			// gcs_PathsCacheVec.Unlock();
			break;
		}
		else iter++;
	}
}

const TCHAR *GetSubdirNext(const TCHAR *dirpath, TCHAR **strend) {
	if(NULL == *strend) {
		*strend = (TCHAR *)dirpath + (wcslen(dirpath) - 1); //
		return dirpath;
	}
	while(_T('\\') != *(--(*strend)))
		if(*strend == dirpath) return NULL;

	if(_T(':') == *((*strend) - 1)) 
		*(*strend + 1) = _T('\0');
	else *(*strend) = _T('\0');

	return dirpath;
}


const TCHAR *GetFileDirectoryPath(const TCHAR *szFileName) {
	TCHAR *strend;

	strend = (TCHAR *)szFileName + wcslen(szFileName); //
	while(_T('\\') != *(--strend));

	if(_T(':') == *(strend - 1)) 
		*(++strend) = _T('\0');
	else *strend =  _T('\0');

	return szFileName;
}

DWORD CCacheObject::InsertFile(const TCHAR *szFileName, unsigned int statcode) { //0 succ; 1 found ; -1 error
	unordered_map<wstring, unsigned int>::iterator file_iter;
	unsigned int instatus = FILE_STATUS_UNKNOWN;
	//wprintf(_T("1 szFileName:%s statcode:%X\n"), szFileName, statcode);
	file_iter = m_hmFilesCache.find(szFileName);
	if(file_iter != m_hmFilesCache.end()) {
		// if(MODIFY_FILE_STATUS_PERMISSION(file_iter->second, statcode))
		if(!CHECK_APPEND_REPETITIVE(file_iter->second, statcode)) {
			instatus = file_iter->second;
			APPEND_FILE_STATUS(file_iter->second, statcode);
			ADJUST_FILE_STATUS_CODE(file_iter->second)		
		} else instatus = -1;
	} else {
		EnterCriticalSection(&gcs_FilesCache);
		m_hmFilesCache[szFileName] = (statcode<<16) | statcode;
		LeaveCriticalSection(&gcs_FilesCache);
	}

	return instatus; 
}

void CCacheObject::InsertDirectoryRecu(const TCHAR *dirpath, unsigned int statcode) {
	unordered_map<wstring, struct DirAttrib>::iterator dir_iter;
	struct DirAttrib tDirAttrib;
	TCHAR *strend;

	strend = NULL;
	while(GetSubdirNext(dirpath, &strend)) {
		memset(&tDirAttrib, '\0', sizeof(struct DirAttrib));
		dir_iter = m_hmDirectoriesCache.find(dirpath);
		if(dir_iter != m_hmDirectoriesCache.end()) {
			DIR_CACHE_REFERENCE_INCREASE(dir_iter->second, statcode);
			ADJUST_DIR_STATUS_CODE(dir_iter->second);
		} else {
			DIR_CACHE_REFERENCE_INCREASE(tDirAttrib, statcode);
			ADJUST_DIR_STATUS_CODE(tDirAttrib);

			EnterCriticalSection(&gcs_DirectoriesCache);
			m_hmDirectoriesCache[dirpath] =  tDirAttrib;
			LeaveCriticalSection(&gcs_DirectoriesCache);
		}
	}
}

void CCacheObject::InsertDirectory(TCHAR *dirpath, unsigned int statcode) {
	unordered_map<wstring, struct DirAttrib>::iterator dir_iter;
	struct DirAttrib tDirAttrib;

	memset(&tDirAttrib, '\0', sizeof(struct DirAttrib));
	dir_iter = m_hmDirectoriesCache.find(dirpath);
	if(dir_iter != m_hmDirectoriesCache.end()) {
		DIR_CACHE_REFERENCE_INCREASE(dir_iter->second, statcode);
		ADJUST_DIR_STATUS_CODE(dir_iter->second);
	} else {
		DIR_CACHE_REFERENCE_INCREASE(tDirAttrib, statcode);
		ADJUST_DIR_STATUS_CODE(tDirAttrib);

		EnterCriticalSection(&gcs_DirectoriesCache);
		m_hmDirectoriesCache[dirpath] = tDirAttrib;
		LeaveCriticalSection(&gcs_DirectoriesCache);
	}
}

void CCacheObject::InsertPathVec(const TCHAR *path, unsigned int statcode) {
	struct path_attrib *pPathAttrib;
	vector<struct path_attrib *>::iterator iter;

	EnterCriticalSection(&gcs_PathsCacheVec);
	for(iter=m_vPathsCacheVec.begin(); iter !=m_vPathsCacheVec.end(); iter++) {
		if(!_tcscmp((*iter)->path, path)) {
			(*iter)->shell_status = statcode;
			LeaveCriticalSection(&gcs_PathsCacheVec);
			return;
		}
	}
	LeaveCriticalSection(&gcs_PathsCacheVec);

	pPathAttrib = (struct path_attrib *)malloc(sizeof(struct path_attrib));
	_tcscpy_s(pPathAttrib->path, MAX_PATH, path);
	pPathAttrib->shell_status = statcode;

	EnterCriticalSection(&gcs_PathsCacheVec);
	m_vPathsCacheVec.push_back(pPathAttrib);
	LeaveCriticalSection(&gcs_PathsCacheVec);
}

struct DirAttrib *CCacheObject::DirectoryCacheFind(const TCHAR *key) {
	unordered_map<wstring, struct DirAttrib>::iterator dir_iter;

	EnterCriticalSection(&gcs_DirectoriesCache);
	dir_iter = m_hmDirectoriesCache.find(key); 
	LeaveCriticalSection(&gcs_DirectoriesCache);
	if(dir_iter != m_hmDirectoriesCache.end()) {
		return &(dir_iter->second);
	}

	return NULL;
}

struct path_attrib *CCacheObject::PathCacheFind(const TCHAR *key) {
	struct path_attrib *pPathAttrib = NULL;
	vector<struct path_attrib *>::iterator iter;

	EnterCriticalSection(&gcs_PathsCacheVec); 
	for(iter=m_vPathsCacheVec.begin(); iter !=m_vPathsCacheVec.end(); iter++) {
		if(!_tcsncmp(key, (*iter)->path, _tcslen((*iter)->path))) {
			pPathAttrib = (*iter);
			break;
		}
	}	
	LeaveCriticalSection(&gcs_PathsCacheVec);

	return pPathAttrib;
}

DWORD CCacheObject::FileCacheFind(const TCHAR *key) {
	unordered_map<wstring, unsigned int>::iterator file_iter;

	EnterCriticalSection(&gcs_FilesCache);
	file_iter = m_hmFilesCache.find(key);
	LeaveCriticalSection(&gcs_FilesCache);
	if(file_iter != m_hmFilesCache.end()) 
		return FILE_CURRENT_STATCODE(file_iter->second);

	return FILE_STATUS_UNKNOWN;
}

#define FILE_ATTRIB_STATCODE(FILE_STATUS)				(FILE_STATUS >> 16)
DWORD CCacheObject::FileCacheFindExt(const TCHAR *key) {
	unordered_map<wstring, unsigned int>::iterator file_iter;

	file_iter = m_hmFilesCache.find(key);
	if(file_iter != m_hmFilesCache.end()) 
		return FILE_ATTRIB_STATCODE(file_iter->second);

	return FILE_STATUS_UNKNOWN;
}


DWORD CCacheObject::CacheErase(const TCHAR *key, unsigned int statcode) {
	TCHAR dupkey[MAX_PATH];	
	_tcscpy_s(dupkey, key);

	if( FILE_STATUS_DIRECTORY & statcode ) {
		//
	}
	if( FILE_STATUS_DIRECTORY & statcode ) {
		if(FILE_STATUS_LOCKED&statcode || FILE_STATUS_READONLY&statcode || FILE_STATUS_FORBID & statcode) //向下的图标继承性
			PathVecErase(key);
		else DirectoryErase(key, statcode); //向上的图标继承性			
	} else {
		unsigned int erase_status = FileErase(key, statcode);
		if(!erase_status) return 0;
		else if(erase_status <= 0) return -1;

		const TCHAR *dirpath = GetFileDirectoryPath(dupkey);
		DirectoryRecuErase(dirpath, statcode);	
	}

	return 0;
}

DWORD CCacheObject::CacheInsert(TCHAR *path, unsigned int statcode) {
	TCHAR szDuplicateDirectory[MAX_PATH];	
	_tcscpy_s(szDuplicateDirectory, path);

	//向下的图标继承性
	if(FILE_STATUS_LOCKED&statcode || FILE_STATUS_READONLY & statcode || FILE_STATUS_FORBID & statcode) {
		InsertPathVec(path, statcode);
		return 0;
	}
	//向上的图标继承性
	if(FILE_STATUS_DIRECTORY & statcode) InsertDirectory(path, statcode);
	else {
		unsigned int instatus = InsertFile(path, statcode);
		if( instatus < 0 ) return -1;

		const TCHAR *dirpath = GetFileDirectoryPath(szDuplicateDirectory);
		InsertDirectoryRecu(dirpath, statcode);
	}

	return 0;
}

DWORD CCacheObject::SetCacheValue(TCHAR *path, unsigned int statcode) {
	if(WATCHED_DIRECTORY & statcode)
		_tcscpy_s(m_wsWatchingDirectory, MAX_PATH, path);
	//else if(FILE_STATUS_WORKFILE & statcode)
	//	_tcscpy_s(m_WorkingFilePath, MAX_PATH, path);

	return 0;
}

void CCacheObject::CacheClear(unsigned int statcode) {
	if(FILE_STATUS_NORMAL & statcode) {
		m_hmDirectoriesCache.clear();	 
		m_hmFilesCache.clear(); 
	} else if( FILE_STATUS_DIRECTORY & statcode)
		m_vPathsCacheVec.clear();
	else if(WATCHED_DIRECTORY & statcode)
		_tcscpy_s(m_wsWatchingDirectory, MAX_PATH, _T(""));
	else {
		_tcscpy_s(m_wsWatchingDirectory, MAX_PATH, _T(""));
		m_hmDirectoriesCache.clear();	 
		m_vPathsCacheVec.clear();
		m_hmDirectoriesCache.clear();	 
		m_hmFilesCache.clear();
	}
}

