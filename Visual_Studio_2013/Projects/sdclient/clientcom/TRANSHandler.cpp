#include "StdAfx.h"

#include "client_macro.h"
#include "third_party.h"
#include "Session.h"
#include "FileUtility.h"
#include "AnchorXml.h"
#include "DiresXml.h"
#include "SlowAttXml.h"
#include "DebugPrinter.h"

#include "TaskListObj.h"
#include "ListVObj.h"

#include "TRANSCache.h"

#include "TRANSHandler.h"

#include <vector>
using std::vector;

int NTRANSHandler::HandleGetAnchorRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length) {
    struct SeionCtrl *sctrl = &pTRANSSeion->control;
    struct InteValue *ival = &pTRANSSeion->ivalue;
	// test
// char buff[DEFAULT_BUFFER] = {0};
// strncpy(buff, body_buffer, body_length);
// _LOG_ANSI("%s", buff);
    if(!objAnchorXml.ParseAnchorXml(body_buffer, body_length)) sctrl->rwrite_tatol += body_length;
// _LOG_DEBUG(_T("---------------------------------------"));
    if(ival->content_length == sctrl->rwrite_tatol) return HANDLE_FINISH;
    return HANDLE_UNDONE;
}

int NTRANSHandler::HandleGetListRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length) {
    struct InteValue *ival = &pTRANSSeion->ivalue;
    struct SeionCtrl *sctrl = &pTRANSSeion->control;
    DWORD wlen = 0;
//
    if (!ival->content_length) return HANDLE_FINISH;
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return HANDLE_FAULT;
    if(!WriteFile(sctrl->file_handle, body_buffer, body_length, &wlen, NULL))
        return HANDLE_FAULT;
//
    sctrl->rwrite_tatol += wlen;
    if(ival->content_length == sctrl->rwrite_tatol) return HANDLE_FINISH;
    return HANDLE_UNDONE;
}

int NTRANSHandler::HandleGetFileRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length) {
    struct InteValue *ival = &pTRANSSeion->ivalue;
    struct SeionCtrl *sctrl = &pTRANSSeion->control;
    DWORD wlen = 0;
//
    if (!ival->content_length) return HANDLE_FINISH;
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return HANDLE_FAULT;
    if(!WriteFile(sctrl->file_handle, body_buffer, body_length, &wlen, NULL))
        return HANDLE_FAULT;
//
    sctrl->rwrite_tatol += wlen;
// _LOG_DEBUG(_T("tatol:%d|length:%d"), sctrl->rwrite_tatol, ival->content_length);
    if(ival->content_length == sctrl->rwrite_tatol) return HANDLE_FINISH;
    return HANDLE_UNDONE;
}

static VOID NChksMD5(TCHAR *label, unsigned char *md5sum) {
    TCHAR chksum_str[64];
    for (int inde=0; inde<MD5_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, MD5_TEXT_LENGTH, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
_LOG_DEBUG(_T("%s:%s"), label, chksum_str);
}

int NTRANSHandler::HandleGetChksRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length) {
    struct InteValue *ival = &pTRANSSeion->ivalue;
    struct SeionCtrl *sctrl = &pTRANSSeion->control;
    DWORD wlen = 0;
//
    if (!ival->content_length) return HANDLE_FINISH;
// _LOG_DEBUG(_T("content_length:%d|rwrite_tatol:%d|body_length:%d"), ival->content_length, sctrl->rwrite_tatol, body_length);
// struct simple_chks *pSimpleChks = (struct simple_chks *)body_buffer;
// _LOG_DEBUG(_T("pSimpleChks->offset:%llX"), pSimpleChks->offset);
// ndp::ChksMD5(_T("pSimpleChks->md5_chks"), pSimpleChks->md5_chks);
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return HANDLE_FAULT;
	if(!WriteFile(sctrl->file_handle, body_buffer, body_length, &wlen, NULL)) {
_LOG_WARN(_T("body_length:%d wlen:%d GetLastError():%d"), body_length, wlen, GetLastError());
        return HANDLE_FAULT;
	}
//
    sctrl->rwrite_tatol += wlen;
// _LOG_DEBUG(_T("rwrite_tatol:%d|wlen:%d"), sctrl->rwrite_tatol, wlen);
    if(ival->content_length == sctrl->rwrite_tatol) return HANDLE_FINISH;
    return HANDLE_UNDONE;
}

