#include "StdAfx.h"

#include "CacheUtility.h"
#include "CacheObject.h"
#include "./utility/ulog.h"

CCacheObject::CCacheObject(void) {
	m_szWatchDire[0] = _T('\0');
}

CCacheObject::~CCacheObject(void) {
}
class CCacheObject objCacheObject;

// 临界区类对象
CRITICAL_SECTION gcsDiresCache;
CRITICAL_SECTION gcsPathsVec;
CRITICAL_SECTION gcsFilesCache;
CRITICAL_SECTION gcsResponse;

const TCHAR *GetSubdirNext(const TCHAR *dirpath, TCHAR **tokep);
const TCHAR *GetFileFolderPathW(const TCHAR *szFileName);

//
DWORD CCacheObject::DireRecursErase(const TCHAR *dirpath, DWORD statcode) {
	unordered_map<wstring, struct dire_attrib *>::iterator dite;
	TCHAR *tokep;
	DWORD reValue = 0x00;
//
	tokep = NULL;
	EnterCriticalSection(&gcsDiresCache);
	while(GetSubdirNext(dirpath, &tokep)) {
		dite = m_mDiresCache.find(dirpath);
		if(m_mDiresCache.end() != dite) {
			DIRE_CACHE_REFERENCE_DECREASE(dite->second, statcode);
			if(!DIRE_CANT_DELETE(dite->second)) {
				free(dite->second);
				m_mDiresCache.erase(dite);
// logger(_T("d:\\invoke.log"), _T("______ erase:%s\r\n"), dirpath);
			} else { // modify the dir status
				ADJUST_DIRE_STATCODE(dite->second);
// logger(_T("d:\\invoke.log"), _T("edr %08x %08x %08x %08x %08x %08x:%s\r\n"),
//	   dite->second->syncing, dite->second->added, dite->second->deleted, 
//	   dite->second->modified, dite->second->conflict, dite->second->dispstat, dirpath);
			}
		} else {
			reValue = ((DWORD)-1);
			break;
		}
	}
	LeaveCriticalSection(&gcsDiresCache);
//
	return reValue;
}

DWORD CCacheObject::DirectoryErase(const TCHAR *dirpath, DWORD statcode) { // 0 succ; 1 not found ; -1 error
	unordered_map<wstring, struct dire_attrib *>::iterator dite;
//
	EnterCriticalSection(&gcsDiresCache);
	dite = m_mDiresCache.find(dirpath);
	if(m_mDiresCache.end() == dite) {
		LeaveCriticalSection(&gcsDiresCache);
		return 0x01;
	}
//
	DIRE_CACHE_REFERENCE_DECREASE(dite->second, statcode);
	if(!DIRE_CANT_DELETE(dite->second)) {
		free(dite->second);
		m_mDiresCache.erase(dite);
// logger(_T("d:\\invoke.log"), _T("______ erase:%s\r\n"), dirpath);
	} else { // modify the dir status
		ADJUST_DIRE_STATCODE(dite->second);
// logger(_T("d:\\invoke.log"), _T("ed %08x %08x %08x %08x %08x %08x:%s\r\n"),
//	   dite->second->syncing, dite->second->added, dite->second->deleted, 
//	   dite->second->modified, dite->second->conflict, dite->second->dispstat, dirpath);
	}
	LeaveCriticalSection(&gcsDiresCache);
//
	return 0x00; 
}

DWORD CCacheObject::FileErase(const TCHAR *szFileName, DWORD statcode) { // < 0  succ; 0 not found ; -1 error
	unordered_map<wstring, DWORD>::iterator aite;
	DWORD erstat =  FILE_STATUS_INVALID;
//
	EnterCriticalSection(&gcsFilesCache);
	aite = m_mFilesCache.find(szFileName);
	if(m_mFilesCache.end() == aite) 
		erstat =  FILE_STATUS_INVALID;
	else { // file_status = aite->second;
		if(ERASE_STATUS_NOT_FOUND(aite->second, statcode)) {
			erstat = statcode;
			ERASE_FILE_STATUS(aite->second, statcode);
			ADJUST_FILE_STATUS_BYTE(aite->second);
// logger(_T("d:\\invoke.log"), _T("ef %08x:%s\r\n"), aite->second, szFileName);
		}
		if(FILE_STATUS_INVALID == aite->second) {
			m_mFilesCache.erase(aite); 
// logger(_T("d:\\invoke.log"), _T("______ erase:%s\r\n"), szFileName);
		}
	}
	LeaveCriticalSection(&gcsFilesCache);
//
	return erstat; 
}


