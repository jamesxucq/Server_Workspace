#include "StdAfx.h"
#include "TRANSSessionBzl.h"

#include "AnchorXml.h"
#include "FileUtility.h"
#include "FileCsums.h"
#include "MatchUtility.h"
#include "SDTProtocol.h"
#include "Anchor.h"
#include "DiresXml.h"
#include "FilesAttribXml.h"
#include "Session.h"
#include "TRANSCache.h"

#ifdef SD_APPLICATION
#include "AnchorBzl.h" //modify bu james 20110211
#include "ExecuteTransmitBzl.h" //modify bu james 20110302
#endif

CTRANSSessionBzl::CTRANSSessionBzl(void):
m_pTRANSSession(NULL)
{
}

CTRANSSessionBzl::~CTRANSSessionBzl(void) {
}

DWORD CTRANSSessionBzl::Initialize() {
	m_pTRANSSession = (struct TRANSSession *) malloc(sizeof(struct TRANSSession)); 
	memset(m_pTRANSSession, '\0', sizeof(struct TRANSSession));

	return 0;
}

void CTRANSSessionBzl::Finalize() {
	if(m_pTRANSSession) free(m_pTRANSSession);
	m_pTRANSSession = NULL;
}

struct TRANSSession* CTRANSSessionBzl::Create(struct SessionArgu *pSessionArgu) {
	if(!pSessionArgu) return NULL;

	//////////////////////////////////////////////////////////////
	struct SessionBuffer *psbuffer = &m_pTRANSSession->buffer;
	struct ListInform *list_inform = m_pTRANSSession->list_inform = &tListInform;

	//////////////////////////////////////////////////////////////
	psbuffer->head_buffer = psbuffer->srecv_buffer;
	psbuffer->body_buffer = NULL; //modify bu james 20100722

	//////////////////////////////////////////////////////////////
	nstriconv::unicode_utf8(list_inform->szUserName, pSessionArgu->szUserName);
	nstriconv::unicode_utf8(list_inform->szPassword, pSessionArgu->szPassword);
	nstriconv::unicode_utf8(list_inform->szLocalIdenti, pSessionArgu->szLocalIdenti);

	_mywscpy_s(list_inform->szLocation, pSessionArgu->szDriveRoot);

#ifdef SD_APPLICATION
	//////////////////////////////////////////////////////////////
	list_inform->dwLastAnchor = NAnchorBzl::GetLastAnchor();
#endif 

	return m_pTRANSSession;
}


DWORD CTRANSSessionBzl::Destroy() {
	return 0;
}

struct TRANSSession *CTRANSSessionBzl::GetTRANSSession() 
{ return m_pTRANSSession; }

struct ListInform *CTRANSSessionBzl::GetListInform() {
	if (!m_pTRANSSession) return NULL;
	return m_pTRANSSession->list_inform;
}

DWORD CTRANSSessionBzl::GetServerNewAncher() {
	if (!m_pTRANSSession) return -1;
	return m_pTRANSSession->inte_value.last_anchor;
}

DWORD CTRANSSessionBzl::OpenGetFileSe(struct TaskNode *task_node) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control);

	///////////////////////////////////////////////////////////////////////////////////////////////
	_tcscpy_s(psvalue->file_name, MAX_PATH, task_node->szFileName);
	psvalue->range_type = TNODE_RANGE_CODE(task_node->control_code);
	psvalue->offset = task_node->offset;

	///////////////////////////////////////////////////////////////////////////////////////////////
	if(psctrl->file_stream != INVALID_HANDLE_VALUE) {
		CloseHandle(psctrl->file_stream);
		psctrl->file_stream = INVALID_HANDLE_VALUE; 
	}

	psctrl->file_stream = NTRANSCache::OpenBuild(task_node->build_cache);
	if( psctrl->file_stream == INVALID_HANDLE_VALUE ) {
		LOG_WARN( _T("get session create file failed. file name is %s.\n"), task_node->build_cache);
		return -1;
	} 

	///////////////////////////////////////////////////////////////////////////////////////////////
	return 0;
}

void CTRANSSessionBzl::CloseGetFileSe(struct TaskNode *task_node) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 

	SetFileTime(psctrl->file_stream, NULL, NULL, &psvalue->last_write); // add by james 20110910
	NTRANSCache::CloseBuild(psctrl->file_stream); 
	psctrl->file_stream = INVALID_HANDLE_VALUE; 
	memset(psctrl, '\0', sizeof(struct SessionCtrl)); 
}