int NTRANSHandler::HandlePostSend(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t *body_length) {
    DWORD dwReadSize;
    struct InteValue *ival = &pTRANSSeion->ivalue;
    struct SeionCtrl *sctrl = &pTRANSSeion->control;
//
    if (!ival->content_length) {
        *body_length = 0x00;
        return HANDLE_FINISH;
    }
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return HANDLE_FAULT;
    if(!ReadFile(sctrl->file_handle, body_buffer, *body_length, &dwReadSize, NULL) || !dwReadSize) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return HANDLE_FAULT;
    }
    *body_length = dwReadSize;
//
    sctrl->rwrite_tatol += (size_t)dwReadSize;
    if(ival->content_length == sctrl->rwrite_tatol) return HANDLE_FINISH;
    return (int)dwReadSize;
}

int NTRANSHandler::HandlePutFileSend(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t *body_length) {
    int send_value = HANDLE_UNDONE;
    DWORD dwReadSize;
    struct InteValue *ival = &pTRANSSeion->ivalue;
    struct SeionCtrl *sctrl = &pTRANSSeion->control;
//
    if (!ival->content_length) {
        *body_length = 0x00;
        return HANDLE_FINISH;
    }
    if (ival->content_length <= (sctrl->rwrite_tatol + *body_length)) {
        *body_length = ival->content_length - sctrl->rwrite_tatol;
        send_value = HANDLE_FINISH;
    }
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return HANDLE_FAULT;
    if(!ReadFile(sctrl->file_handle, body_buffer, *body_length, &dwReadSize, NULL) || !dwReadSize) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return HANDLE_FAULT;
    }
    *body_length = dwReadSize;
//
    sctrl->rwrite_tatol += (size_t)dwReadSize;
    // BinaryLogger::LogBinaryCode(body_buffer, dwReadSize);
    return send_value;
}

int NTRANSHandler::HandlePutChksRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length) {
    struct InteValue *ival = &pTRANSSeion->ivalue;
    struct SeionCtrl *sctrl = &pTRANSSeion->control;
    DWORD wlen = 0;
//
    if (!ival->content_length) return HANDLE_FINISH;
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return HANDLE_FAULT;
    if(!WriteFile(sctrl->file_handle, body_buffer, body_length, &wlen, NULL))
        return HANDLE_FAULT;
//
    sctrl->rwrite_tatol += wlen;
// BinaryLogger::LogBinaryCode(body_buffer, body_length);
// _LOG_DEBUG(_T("sctrl->rwrite_tatol:%d ival->content_length:%d"), sctrl->rwrite_tatol, ival->content_length);
    if(ival->content_length == sctrl->rwrite_tatol) return HANDLE_FINISH;
    return HANDLE_UNDONE;
}

int NTRANSHandler::HandlePutChksSend(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t *body_length) {
    DWORD dwReadSize;
    struct InteValue *ival = &pTRANSSeion->ivalue;
    struct SeionCtrl *sctrl = &pTRANSSeion->control;
//
    if (!ival->content_length) {
        *body_length = 0x00;
        return HANDLE_FINISH;
    }
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return HANDLE_FAULT;
    if(!ReadFile(sctrl->file_handle, body_buffer, *body_length, &dwReadSize, NULL) || !dwReadSize) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return HANDLE_FAULT;
    }
    *body_length = dwReadSize;
//
    sctrl->rwrite_tatol += (size_t)dwReadSize;
    if(ival->content_length == sctrl->rwrite_tatol) return HANDLE_FINISH;
    return (int)dwReadSize;
}