VOID CCacheObject::PathVecErase(const TCHAR *path) {
	vector<struct path_attrib *>::iterator iter;
//
	EnterCriticalSection(&gcsPathsVec);
	for(iter=m_vPathsVec.begin(); m_vPathsVec.end()!=iter; ) { // ++iter
		if(!_tcscmp((*iter)->path, path)) {
			free(*iter); 
			iter = m_vPathsVec.erase(iter);
			break;
		} else ++iter;
	}
	LeaveCriticalSection(&gcsPathsVec);
}

const TCHAR *GetSubdirNext(const TCHAR *dirpath, TCHAR **tokep) {
	TCHAR *toke = (*tokep);
	if(NULL == toke) {
		*tokep = (TCHAR *)NCacheUtility::wcsend(dirpath);
		return dirpath;
	}
	while(_T('\\') != *(--toke)) {
		if(toke == dirpath) return NULL;
	}
	if(_T(':') == *(toke - 1))
		*(toke + 1) = _T('\0');	
	else toke[0] = _T('\0');
	*tokep = toke;
	//
	return dirpath;
}

const TCHAR *GetFileFolderPathW(const TCHAR *szFileName) {
	TCHAR *toke;
//
	toke = (TCHAR *)NCacheUtility::wcsend(szFileName);
	while(_T('\\') != *(--toke));
//
	if(_T(':') == *(toke - 1)) 
		*(++toke) = _T('\0');
	else toke[0] =  _T('\0');
//
	return szFileName;
}


DWORD CCacheObject::InsertFile(const TCHAR *szFileName, DWORD statcode) { // 0 succ; 1 found ; -1 error
	unordered_map<wstring, DWORD>::iterator aite;
	DWORD instat = FILE_STATUS_INVALID;
// logger(_T("d:\\invoke.log"), _T("1 szFileName:%s statcode:%X\n"), szFileName, statcode);
	EnterCriticalSection(&gcsFilesCache);
	aite = m_mFilesCache.find(szFileName);
	if(m_mFilesCache.end() != aite) {
		// if(MODIFY_FILE_STATUS_PERMISSION(aite->second, statcode))
		if(!CHECK_APPEND_REPETITIVE(aite->second, statcode)) {
			instat = aite->second;
			APPEND_FILE_STATUS(aite->second, statcode);
			ADJUST_FILE_STATUS_BYTE(aite->second)
// logger(_T("d:\\invoke.log"), _T("if %08x:%s\r\n"), aite->second, szFileName);
		}
	} else { 
		instat = statcode;
		m_mFilesCache[szFileName] = (statcode<<16) | statcode;
// logger(_T("d:\\invoke.log"), _T("if i %08x:%s\r\n"), (statcode<<16) | statcode, szFileName);
	}
	LeaveCriticalSection(&gcsFilesCache);
//
	return instat; 
}

VOID CCacheObject::InsertDireRecurs(const TCHAR *dirpath, DWORD statcode) {
	unordered_map<wstring, struct dire_attrib *>::iterator dite;
	struct dire_attrib *dire_attri;
	TCHAR *tokep;
	//
	tokep = NULL;
	EnterCriticalSection(&gcsDiresCache);
	while(GetSubdirNext(dirpath, &tokep)) {
		dite = m_mDiresCache.find(dirpath);
		if(m_mDiresCache.end() != dite) {
			DIRE_CACHE_REFERENCE_INCREASE(dite->second, statcode);
			ADJUST_DIRE_STATCODE(dite->second);
// logger(_T("d:\\invoke.log"), _T("idr %08x %08x %08x %08x %08x %08x:%s\r\n"),
//	dite->second->syncing, dite->second->added, dite->second->deleted, 
//	dite->second->modified, dite->second->conflict, dite->second->dispstat, dirpath);
		} else {
			dire_attri = NCacheUtility::DireAttrib();
			DIRE_CACHE_REFERENCE_INCREASE(dire_attri, statcode);
			ADJUST_DIRE_STATCODE(dire_attri);
			m_mDiresCache[dirpath] = dire_attri;
// logger(_T("d:\\invoke.log"), _T("idr i %08x %08x %08x %08x %08x %08x:%s\r\n"),
//	dire_attri->syncing, dire_attri->added, dire_attri->deleted, 
//	dire_attri->modified, dire_attri->conflict, dire_attri->dispstat, dirpath);
		}
	}
	LeaveCriticalSection(&gcsDiresCache);
}

