#include "StdAfx.h"
#include <afxmt.h>
#include "AnchorCache.h"

#include "AnchorFile.h"

CAnchorCache::CAnchorCache(void)
{
	m_htFilesAttNormal.set_empty_key(_T(""));
	m_htFilesAttNormal.set_deleted_key(_T(":"));

	m_htFilesAttExistNew.set_empty_key(_T(""));
	m_htFilesAttExistNew.set_deleted_key(_T(":"));
}

CAnchorCache::~CAnchorCache(void)
{
}

CAnchorCache OAnchorCache;

bool CAnchorCache::Create()
{
	m_bSyncingLocked = false; //0 no 1 yes
	//m_bSyncingLocked = true; //for test

	//DWORD m_uiLastAnchor;
	//FilesAttribVec m_vFilesAttCache; //after last anchor modify files; 
	if(OAnchorFile.LoadLastFilesVec(&m_vFilesAttCache, &m_uiLastAnchor))
		return false;

	return true;
}

bool CAnchorCache::Destroy()
{
	if(OAnchorFile.FlushFilesVecFile(&m_vFilesAttCache))
		return false;
	m_htFilesAttNormal.clear();
	m_htFilesAttExistNew.clear();
	CFilesVec::DeleteFilesVec(&m_vFilesAttCache);

	return false;
}

// MFC临界区类对象
CCriticalSection gcs_AnchorLocker;
void CAnchorCache::SetAnchorLockStatus(bool bSyncingLocked)
{
	// 进入临界区
	gcs_AnchorLocker.Lock();
	m_bSyncingLocked = bSyncingLocked; 
	//离开临界区
	gcs_AnchorLocker.Unlock();
}

bool CAnchorCache::GetAnchorLockStatus()
{
	// 进入临界区
	gcs_AnchorLocker.Lock();
	bool bAnchorLockStatus = m_bSyncingLocked;
	//离开临界区
	gcs_AnchorLocker.Unlock();

	return bAnchorLockStatus;
}

//#define _FILE_STRUCT(STRUCT_PTR)	(*(STRUCT_PTR))
DWORD CAnchorCache::FilesCacheInsert(wchar_t *sChangeName, char file_status) //add modify delete
{
	//FilesAttribVec::iterator iter; 
	dense_hash_map <wstring, struct FileAttrib *>::iterator htiter;
	struct FileAttrib *fstw, *exsn_fst;
	char sFinalStatus = UNKNOWN;

	htiter = m_htFilesAttNormal.find(sChangeName);
	if(htiter != m_htFilesAttNormal.end()) {
		switch((htiter->second)->mod_type) {
			case ADD:
				//if(ADD == file_status)
				//else if(MODIFY == file_status)
				if(DEL == file_status) {
					CFilesVec::DeleteFromFilesVec(&m_vFilesAttCache, htiter->second);
					CFilesVec::DeleteFilesAtt(htiter->second);
					m_htFilesAttNormal.erase(htiter);
					//SET_FILESVEC_CHANGED
					sFinalStatus = ERASE; //erase
				}
			break;
			case MODIFY:
				//if(ADD == file_status)
				//else if(MODIFY == file_status)
				if(DEL == file_status) {
					(htiter->second)->mod_type = DEL;
					//SET_FILESVEC_CHANGED	
					sFinalStatus = DEL;
				}
			break;
			case DEL:
				if(ADD == file_status) {
					(htiter->second)->mod_type = MODIFY;
					//SET_FILESVEC_CHANGED	
					sFinalStatus = MODIFY;
				}
				//else if(MODIFY == file_status)
				//else if(DEL == file_status)
			break;
		}
		return sFinalStatus;
	}

	htiter = m_htFilesAttExistNew.find(sChangeName);
	if(htiter != m_htFilesAttExistNew.end()) {
		switch((htiter->second)->mod_type) {
			case EXIST:
				if(ADD == file_status) {
					fstw = CFilesVec::AppendNewFilesNode(&m_vFilesAttCache);
					m_htFilesAttNormal[sChangeName] = fstw;
					_tcscpy_s(fstw->filename, MAX_PATH, sChangeName);
					fstw->mod_type = ADD;
					//SET_FILESVEC_CHANGED
					sFinalStatus = ADD;
				}
				//else if(MODIFY == file_status)
				//else if(DEL == file_status)	
			break;
			case NEW:
				//if(ADD == file_status)
				if(MODIFY == file_status) {
					fstw = CFilesVec::AppendNewFilesNode(&m_vFilesAttCache);
					m_htFilesAttNormal[sChangeName] = fstw;
					_tcscpy_s(fstw->filename, MAX_PATH, sChangeName);
					fstw->mod_type = MODIFY;
					//SET_FILESVEC_CHANGED
					sFinalStatus = MODIFY;
				}
				else if(DEL == file_status) {
					exsn_fst = CFilesVec::FilesRenameExist(&m_vFilesAttCache, htiter->second);
					exsn_fst->mod_type = DEL;

					CFilesVec::DeleteFromFilesVec(&m_vFilesAttCache, htiter->second);
					CFilesVec::DeleteFilesAtt(htiter->second);
					m_htFilesAttNormal.erase(htiter);
					//SET_FILESVEC_CHANGED
					sFinalStatus = DEL;
				}
			break;
		}
		return sFinalStatus;
	}

	// install new node
	fstw = CFilesVec::AppendNewFilesNode(&m_vFilesAttCache);
	m_htFilesAttNormal[sChangeName] = fstw;
	_tcscpy_s(fstw->filename, MAX_PATH, sChangeName);
	fstw->mod_type = file_status;
	//SET_FILESVEC_CHANGED
	sFinalStatus = file_status;	

	return sFinalStatus;
}


