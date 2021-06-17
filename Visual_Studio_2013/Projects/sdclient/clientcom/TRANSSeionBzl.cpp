#include "StdAfx.h"
#include "TRANSSeionBzl.h"

#include "AnchorXml.h"
#include "FileUtility.h"
#include "FileChks.h"
#include "MatchUtility.h"
#include "SDTProtocol.h"
#include "Anchor.h"
#include "DiresXml.h"
#include "SlowAttXml.h"
#include "Session.h"
#include "TRANSCache.h"

#ifdef SWD_APPLICATION
#include "AnchorBzl.h" // modify bu james 20110211
#include "ExecuteTransmitBzl.h" // modify bu james 20110302
#endif

CTRANSSessionBzl::CTRANSSessionBzl(void):
    m_pTRANSSeion(NULL)
{
}

CTRANSSessionBzl::~CTRANSSessionBzl(void) {
}

DWORD CTRANSSessionBzl::Initialize() {
    if(m_pTRANSSeion) free(m_pTRANSSeion);
    m_pTRANSSeion = (struct TRANSSession *) malloc(sizeof(struct TRANSSession));
	if(!m_pTRANSSeion) {
_LOG_FATAL(_T("m_pTRANSSeion is null!"));
		return ((DWORD)-1);
	}
    memset(m_pTRANSSeion, '\0', sizeof(struct TRANSSession));
    //
    return 0x00;
}

VOID CTRANSSessionBzl::Finalize() {
    if(m_pTRANSSeion) free(m_pTRANSSeion);
    m_pTRANSSeion = NULL;
}

struct TRANSSession* CTRANSSessionBzl::Create(struct SessionArgu *pSeionArgu) {
    if(!pSeionArgu) return NULL;
    //
    struct SeionBuffer *sbuffer = &m_pTRANSSeion->buffer;
    struct ListData *ldata = m_pTRANSSeion->ldata = &tListData;
    //
    sbuffer->head_buffer = sbuffer->srecv_buffer;
    sbuffer->body_buffer = NULL; // modify bu james 20100722
    //
    strcon::ustr_utf8(ldata->szUserName, pSeionArgu->szUserName);
    strcon::ustr_utf8(ldata->szPassword, pSeionArgu->szPassword);
    strcon::ustr_utf8(ldata->szLocalIdenti, pSeionArgu->szLocalIdenti);
    _tcscpy_s(ldata->szLocation, pSeionArgu->szDriveLetter);
    //
    return m_pTRANSSeion;
}


DWORD CTRANSSessionBzl::Destroy() {
    return 0x00;
}

struct TRANSSession *CTRANSSessionBzl::GetTRANSSession() {
    return m_pTRANSSeion;
}

struct ListData *CTRANSSessionBzl::GetListData() {
    if (!m_pTRANSSeion) return NULL;
    return m_pTRANSSeion->ldata;
}

DWORD CTRANSSessionBzl::GetNewAncher() {
    if (!m_pTRANSSeion) return ((DWORD)-1);
    return m_pTRANSSeion->ivalue.last_anchor;
}

DWORD CTRANSSessionBzl::OpenHeadListSe(DWORD dwListType, DWORD dwAnchor, TCHAR *szDirectory) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    ival->list_type = dwListType;
    switch(dwListType) {
    case HLIST_LIST_DIREC:
    case HLIST_RECU_FILES:
    case HLIST_RECU_DIRES:
        _tcscpy_s(ival->data_buffer, szDirectory);
        break;
    case HLIST_ANCH_FILES:
        _stprintf_s(ival->data_buffer, _T("%u"), dwAnchor);
        break;
    }
    //
    return 0x00;
}

VOID CTRANSSessionBzl::CloseHeadListSe() {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

DWORD CTRANSSessionBzl::OpenHeadFileSe(TCHAR *szFileName) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    _tcscpy_s(ival->file_name, MAX_PATH, szFileName);
    return 0x00;
}

VOID CTRANSSessionBzl::CloseHeadFileSe() {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

DWORD CTRANSSessionBzl::OpenGetFileSe(struct TaskNode *task_node) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    _tcscpy_s(ival->file_name, MAX_PATH, task_node->szFileName1);
    ival->range_type = TNODE_RANGE_BYTE(task_node->control_code);
    ival->offset = task_node->offset;
    //
    if(INVALID_HANDLE_VALUE != sctrl->file_handle) {
        CloseHandle(sctrl->file_handle);
        sctrl->file_handle = INVALID_HANDLE_VALUE;
    }
    sctrl->file_handle = NTRANSCache::OpenBuilder(task_node->builder_cache);
    if( INVALID_HANDLE_VALUE == sctrl->file_handle ) {
        _LOG_WARN( _T("get session create file failed. file name is %s."), task_node->builder_cache);
        return ((DWORD)-1);
    }
    //
    return 0x00;
}

