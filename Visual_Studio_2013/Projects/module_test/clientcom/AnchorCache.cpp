#include "StdAfx.h"
#include <afxmt.h>
#include "AnchorCache.h"

#include "third_party.h"
#include "ShellInterface.h"
#include "Anchor.h"

CAnchorCache::CAnchorCache(void):
m_dwAnchorLocked(FALSE),
m_dwLastAnchor(-1),
m_dwLastTickCount(0)
{
}

CAnchorCache::~CAnchorCache(void) {
}

CAnchorCache objAnchorCache;

DWORD CAnchorCache::Initialize() {
	m_dwAnchorLocked = FALSE; //0 no 1 yes
	//m_dwAnchorLocked = true; //for test

	DWORD last_anchor;
	if(objAnchor.LoadLastFiles(&m_vFilesCache, &last_anchor))
		return -1;
	SetLastAnchor(last_anchor);

	return 0;
}

DWORD CAnchorCache::Finalize() {
	if(objAnchor.FlushFilesVecFile(&m_vFilesCache))
		return -1;
	
	m_hmFilesSingle.clear();
	m_hmFilesDual.clear();
	NFilesVec::DeleteFilesVec(&m_vFilesCache);

	return 0;
}

#define CHANGE_DISPLAY			0x00000100
#define FOUND					0x00010000
#define SHOULD_APPEND			0x00000001
void CAnchorCache::FilesCacheAppend(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szFileName, const DWORD modify_type) { //文件 add modify delete 的插入
	unordered_map <wstring, struct file_attrib *>::iterator file_iter;
	struct file_attrib *fast, *exis_node;
	DWORD dwFinalValue = UNKNOWN, dwEraseValue = UNKNOWN;
	DWORD dwFoundAppend = 0x00000000;

	file_iter = m_hmFilesSingle.find(NO_ROOT(szFileName));
	if(file_iter != m_hmFilesSingle.end()) {
		fast = file_iter->second;
		dwEraseValue = fast->modify_type;
		switch(fast->modify_type) {
			case ADD:
				// if(ADD & modify_type)
				// else if(MODIFY & modify_type)
				if(DEL & modify_type) {
					NFilesVec::DeleteFromVec(&m_vFilesCache, fast);
					NFilesVec::DeleteNode(fast);
					m_hmFilesSingle.erase(file_iter);

					dwFinalValue = UNKNOWN; //erase
					dwFoundAppend |= CHANGE_DISPLAY;
				}
				break;
			case MODIFY:
				//if(ADD & modify_type)
				//else if(MODIFY & modify_type)
				if(DEL & modify_type) {
					fast->modify_type = DEL;

					dwFinalValue = UNKNOWN;
					dwFoundAppend |= CHANGE_DISPLAY;
				}
				break;
			case DEL:
				if(ADD & modify_type) {
					fast->modify_type = MODIFY;

					dwFinalValue = MODIFY;
					dwFoundAppend |= CHANGE_DISPLAY;
				}
				//else if(MODIFY & modify_type)
				//else if(DEL & modify_type)
				break;
		}
		dwFoundAppend |= FOUND;
	}

	file_iter = m_hmFilesDual.find(NO_ROOT(szFileName));
	if(file_iter != m_hmFilesDual.end()) {
		fast = file_iter->second;
		dwEraseValue = fast->modify_type;
		switch(fast->modify_type) {
			case EXISTS:
				if(ADD & modify_type) dwFoundAppend |= SHOULD_APPEND;
				//else if(MODIFY & modify_type)
				//else if(DEL & modify_type)
				break;
			case COPY:
				//if(ADD & modify_type)
				if(MODIFY & modify_type)  dwFoundAppend |= SHOULD_APPEND;
				else if(DEL & modify_type) {
					exis_node = NFilesVec::FindExistsNode(&m_vFilesCache, fast);
					m_hmFilesDual.erase(file_iter);
					NFilesVec::DeleteFromVec(&m_vFilesCache, fast);
					NFilesVec::DeleteNode(fast);

					file_iter = m_hmFilesDual.find(exis_node->file_name);
					if(file_iter != m_hmFilesDual.end()) m_hmFilesDual.erase(file_iter);
					NFilesVec::DeleteFromVec(&m_vFilesCache, exis_node);
					NFilesVec::DeleteNode(exis_node);

					dwFinalValue = UNKNOWN;
					dwFoundAppend |= CHANGE_DISPLAY;
				}
				break;
			case MOVE:
				//if(ADD & modify_type)
				if(MODIFY & modify_type)  dwFoundAppend |= SHOULD_APPEND;
				else if(DEL & modify_type) {
					exis_node = NFilesVec::FindExistsNode(&m_vFilesCache, fast);
					exis_node->modify_type = DEL;

					NFilesVec::DeleteFromVec(&m_vFilesCache, fast);
					NFilesVec::DeleteNode(fast);
					m_hmFilesSingle.erase(file_iter);

					dwFinalValue = UNKNOWN;
					dwFoundAppend |= CHANGE_DISPLAY;
				}
				break;
		}
		dwFoundAppend |= FOUND;
	}

	if(CHANGE_DISPLAY & dwFoundAppend){
		DisplayChanged(szFileName, statcode_conv(CONTROL_TYPE_ERASE, dwEraseValue));
		if(dwFinalValue) DisplayChanged(szFileName, statcode_conv(CONTROL_TYPE_INSERT, dwFinalValue));
	} else if((SHOULD_APPEND&dwFoundAppend) || !(FOUND&dwFoundAppend)) {
		LOG_DEBUG(_T("install new node file name:%s modify_type:%x"), szFileName, modify_type);	
		fast = NFilesVec::AppendNewNode(&m_vFilesCache);
		m_hmFilesSingle[NO_ROOT(szFileName)] = fast;
		_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szFileName));
		fast->modify_type = modify_type;
		dwFinalValue = modify_type;
		DisplayChanged(szFileName, statcode_conv(CONTROL_TYPE_INSERT, modify_type));
	}
}