DWORD CAnchorCache::FilesCacheInsert(wchar_t *sExistName, wchar_t *sNewName) //exist new
{
	//FilesAttribVec::iterator iter;
	dense_hash_map <wstring, struct FileAttrib *>::iterator htiter;
	struct FileAttrib *fstw;
	char sFinalStatus = UNKNOWN;

	htiter = m_htFilesAttNormal.find(sExistName);
	if(htiter != m_htFilesAttNormal.end()) {
		switch((htiter->second)->mod_type) {
			case ADD: {
				_tcscpy_s((htiter->second)->filename, MAX_PATH, sNewName);	
				//SET_FILESVEC_CHANGED
				sFinalStatus = ADD;
			}
			break;
			case MODIFY:
				fstw = CFilesVec::PrevFilesInsertNew(&m_vFilesAttCache, htiter->second);
				m_htFilesAttNormal[sExistName] = fstw;
				_tcscpy_s(fstw->filename, MAX_PATH, sExistName);
				fstw->mod_type = EXIST;

				_tcscpy_s((htiter->second)->filename, MAX_PATH, sNewName);
				(htiter->second)->mod_type = NEW;

				fstw = CFilesVec::AppendNewFilesNode(&m_vFilesAttCache);
				m_htFilesAttNormal[sNewName] = fstw;
				_tcscpy_s(fstw->filename, MAX_PATH, sNewName);
				fstw->mod_type = MODIFY;
					
				//SET_FILESVEC_CHANGED
				sFinalStatus = MODIFY;
			break;
			case DEL:
			break;
		}
		return sFinalStatus;
	}

	htiter = m_htFilesAttExistNew.find(sExistName);
	if(htiter != m_htFilesAttExistNew.end()) {
		switch((htiter->second)->mod_type) {
				case EXIST:
				break;
				case NEW:
					_tcscpy_s((htiter->second)->filename, MAX_PATH, sNewName);
					//SET_FILESVEC_CHANGED
					sFinalStatus = MODIFY;
				break;
		}
		return sFinalStatus;
	}

	// install new node
	fstw = CFilesVec::AppendNewFilesNode(&m_vFilesAttCache);
	m_htFilesAttExistNew[sExistName] = fstw;
	_tcscpy_s(fstw->filename, MAX_PATH, sExistName);
	fstw->mod_type = EXIST;

	fstw = CFilesVec::AppendNewFilesNode(&m_vFilesAttCache);
	m_htFilesAttExistNew[sNewName] = fstw;
	_tcscpy_s(fstw->filename, MAX_PATH, sNewName);
	fstw->mod_type = NEW;

	sFinalStatus = MODIFY;
	return sFinalStatus;
}


/*
DWORD CAnchorCache::FilesCacheInsert(char *sExistFile, char *sNewFile) //exist new
{
	FilesAttribVec::iterator iter;
	struct FileAttrib *fstw;
	DWORD dwFound = 0;
	char sFinalStatus = UNKNOWN;

	for(iter=m_vFilesAttCache.begin(); iter!=m_vFilesAttCache.end(); ) { //iter++
		if(!strcmp((*iter)->filename, sExistFile)) {
			dwFound = 1;
			switch((*iter)->type) {
				case ADD: {
					strcpy_s((*iter)->filename, sNewFile);	
					//SET_FILESVEC_CHANGED
					sFinalStatus = ADD;
				}
					iter++;
				break;
				case MODIFY:
					fstw = CFilesVec::AppendNewFilesNode(m_vFilesAttCache);
					strcpy_s(fstw->filename, sExistFile);
					fstw->type = EXIST;
					fstw = CFilesVec::AppendNewFilesNode(m_vFilesAttCache);
					strcpy_s(fstw->filename, sNewFile);
					fstw->type = NEW;
					fstw = CFilesVec::AppendNewFilesNode(m_vFilesAttCache);
					strcpy_s(fstw->filename, sNewFile);
					fstw->type = MODIFY;
					
					CFilesVec::DeleteFilesAtt(*iter);
					iter = m_vFilesAttCache.erase(iter);
					//SET_FILESVEC_CHANGED
					sFinalStatus = MODIFY;
				break;
				case DEL:
				case EXIST:
					iter++;
				break;
				case NEW:
					strcpy_s((*iter)->filename, sNewFile);
					//SET_FILESVEC_CHANGED
					sFinalStatus = MODIFY;
					iter++;
				break;
				default:
					iter++;
				break;
			}
		}
		else iter++;
	}
	if(!dwFound) {
		fstw = CFilesVec::AppendNewFilesNode(m_vFilesAttCache);
		m_htFilesAttNormal[sExistFile] = fstw;

		strcpy_s(fstw->filename, sExistFile);
		fstw->type = EXIST;

		fstw = CFilesVec::AppendNewFilesNode(m_vFilesAttCache);
		m_htFilesAttNormal[sNewFile] = fstw;

		strcpy_s(fstw->filename, sNewFile);
		fstw->type = NEW;
		sFinalStatus = MODIFY;
	}

	return sFinalStatus;
}
*/
int CAnchorCache::AddNewAnchor(DWORD uiAnchor)
{
	if(OAnchorFile.FlushFilesVecFile(&m_vFilesAttCache))
		return -1;
	m_htFilesAttNormal.clear();
	m_htFilesAttExistNew.clear();
	CFilesVec::DeleteFilesVec(&m_vFilesAttCache);
	OAnchorCache.m_uiLastAnchor = uiAnchor; 

	return 0;
}