VOID CTRANSSessionBzl::CloseGetFileSe() {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    NTRANSCache::CloseBuilder(sctrl->file_handle);
    sctrl->file_handle = INVALID_HANDLE_VALUE;
    memset(sctrl, '\0', sizeof(struct SeionCtrl));
	// set last_write at overwrite and finish trans
}

DWORD CTRANSSessionBzl::OpenPutFileSe(struct TaskNode *task_node) { // 0:succ -1:error 0x01:not found 0x02:violation
	struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    struct ListData *ldata = m_pTRANSSeion->ldata;
    TCHAR szFileName[MAX_PATH];
    //
    struti::full_path(szFileName, ldata->szLocation, task_node->szFileName1);
    _tcscpy_s(ival->file_name, MAX_PATH, task_node->szFileName1);
    // NFileUtility::FileLastWrite(&ival->last_write, szFileName);
    NFileUtility::FileSizeTime(&ival->file_size, &ival->last_write, szFileName);
    ival->range_type = TNODE_RANGE_BYTE(task_node->control_code);
    ival->offset = task_node->offset;
    ival->content_length = task_node->build_length;
    ival->content_type = CONTENT_TYPE_OCTET;
    //
    if(INVALID_HANDLE_VALUE != sctrl->file_handle) {
        CloseHandle(sctrl->file_handle);
        sctrl->file_handle = INVALID_HANDLE_VALUE;
    }
    sctrl->file_handle = CreateFile( szFileName,
                                     GENERIC_READ,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL);
    if( INVALID_HANDLE_VALUE == sctrl->file_handle ) {
_LOG_WARN( _T("put session create file failed: %d"), GetLastError() );
_LOG_DEBUG( _T("file name: %s"), szFileName);
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return OPEN_NOT_FOUND;
		} else { return OPEN_VIOLATION; }
    }
    // lock on the opened file
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
    OverLapped.Offset = task_node->offset & 0xffffffff;
    OverLapped.OffsetHigh = task_node->offset >> 32;
    if(!LockFileEx(sctrl->file_handle,
                   LOCKFILE_EXCLUSIVE_LOCK,
                   0,
                   task_node->build_length,
                   0x00000000,
                   &OverLapped)) {
        _LOG_WARN(_T("put session lock file %s fail!"), szFileName);
        return OPEN_VIOLATION;
    }
    long lFileSizeHigh = task_node->offset >> 32;
    DWORD dwResult = SetFilePointer(sctrl->file_handle, task_node->offset&0xffffffff, &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        _LOG_WARN(_T("put session set file pointer fail!"));
        return OPEN_FAULT;
    }
    //
    return OPEN_SUCCESS;
}

VOID CTRANSSessionBzl::ClosePutFileSe(struct TaskNode *task_node) {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    // unlock on the opened file
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = task_node->offset & 0xffffffff;
    OverLapped.OffsetHigh = task_node->offset >> 32;
    UnlockFileEx(sctrl->file_handle, 0, task_node->build_length, 0x00000000, &OverLapped);
    //
    CloseHandle(sctrl->file_handle);
    sctrl->file_handle = INVALID_HANDLE_VALUE;
    memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

DWORD CTRANSSessionBzl::OpenPutDireSe(struct TaskNode *task_node) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct ListData *ldata = m_pTRANSSeion->ldata;
    TCHAR szFileName[MAX_PATH];
    //
    _tcscpy_s(ival->file_name, MAX_PATH, task_node->szFileName1);
    // NFileUtility::FileLastWrite(&ival->last_write, szFileName);
    struti::full_path(szFileName, ldata->szLocation, task_node->szFileName1);
	CLEAR_LACHR(szFileName)
    if(NFileUtility::FileLastWrite(&ival->last_write, szFileName))
		return ((DWORD)-1);
    //
    return 0x00;
}

