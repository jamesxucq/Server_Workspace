#include "StdAfx.h"
//#include <afxmt.h>
#include "logger.h"

#include "ShellThread.h"
#include "OverlayBzl.h"

static DWORD dwInitialize = FALSE;
DWORD NOverlayBzl::Initialize() {
	if(!dwInitialize) {
// logger(_T("c:\\explorer.log"), _T("%s"), _T("pppppppppppppppppppp"));
		// Load location config
		InitializeCriticalSection(&gcs_ResponseCache);
		InitializeCriticalSection(&gcs_DirectoriesCache);
		InitializeCriticalSection(&gcs_PathsCacheVec);
		InitializeCriticalSection(&gcs_FilesCache);
		/////////////////////////////////////////////////
		ShellThread::EnsureThreadOpen();
		/////////////////////////////////////////////////		
		dwInitialize = TRUE;
	}

	return 0;
}

DWORD NOverlayBzl::Finalize() {
	if(dwInitialize) {
		ShellThread::CloseThread();
		/////////////////////////////////////////////////
		// uninit log module
		DeleteCriticalSection(&gcs_FilesCache);
		DeleteCriticalSection(&gcs_PathsCacheVec);
		DeleteCriticalSection(&gcs_DirectoriesCache);
		DeleteCriticalSection(&gcs_ResponseCache);
		/////////////////////////////////////////////////		
		dwInitialize = FALSE;
	}

	return 0;	
}

VOID NOverlayBzl::GetAnswerToResponse(struct CacheResponse* pReply, const struct ShellRequest* pRequest) {
	DWORD dwFound = 0;
	DWORD dwRequestPathLength = 0;

	_tcscpy_s(pReply->path, MAX_PATH, pRequest->path);	
	pReply->file_status = FILE_STATUS_NORMAL;

	//查普通冲突的目录容器 //only for root path
	dwRequestPathLength = _tcslen(pRequest->path);
	if(pRequest->dwAttrib&FILE_ATTRIBUTE_DIRECTORY ||  _T('\0')==*(pRequest->path+0x03)) {
		struct DirAttrib *pDirAttrib;

		pDirAttrib = CACHE_OBJECT(DirectoryCacheFind(pRequest->path));
		if(pDirAttrib) {
			pReply->file_status = pDirAttrib->dirstatus;
			dwFound = 1;
		}
	} else { //查普通冲突的文件容器
		unsigned int file_status;

		file_status = CACHE_OBJECT(FileCacheFind(pRequest->path));
		if(file_status != FILE_STATUS_UNKNOWN){
			pReply->file_status = file_status;
			dwFound = 1;
		}
	}
	//没找到查有向下继承属性目录容器
	if(!dwFound) { 
		struct path_attrib *pPathAttrib;

		// pReply->file_status = FILE_STATUS_NORMAL;
		pPathAttrib = CACHE_OBJECT(PathCacheFind(pRequest->path));
		if(pPathAttrib) {
			pReply->file_status = pPathAttrib->shell_status;
		}
	}
}


VOID NOverlayBzl::HandleRequestBzl(DWORD *dwFileStatus, LPCWSTR pwszPath, DWORD dwAttrib) {
	struct ShellRequest tRequest;

	if(pwszPath[0] == *CACHE_OBJECT(m_wsWatchingDirectory)) {
		// 进入临界区
		EnterCriticalSection(&gcs_ResponseCache);

		if(_tcscmp(CACHE_OBJECT(tReply).path, pwszPath)) {
			_tcscpy_s(tRequest.path, _tcslen(pwszPath)+1, pwszPath);
			tRequest.dwAttrib = dwAttrib;			
			NOverlayBzl::GetAnswerToResponse(&CACHE_OBJECT(tReply), &tRequest);
		}
		*dwFileStatus = CACHE_OBJECT(tReply).file_status;

		//离开临界区
		LeaveCriticalSection(&gcs_ResponseCache);
	} else dwFileStatus = FILE_STATUS_UNKNOWN;
}


bool NOverlayBzl::HandleCacheErase(const TCHAR *key, unsigned int statcode) {
	if( FILE_STATUS_NORMAL & statcode) {
		unsigned int file_status;

		file_status = CACHE_OBJECT(FileCacheFindExt(key));
		if(file_status != FILE_STATUS_UNKNOWN) {
			if(file_status & FILE_STATUS_SYNCING) CACHE_OBJECT(CacheErase(key, FILE_STATUS_SYNCING));
			if(file_status & FILE_STATUS_DELETE) CACHE_OBJECT(CacheErase(key, FILE_STATUS_DELETE));
			if(file_status & FILE_STATUS_MODIFIED) CACHE_OBJECT(CacheErase(key, FILE_STATUS_MODIFIED));
			if(file_status & FILE_STATUS_ADDED) CACHE_OBJECT(CacheErase(key, FILE_STATUS_ADDED));
		}
	}
	else CACHE_OBJECT(CacheErase(key, statcode));

	return true;
}

VOID NOverlayBzl::HandleCommandBzl(struct CacheCommand *pCommand) {
	// flush response cache
	if(!_tcscmp(CACHE_OBJECT(tReply.path), pCommand->path)) {
		// 进入临界区
		EnterCriticalSection(&gcs_ResponseCache);
		memset(CACHE_OBJECT(tReply.path), '\0', MAX_PATH);
		//离开临界区
		LeaveCriticalSection(&gcs_ResponseCache);
	}
	// change cache data 
	switch(FILE_CONTROL_CODE(pCommand->file_status)) {
		case CACHE_CONTROL_INSERT:
			CACHE_OBJECT(CacheInsert(pCommand->path, FILE_STATUS_CODE(pCommand->file_status)));
			break;
		case CACHE_CONTROL_ERASE:
			HandleCacheErase(pCommand->path, FILE_STATUS_CODE(pCommand->file_status));
			break;
		case CACHE_CONTROL_CLEAR:
			CACHE_OBJECT(CacheClear(FILE_STATUS_CODE(pCommand->file_status)));
			break;
		case CACHE_CONTROL_SETVALUE:
			CACHE_OBJECT(SetCacheValue(pCommand->path, FILE_STATUS_CODE(pCommand->file_status)));
			break;
		case CACHE_CONTROL_QUIT:
			ShellThread::CloseThread();
			Sleep(512);
			ExitProcess(0);//exit(0);
			break;
		default:
			break;
	}
}
