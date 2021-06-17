#include "StdAfx.h"

#include "ListVObj.h"
#include "DebugPrinter.h"

//
CListHObj::CListHObj(void) :
m_dwHandNode(0)
{
	memset(list_md5sum, 0, MD5_DIGEST_LEN);
	InitializeCriticalSection(&m_csHandNumbe);
}

CListHObj::~CListHObj(void) {
	DeleteCriticalSection(&m_csHandNumbe);
}

CListHObj objListHead;

VOID DireListNodeAdd(ListVec *pListVec, unsigned __int64 list_length, DWORD list_code, TCHAR *directory) {
	struct ListNode *pListNode;
	DWORD chunk_tatol;

	if(list_length <= CHUNK_SIZE) {
		pListNode = (struct ListNode *)malloc(sizeof(struct ListNode));
		if(pListNode) {
			pListNode->node_inde = 0;
			pListNode->control_code = list_code|RANGE_TYPE_FILE;
			NTRANSCache::CreatBuilderName(pListNode->builder_cache, PRODU_LIST_DEFAULT, 0);
_LOG_DEBUG(_T("file name:%s build cache:%s"), PRODU_LIST_DEFAULT, pListNode->builder_cache);
			pListNode->offset = 0;
			_tcscpy_s(pListNode->directory, MAX_PATH, directory);
			//
			pListVec->push_back(pListNode);
		}
	} else {
		for(chunk_tatol = 0; chunk_tatol*(unsigned __int64)CHUNK_SIZE <= list_length; chunk_tatol++) {
			pListNode = (struct ListNode *)malloc(sizeof(struct ListNode));
			if(pListNode) {
				pListNode->node_inde = chunk_tatol;
				if((chunk_tatol + 1)*(unsigned __int64)CHUNK_SIZE > list_length) pListNode->control_code = list_code|RANGE_TYPE_FILE;
				else pListNode->control_code = list_code|RANGE_TYPE_CHUNK;
				NTRANSCache::CreatBuilderName(pListNode->builder_cache, PRODU_LIST_DEFAULT, chunk_tatol*(unsigned __int64)CHUNK_SIZE);
_LOG_DEBUG(_T("file name:%s build cache:%s"), PRODU_LIST_DEFAULT, pListNode->builder_cache);
				pListNode->offset = chunk_tatol*(unsigned __int64)CHUNK_SIZE;
				_tcscpy_s(pListNode->directory, MAX_PATH, directory);
				//
				pListVec->push_back(pListNode);
			}
		}
	}
}

VOID AnchListNodeAdd(ListVec *pListVec, unsigned __int64 list_length, DWORD list_code, DWORD llast_anchor) {
	struct ListNode *pListNode;
	DWORD chunk_tatol;

	if(list_length <= CHUNK_SIZE) {
		pListNode = (struct ListNode *)malloc(sizeof(struct ListNode));
		if(pListNode) {
			pListNode->node_inde = 0;
			pListNode->control_code = list_code|RANGE_TYPE_FILE;
			NTRANSCache::CreatBuilderName(pListNode->builder_cache, PRODU_LIST_DEFAULT, 0);
_LOG_DEBUG(_T("file name:%s build cache:%s"), PRODU_LIST_DEFAULT, pListNode->builder_cache);
			pListNode->offset = 0;
			pListNode->llast_anchor = llast_anchor;
			//
			pListVec->push_back(pListNode);
		}
	} else {
		for(chunk_tatol = 0; chunk_tatol*(unsigned __int64)CHUNK_SIZE <= list_length; chunk_tatol++) {
			pListNode = (struct ListNode *)malloc(sizeof(struct ListNode));
			if(pListNode) {
				pListNode->node_inde = chunk_tatol;
				if((chunk_tatol + 1)*(unsigned __int64)CHUNK_SIZE > list_length) pListNode->control_code = list_code|RANGE_TYPE_FILE;
				else pListNode->control_code = list_code|RANGE_TYPE_CHUNK;
				NTRANSCache::CreatBuilderName(pListNode->builder_cache, PRODU_LIST_DEFAULT, chunk_tatol*(unsigned __int64)CHUNK_SIZE);
_LOG_DEBUG(_T("file name:%s build cache:%s"), PRODU_LIST_DEFAULT, pListNode->builder_cache);
				pListNode->offset = chunk_tatol*(unsigned __int64)CHUNK_SIZE;	
				pListNode->llast_anchor = llast_anchor;
				//
				pListVec->push_back(pListNode);
			}
		}
	}
}