VOID CCacheObject::InsertDirectory(const TCHAR *dirpath, DWORD statcode) {
	unordered_map<wstring, struct dire_attrib *>::iterator dite;
	struct dire_attrib *dire_attri;
//
	EnterCriticalSection(&gcsDiresCache);
	dite = m_mDiresCache.find(dirpath);
	if(m_mDiresCache.end() != dite) {
		DIRE_CACHE_REFERENCE_INCREASE(dite->second, statcode);
		ADJUST_DIRE_STATCODE(dite->second);
// logger(_T("d:\\invoke.log"), _T("id %08x %08x %08x %08x %08x %08x:%s\r\n"),
//	   dite->second->syncing, dite->second->added, dite->second->deleted, 
//	   dite->second->modified, dite->second->conflict, dite->second->dispstat, dirpath);
	} else {
		dire_attri = NCacheUtility::DireAttrib();
		DIRE_CACHE_REFERENCE_INCREASE(dire_attri, statcode);
		ADJUST_DIRE_STATCODE(dire_attri);
		m_mDiresCache[dirpath] = dire_attri;
// logger(_T("d:\\invoke.log"), _T("id i %08x %08x %08x %08x %08x %08x:%s\r\n"),
//	   dire_attri->syncing, dire_attri->added, dire_attri->deleted, 
//	   dire_attri->modified, dire_attri->conflict, dire_attri->dispstat, dirpath);
	}
	LeaveCriticalSection(&gcsDiresCache);
}

VOID CCacheObject::InsertPathVec(const TCHAR *path, DWORD statcode) {
	struct path_attrib *path_attri;
	vector<struct path_attrib *>::iterator iter;
//
	EnterCriticalSection(&gcsPathsVec);
	for(iter=m_vPathsVec.begin(); m_vPathsVec.end()!=iter; ++iter) {
		if(!_tcscmp((*iter)->path, path)) {
			(*iter)->shell_status = statcode;
			LeaveCriticalSection(&gcsPathsVec);
			return;
		}
	}
	LeaveCriticalSection(&gcsPathsVec);
//
	path_attri = NCacheUtility::PathAttrib(path, statcode);
	EnterCriticalSection(&gcsPathsVec);
	m_vPathsVec.push_back(path_attri);
	LeaveCriticalSection(&gcsPathsVec);
}

struct dire_attrib *CCacheObject::DiresCacheFind(const TCHAR *key) {
	unordered_map<wstring, struct dire_attrib *>::iterator dite;
	struct dire_attrib *dire_attri = NULL;
//
	EnterCriticalSection(&gcsDiresCache);
	dite = m_mDiresCache.find(key); 
	if(m_mDiresCache.end() != dite)
		dire_attri = dite->second;
	LeaveCriticalSection(&gcsDiresCache);
	//
	return dire_attri;
}

struct path_attrib *CCacheObject::PathsVecFind(const TCHAR *key) {
	struct path_attrib *path_attri = NULL;
	vector<struct path_attrib *>::iterator iter;
//
	EnterCriticalSection(&gcsPathsVec); 
	for(iter=m_vPathsVec.begin(); m_vPathsVec.end()!=iter; ++iter) {
		if(!_tcsncmp(key, (*iter)->path, _tcslen((*iter)->path))) {
			path_attri = (*iter);
			break;
		}
	}	
	LeaveCriticalSection(&gcsPathsVec);
	return path_attri;
}

DWORD CCacheObject::FilesCacheFind(const TCHAR *key) {
	unordered_map<wstring, DWORD>::iterator aite;
	DWORD file_status = FILE_STATUS_INVALID;
//
	EnterCriticalSection(&gcsFilesCache);
	aite = m_mFilesCache.find(key);
	if(m_mFilesCache.end() != aite) 
		file_status = FILE_CURRENT_STATCODE(aite->second);
	LeaveCriticalSection(&gcsFilesCache);
//
	return file_status;
}

#define FILE_ATTRIB_STATCODE(FILE_STATUS)	(FILE_STATUS >> 16)
DWORD CCacheObject::FilesAttribFind(const TCHAR *key) {
	unordered_map<wstring, DWORD>::iterator aite;
	DWORD file_status = FILE_STATUS_INVALID;
//
	EnterCriticalSection(&gcsFilesCache);
	aite = m_mFilesCache.find(key);
	if(m_mFilesCache.end() != aite) 
		file_status = FILE_ATTRIB_STATCODE(aite->second);
	LeaveCriticalSection(&gcsFilesCache);
//
	return file_status;
}