DWORD CTRANSSessionBzl::OpenPutFileSe(struct TaskNode *task_node) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 
	struct ListInform *list_inform = m_pTRANSSession->list_inform;
	TCHAR szFileName[MAX_PATH];
	///////////////////////////////////////////////////////////////////////////////////////////////
	_tcscpy_s(psvalue->file_name, MAX_PATH, task_node->szFileName);
	// NFileUtility::FileLastWrite(&psvalue->last_write, szFileName);
	nstriutility::full_path(szFileName, list_inform->szLocation, task_node->szFileName);
	NFileUtility::FileSizeTime(&psvalue->file_length, &psvalue->last_write, szFileName);
	psvalue->range_type = TNODE_RANGE_CODE(task_node->control_code);;
	psvalue->offset = task_node->offset;
	psvalue->content_length = task_node->build_length;
	psvalue->content_type = CONTENT_TYPE_OCTET;
	///////////////////////////////////////////////////////////////////////////////////////////////
	if(psctrl->file_stream != INVALID_HANDLE_VALUE) {
		CloseHandle(psctrl->file_stream); 
		psctrl->file_stream = INVALID_HANDLE_VALUE;
	}
	nstriutility::full_path(szFileName, list_inform->szLocation, psvalue->file_name);
	psctrl->file_stream = CreateFile( szFileName,
		GENERIC_READ, /*GENERIC_WRITE | GENERIC_READ,*/
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( psctrl->file_stream == INVALID_HANDLE_VALUE ) {
		LOG_WARN( _T("put session create file failed.\n") );
		return -1;
	}
	// lock on the opened file
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
	OverLapped.Offset = task_node->offset;
	if(!LockFileEx(psctrl->file_stream, 
		LOCKFILE_EXCLUSIVE_LOCK, 
		0, 
		task_node->build_length, 
		0x00000000, 
		&OverLapped)) {
			LOG_WARN(_T("put session lock file %s fail!!!"), szFileName);
			return -1;
	}
	DWORD result = SetFilePointer(psctrl->file_stream, task_node->offset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) {
		LOG_WARN(_T("put session SetFilePointer fail!!!"));
		return -1;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////


	return 0;
}

void CTRANSSessionBzl::ClosePutFileSe(struct TaskNode *task_node) {
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 

	// unlock on the opened file
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = task_node->offset;
	UnlockFileEx(psctrl->file_stream, 0, task_node->build_length, 0x00000000, &OverLapped);

	CloseHandle(psctrl->file_stream); 
	psctrl->file_stream = INVALID_HANDLE_VALUE; 
	memset(psctrl, '\0', sizeof(struct SessionCtrl)); 
}

//FilesVec *pFilesVec
DWORD CTRANSSessionBzl::OpenGetListSe(DWORD dwListType, DWORD dwAnchor, TCHAR *szDirectory, void *serv_point) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 

	if (!serv_point) return -1;

	psctrl->control_stype = psvalue->list_type = dwListType;
	switch(dwListType) {
	case GLIST_LIST_FILES:
	case GLIST_RECU_FILES:
		_mywscpy_exs(psvalue->data_buffer, szDirectory);
		objFilesAttribXml.Initialize((FilesVec *)serv_point);
		break;
	case GLIST_RECU_DIRES:
		_mywscpy_exs(psvalue->data_buffer, szDirectory);
		objDiresXml.Initialize((DiresVec *)serv_point);
		break;
	case GLIST_ANCH_FILES:
		_stprintf_s(psvalue->data_buffer, _T("%u"), dwAnchor);
		objFilesAttribXml.Initialize((FilesVec *)serv_point);
		break;
	}

	return 0;
}

void CTRANSSessionBzl::CloseGetListSe() {
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control);

	switch(psctrl->control_stype) {
	case GLIST_LIST_FILES:
	case GLIST_RECU_FILES:
	case GLIST_ANCH_FILES:
		objFilesAttribXml.Finalize();
		break;
	case GLIST_RECU_DIRES:
		objDiresXml.Finalize();
		break;
	}
	memset(psctrl, '\0', sizeof(struct SessionCtrl)); 
}

DWORD CTRANSSessionBzl::OpenGetAnchorSe(struct XmlAnchor *pXmlAnchor) {
	if (!pXmlAnchor) return -1;
	objAnchorXml.Initialize(pXmlAnchor);
	return 0;
}

void CTRANSSessionBzl::CloseGetAnchorSe() {
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control);
	objAnchorXml.Finalize();
	memset(psctrl, '\0', sizeof(struct SessionCtrl)); 
}