VOID CTRANSSessionBzl::ClosePutDireSe(struct TaskNode *task_node) {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

DWORD CTRANSSessionBzl::OpenGetListSe(struct ListNode *list_node) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    ival->list_type = LNODE_LTYPE_BYTE(list_node->control_code);
// _LOG_DEBUG(_T("ival->list_type:%08x"), ival->list_type);
    switch(LNODE_LTYPE_BYTE(list_node->control_code)) {
    case LIST_RECU_FILES:
    case LIST_LIST_DIREC:
    case LIST_RECU_DIRES:
        _tcscpy_s(ival->file_name, MAX_PATH, list_node->directory);
// _LOG_DEBUG(_T("ival->file_name:%s"), ival->file_name);
        break;
    case LIST_ANCH_FILES:
        ival->last_anchor = list_node->llast_anchor;
// _LOG_DEBUG(_T("ival->last_anchor:%d"), ival->last_anchor);
        break;
    }
    ival->offset = list_node->offset;
    ival->range_type = LNODE_RANGE_BYTE(list_node->control_code);
    //
    if(INVALID_HANDLE_VALUE != sctrl->file_handle) {
        CloseHandle(sctrl->file_handle);
        sctrl->file_handle = INVALID_HANDLE_VALUE;
    }
    sctrl->file_handle = NTRANSCache::OpenBuilder(list_node->builder_cache);
    if( INVALID_HANDLE_VALUE == sctrl->file_handle ) {
        _LOG_WARN( _T("get session create file failed. file name is %s."), list_node->builder_cache);
        return ((DWORD)-1);
    }
// _LOG_DEBUG(_T("sctrl->file_handle:%08x list_node->builder_cache:%s"), sctrl->file_handle, list_node->builder_cache);
    //
    return 0x00;
}