void CAnchorCache::FilesCacheCopy(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szExistsName, const TCHAR *szNewName) { //文件改名的插入
	unordered_map <wstring, struct file_attrib *>::iterator file_iter;
	struct file_attrib *fast;
	DWORD dwFinalValue = UNKNOWN, dwEraseValue = UNKNOWN;;

	file_iter = m_hmFilesSingle.find(NO_ROOT(szExistsName));
	if(file_iter != m_hmFilesSingle.end()) {
		fast = file_iter->second;
		dwEraseValue = fast->modify_type;
		switch(fast->modify_type) {
		case ADD:
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));	
			dwFinalValue = ADD;
			break;
		case MODIFY:
			fast = NFilesVec::InsertPrevNew(&m_vFilesCache, fast);
			m_hmFilesSingle[NO_ROOT(szExistsName)] = fast;
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szExistsName));
			fast->modify_type = EXISTS;

			fast = NFilesVec::InsertPrevNew(&m_vFilesCache, fast);
			m_hmFilesSingle[NO_ROOT(szNewName)] = fast;
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
			fast->modify_type = MOVE;

			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
			dwFinalValue = MODIFY;
			break;
		case DEL:
			break;
		}
	}
	file_iter = m_hmFilesDual.find(NO_ROOT(szExistsName));
	if(file_iter != m_hmFilesDual.end()) {
		fast = file_iter->second;
		dwEraseValue = fast->modify_type;
		switch(fast->modify_type) {
		case EXISTS:
			break;				
		case COPY:
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
			dwFinalValue = MODIFY;
			break;
		case MOVE:
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
			dwFinalValue = MODIFY;
			break;
		}

	}

	if(dwFinalValue) {
		DisplayChanged(szExistsName, statcode_conv(CONTROL_TYPE_ERASE, dwEraseValue));
		DisplayChanged(szNewName, statcode_conv(CONTROL_TYPE_INSERT, dwFinalValue));
	} else { // install new node
		LOG_DEBUG(_T("install new node exists:%s new:%x"), szExistsName, szNewName);
		///////////////////////////////////////////////////////
		DWORD couple_id = GetTickCount();
		///////////////////////////////////////////////////////
		fast = NFilesVec::AppendNewNode(&m_vFilesCache);
		m_hmFilesDual[NO_ROOT(szExistsName)] = fast;
		_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szExistsName));
		fast->modify_type = EXISTS;
		fast->reserved = couple_id;
		DisplayChanged(szExistsName, statcode_conv(CONTROL_TYPE_INSERT, EXISTS));
		///////////////////////////////////////////////////////
		fast = NFilesVec::AppendNewNode(&m_vFilesCache);
		m_hmFilesDual[NO_ROOT(szNewName)] = fast;
		_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
		fast->modify_type = MOVE;
		fast->reserved = couple_id;
		DisplayChanged(szNewName, statcode_conv(CONTROL_TYPE_INSERT, MOVE));
	}
}