unsigned char *get_md5sum_bin(unsigned char *md5bin, TCHAR *md5sum) {
	long int num;
	TCHAR hex[3];
	TCHAR *tok = md5sum;
	//
	hex[2] = _T('\0');
	for(int inde=0; inde<16; inde++) {
		hex[0] = *tok++;
		hex[1] = *tok++;
		num = _tcstol(hex, NULL, 16);
		md5bin[inde] = (BYTE)num;
	}
	//
	return md5bin;
}

DWORD CListHObj::CreatListVec(TCHAR *list_sum, unsigned __int64 list_length, DWORD list_type, DWORD llast_anchor, TCHAR *directory) {
	// m_iListLength = list_length;
	get_md5sum_bin(list_md5sum, list_sum);
	// _LOG_DEBUG(_T("list_length:%d list_type:%08x"), list_length, list_type); // disable by james 20130410
	switch(list_type) {
	case LIST_RECU_FILES:
	case LIST_LIST_DIREC:
	case LIST_RECU_DIRES:
		DireListNodeAdd(&m_vListVector, list_length, list_type, directory);
		// _tcscpy_s(ival->file_name, MAX_PATH, list_node->directory);
		break;	
	case LIST_ANCH_FILES:
		AnchListNodeAdd(&m_vListVector, list_length, list_type, llast_anchor);
		// ival->last_anchor = list_node->llast_anchor;
		break;	
	}
	// ndp::ListVector(&m_vListVector);
	return 0x00;
}

VOID CListHObj::DeleListVec() {
	ListVec::iterator iter;
	for(iter = m_vListVector.begin(); m_vListVector.end() != iter; ++iter) {
		free(*iter);
	}
	m_vListVector.clear();
}

VOID CListHObj::SetHandNumbe(DWORD dwNodeInde) {
	EnterCriticalSection(&m_csHandNumbe);
	m_dwHandNode = dwNodeInde;
	LeaveCriticalSection(&m_csHandNumbe);
}

DWORD CListHObj::SetNode(struct ListNode *list_node, DWORD dwNodeInde) {
	ListVec::iterator iter;
	DWORD retValue = ((DWORD)-1);
	//
	EnterCriticalSection(&m_csHandNumbe);
	for(iter = m_vListVector.begin(); m_vListVector.end() != iter; ++iter) {
		if(dwNodeInde == (*iter)->node_inde) {
			(*iter)->control_code = list_node->control_code;
			(*iter)->offset = list_node->offset;
			retValue = 0x00;
			break;
		}
	}
	LeaveCriticalSection(&m_csHandNumbe);
	//
	return retValue;
}

DWORD CListHObj::GetNodeAddition(struct ListNode **list_node) {
	DWORD retValue = ((DWORD)-1);
	//
	EnterCriticalSection(&m_csHandNumbe);
	try {
		*list_node = m_vListVector.at(m_dwHandNode);
		++m_dwHandNode;
		retValue = 0x01;
	} catch (std::out_of_range &e) {
		// _LOG_INFO(_T("out_of_range:%s"), e.what());
		e.what();
// _LOG_DEBUG(_T("out_of_range."));
		retValue = 0x00;
	}
	LeaveCriticalSection(&m_csHandNumbe);
	//
	return retValue;
}

//
CListVObj::CListVObj(void) :
m_hListFile(INVALID_HANDLE_VALUE)
{
	MKZEO(m_szListName);
	InitializeCriticalSection(&m_csListHand);
}

CListVObj::~CListVObj(void) {
	DeleteCriticalSection(&m_csListHand);
}

CListVObj objListVec;

