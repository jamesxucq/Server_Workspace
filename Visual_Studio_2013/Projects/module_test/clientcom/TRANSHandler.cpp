#include "StdAfx.h"

#include "client_macro.h"
#include "third_party.h"
#include "Session.h"
#include "FileUtility.h"
#include "AnchorXml.h"
#include "DiresXml.h"
#include "FilesAttribXml.h"
#include "FilesVec.h"

#include "TasksListObject.h"
#include "TRANSCache.h"

#include "TRANSHandler.h"

#include <vector>
using std::vector;

DWORD NTRANSHandler::HandleGetAnchorRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length) {
	struct SessionCtrl *psctrl = &pTRANSSession->control;
	struct InternalValue *psvalue = &pTRANSSession->inte_value;

	TCHAR uniBodyBuffer[DEFAULT_BUFFER];
	nstriconv::utf8_unicode(uniBodyBuffer, body_buffer);
	LOG_DEBUG(_T("%s"), uniBodyBuffer);
	if(!objAnchorXml.ParseAnchorXml(body_buffer, body_length)) psctrl->rwrite_count += body_length;
	//LOG_DEBUG(_T("---------------------------------------"));

	if(psctrl->rwrite_count == psvalue->content_length) return HANDLE_FINISH;
	return HANDLE_UNDONE;
}

DWORD NTRANSHandler::HandleGetListRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length) {
	struct SessionCtrl *psctrl = &pTRANSSession->control;
	struct InternalValue *psvalue = &pTRANSSession->inte_value;

	TCHAR uniBodyBuffer[DEFAULT_BUFFER];
	nstriconv::utf8_unicode(uniBodyBuffer, body_buffer);
	LOG_DEBUG(_T("%s"), uniBodyBuffer);
	switch(psctrl->control_stype) {
	case GLIST_LIST_FILES:
	case GLIST_RECU_FILES:
	case GLIST_ANCH_FILES:
		if(!objFilesAttribXml.ParseFilesAttXml(body_buffer, body_length))
			psctrl->rwrite_count += body_length;
		break;
	case GLIST_RECU_DIRES:
		if(!objDiresXml.ParseDiresNameXml(body_buffer, body_length))
			psctrl->rwrite_count += body_length;
		break;
	}
	//LOG_DEBUG(_T("---------------------------------------"));

	if(psctrl->rwrite_count == psvalue->content_length) return HANDLE_FINISH;
	return HANDLE_UNDONE;
}

DWORD NTRANSHandler::HandleGetFileRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length) {
	struct InternalValue *psvalue = &pTRANSSession->inte_value;
	struct SessionCtrl *psctrl = &pTRANSSession->control;
	DWORD wlen = 0;

	if (!psvalue->content_length) return 0;
	// if (!pTRANSSession || !body_buffer || 0>body_length) return -1;

	if(psctrl->file_stream == INVALID_HANDLE_VALUE) return -1;
	if(!WriteFile(psctrl->file_stream, body_buffer, body_length, &wlen, NULL)) 
		return -1;
	psctrl->rwrite_count += wlen;

	if(psctrl->rwrite_count == psvalue->content_length) return HANDLE_FINISH;
	return HANDLE_UNDONE;
}

DWORD NTRANSHandler::HandleGetCsumRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length) {
	struct InternalValue *psvalue = &pTRANSSession->inte_value;
	struct SessionCtrl *psctrl = &pTRANSSession->control;
	DWORD wlen = 0;

	if (!psvalue->content_length) return 0;
	// if (!pTRANSSession || !body_buffer || 0>body_length) return -1;
	//LOG_DEBUG(_T("psctrl->rwrite_count:%d|psvalue->content_length:%d"), psctrl->rwrite_count, psvalue->content_length);
	if(psctrl->file_stream == INVALID_HANDLE_VALUE) return -1;
	if(!WriteFile(psctrl->file_stream, body_buffer, body_length, &wlen, NULL)) 
		return -1;
	psctrl->rwrite_count += wlen;
	//LOG_DEBUG(_T("psctrl->rwrite_count:%d|psvalue->content_length:%d"), psctrl->rwrite_count, psvalue->content_length);

	if(psctrl->rwrite_count == psvalue->content_length) return HANDLE_FINISH;
	return HANDLE_UNDONE;
}