VOID CTRANSSessionBzl::CloseGetListSe() {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    NTRANSCache::CloseBuilder(sctrl->file_handle);
// _LOG_DEBUG(_T("sctrl->file_handle:%08x"), sctrl->file_handle);
    sctrl->file_handle = INVALID_HANDLE_VALUE;
    memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

DWORD CTRANSSessionBzl::OpenGetAnchorSe(struct XmlAnchor *pXmlAnchor) {
    if (!pXmlAnchor) return ((DWORD)-1);
    objAnchorXml.Initialize(pXmlAnchor);
    return 0x00;
}

VOID CTRANSSessionBzl::CloseGetAnchorSe() {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    objAnchorXml.Finalize();
    memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

DWORD CTRANSSessionBzl::OpenPutSimpChksSendSe(struct TaskNode *task_node, struct simple_head *phsim) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    _tcscpy_s(ival->file_name, MAX_PATH, task_node->szFileName1);
    //
    DWORD content_length = phsim->tatol * sizeof(struct simple_chks);
    sctrl->file_handle = NFileUtility::BuildChksFile(NULL);
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return ((DWORD)-1);
    DWORD dwWritenSize;
    if(!WriteFile(sctrl->file_handle, phsim->chks, content_length, &dwWritenSize, NULL))
        return ((DWORD)-1);
    DWORD dwResult = SetFilePointer(sctrl->file_handle, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    //
    ival->chksum_type = SIMPLE_CHKS;
    ival->range_type = TNODE_RANGE_BYTE(task_node->control_code);
	ival->offset = task_node->offset;
    ival->content_length = content_length;
    ival->content_type = CONTENT_TYPE_OCTET;
    //
    return 0x00;
}

DWORD CTRANSSessionBzl::OpenPutCompChksSendSe(struct TaskNode *task_node, struct complex_head *phcom) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    _tcscpy_s(ival->file_name, MAX_PATH, task_node->szFileName1);
    //
    DWORD content_length = phcom->tatol * sizeof(struct complex_chks);
    sctrl->file_handle = NFileUtility::BuildChksFile(NULL);
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return ((DWORD)-1);
    DWORD dwWritenSize;
    if(!WriteFile(sctrl->file_handle, phcom->chks, content_length, &dwWritenSize, NULL))
        return ((DWORD)-1);
    DWORD dwResult = SetFilePointer(sctrl->file_handle, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    //
    ival->chksum_type = COMPLEX_CHKS;
    ival->range_type = TNODE_RANGE_BYTE(task_node->control_code);
	ival->offset = task_node->offset;
    ival->content_length = content_length;
    ival->content_type = CONTENT_TYPE_OCTET;
// _LOG_DEBUG(_T("content_length:%d"), content_length); // disable by james 20130410
    return 0x00;
}

VOID CTRANSSessionBzl::ClosePutChksSendSe() {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    sctrl->rwrite_tatol = 0;
/*
    DWORD dwResult = SetFilePointer(sctrl->file_handle, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError())
		return;
    SetEndOfFile(sctrl->file_handle);
    FlushFileBuffers(sctrl->file_handle);
*/
}

DWORD CTRANSSessionBzl::OpenPutChksRecvSe(struct TaskNode *task_node) {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    DWORD dwResult = SetFilePointer(sctrl->file_handle, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    SetEndOfFile(sctrl->file_handle);
    FlushFileBuffers(sctrl->file_handle);
    //
    return 0x00;
}

VOID CTRANSSessionBzl::ClosePutChksRecvSe(vector<struct local_match *> &vlocal_match) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    struct file_matcher matcher;
    DWORD dwReadSize = 0x00;
    size_t inde, match_tatol;
    //
// _LOG_DEBUG(_T("FileSizePoint(sctrl->file_handle):%d"), NFileUtility::FileSizeHandle(sctrl->file_handle)); // disable by james 20130410
    DWORD dwResult = SetFilePointer(sctrl->file_handle, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return;
    //
    for(inde = 0; ival->content_length > inde; inde += match_tatol) {
        match_tatol = sizeof(struct file_matcher);
        if(!ReadFile(sctrl->file_handle, &matcher, sizeof(struct file_matcher), &dwReadSize, NULL) || !dwReadSize) {
            // if(ERROR_HANDLE_EOF != GetLastError()) return ((DWORD)-1);
            break;
        }
// _LOG_DEBUG(_T("match_type:%d offset:%u inde_len:%u"), matcher.match_type, matcher.offset, matcher.inde_len); // disable by james 20130410
        struct local_match *alloc_local = MUtil::AppendLocalMatch(vlocal_match, &matcher);
        if (CONEN_MODI & matcher.match_type) {
            alloc_local->buffer = (unsigned char *) malloc(matcher.inde_len);
			if(!alloc_local->buffer) {
_LOG_FATAL(_T("alloc_local->buffer malloc error!"));
				break;
			}
            if(!ReadFile(sctrl->file_handle, alloc_local->buffer, matcher.inde_len, &dwReadSize, NULL) || !dwReadSize) {
                // if(ERROR_HANDLE_EOF != GetLastError()) return ((DWORD)-1);
                break;
            }
            match_tatol += matcher.inde_len;
        }
    }
    //
    CloseHandle(sctrl->file_handle);
    sctrl->file_handle = INVALID_HANDLE_VALUE;
    //
    memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

DWORD CTRANSSessionBzl::OpenGetChksSe(struct TaskNode *task_node) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    sctrl->file_handle = NFileUtility::BuildComplexFile(NULL);
    if(INVALID_HANDLE_VALUE == sctrl->file_handle) return ((DWORD)-1);
    _tcscpy_s(ival->file_name, task_node->szFileName1);
    ival->chksum_type = COMPLEX_CHKS;
	ival->cache_verify = VERIFY_CACHE;
    ival->range_type = TNODE_RANGE_BYTE(task_node->control_code);
    ival->offset = task_node->offset;
// _LOG_DEBUG(_T("+++++++++++++++++++++++++ ival->file_name:%s ival->range_type:%d ival->chksum_type:%d"), ival->file_name, ival->range_type, ival->chksum_type); // disable by james 20130410
    return 0x00;
}

VOID CTRANSSessionBzl::CloseGetChksSe() {
// _LOG_DEBUG(_T("close get chks seion."));
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    sctrl->rwrite_tatol = 0;
}

DWORD CTRANSSessionBzl::OpenPostSe(struct TaskNode *task_node) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    struct ListData *ldata = m_pTRANSSeion->ldata;
    TCHAR szFileName[MAX_PATH];
    //
    DWORD dwResult = SetFilePointer(sctrl->file_handle, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        _LOG_WARN(_T("post session set file pointer fail!"));
        return ((DWORD)-1);
    }
    //
    _tcscpy_s(ival->file_name, task_node->szFileName1);
    // TCHAR *full_path(TCHAR *fpath, const TCHAR *path, const TCHAR *sub_dire);
    struti::full_path(szFileName, ldata->szLocation, task_node->szFileName1);
    NFileUtility::FileSizeTime(&ival->file_size, &ival->last_write, szFileName);
    ival->range_type = TNODE_RANGE_BYTE(task_node->control_code);
    ival->offset = task_node->offset;
    ival->content_length = (size_t)NFileUtility::FileSizeHandle(sctrl->file_handle);
    ival->content_type = CONTENT_TYPE_OCTET;
    //
    return 0x00;
}

VOID CTRANSSessionBzl::ClosePostSe() {
// _LOG_DEBUG(_T("close post seion."));
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
	sctrl->rwrite_tatol = 0;
    // memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

DWORD CTRANSSessionBzl::OpenMoveServSe(struct TaskNode *task_node) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    //
    _tcscpy_s(ival->file_name, task_node->szFileName1);
    _tcscpy_s(ival->data_buffer, task_node->szFileName2);
    ival->last_write = task_node->last_write;
    //
    return 0x00;
}

DWORD CTRANSSessionBzl::OpenCopyServSe(struct TaskNode *task_node) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    //
    _tcscpy_s(ival->file_name, task_node->szFileName1);
    _tcscpy_s(ival->data_buffer, task_node->szFileName2);
    ival->last_write = task_node->last_write;
    //
    return 0x00;
}

DWORD CTRANSSessionBzl::OpenDeleServSe(struct TaskNode *task_node) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    _tcscpy_s(ival->file_name, task_node->szFileName1);
    return 0x00;
}