#define COPY_BUFFER	0x2000  // 8K
DWORD NTRANSHandler::HandleOwriteModified(struct TaskNode *task_node, const TCHAR *szDriveLetter) {
    HANDLE	hBuilderCache = INVALID_HANDLE_VALUE;
    HANDLE	hFileLocal = INVALID_HANDLE_VALUE;
    TCHAR szFileName[MAX_PATH];
    if(ONLY_TRANSMIT_VERIFY_OKAY(task_node->success_flags) || !ONLY_OWRTMRVE_OKAY(task_node->success_flags))
        return OWRITE_STATUS_FAULT;
    //
    // open source and desti file
    hBuilderCache = NTRANSCache::OpenBuilder(task_node->builder_cache);
    if( INVALID_HANDLE_VALUE == hBuilderCache ) {
_LOG_WARN( _T("overwrite open file failed. file name is %s."), task_node->builder_cache);
        return OWRITE_STATUS_FAULT;
    }
    struti::full_path(szFileName, szDriveLetter, task_node->szFileName1);
// _LOG_DEBUG(_T("szFileName:%s szDriveLetter:%s task_node->szFileName1:%s"), szFileName, szDriveLetter, task_node->szFileName1);
    NDireUtility::MakeFileFolder(szFileName);
    hFileLocal = CreateFile( szFileName,
                      GENERIC_WRITE,
                      FILE_SHARE_READ | FILE_SHARE_WRITE, // | FILE_SHARE_DELETE,
                      NULL,
                      OPEN_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL);
    if( INVALID_HANDLE_VALUE == hFileLocal ) {
_LOG_WARN( _T("overwrite create file failed. file name is %s."), szFileName);
		NTRANSCache::DestroyBuilder(hBuilderCache, task_node->builder_cache);
        return OWRITE_STATUS_FAILED;
    }
    // lock on the opened file
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
    OverLapped.Offset = task_node->offset & 0xffffffff;
    OverLapped.OffsetHigh = task_node->offset >> 32;
    if(!LockFileEx(hFileLocal,
                   LOCKFILE_EXCLUSIVE_LOCK,
                   0,
                   task_node->build_length,
                   0x00000000,
                   &OverLapped)) {
_LOG_WARN(_T("overwrite lock file %s fail!"), szFileName);
		NTRANSCache::DestroyBuilder(hBuilderCache, task_node->builder_cache);
		CloseHandle(hFileLocal);
        return OWRITE_STATUS_FAILED;
    }
    long lFileSizeHigh = task_node->offset >> 32;
    DWORD dwResult = SetFilePointer(hFileLocal, task_node->offset&0xffffffff, &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
_LOG_WARN(_T("overwrite set file pointer fail!"));
		UnlockFileEx(hFileLocal, 0, task_node->build_length, 0x00000000, &OverLapped);
		NTRANSCache::DestroyBuilder(hBuilderCache, task_node->builder_cache);
		CloseHandle(hFileLocal);
        return OWRITE_STATUS_FAULT;
    }
    // overwrite desti file
	DWORD dwWriteValue = OWRITE_STATUS_SUCCESS;
    DWORD dwWritenSize = 0x00;
    DWORD dwReadSize = 0x00;
    CHAR szBuffer[COPY_BUFFER];

    while(ReadFile(hBuilderCache, szBuffer, COPY_BUFFER, &dwReadSize, NULL) && 0<dwReadSize) {
        WriteFile(hFileLocal, szBuffer, dwReadSize, &dwWritenSize, NULL);
        if(dwReadSize != dwWritenSize) {
_LOG_WARN(_T("fatal write error: %d"), GetLastError());
            dwWriteValue = OWRITE_STATUS_FAULT;
			break;
        }
    }
    // close source and desti file
    // unlock on the opened file
    UnlockFileEx(hFileLocal, 0, task_node->build_length, 0x00000000, &OverLapped);
    CloseHandle(hFileLocal);
    NTRANSCache::DestroyBuilder(hBuilderCache, task_node->builder_cache);
//
    return dwWriteValue;
}