void CAnchorCache::FilesCacheMove(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szExistsName, const TCHAR *szNewName) { //文件改名的插入
	unordered_map <wstring, struct file_attrib *>::iterator file_iter;
	struct file_attrib *fast;
	DWORD dwFinalValue = UNKNOWN, dwEraseValue = UNKNOWN;;

	file_iter = m_hmFilesSingle.find(NO_ROOT(szExistsName));
	if(file_iter != m_hmFilesSingle.end()) {
		fast = file_iter->second;
		dwEraseValue = fast->modify_type;
		switch(fast->modify_type) {
		case ADD:
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));	
			dwFinalValue = ADD;
			break;
		case MODIFY:
			fast = NFilesVec::InsertPrevNew(&m_vFilesCache, fast);
			m_hmFilesSingle[NO_ROOT(szExistsName)] = fast;
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szExistsName));
			fast->modify_type = EXISTS;

			fast = NFilesVec::InsertPrevNew(&m_vFilesCache, fast);
			m_hmFilesSingle[NO_ROOT(szNewName)] = fast;
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
			fast->modify_type = MOVE;

			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
			dwFinalValue = MODIFY;
			break;
		case DEL:
			break;
		}
	}
	file_iter = m_hmFilesDual.find(NO_ROOT(szExistsName));
	if(file_iter != m_hmFilesDual.end()) {
		fast = file_iter->second;
		dwEraseValue = fast->modify_type;
		switch(fast->modify_type) {
		case EXISTS:
			break;				
		case COPY:
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
			dwFinalValue = MODIFY;
			break;
		case MOVE:
			_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
			dwFinalValue = MODIFY;
			break;
		}

	}

	if(dwFinalValue) {
		DisplayChanged(szExistsName, statcode_conv(CONTROL_TYPE_ERASE, dwEraseValue));
		DisplayChanged(szNewName, statcode_conv(CONTROL_TYPE_INSERT, dwFinalValue));
	} else { // install new node
		LOG_DEBUG(_T("install new node exists:%s new:%x"), szExistsName, szNewName);
		///////////////////////////////////////////////////////
		DWORD couple_id = GetTickCount();
		///////////////////////////////////////////////////////
		fast = NFilesVec::AppendNewNode(&m_vFilesCache);
		m_hmFilesDual[NO_ROOT(szExistsName)] = fast;
		_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szExistsName));
		fast->modify_type = EXISTS;
		fast->reserved = couple_id;
		DisplayChanged(szExistsName, statcode_conv(CONTROL_TYPE_INSERT, EXISTS));
		///////////////////////////////////////////////////////
		fast = NFilesVec::AppendNewNode(&m_vFilesCache);
		m_hmFilesDual[NO_ROOT(szNewName)] = fast;
		_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
		fast->modify_type = MOVE;
		fast->reserved = couple_id;
		DisplayChanged(szNewName, statcode_conv(CONTROL_TYPE_INSERT, MOVE));
	}
}

