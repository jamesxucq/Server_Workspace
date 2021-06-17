#include "StdAfx.h"
#include "AnchorBzl.h"

#include "Logger.h"
//#include "ClientBzl.h"
//#include "CacheDisplayBzl.h"


CAnchorBzl::CAnchorBzl(void)
{
}

CAnchorBzl::~CAnchorBzl(void)
{
}

DWORD CAnchorBzl::Create(wchar_t *sSyncingType, wchar_t *sAnchorIndex, wchar_t *sAnchorData, wchar_t *sLocation)
{
	if (!sSyncingType || !sAnchorIndex || !sAnchorData || !sLocation) return -1;

	// Init static object
	if(!OAnchorFile.Create(sSyncingType, sAnchorIndex, sAnchorData, sLocation))
	{
		LOG_TRACE(_T("OAnchorFile Create False !\nFile: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}	

	if(!OAnchorCache.Create()) {
		LOG_TRACE(_T("OAnchorCache Create False !\nFile: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}

	if(OAnchorFile.CheckSlockedFilesExist()) {	
		if(GetSlockedFilesTurnon()) return -1;
	}

	return 0;
}

DWORD CAnchorBzl::Destroy()
{
	//flush the cache to file
	OAnchorCache.Destroy();

	//close the open handle
	OAnchorFile.Destroy();

	return 0;
}

//int CAnchorBzl::WatchDirFileChged() //0 no ; not 0 yes
//{ return  pFilesVecCache->empty(); }
DWORD CAnchorBzl::AnchFilesVecInsert(wchar_t *sChangeName, char file_status) //0:succ 1:found -1:err
{
	static int iInsertCount = 0;
	DWORD iFinalStatus;
LOG_DEBUG(_T("sChangeName:%s file_status:%c"), sChangeName, file_status);
	if(OAnchorCache.GetAnchorLockStatus()) {
		OAnchorFile.LockedFilesInsert(NOROOT_PATH(sChangeName), file_status);
		iFinalStatus = file_status;
	}
	else {
		iFinalStatus = OAnchorCache.FilesCacheInsert(NOROOT_PATH(sChangeName), file_status);
		if(UNKNOWN == iFinalStatus) return -1;
		//if(!m_dwFilesVecChged && !m_dwSyncingLocked)
		if(!((++iInsertCount)%FLUSH_FILESVEC_COUNT)) 
			OAnchorFile.FlushFilesVecFile(OAnchorCache.GetLastFilesVecPointer());
	}
//	CCacheDisplayBzl::DisplayStatusChanged(sChangeName, ConvertFileStatusCode((char)iFinalStatus));
//	CSyncingBzl::SetWaitingEvent();

	return 0;
}

DWORD CAnchorBzl::AnchFilesVecInsert(wchar_t *sExistName, wchar_t *sNewName) //0:succ 1:found -1:err
{
	DWORD iFinalStatus;
LOG_DEBUG(_T("sExistName:%s sNewName:%s"), sExistName, sNewName);
	if(OAnchorCache.GetAnchorLockStatus()) {
		OAnchorFile.LockedFilesInsert(NOROOT_PATH(sExistName), EXIST);
		OAnchorFile.LockedFilesInsert(NOROOT_PATH(sNewName), NEW);
		iFinalStatus = MODIFY;
	}
	else {
		iFinalStatus = OAnchorCache.FilesCacheInsert(NOROOT_PATH(sExistName), NOROOT_PATH(sNewName));
		if(UNKNOWN == iFinalStatus) return -1;

		//if(!m_dwFilesVecChged && !m_dwSyncingLocked)
		OAnchorFile.FlushFilesVecFile(OAnchorCache.GetLastFilesVecPointer());
	}
//	CCacheDisplayBzl::DisplayStatusChanged(sNewName, ConvertFileStatusCode((char)iFinalStatus));
//	CSyncingBzl::SetWaitingEvent();

	return 0;
}


DWORD CAnchorBzl::AnchFilesVecInsert(FilesAttribVec *vpLockedFilesVec) 
{
	FilesAttribVec::iterator iter;
	wchar_t sExistFileName[MAX_PATH];
	//wchar_t sChangeName[MAX_PATH];
	DWORD iFinalStatus = UNKNOWN;

	memset(sExistFileName, '\0', MAX_PATH);
	for(iter=vpLockedFilesVec->begin(); iter!=vpLockedFilesVec->end(); iter++) {
		if(ADD==(*iter)->mod_type || MODIFY==(*iter)->mod_type || DEL==(*iter)->mod_type) {
			iFinalStatus = OAnchorCache.FilesCacheInsert((*iter)->filename, (*iter)->mod_type);
			//CStringCon::Ansi2Unicode(sChangeName, (*iter)->filename);
//			CCacheDisplayBzl::DisplayStatusChanged((*iter)->filename, ConvertFileStatusCode((char)iFinalStatus));
		}
		else if(EXIST==(*iter)->mod_type)
			_tcscpy_s(sExistFileName, MAX_PATH, (*iter)->filename);
		else if(NEW==(*iter)->mod_type) {
			iFinalStatus = OAnchorCache.FilesCacheInsert(sExistFileName, (*iter)->filename);
			//CStringCon::Ansi2Unicode(sChangeName, (*iter)->filename);
//			CCacheDisplayBzl::DisplayStatusChanged((*iter)->filename, ConvertFileStatusCode((char)iFinalStatus));
		}
	}
	OAnchorFile.FlushFilesVecFile(OAnchorCache.GetLastFilesVecPointer());
//	CSyncingBzl::SetWaitingEvent();

	return iFinalStatus;
}


#define HandleAmdFilesAtt(vModifyFilesAtt, vIndexFilesAtt, iter) \
{ \
	vpModifyFilesAtt->push_back(*iter); \
	iter = vpIndexFilesAtt->erase(iter); \
}
#define HandleExsnFilesAtt(vModifyFilesAtt, vIndexFilesAtt, iter) \
{ \
	vpModifyFilesAtt->push_back(*iter); \
	iter = vpIndexFilesAtt->erase(iter); \
	vpModifyFilesAtt->push_back(*iter); \
	iter = vpIndexFilesAtt->erase(iter); \
}
int CAnchorBzl::CreatModifyFilesVec(FilesAttribVec *vpModifyFilesAtt, FilesAttribVec *vpIndexFilesAtt)
{
	vector <struct FileAttrib *>::iterator iter;
	struct FileAttrib *exsn_fst;
	struct FileAttrib *IndexFilesAtt = NULL, *ModifyFilesAtt = NULL;

	if(!vpModifyFilesAtt || !vpIndexFilesAtt)
		return -1;
	if(vpIndexFilesAtt->empty()) 
		return 0;

	for(iter = vpIndexFilesAtt->begin(); iter != vpIndexFilesAtt->end(); ) {
		IndexFilesAtt = *iter;
		ModifyFilesAtt = CFilesVec::FindFromFilesVecEx(vpModifyFilesAtt, *iter);
		if(!ModifyFilesAtt){
			if(EXIST != IndexFilesAtt->mod_type)
				HandleAmdFilesAtt(vModifyFilesAtt, vIndexFilesAtt, iter)
			else HandleExsnFilesAtt(vModifyFilesAtt, vIndexFilesAtt, iter)
			continue;
		}
		while(ModifyFilesAtt){
			//if(ADD==ModifyFilesAtt->mod_type && ADD==IndexFilesAtt->mod_type){
			//}		
			//if(ADD==ModifyFilesAtt->mod_type && MODIFY==IndexFilesAtt->mod_type){//增加后修改->增加 M 1
			//}
			if(ADD==ModifyFilesAtt->mod_type && DEL==IndexFilesAtt->mod_type){//增加后删除-> M 0
				CFilesVec::DeleteFromFilesVec(vpModifyFilesAtt, ModifyFilesAtt);
				CFilesVec::DeleteFilesAtt(ModifyFilesAtt);
			}
			else if(ADD==ModifyFilesAtt->mod_type && EXIST==IndexFilesAtt->mod_type){//增加后旧名->增加新名文件 M 3
				exsn_fst = CFilesVec::FilesRenameNew(vpIndexFilesAtt, IndexFilesAtt);
				CFilesVec::DeleteFromFilesVec(vpIndexFilesAtt, exsn_fst);

				_tcscpy_s(ModifyFilesAtt->filename, MAX_PATH, exsn_fst->filename);
				CFilesVec::DeleteFilesAtt(exsn_fst);;
			}
			//else if(ADD==ModifyFilesAtt->mod_type && NEW==IndexFilesAtt->mod_type){
			//}
			//else if(MODIFY==ModifyFilesAtt->mod_type && ADD==IndexFilesAtt->mod_type){
			//}
			//else if(MODIFY==ModifyFilesAtt->mod_type && MODIFY==IndexFilesAtt->mod_type){//修改后修改->修改 M 1
			//}
			else if(MODIFY==ModifyFilesAtt->mod_type && DEL==IndexFilesAtt->mod_type){//修改后删除->删除  M 0
				ModifyFilesAtt->mod_type = DEL;
			}
			else if(MODIFY==ModifyFilesAtt->mod_type && EXIST==IndexFilesAtt->mod_type){//修改后改名->改名后修改 M 2
				CFilesVec::PrevFilesInsertNew(vpModifyFilesAtt, IndexFilesAtt, ModifyFilesAtt);

				exsn_fst = CFilesVec::FilesRenameNew(vpIndexFilesAtt, IndexFilesAtt);
				CFilesVec::DeleteFromFilesVec(vpIndexFilesAtt, exsn_fst);
				CFilesVec::PrevFilesInsert(vpModifyFilesAtt, exsn_fst, ModifyFilesAtt);

				_tcscpy_s(ModifyFilesAtt->filename, MAX_PATH, exsn_fst->filename);
			}
			//if(MODIFY==ModifyFilesAtt->mod_type && NEW==IndexFilesAtt->mod_type){
			//}
			else if(DEL==ModifyFilesAtt->mod_type && ADD==IndexFilesAtt->mod_type){//删除后增加->修改 M 1 
				ModifyFilesAtt->mod_type = MODIFY;
			}
			//else if(DEL==ModifyFilesAtt->mod_type && MODIFY==IndexFilesAtt->mod_type){
			//}
			//else if(DEL==ModifyFilesAtt->mod_type && DEL==IndexFilesAtt->mod_type){
			//}
			//else if(DEL==ModifyFilesAtt->mod_type && EXIST==IndexFilesAtt->mod_type){
			//}
			else if(DEL==ModifyFilesAtt->mod_type && NEW==IndexFilesAtt->mod_type){//删除后新名->修改 M 1 
				ModifyFilesAtt->mod_type = MODIFY;
			}
			else if(EXIST==ModifyFilesAtt->mod_type && ADD==IndexFilesAtt->mod_type){//旧名后增加->增加 M 2
				CFilesVec::AppendNewFilesNode(vpModifyFilesAtt, IndexFilesAtt);
			}
			//else if(EXIST==ModifyFilesAtt->mod_type && MODIFY==IndexFilesAtt->mod_type){//旧名后修改->
			//}
			//else if(EXIST==ModifyFilesAtt->mod_type && DEL==IndexFilesAtt->mod_type){//旧名后删除->
			//}
			//else if(EXIST==ModifyFilesAtt->mod_type && EXIST==IndexFilesAtt->mod_type){//旧名后旧名->
			//}
			else if(EXIST==ModifyFilesAtt->mod_type && NEW==IndexFilesAtt->mod_type){//旧名后新名->改名 M 2
				CFilesVec::AppendNewFilesNode(vpModifyFilesAtt, IndexFilesAtt);
			}
			//else if(NEW==ModifyFilesAtt->mod_type && ADD==IndexFilesAtt->mod_type){//新名后改名->改名 M 1
			//}
			else if(NEW==ModifyFilesAtt->mod_type && MODIFY==IndexFilesAtt->mod_type){//新名后修改->改名 修改 M 1
				CFilesVec::AppendNewFilesNode(vpModifyFilesAtt, IndexFilesAtt);
			}
			else if(NEW==ModifyFilesAtt->mod_type && DEL==IndexFilesAtt->mod_type){//新名后删除->删除旧名 M 1
				//exist_item = RenExistItem(*ModFlistH, mod_item);
				exsn_fst = CFilesVec::FilesRenameExist(vpModifyFilesAtt, ModifyFilesAtt);
				ModifyFilesAtt->mod_type = DEL;
				ModifyFilesAtt->Reserved = 0;
				CFilesVec::DeleteFromFilesVec(vpModifyFilesAtt, exsn_fst);
				CFilesVec::DeleteFilesAtt(exsn_fst);
			}
			else if(NEW==ModifyFilesAtt->mod_type && EXIST==IndexFilesAtt->mod_type){//新名后旧名->改名 M 1
				//exist_item = RenExistItem(IndexFlist, ndx_item);
				exsn_fst = CFilesVec::FilesRenameNew(vpIndexFilesAtt, IndexFilesAtt);
				CFilesVec::DeleteFromFilesVec(vpIndexFilesAtt, exsn_fst);

				_tcscpy_s(ModifyFilesAtt->filename, MAX_PATH, (*iter)->filename);
				CFilesVec::DeleteFilesAtt(exsn_fst);
			}
			//else if(NEW==ModifyFilesAtt->mod_type && NEW==IndexFilesAtt->mod_type){//新名后改名->改名 M 1	
			//}

			//if(EXIST==ModifyFilesAtt->mod_type)
			ModifyFilesAtt = CFilesVec::FindFromFilesVecEx(NULL, NULL);
			//else break;
		}
		iter++;
	}

	return 0;
}

int CAnchorBzl::GetFastsyncFilesVecByAnchor(FilesAttribVec *vpFastsyncFileAttrib, DWORD uiAnchor)
{
	struct IndexrowStruct stIndexrowStart, stIndexrowEnd;
	FilesAttribVec vIndexFilesAttVec;

	if(!vpFastsyncFileAttrib) return -1;

	memset(&stIndexrowStart, '\0', sizeof(struct IndexrowStruct));
	memset(&stIndexrowEnd, '\0', sizeof(struct IndexrowStruct));
	if(uiAnchor == OAnchorCache.m_uiLastAnchor) {
		if(!CFilesVec::CopyFilesAttribVec(vpFastsyncFileAttrib, OAnchorCache.GetLastFilesVecPointer()))
			return -1;
	}
	else {
		if(OAnchorFile.SearchAnchor(&stIndexrowStart, &stIndexrowEnd, uiAnchor))
			return -1;
		if(OAnchorFile.LoadFilesVecByIndexrow(&vIndexFilesAttVec, &stIndexrowStart, &stIndexrowEnd))
			return -1;
		if(CreatModifyFilesVec(vpFastsyncFileAttrib, &vIndexFilesAttVec))
			return -1;
		CFilesVec::DeleteFilesVec(&vIndexFilesAttVec);
	}

	return 0;
}

DWORD CAnchorBzl::ConvertFileStatusCode(char file_status)
{
	DWORD dwDisplayCode = FILE_STATUS_UNKNOWN;

	switch(file_status) {
		case ADD:
			dwDisplayCode = SD_CACHECTRL_INSERT | FILE_STATUS_ADDED;
		break;
		case MODIFY:
			dwDisplayCode = SD_CACHECTRL_INSERT | FILE_STATUS_MODIFIED;
		break;	
		case DEL:
			dwDisplayCode = SD_CACHECTRL_INSERT | FILE_STATUS_DELETE;
		break;
		case ERASE:
			dwDisplayCode = SD_CACHECTRL_ERASE;
		break;	
	}

	return dwDisplayCode;
}


DWORD CAnchorBzl::GetSyncingLockedFiles() 
{
	FilesAttribVec vSlockedFilesVec;
	
	if(OAnchorFile.LoadLockedFilesVec(&vSlockedFilesVec))
		return -1;	
	if(AnchFilesVecInsert(&vSlockedFilesVec))
		return -1;
	CFilesVec::DeleteFilesVec(&vSlockedFilesVec);

	return 0; //succ
}

DWORD CAnchorBzl::GetSlockedFilesTurnon() 
{
	FilesAttribVec vSlockedFilesVec;
	
	if(OAnchorFile.LoadLockedFilesVecTurnon(&vSlockedFilesVec))
		return -1;	
	if(AnchFilesVecInsert(&vSlockedFilesVec))
		return -1;
	CFilesVec::DeleteFilesVec(&vSlockedFilesVec);

	return 0; //succ
}

int CAnchorBzl::UnlockAnchorFile() 
{ 
	int iAnchStatus = 0;

	OAnchorCache.SetAnchorLockStatus(false); 
	if(OAnchorFile.CheckSlockedFilesExist()) {	
		if(GetSyncingLockedFiles()) iAnchStatus = -1;
	}
	//OAnchorFile.AddNewAnchor(uiAnchor);
	
LOG_DEBUG(_T("set anchor file unlocked "));
	return 0;
}

int CAnchorBzl::AddNewAnchAndUnlock(DWORD uiAnchor) 
{
	int iAnchStatus = 0;

	if(OAnchorCache.AddNewAnchor(uiAnchor))
		iAnchStatus = -1; 
	if(OAnchorFile.AddNewAnchor(uiAnchor))
		iAnchStatus = -1; 
	OAnchorCache.SetAnchorLockStatus(false); 
	if(OAnchorFile.CheckSlockedFilesExist()) {	
		if(GetSyncingLockedFiles()) iAnchStatus = -1;
	}

LOG_DEBUG(_T("set anchor file unlocked "));		
	return iAnchStatus;
}