DWORD CCacheObject::CacheErase(const TCHAR *path, DWORD statcode) {
	TCHAR dupkey[MAX_PATH];
// logger(_T("d:\\invoke.log"), _T("------------------------------------\r\n"));
// logger(_T("d:\\invoke.log"), _T("- %08x:%s\r\n"), statcode, path);
	//
	if(FILE_DIRECTORY_BYTE(statcode)) { // 数据有目录属性
		if(FILE_STATUS_FILE & statcode) {
			DWORD erstat = FileErase(path, REALITY_BYTE(statcode));
// logger(_T("d:\\invoke.log"), _T("erstat k %08x:%s\r\n"), erstat, path);
			if(!erstat) return ((DWORD)-1); //
			//
			_tcscpy_s(dupkey, path);
			const TCHAR *dirpath = GetFileFolderPathW(dupkey);
			DireRecursErase(dirpath, REALITY_BYTE(statcode));	
		} else if( FILE_STATUS_DIRECTORY & statcode ) {
			if((FILE_STATUS_LOCKED|FILE_STATUS_READONLY|FILE_STATUS_FORBID) & statcode)
				PathVecErase(path); // 向下的图标继承性
			else DirectoryErase(path, REALITY_BYTE(statcode)); // 向上的图标继承性			
		}
	} else { // 数据没有目录属性
		DWORD erstat = FileErase(path, REALITY_BYTE(statcode));
		if(!erstat) return ((DWORD)-1); //
// logger(_T("d:\\invoke.log"), _T("erstat u %08x:%s\r\n"), erstat, path);
		_tcscpy_s(dupkey, path);
		DireRecursErase(dupkey, REALITY_BYTE(statcode));
	}
	//
	return 0x00;
}

DWORD CCacheObject::CacheSeta(const TCHAR *path, DWORD statcode) {
	TCHAR dupkey[MAX_PATH];	
// logger(_T("d:\\invoke.log"), _T("------------------------------------\r\n"));
// logger(_T("d:\\invoke.log"), _T("+ %08x:%s\r\n"), statcode, path);
	// 向上的图标继承性
	if(FILE_DIRECTORY_BYTE(statcode)) { // 数据有目录属性
		if(FILE_STATUS_FILE & statcode) {
			DWORD instat = InsertFile(path, REALITY_BYTE(statcode));
// logger(_T("d:\\invoke.log"), _T("instat u %08x:%s\r\n"), instat, path);
			if(!instat) return ((DWORD)-1); //
			//
			_tcscpy_s(dupkey, path);
			const TCHAR *dirpath = GetFileFolderPathW(dupkey);
			InsertDireRecurs(dirpath, REALITY_BYTE(statcode));
		} else if(FILE_STATUS_DIRECTORY & statcode) {
			if((FILE_STATUS_LOCKED|FILE_STATUS_READONLY|FILE_STATUS_FORBID) & statcode)
				InsertPathVec(path, REALITY_BYTE(statcode)); // 向下的图标继承性
			else InsertDirectory(path, REALITY_BYTE(statcode));
		}
	} else { // 数据没有目录属性 // 插入文件
		DWORD instat = InsertFile(path, REALITY_BYTE(statcode));
// logger(_T("d:\\invoke.log"), _T("instat k %08x:%s\r\n"), instat, path);
		if(!instat) return ((DWORD)-1); //
		//
		_tcscpy_s(dupkey, path);
		InsertDireRecurs(dupkey, REALITY_BYTE(statcode));
	}
	//
	return 0x00;
}

DWORD CCacheObject::SetCacheValue(TCHAR *path, DWORD statcode) {
	if(WATCH_DIRECTORY & statcode)
		_tcscpy_s(m_szWatchDire, MAX_PATH, path);
//
	return 0x00;
}

VOID CCacheObject::CacheClear(DWORD statcode) {
	if(FILE_STATUS_NORMAL & statcode) {
		EnterCriticalSection(&gcsDiresCache);
		NCacheUtility::ClearDireCache(&m_mDiresCache);
		LeaveCriticalSection(&gcsDiresCache);
		//
		EnterCriticalSection(&gcsFilesCache);
		m_mFilesCache.clear(); 
		LeaveCriticalSection(&gcsFilesCache);
	} else if(FILE_STATUS_DIRECTORY & statcode) {
		EnterCriticalSection(&gcsPathsVec); 
		NCacheUtility::ClearPathsVec(&m_vPathsVec);
		LeaveCriticalSection(&gcsPathsVec);
	} else if(WATCH_DIRECTORY & statcode) {
		m_szWatchDire[0] = _T('\0');
	} else {
		m_szWatchDire[0] = _T('\0');
		//
		EnterCriticalSection(&gcsPathsVec); 
		NCacheUtility::ClearPathsVec(&m_vPathsVec);
		LeaveCriticalSection(&gcsPathsVec);
		//
		EnterCriticalSection(&gcsDiresCache);
		NCacheUtility::ClearDireCache(&m_mDiresCache);
		LeaveCriticalSection(&gcsDiresCache);
		//
		EnterCriticalSection(&gcsFilesCache);
		m_mFilesCache.clear();
		LeaveCriticalSection(&gcsFilesCache);
	}
}