DWORD CTRANSSessionBzl::OpenPutSimpleCsumsSendSe(struct TaskNode *task_node, struct simple_sums *psums_simple) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 
	_tcscpy_s(psvalue->file_name, MAX_PATH, task_node->szFileName);

	DWORD content_length = psums_simple->count * sizeof(struct simple_sum);
	psctrl->file_stream = NFileUtility::BuildCsumsFile(NULL);
	if(psctrl->file_stream == INVALID_HANDLE_VALUE) 
		return -1;
	DWORD dwWritenSize;
	if(!WriteFile(psctrl->file_stream, psums_simple->sums, content_length, &dwWritenSize, NULL)) 
		return -1;	
	DWORD result = SetFilePointer(psctrl->file_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;

	psvalue->csum_type = SIMPLE_CSUM;
	psvalue->range_type = TNODE_RANGE_CODE(task_node->control_code);
	psvalue->content_length = content_length;
	psvalue->content_type = CONTENT_TYPE_OCTET;

	return 0;
}

DWORD CTRANSSessionBzl::OpenPutComplexCsumsSendSe(struct TaskNode *task_node, struct complex_sums *psums_complex) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 
	_tcscpy_s(psvalue->file_name, MAX_PATH, task_node->szFileName);

	DWORD content_length = psums_complex->count * sizeof(struct complex_sum);
	psctrl->file_stream = NFileUtility::BuildCsumsFile(NULL);
	if(psctrl->file_stream == INVALID_HANDLE_VALUE) 
		return -1;
	DWORD dwWritenSize;
	if(!WriteFile(psctrl->file_stream, psums_complex->sums, content_length, &dwWritenSize, NULL)) 
		return -1;
	DWORD result = SetFilePointer(psctrl->file_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;

	psvalue->csum_type = COMPLEX_CSUM;
	psvalue->range_type = TNODE_RANGE_CODE(task_node->control_code);
	psvalue->content_length = content_length;
	psvalue->content_type = CONTENT_TYPE_OCTET;

	LOG_DEBUG(_T("content_length:%d"), content_length);
	return 0;
}

void CTRANSSessionBzl::ClosePutCsumSendSe() {
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 
	psctrl->rwrite_count = 0;
	/*
	DWORD result = SetFilePointer(psctrl->file_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
	return;
	SetEndOfFile(psctrl->file_stream);
	FlushFileBuffers(psctrl->file_stream);
	*/
}

DWORD CTRANSSessionBzl::OpenPutCsumRecvSe(struct TaskNode *task_node) {
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 

	DWORD result = SetFilePointer(psctrl->file_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	SetEndOfFile(psctrl->file_stream);
	FlushFileBuffers(psctrl->file_stream);

	return 0;
}

void CTRANSSessionBzl::ClosePutCsumRecvSe(OUT vector<struct local_match *> &vlocal_match) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 
	struct file_matcher inform;
	DWORD slen = 0;
	off_t index, match_count;

	LOG_DEBUG(_T("------------ FileSizePoint(psctrl->file_stream):%d"), NFileUtility::FileSizeHandle(psctrl->file_stream));
	DWORD result = SetFilePointer(psctrl->file_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return;

	for(index = 0; index < psvalue->content_length; index += match_count) {
		match_count = sizeof(struct file_matcher);
		if(!ReadFile(psctrl->file_stream, &inform, sizeof(struct file_matcher), &slen, NULL)) {
			// if(ERROR_HANDLE_EOF != GetLastError()) return -1;
			break;
		}
		LOG_DEBUG(_T("------------ match_type:%d offset:%d index_length:%d"), inform.match_type, inform.offset, inform.index_length);
		struct local_match *alloc_local = AppendLocalMatch(vlocal_match, &inform);
		if (MODIFY_CONTENT & inform.match_type) {
			alloc_local->buffer = (char *) malloc(inform.index_length);
			if(!ReadFile(psctrl->file_stream, alloc_local->buffer, inform.index_length, &slen, NULL)) {
				// if(ERROR_HANDLE_EOF != GetLastError()) return -1;
				break;
			}
			match_count += inform.index_length;
		}
	}

	CloseHandle(psctrl->file_stream);
	psctrl->file_stream = INVALID_HANDLE_VALUE; 

	memset(psctrl, '\0', sizeof(struct SessionCtrl)); 
}

DWORD CTRANSSessionBzl::OpenGetCsumSe(struct TaskNode *task_node) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 

	psctrl->file_stream = NFileUtility::BuildComplexFile(NULL);
	if(psctrl->file_stream == INVALID_HANDLE_VALUE) 
		return -1;
	_tcscpy_s(psvalue->file_name, task_node->szFileName);
	psvalue->csum_type = COMPLEX_CSUM;
	psvalue->range_type = TNODE_RANGE_CODE(task_node->control_code);
	psvalue->offset = task_node->offset;
	LOG_DEBUG(_T("+++++++++++++++++++++++++++++ psvalue->file_name:%s psvalue->range_type:%d psvalue->csum_type:%d"), psvalue->file_name, psvalue->range_type, psvalue->csum_type);
	return 0;
}

