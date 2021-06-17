#include "StdAfx.h"
//#include <afxmt.h>
// #include "logger.h"

#include "ShellThread.h"
#include "OverlayBzl.h"

#define MKZEO(TEXT) TEXT[0] = _T('\0')

static DWORD dwInitialize = FALSE;
DWORD NOverlayBzl::Initialize() {
	if(!dwInitialize) {
// logger(_T("d:\\explo.log"), _T("%s\r\n"), _T("ppp initialize"));
		// Load location config
		InitializeCriticalSection(&gcsResponse);
		InitializeCriticalSection(&gcsDiresCache);
		InitializeCriticalSection(&gcsPathsVec);
		InitializeCriticalSection(&gcsFilesCache);
		//
		ShellThread::OpenThread();
		dwInitialize = TRUE;
	}
//
	return 0x00;
}

DWORD NOverlayBzl::Finalize() {
	if(dwInitialize) {
		ShellThread::CloseThread();
		// uninit log module
		DeleteCriticalSection(&gcsFilesCache);
		DeleteCriticalSection(&gcsPathsVec);
		DeleteCriticalSection(&gcsDiresCache);
		DeleteCriticalSection(&gcsResponse);
		//		
		dwInitialize = FALSE;
	}
//
	return 0x00;	
}

VOID NOverlayBzl::GetAnswerToResponse(struct CacheResponse* pReply, const struct ShellRequest* pRequest) {
	BOOL bFound = 0;
//
	_tcscpy_s(pReply->path, MAX_PATH, pRequest->path);	
	pReply->fstat = FILE_STATUS_NORMAL;
	// 查普通冲突的目录容器 // only for root path
	if(pRequest->attrib&FILE_ATTRIBUTE_DIRECTORY ||  _T('\0')==*(pRequest->path+0x03)) {
		struct dire_attrib *dire_attri;
		dire_attri = CACHE_OBJECT(DiresCacheFind(pRequest->path));
		if(dire_attri) {
			pReply->fstat = dire_attri->dispstat;
			bFound = TRUE;
		}
	} else { // 查普通冲突的文件容器
		DWORD file_attri = CACHE_OBJECT(FilesCacheFind(pRequest->path));
		if(file_attri != FILE_STATUS_INVALID){
			pReply->fstat = file_attri;
			bFound = TRUE;
		}
	}
	// 没找到查有向下继承属性目录容器
	if(!bFound) { 
		struct path_attrib *path_attri;
		// pReply->fstat = FILE_STATUS_NORMAL;
		path_attri = CACHE_OBJECT(PathsVecFind(pRequest->path));
		if(path_attri) {
			pReply->fstat = path_attri->shell_status;
		}
	}
}

DWORD NOverlayBzl::HandleRequestBzl(LPCWSTR szPath, DWORD dwAttrib) {
	struct ShellRequest tRequest;
	DWORD dwFileStatus = FILE_STATUS_INVALID;
	//
// logger(_T("d:\\icon.log"), _T("dire:%s"), CACHE_OBJECT(m_szWatchDire));
// logger(_T("d:\\icon.log"), _T(" path:%s attrib:%08X"), szPath, dwAttrib);
	if(szPath[0] == CACHE_OBJECT(m_szWatchDire[0])) {
		EnterCriticalSection(&gcsResponse);
		//
		if(_tcscmp(CACHE_OBJECT(tReply).path, szPath)) {
			_tcscpy_s(tRequest.path, MAX_PATH, szPath);
			tRequest.attrib = dwAttrib;	
			NOverlayBzl::GetAnswerToResponse(&CACHE_OBJECT(tReply), &tRequest);
// logger(_T("d:\\icon.log"), _T(" get answer to response."));
		}
		dwFileStatus = CACHE_OBJECT(tReply).fstat;
// logger(_T("d:\\icon.log"), _T(" %08x:%s"), CACHE_OBJECT(tReply).fstat, szPath);
		//
		LeaveCriticalSection(&gcsResponse);
	}
// logger(_T("d:\\icon.log"), _T(" status:%08X\r\n"), dwFileStatus);
	return dwFileStatus;
}

//
// flush response cache
#define FLUSH_RESPONSE_CACHE(REPLY_PATH) \
	EnterCriticalSection(&gcsResponse); \
	if(!_tcsncmp(REPLY_PATH, pCommand->path, _tcslen(REPLY_PATH))) \
		MKZEO(REPLY_PATH); \
	LeaveCriticalSection(&gcsResponse);

#define CLEAR_RESPONSE_CACHE(REPLY_PATH) \
	EnterCriticalSection(&gcsResponse); \
	MKZEO(REPLY_PATH); \
	LeaveCriticalSection(&gcsResponse);

VOID NOverlayBzl::HandleCommandBzl(struct CacheCommand *pCommand) {
	// change cache data 
// logger(_T("d:\\explo.log"), _T("%08x:%s\r\n"), pCommand->fstat, pCommand->path);
	switch(FILE_CONTROL_BYTE(pCommand->fstat)) {
		case CACHE_CONTROL_SETATTRI:
			FLUSH_RESPONSE_CACHE(CACHE_OBJECT(tReply.path))
			CACHE_OBJECT(CacheSeta(pCommand->path, FILE_STATUS_BYTE(pCommand->fstat)));
			break;
		case CACHE_CONTROL_ERASE:
			FLUSH_RESPONSE_CACHE(CACHE_OBJECT(tReply.path))
			CACHE_OBJECT(CacheErase(pCommand->path, FILE_STATUS_BYTE(pCommand->fstat)));
			break;
		case CACHE_CONTROL_CLEAR:
			CLEAR_RESPONSE_CACHE(CACHE_OBJECT(tReply.path))
			CACHE_OBJECT(CacheClear(FILE_STATUS_BYTE(pCommand->fstat)));
			break;
		case CACHE_CONTROL_SETVALUE:
			CACHE_OBJECT(SetCacheValue(pCommand->path, FILE_STATUS_BYTE(pCommand->fstat)));
			break;
		case CACHE_CONTROL_QUIT:
			ShellThread::CloseThread();
			Sleep(512);
			ExitProcess(0); // exit(0);
			break;
		default:
			break;
	}
}