DWORD CListVObj::Initialize(TCHAR *szListFile, TCHAR *szLocation) {
	struti::get_name(m_szListName, szLocation, szListFile);
	//
	m_hListFile = CreateFile( m_szListName, 
		GENERIC_WRITE | GENERIC_READ,
		NULL,
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( INVALID_HANDLE_VALUE == m_hListFile ) {
		_LOG_WARN( _T("create file failed: %d"), GetLastError() );
		return ((DWORD)-1);
	}
_LOG_DEBUG(_T("create list name:%s"), m_szListName);
	return 0x00;
}

DWORD CListVObj::Finalize() {
	if(INVALID_HANDLE_VALUE != m_hListFile) {
		CloseHandle( m_hListFile );
		m_hListFile = INVALID_HANDLE_VALUE;
	}
// MoveFile(m_szListName, _T("D:\\produ_list.xml")); // test
	DeleteFile(m_szListName); // delete by james 20110414
_LOG_DEBUG(_T("delete list name:%s"), m_szListName);
	return 0x00;
}

DWORD CListVObj::WriteListFile(char *buffe, DWORD size, unsigned __int64 offset) {
	DWORD dwWritenSize = 0x00;
	//
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
	OverLapped.Offset = offset&0xffffffff;
	OverLapped.OffsetHigh = offset >> 32;
	//
	EnterCriticalSection(&m_csListHand);
	WriteFile(m_hListFile, buffe, size, &dwWritenSize, &OverLapped);
	LeaveCriticalSection(&m_csListHand);
	if(size != dwWritenSize) {
		_LOG_WARN(_T("fatal write error: %d"), GetLastError());
		dwWritenSize = ((DWORD)-1);		
	}
	//
	return dwWritenSize;
}

VOID CListVObj::ResetListFilePointer() {
	DWORD dwResult = SetFilePointer(m_hListFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
_LOG_WARN(_T("overwrite set file pointer fail!"));
	}
}

DWORD CListVObj::ReadListFile(char *buffe, DWORD size) {
	DWORD dwReadSize = 0x00;
	if(!ReadFile(m_hListFile, buffe, size, &dwReadSize, NULL)) {
_LOG_WARN(_T("fatal read error: %d"), GetLastError());
		return 0x00;		
	}
	return dwReadSize;
}

void CListVObj::GetListMd5sum(unsigned char *list_sum) {
	// _LOG_DEBUG(_T("get list file md5sum:%s"), m_szListName);
	FileChks::HandleChksMD5(m_hListFile, list_sum);
}

#define PARSE_PAGE_SIZE	0x1000 // 0x2000  // 8K
int CListVObj::ParseListXml(VOID *serv_list, DWORD list_type) {
	char buffe [PARSE_PAGE_SIZE + 1];
	unsigned int length;
	//
	switch(list_type) {
	case GLIST_RECU_FILES:
		objSlowAttXml.Initialize((HANDLE)serv_list, PARSE_PAGE_SIZE);
		ResetListFilePointer();
		for(; 0 < (length = ReadListFile(buffe, PARSE_PAGE_SIZE)); ) {
			buffe[length] = '\0'; // add by james 20141218
// _LOG_ANSI("%s", buffe); // attention the buffer size
			objSlowAttXml.ParseFilesAttXml(buffe, length);
		}
		objSlowAttXml.Finalize();
		break;
	case GLIST_LIST_DIREC:
	case GLIST_ANCH_FILES:
		objFastActXml.Initialize((FilesVec *)serv_list, PARSE_PAGE_SIZE);
		ResetListFilePointer();
		for(; 0 < (length = ReadListFile(buffe, PARSE_PAGE_SIZE)); ) {
			buffe[length] = '\0'; // add by james 20141218
// _LOG_ANSI("%s", buffe); // attention the buffer size
			objFastActXml.ParseFilesActXml(buffe, length);
		}
		objFastActXml.Finalize();
		break;
	case GLIST_RECU_DIRES:
		objDiresXml.Initialize((DiresVec *)serv_list, PARSE_PAGE_SIZE);
		ResetListFilePointer();
		for(; 0 < (length = ReadListFile(buffe, PARSE_PAGE_SIZE)); ) {
			buffe[length] = '\0'; // add by james 20141218
// _LOG_DEBUG(_T("%s"), buf); // attention the buffer size
			objDiresXml.ParseDiresPathXml(buffe, length);
		}
		objDiresXml.Finalize();
		break;
	}
	//
	return 0;
}

//
int ListWorkBzl::CheckListFile(class CListHObj *pListHead, class CListVObj *pListVec) {
	unsigned char correct_chks[MD5_DIGEST_LEN];
	pListHead->GetListMd5sum(correct_chks);
// _LOG_DEBUG(_T("--------------------------------"));
// ndp::ChksMD5(_T("correct_chks"), correct_chks);
	unsigned char local_md5sum[MD5_DIGEST_LEN];
	pListVec->GetListMd5sum(local_md5sum);
// ndp::ChksMD5(_T("local_md5sum"), local_md5sum);
	return memcmp(correct_chks, local_md5sum, MD5_DIGEST_LEN);
}