DWORD NTRANSHandler::HandlePostSend(struct TRANSSession *pTRANSSession, char *body_buffer, size_t *body_length) {
	DWORD slen;
	struct InternalValue *psvalue = &pTRANSSession->inte_value;
	struct SessionCtrl *psctrl = &pTRANSSession->control;

	if (!psvalue->content_length) return 0;
	// if (!pTRANSSession || !body_buffer) return -1;

	if(psctrl->file_stream == INVALID_HANDLE_VALUE) return -1;
	if(!ReadFile(psctrl->file_stream, body_buffer, *body_length, &slen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}
	*body_length = slen;
	psctrl->rwrite_count += (off_t)slen;

	if(psctrl->rwrite_count == psvalue->content_length) return HANDLE_FINISH;
	return (int)slen;
}

DWORD NTRANSHandler::HandlePutFileSend(struct TRANSSession *pTRANSSession, char *body_buffer, size_t *body_length) {
	DWORD slen;
	struct InternalValue *psvalue = &pTRANSSession->inte_value;
	struct SessionCtrl *psctrl = &pTRANSSession->control;

	if (!psvalue->content_length) return 0;
	// if (!pTRANSSession || !body_buffer) return -1;

	if(psctrl->file_stream == INVALID_HANDLE_VALUE) return -1;	
	if(!ReadFile(psctrl->file_stream, body_buffer, *body_length, &slen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}
	*body_length = slen;
	psctrl->rwrite_count += (off_t)slen;

	if(psctrl->rwrite_count == psvalue->content_length) return HANDLE_FINISH;
	return (int)slen;
}

DWORD NTRANSHandler::HandlePutCsumRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length) {
	struct InternalValue *psvalue = &pTRANSSession->inte_value;
	struct SessionCtrl *psctrl = &pTRANSSession->control;
	DWORD wlen = 0;

	if (!psvalue->content_length) return 0;
	// if (!pTRANSSession || !body_buffer || 0>body_length) return -1;

	if(psctrl->file_stream == INVALID_HANDLE_VALUE) return -1;
	if(!WriteFile(psctrl->file_stream, body_buffer, body_length, &wlen, NULL)) 
		return -1;
	psctrl->rwrite_count += wlen;
// BinaryLogger::LogBinaryCode(body_buffer, body_length);
// LOG_DEBUG(_T("psctrl->rwrite_count:%d psvalue->content_length:%d"), psctrl->rwrite_count, psvalue->content_length);

	if(psctrl->rwrite_count == psvalue->content_length) return HANDLE_FINISH;
	return HANDLE_UNDONE;
}


DWORD NTRANSHandler::HandlePutCsumSend(struct TRANSSession *pTRANSSession, char *body_buffer, size_t *body_length) {
	DWORD slen;
	struct InternalValue *psvalue = &pTRANSSession->inte_value;
	struct SessionCtrl *psctrl = &pTRANSSession->control;

	if (!psvalue->content_length) return 0;
	// if (!pTRANSSession || !body_buffer) return -1;

	if(psctrl->file_stream == INVALID_HANDLE_VALUE) return -1;
	if(!ReadFile(psctrl->file_stream, body_buffer, *body_length, &slen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}
	*body_length = slen;
	psctrl->rwrite_count += (off_t)slen;

	if(psctrl->rwrite_count == psvalue->content_length) return HANDLE_FINISH;
	return (int)slen;
}

#define COPY_BUFFER	0x2000  // 8K
DWORD NTRANSHandler::HandleOverwriteModified(struct TaskNode *task_node, const TCHAR *szDriveRoot) {
	HANDLE	hBuildCache = INVALID_HANDLE_VALUE;
	HANDLE	hDestination = INVALID_HANDLE_VALUE;
	TCHAR szFileName[MAX_PATH];

	if(!task_node) return OVERWRITE_STATUS_FAULT;
	if(((TRANSMIT_UNDONE|TRANSMIT_EXCEPTION)&task_node->success_flags) || !(OVERWRITE_UNDONE&task_node->success_flags)) 
		return OVERWRITE_STATUS_FAILED;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// open source and destination file
	hBuildCache = NTRANSCache::OpenBuild(task_node->build_cache);
	if( hBuildCache == INVALID_HANDLE_VALUE ) {
		LOG_WARN( _T("overwrite open file failed. file name is %s.\n"), task_node->build_cache);
		return -1;
	}

	nstriutility::full_path(szFileName, szDriveRoot, task_node->szFileName);
	NFileUtility::CreatFileDirectoryPath(szFileName);
	hDestination = CreateFile( szFileName, 
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hDestination == INVALID_HANDLE_VALUE ) {
		LOG_WARN( _T("overwrite create file failed. file name is %s.\n"), szFileName);
		return -1;
	} 
	// lock on the opened file
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
	OverLapped.Offset = task_node->offset;
	if(!LockFileEx(hDestination, 
		LOCKFILE_EXCLUSIVE_LOCK, 
		0, 
		task_node->build_length, 
		0x00000000, 
		&OverLapped)) {
			LOG_WARN(_T("overwrite lock file %s fail!!!"), szFileName);
			return -1;
	}
	DWORD result = SetFilePointer(hDestination, task_node->offset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) {
		LOG_WARN(_T("overwrite SetFilePointer fail!!!"));
		return -1;
	}
	SetFileTime(hDestination, NULL, NULL, NULL);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// overwrite destination fike
	DWORD dwWritenSize = 0;
	DWORD dwReadSize = 0;
	CHAR Buffer[COPY_BUFFER];

	while(ReadFile(hBuildCache, Buffer, COPY_BUFFER, &dwReadSize, NULL) && dwReadSize>0) {
		WriteFile(hDestination, Buffer, dwReadSize, &dwWritenSize, NULL);
		if(dwReadSize != dwWritenSize) {
			LOG_WARN(_T("fatal write error: %x!!!"), GetLastError());
			return -1;		
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// close source and destination file
	SetFileTime(hDestination, NULL, NULL, &task_node->ftLastWrite);
	// unlock on the opened file
	UnlockFileEx(hDestination, 0, task_node->build_length, 0x00000000, &OverLapped);
	CloseHandle(hDestination); 
	NTRANSCache::DestroyBuild(hBuildCache, task_node->build_cache);

	return OVERWRITE_STATUS_SUCCESS;
}