//
DWORD CTRANSSessionBzl::OpenSha1VerifySe(struct TaskNode *task_node, HANDLE hFileValid, DWORD dwCacheVerify) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    if(!task_node || INVALID_HANDLE_VALUE==hFileValid)
        return ((DWORD)-1);
    sctrl->file_handle = hFileValid;
    _tcscpy_s(ival->file_name, task_node->szFileName2);
    ival->chksum_type = WHOLE_CHKS;
	ival->cache_verify = dwCacheVerify;
    ival->range_type = RANGE_TYPE_FILE;
    ival->offset = task_node->offset;
    //
    return 0x00;
}

VOID CTRANSSessionBzl::CloseSha1VerifySe() {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    sctrl->rwrite_tatol = 0;
}


DWORD CTRANSSessionBzl::OpenFileVerifySe(struct TaskNode *task_node, HANDLE hFileValid, DWORD dwCacheVerify) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    if(!task_node || INVALID_HANDLE_VALUE==hFileValid)
        return ((DWORD)-1);
    sctrl->file_handle = hFileValid;
    _tcscpy_s(ival->file_name, task_node->szFileName1);
    ival->chksum_type = SIMPLE_CHKS;
	ival->cache_verify = dwCacheVerify;
    ival->range_type = RANGE_TYPE_FILE;
    ival->offset = task_node->offset;
    //
    return 0x00;
}

VOID CTRANSSessionBzl::CloseFileVerifySe() {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    sctrl->rwrite_tatol = 0;
}

DWORD CTRANSSessionBzl::OpenChunkVerifySe(struct TaskNode *task_node, HANDLE hFileValid, DWORD dwCacheVerify) {
    struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    //
    if(!task_node || INVALID_HANDLE_VALUE==hFileValid) return ((DWORD)-1);
    sctrl->file_handle = hFileValid;
    _tcscpy_s(ival->file_name, task_node->szFileName1);
    ival->chksum_type = SIMPLE_CHKS;
	ival->cache_verify = dwCacheVerify;
    ival->range_type = RANGE_TYPE_CHUNK;
    ival->offset = task_node->offset;
    //
    return 0x00;
}

VOID CTRANSSessionBzl::CloseChunkVerifySe() {
    struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    sctrl->rwrite_tatol = 0;
}

VOID CTRANSSessionBzl::KillTRANSSession() {
    if(INVALID_HANDLE_VALUE != m_pTRANSSeion->control.file_handle) {
        CloseHandle(m_pTRANSSeion->control.file_handle);
        m_pTRANSSeion->control.file_handle = INVALID_HANDLE_VALUE;
    }
}

CTRANSSessionBzl *NTRANSSessionBzl::Factory(struct SessionArgu *pSeionArgu) {
    CTRANSSessionBzl *pSeionBzl = new CTRANSSessionBzl();
	if(!pSeionBzl) {
_LOG_FATAL(_T("pSeionBzl is null!"));
		return NULL;
	}
    pSeionBzl->Initialize();
    pSeionBzl->Create(pSeionArgu);
    return pSeionBzl;
}

VOID NTRANSSessionBzl::DestroyObject(CTRANSSessionBzl *pSeionBzl) {
    pSeionBzl->Destroy();
    pSeionBzl->Finalize();
    delete pSeionBzl;
}

VOID CONVERT_SESSION_ARGU(struct SessionArgu *pSeionArgu, struct ClientConfig *pClientConfig) {
    _tcscpy_s((pSeionArgu)->szUserName, pClientConfig->szUserName);
    _tcscpy_s((pSeionArgu)->szPassword, pClientConfig->szPassword);
    _tcscpy_s((pSeionArgu)->szLocalIdenti, pClientConfig->szLocalIdenti);
    _tcscpy_s((pSeionArgu)->szDriveLetter, pClientConfig->szDriveLetter);
}