void CAnchorCache::FolderCacheAppend(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szDirectory) { //目录删除的插入
	FilesVec::iterator iter; 
	struct file_attrib *fast, *exis_node = NULL;
	TCHAR szFinalValue[MAX_PATH], szDisplayFile[MAX_PATH];

	_tcscpy_s(szFinalValue, NO_ROOT(szDirectory));
	_tcscpy_s(szDisplayFile, szDirectory);
	int iFileLength = _tcslen(NO_ROOT(szDirectory));
	for(iter=m_vFilesCache.begin(); iter!=m_vFilesCache.end(); ) {
		fast = *iter;
		if(!(EXISTS&fast->modify_type) && !_tcsncmp(fast->file_name, NO_ROOT(szDirectory), iFileLength)) {
			// delete file attrib
			if(COPY & fast->modify_type) {
				iter = m_vFilesCache.erase(--iter);
				//////////////////////////////////////////////////////////////////
				NFilesVec::DeleteFromHmap(&m_hmFilesDual, exis_node);	
				_tcscpy_s(szFinalValue, exis_node->file_name);
				// NFilesVec::DeleteFromVec(&m_vFilesCache, exis_node);
				NFilesVec::DeleteNode(exis_node);

				NFilesVec::DeleteFromHmap(&m_hmFilesDual, fast);			
			} if(MOVE & fast->modify_type) {
				iter = m_vFilesCache.erase(--iter);
				//////////////////////////////////////////////////////////////////
				NFilesVec::DeleteFromHmap(&m_hmFilesDual, exis_node);	
				_tcscpy_s(szFinalValue, exis_node->file_name);
				// NFilesVec::DeleteFromVec(&m_vFilesCache, exis_node);
				NFilesVec::DeleteNode(exis_node);

				NFilesVec::DeleteFromHmap(&m_hmFilesDual, fast);
			} else NFilesVec::DeleteFromHmap(&m_hmFilesSingle, fast);
			//////////////////////////////////////////////////////////////////
			// refresh icon status
			if(!(DIRECTORY & fast->modify_type)) {
				_tcscpy_s(NO_ROOT(szDisplayFile), MAX_PATH-ROOT_LENGTH, fast->file_name);
				DisplayChanged(szDisplayFile, statcode_conv(CONTROL_TYPE_ERASE, fast->modify_type));
			}
			NFilesVec::DeleteNode(fast);
			//////////////////////////////////////////////////////////////////
			iter = m_vFilesCache.erase(iter);
		} else {
			iter++;	
			if(EXISTS & fast->modify_type) exis_node = fast;
		}
	}

	// install new node
	fast = NFilesVec::AppendNewNode(&m_vFilesCache);
	m_hmFilesSingle[szFinalValue] = fast;
	_tcscpy_s(fast->file_name, MAX_PATH, szFinalValue);
	fast->modify_type = DEL|DIRECTORY;
	// DisplayChanged(szDirectory, statcode_conv(CONTROL_TYPE_INSERT, DEL|DIRECTORY));
}