void CTRANSSessionBzl::CloseGetCsumSe() {
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 
	psctrl->rwrite_count = 0;
}

DWORD CTRANSSessionBzl::OpenPostSe(struct TaskNode *task_node) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 
	struct ListInform *list_inform = m_pTRANSSession->list_inform;
	TCHAR szFileName[MAX_PATH];

	DWORD result = SetFilePointer(psctrl->file_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) {
		LOG_WARN(_T("put session SetFilePointer fail!!!"));
		return -1;
	}

	_tcscpy_s(psvalue->file_name, task_node->szFileName);
	// TCHAR *full_path(TCHAR *fpath, const TCHAR *path, const TCHAR *subdir);
	nstriutility::full_path(szFileName, list_inform->szLocation, task_node->szFileName);
	NFileUtility::FileSizeTime(&psvalue->file_length, &psvalue->last_write, szFileName);
	psvalue->range_type = TNODE_RANGE_CODE(task_node->control_code);
	psvalue->offset = task_node->offset;
	psvalue->content_length = NFileUtility::FileSizeHandle(psctrl->file_stream);
	psvalue->content_type = CONTENT_TYPE_OCTET;

	return 0;
}

void CTRANSSessionBzl::ClosePostSe() {
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control); 

	CloseHandle(psctrl->file_stream); 
	psctrl->file_stream = INVALID_HANDLE_VALUE;

	memset(psctrl, '\0', sizeof(struct SessionCtrl));
}

DWORD CTRANSSessionBzl::OpenMoveServerSe(struct TaskNode *exist_task_node, struct TaskNode *new_task_node) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);

	_tcscpy_s(psvalue->file_name, exist_task_node->szFileName);
	_tcscpy_s(psvalue->data_buffer, new_task_node->szFileName);
	psvalue->last_write = new_task_node->ftLastWrite;

	return 0;
}

DWORD CTRANSSessionBzl::OpenDeleteServerSe(struct TaskNode *task_node) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	_tcscpy_s(psvalue->file_name, task_node->szFileName);
	return 0;
}

DWORD CTRANSSessionBzl::OpenFileVerifySe(struct TaskNode *task_node, HANDLE hFileVerify) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control);

	if(!task_node || INVALID_HANDLE_VALUE==hFileVerify)
		return -1;
	psctrl->file_stream = hFileVerify;
	_tcscpy_s(psvalue->file_name, task_node->szFileName);
	psvalue->csum_type = SIMPLE_CSUM;
	psvalue->range_type = RANGE_TYPE_FILE;
	psvalue->offset = task_node->offset;

	return 0;	
}


DWORD CTRANSSessionBzl::OpenChunkVerifySe(struct TaskNode *task_node, HANDLE hFileVerify) {
	struct InternalValue *psvalue = &(m_pTRANSSession->inte_value);
	struct SessionCtrl *psctrl = &(m_pTRANSSession->control);

	if(!task_node || INVALID_HANDLE_VALUE==hFileVerify)
		return -1;
	psctrl->file_stream = hFileVerify;
	_tcscpy_s(psvalue->file_name, task_node->szFileName);
	psvalue->csum_type = SIMPLE_CSUM;
	psvalue->range_type = RANGE_TYPE_CHUNK;
	psvalue->offset = task_node->offset;

	return 0;	
}

void CTRANSSessionBzl::KillTRANSSession() {
	if(m_pTRANSSession->control.file_stream != INVALID_HANDLE_VALUE) {
		CloseHandle(m_pTRANSSession->control.file_stream);
		m_pTRANSSession->control.file_stream = INVALID_HANDLE_VALUE;
	}
}

CTRANSSessionBzl *NTRANSSessionBzl::Factory(struct SessionArgu *pSessionArgu) {
	CTRANSSessionBzl *pSessionBzl = new CTRANSSessionBzl;
	pSessionBzl->Initialize();
	pSessionBzl->Create(pSessionArgu);

	return pSessionBzl;
}

void NTRANSSessionBzl::DestroyObject(CTRANSSessionBzl *pSessionBzl) {
	pSessionBzl->Destroy();
	pSessionBzl->Finalize();

	delete pSessionBzl;
}

void CONVERT_SESSION_ARGU(struct SessionArgu *pSessionArgu, struct ClientConfig *pClientConfig) { 
	_tcscpy_s((pSessionArgu)->szUserName, pClientConfig->szUserName); 
	_tcscpy_s((pSessionArgu)->szPassword, pClientConfig->szPassword); 
	_tcscpy_s((pSessionArgu)->szLocalIdenti, pClientConfig->szLocalIdenti); 
	_tcscpy_s((pSessionArgu)->szDriveRoot, pClientConfig->szDriveRoot); 
}