void CAnchorCache::FolderCacheCopy(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szExistsName, const TCHAR *szNewName) { //目录改名的插入
	FilesVec::iterator iter;
	struct file_attrib *fast;
	TCHAR szFileName[MAX_PATH], szDisplayFile[MAX_PATH];
	DWORD IsFound = 0, dwDeleteDefault = 0;

	_tcscpy_s(szDisplayFile, szExistsName);
	int iExistsLength = _tcslen(NO_ROOT(szExistsName));
	for(iter=m_vFilesCache.begin(); iter!=m_vFilesCache.end(); iter++) {
		fast = *iter;
		if(!(EXISTS&fast->modify_type) && !_tcsncmp(fast->file_name, NO_ROOT(szExistsName), iExistsLength)) {
			if((COPY|MOVE) & fast->modify_type) IsFound = 1;
			//////////////////////////////////////////////////////////////////
			// refresh icon status
			if(!(DIRECTORY & fast->modify_type)) {
				_tcscpy_s(NO_ROOT(szDisplayFile), MAX_PATH-ROOT_LENGTH, fast->file_name);
				DisplayChanged(szDisplayFile, statcode_conv(CONTROL_TYPE_ERASE, fast->modify_type));
			}
			//////////////////////////////////////////////////////////////////
			if(NFilesVec::DeleteFromHmap(&m_hmFilesSingle, fast)) dwDeleteDefault |= 0x00000001;	
			if(NFilesVec::DeleteFromHmap(&m_hmFilesDual, fast)) dwDeleteDefault |= 0x00000002;	

			_tcscpy_s(szFileName, fast->file_name+iExistsLength);
			_tcscpy_s(fast->file_name, NO_ROOT(szNewName));
			_tcscat_s(fast->file_name, szFileName);
			
			if(0x00000001 & dwDeleteDefault) m_hmFilesSingle[fast->file_name] = fast;
			if(0x00000002 & dwDeleteDefault) m_hmFilesDual[fast->file_name] = fast;
			//////////////////////////////////////////////////////////////////
			// refresh icon status
			if(!(DIRECTORY & fast->modify_type)) {
				_tcscpy_s(NO_ROOT(szDisplayFile), MAX_PATH-ROOT_LENGTH, fast->file_name);
				DisplayChanged(szDisplayFile, statcode_conv(CONTROL_TYPE_INSERT, fast->modify_type));
			}
		}
	}
	if(IsFound) return;

	///////////////////////////////////////////////////////
	// install new node
	DWORD couple_id = GetTickCount();
	///////////////////////////////////////////////////////
	fast = NFilesVec::AppendNewNode(&m_vFilesCache);
	m_hmFilesDual[NO_ROOT(szExistsName)] = fast;
	_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szExistsName));
	fast->modify_type = EXISTS|DIRECTORY;
	fast->reserved = couple_id;
	// DisplayChanged(szExistsName, statcode_conv(CONTROL_TYPE_INSERT, EXISTS|DIRECTORY));
	///////////////////////////////////////////////////////
	fast = NFilesVec::AppendNewNode(&m_vFilesCache);
	m_hmFilesDual[NO_ROOT(szNewName)] = fast;
	_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
	fast->modify_type = MOVE|DIRECTORY;
	fast->reserved = couple_id;
	// DisplayChanged(szNewName, statcode_conv(CONTROL_TYPE_INSERT, MOVE|DIRECTORY));
}

void CAnchorCache::FolderCacheMove(void(*DisplayChanged)(const TCHAR *, DWORD), const TCHAR *szExistsName, const TCHAR *szNewName) { //目录改名的插入
	FilesVec::iterator iter;
	struct file_attrib *fast;
	TCHAR szFileName[MAX_PATH], szDisplayFile[MAX_PATH];
	DWORD IsFound = 0, dwDeleteDefault = 0;

	_tcscpy_s(szDisplayFile, szExistsName);
	int iExistsLength = _tcslen(NO_ROOT(szExistsName));
	for(iter=m_vFilesCache.begin(); iter!=m_vFilesCache.end(); iter++) {
		fast = *iter;
		if(!(EXISTS&fast->modify_type) && !_tcsncmp(fast->file_name, NO_ROOT(szExistsName), iExistsLength)) {
			if((COPY|MOVE) & fast->modify_type) IsFound = 1;
			//////////////////////////////////////////////////////////////////
			// refresh icon status
			if(!(DIRECTORY & fast->modify_type)) {
				_tcscpy_s(NO_ROOT(szDisplayFile), MAX_PATH-ROOT_LENGTH, fast->file_name);
				DisplayChanged(szDisplayFile, statcode_conv(CONTROL_TYPE_ERASE, fast->modify_type));
			}
			//////////////////////////////////////////////////////////////////
			if(NFilesVec::DeleteFromHmap(&m_hmFilesSingle, fast)) dwDeleteDefault |= 0x00000001;	
			if(NFilesVec::DeleteFromHmap(&m_hmFilesDual, fast)) dwDeleteDefault |= 0x00000002;	

			_tcscpy_s(szFileName, fast->file_name+iExistsLength);
			_tcscpy_s(fast->file_name, NO_ROOT(szNewName));
			_tcscat_s(fast->file_name, szFileName);
			
			if(0x00000001 & dwDeleteDefault) m_hmFilesSingle[fast->file_name] = fast;
			if(0x00000002 & dwDeleteDefault) m_hmFilesDual[fast->file_name] = fast;
			//////////////////////////////////////////////////////////////////
			// refresh icon status
			if(!(DIRECTORY & fast->modify_type)) {
				_tcscpy_s(NO_ROOT(szDisplayFile), MAX_PATH-ROOT_LENGTH, fast->file_name);
				DisplayChanged(szDisplayFile, statcode_conv(CONTROL_TYPE_INSERT, fast->modify_type));
			}
		}
	}
	if(IsFound) return;

	///////////////////////////////////////////////////////
	// install new node
	DWORD couple_id = GetTickCount();
	///////////////////////////////////////////////////////
	fast = NFilesVec::AppendNewNode(&m_vFilesCache);
	m_hmFilesDual[NO_ROOT(szExistsName)] = fast;
	_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szExistsName));
	fast->modify_type = EXISTS|DIRECTORY;
	fast->reserved = couple_id;
	// DisplayChanged(szExistsName, statcode_conv(CONTROL_TYPE_INSERT, EXISTS|DIRECTORY));
	///////////////////////////////////////////////////////
	fast = NFilesVec::AppendNewNode(&m_vFilesCache);
	m_hmFilesDual[NO_ROOT(szNewName)] = fast;
	_tcscpy_s(fast->file_name, MAX_PATH, NO_ROOT(szNewName));
	fast->modify_type = MOVE|DIRECTORY;
	fast->reserved = couple_id;
	// DisplayChanged(szNewName, statcode_conv(CONTROL_TYPE_INSERT, MOVE|DIRECTORY));
}

DWORD CAnchorCache::AddNewAnchor(DWORD dwNewAnchor) {
	if(objAnchor.FlushFilesVecFile(&m_vFilesCache))
		return -1;
	m_hmFilesSingle.clear();
	m_hmFilesDual.clear();
	NFilesVec::DeleteFilesVec(&m_vFilesCache);
	SetLastAnchor(dwNewAnchor); 

	return 0;
}

DWORD CAnchorCache::statcode_conv(DWORD control_type, DWORD modify_type) {
	DWORD dwDisplayCode = FILE_STATUS_UNKNOWN;

	switch(control_type) {
		case CONTROL_TYPE_INSERT:
			if(ADD & modify_type) dwDisplayCode = CACHE_CONTROL_INSERT | FILE_STATUS_ADDED;
			else if((EXISTS|COPY|MOVE|MODIFY) & modify_type) dwDisplayCode = CACHE_CONTROL_INSERT | FILE_STATUS_MODIFIED;
			else if(DEL & modify_type) dwDisplayCode = CACHE_CONTROL_INSERT | FILE_STATUS_DELETE;
			break;
		case CONTROL_TYPE_ERASE:
			if(ADD & modify_type) dwDisplayCode = CACHE_CONTROL_ERASE | FILE_STATUS_ADDED;
			else if((EXISTS|COPY|MOVE|MODIFY) & modify_type) dwDisplayCode = CACHE_CONTROL_ERASE | FILE_STATUS_MODIFIED;
			else if(DEL & modify_type) dwDisplayCode = CACHE_CONTROL_ERASE | FILE_STATUS_DELETE;
			break;
		default:
			break;
	}
	// if(DIRECTORY & modify_type) dwDisplayCode |= FILE_STATUS_DIRECTORY;

	return dwDisplayCode;
}
