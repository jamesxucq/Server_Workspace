#include "StdAfx.h"

#include "FastUtility.h"
#include "./RiverFS/RiverFS.h"
#include "DebugPrinter.h"
#include "LocalBzl.h"
#include "ChksAddi.h"
#include "ShellLinkBzl.h"

#include "AnchorBzl.h"

DWORD NAnchorBzl::Initialize(TCHAR *szAnchorInde, TCHAR *szAnchorData, TCHAR *szLocation) {
    if (!szAnchorInde || !szAnchorData || !szLocation) return ((DWORD)-1);
//
    // Init static object
    if(objAnchor.Initialize(szAnchorInde, szAnchorData, szLocation)) {
        _LOG_INFO(_T("anchor object initialize fail !\nFile: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    DWORD last_anchor;
    if(objAnchor.LoadLastActio(objAnchorCache.GetLastActioVec(), &last_anchor))
        return ((DWORD)-1);
    //
    if(objAnchorCache.Initialize(last_anchor)) {
        _LOG_INFO(_T("anchor cache object initialize fail !\nFile: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
//
    return 0x00;
}

DWORD NAnchorBzl::Finalize() {
    // flush the cache to file
    if(objAnchor.FlushActioVec(objAnchorCache.GetLastActioVec()))
        return ((DWORD)-1);
    //
    objAnchorCache.Finalize();
    // close the open handle
    objAnchor.Finalize();
    return 0x00;
}

//
static VOID HandleAdded(struct LockActio *lock_acti, const TCHAR *szDriveLetter) {
	struct FileID tAddedID = {0 , 0, 0, 0, NULL};
//
    if(LATT_DIRECTORY & lock_acti->dwActioType) {
		objChksAddi.GetFolderAttri(&tAddedID, lock_acti->ulFileInde);
		TCHAR timestr[64];
_LOG_DEBUG(_T("dire add:%s ftCreatTime:%s"), lock_acti->szFileName, timcon::ftim_unis(timestr, &tAddedID.ftCreatTime));
        RiverFS::FolderAppendVc(lock_acti->szFileName, &tAddedID.ftCreatTime, szDriveLetter);
		if(!lock_acti->dwExcepFound) objAnchorCache.FolderAddition(lock_acti->szFileName);
_LOG_INFO(_T("dire add:%s"), lock_acti->szFileName);
    } else {
		objChksAddi.GetFileAttri(&tAddedID, lock_acti->ulFileInde);
        RiverFS::FileAppend(&tAddedID, szDriveLetter);
		CloseFileID(tAddedID);
        if(!lock_acti->dwExcepFound) objAnchorCache.FileAddition(lock_acti->szFileName);
_LOG_INFO(_T("file add:%s\r\n"), lock_acti->szFileName); // disable by james 20140115
    }
}

static VOID HandleRemoved(struct LockActio *lock_acti) {
	// wprintf_s( L"Removed   - %s\n", szFileName );
	if(LATT_DIRECTORY & lock_acti->dwActioType) {
		ERASE_FOLDER_VC(lock_acti->szFileName);
		if(!lock_acti->dwExcepFound) objAnchorCache.FolderDelete(lock_acti->szFileName);
_LOG_INFO(_T("dire del:%s"), lock_acti->szFileName); // disable by james 20140115
	} else {
		RiverFS::DeleFileV(lock_acti->szFileName);
		if(!lock_acti->dwExcepFound) objAnchorCache.FileDelete(lock_acti->szFileName);
_LOG_INFO(_T("file del:%s"), lock_acti->szFileName); // disable by james 20140115
	}
}

static VOID HandleModified(struct LockActio *lock_acti) {
    // except directory in execute utility
    if(!lock_acti->dwExcepFound) objAnchorCache.FileModify(lock_acti->szFileName);
_LOG_INFO(_T("file modify:%s"), lock_acti->szFileName); // disable by james 20140115
}

static VOID HandleCopied(struct LockActio *exis_acti, struct LockActio *lock_acti, const TCHAR *szDriveLetter) {
	RiverFS::FileCopyV(exis_acti->szFileName, lock_acti->szFileName, szDriveLetter);
    if(!lock_acti->dwExcepFound) 
		objAnchorCache.FileCopy(exis_acti->szFileName, lock_acti->szFileName);
_LOG_INFO(_T("file copy:%s %s"), exis_acti->szFileName, lock_acti->szFileName); // disable by james 20140115
}

static VOID HandleMoved(struct LockActio *exis_acti, struct LockActio *lock_acti, const TCHAR *szDriveLetter) {
	RiverFS::FileMoveV(exis_acti->szFileName, lock_acti->szFileName, szDriveLetter);
	if(!lock_acti->dwExcepFound) 
		objAnchorCache.FileMoved(exis_acti->szFileName, lock_acti->szFileName);
_LOG_INFO(_T("file move:%s %s"), exis_acti->szFileName, lock_acti->szFileName); // disable by james 20140115
}

//
DWORD NAnchorBzl::InsAnchorAction(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter) { // >0:finish 0:nothing
    LkAiVec::iterator iter;
	struct LockActio *exis_acti;
    struct LockActio *lock_acti;
_LOG_DEBUG(_T("+++++++++++++++++++++++++ insert anchor files +++++++++++++++++++++++++")); // disable by james 20130410
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
        lock_acti = *iter;
// _LOG_DEBUG(_T("lock_acti->szFileName:%s lock_acti->dwActioType:%08X"), lock_acti->szFileName, lock_acti->dwActioType);
        if(ADDI & lock_acti->dwActioType) HandleAdded(lock_acti, szDriveLetter);
        else if(DELE & lock_acti->dwActioType) HandleRemoved(lock_acti);
		else if(MODIFY & lock_acti->dwActioType) HandleModified(lock_acti);
		else if(EXIST & lock_acti->dwActioType) exis_acti = lock_acti;
		else if(COPY & lock_acti->dwActioType) HandleCopied(exis_acti, lock_acti, szDriveLetter);
		else if(MOVE & lock_acti->dwActioType) HandleMoved(exis_acti, lock_acti, szDriveLetter);
    }
    //
_LOG_DEBUG(_T("+++++++++++++++++++++++++")); // disable by james 20130410
    // flush action cache to file
	if(objAnchor.FlushActioVec(objAnchorCache.GetLastActioVec())) {
        _LOG_ERROR(_T("flush action to file error."));
	}
// ndp::FilesVector(objAnchorCache.GetLastActioVec());
    //
    return 0x01;
}

// directory only addition delete
static void HandleActiveDirectory(FilesVec *pActiveVec, struct attent *inde_nod, FastKHmap *pActiveKHmap, struct FastKNode *pFoundKNode) {
	struct attent *found_nod;
	//
	found_nod = NFastUtili::FindNextValue(pFoundKNode);
	while(found_nod) {
		// _LOG_DEBUG(_T("--- 3 --- pFoundKNode:%08x %x %s"), pFoundKNode, pFoundKNode->atte->action_type, pFoundKNode->atte->file_name);
		// if(ADDI&found_nod->action_type && ADDI&inde_nod->action_type) { // } 
		if(ADDI&found_nod->action_type && DELE&inde_nod->action_type) { // 增加后删除-> M 0 
			NFastUtili::DeleFromHmap(pActiveKHmap, found_nod);
			NFilesVec::DeleFromVec(pActiveVec, found_nod); // --
			NFilesVec::DeleNode(found_nod);
		} else if(DELE&found_nod->action_type && ADDI&inde_nod->action_type) { // 删除后增加->修改 M 1 
			NFastUtili::DeleFromHmap(pActiveKHmap, found_nod);
			NFilesVec::DeleFromVec(pActiveVec, found_nod); // --
			NFilesVec::DeleNode(found_nod);
		} else if(DELE&found_nod->action_type && DELE&inde_nod->action_type) { //
			NFilesVec::MoveToLastNode(pActiveVec, found_nod);
		}
		//
		found_nod = NFastUtili::FindNextValue(NULL);
	}
}

static void CompActiveFile(FilesVec *pAddedVec, FilesVec *pActiveVec, struct attent *inde_nod, struct attent *nexis, FastKHmap *pActiveKHmap, struct FastKNode *pFoundKNode) {
	struct attent *found_nod, *oexis;
	//
	found_nod = NFastUtili::FindNextValue(pFoundKNode);
	while(found_nod) {
		// _LOG_DEBUG(_T("--- 3 --- pFoundKNode:%08x %x %s"), pFoundKNode, pFoundKNode->atte->action_type, pFoundKNode->atte->file_name);
		// if(ADDI&found_nod->action_type && ADDI&inde_nod->action_type) { // }
		// else if(ADDI&found_nod->action_type && MODIFY&inde_nod->action_type) { // 增加后修改->增加 M 1 // }
		if(ADDI&found_nod->action_type && DELE&inde_nod->action_type) { // 增加后删除-> M 0
			NFastUtili::DeleFromHmap(pActiveKHmap, found_nod);
			NFilesVec::DeleFromVec(pActiveVec, found_nod); // --
			NFilesVec::DeleNode(found_nod);
		} else if(ADDI&found_nod->action_type && EXIST&inde_nod->action_type) { // 增加后旧名->增加新名文件 M 3
			if(COPY & nexis->action_type) {
				if(NFilesVec::CompActioNode(pAddedVec, inde_nod, nexis)) {
					NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
					NFilesVec::AppNewNode(pAddedVec, nexis); // --				
				}
			} else if(MOVE & nexis->action_type) {
				_tcscpy_s(found_nod->file_name, MAX_PATH, nexis->file_name);
			}
		} //
		else if(MODIFY&found_nod->action_type && ADDI&inde_nod->action_type) { // fix NValidRouter::HandleValid bug by james 20130315
			found_nod->action_type = ADDI;
		} else if(MODIFY&found_nod->action_type && MODIFY&inde_nod->action_type) { // 修改后修改->修改 M 1
			found_nod->file_size = inde_nod->file_size;
		} else if(MODIFY&found_nod->action_type && DELE&inde_nod->action_type) { // 修改后删除->删除  M 0
			found_nod->action_type = DELE;
			NFilesVec::MoveToLastNode(pActiveVec, found_nod);
		} else if(MODIFY&found_nod->action_type && EXIST&inde_nod->action_type) { // 修改后改名->改名后修改 M 2
			if(NFilesVec::CompActioNode(pAddedVec, inde_nod, nexis)) {
				NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
				NFilesVec::AppNewNode(pAddedVec, nexis); // --			
			}
			_tcscpy_s(found_nod->file_name, MAX_PATH, nexis->file_name);
		} //
		else if(DELE&found_nod->action_type && ADDI&inde_nod->action_type) { // 删除后增加->修改 M 1
			found_nod->action_type = MODIFY;
			NFilesVec::MoveToLastNode(pActiveVec, found_nod);
		}
		// else if(DELE&found_nod->action_type && MODIFY&inde_nod->action_type) { // }
		else if(DELE&found_nod->action_type && DELE&inde_nod->action_type) { //
			NFilesVec::MoveToLastNode(pActiveVec, found_nod);
		} else if(DELE&found_nod->action_type && EXIST&inde_nod->action_type) { // fix file move bug by james 20130315
			// _LOG_DEBUG(_T("del:%s exis:%s new:%s"), found_nod->file_name, inde_nod->file_name, nexis->file_name);
			NFastUtili::DeleFromHmap(pActiveKHmap, found_nod);
			NFilesVec::DeleFromVec(pActiveVec, found_nod); // --
			NFilesVec::DeleNode(found_nod);
			if(NFilesVec::CompActioNode(pAddedVec, inde_nod, nexis)) {
				NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
				NFilesVec::AppNewNode(pAddedVec, nexis); // --			
			}
		} //
		else if(EXIST&found_nod->action_type && ADDI&inde_nod->action_type) { // 旧名后增加->增加 M 2
			if(NFilesVec::CompActioNode(pAddedVec, inde_nod)) {
				NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
			}
		}
		// else if(EXIST&found_nod->action_type && MODIFY&inde_nod->action_type){ // 旧名后修改-> // }
		// else if(EXIST&found_nod->action_type && DELE&inde_nod->action_type){ // 旧名后删除-> // }
		else if(EXIST&found_nod->action_type && EXIST&inde_nod->action_type) { // 旧名后旧名->
			if(NFilesVec::CompActioNode(pAddedVec, inde_nod, nexis)) {
				NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
				NFilesVec::AppNewNode(pAddedVec, nexis); // --			
			}
		} //
		// else if(COPY&found_nod->action_type && ADDI&inde_nod->action_type){ // 新名后改名->改名 M 1 // }
		else if(COPY&found_nod->action_type && MODIFY&inde_nod->action_type) { // 新名后改名->改名 M 1
			if(NFilesVec::CompActioNode(pAddedVec, inde_nod)) {
				NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
			}
		} else if(COPY&found_nod->action_type && DELE&inde_nod->action_type) { // 新名后改名->改名 M 1
			oexis = NFilesVec::FindExistNode(pActiveVec, found_nod); // --
			NFastUtili::DeleFromHmap(pActiveKHmap, found_nod);
			NFilesVec::DeleFromVec(pActiveVec, found_nod); // --
			NFilesVec::DeleNode(found_nod);
			NFastUtili::DeleFromHmap(pActiveKHmap, oexis);
			NFilesVec::DeleFromVec(pActiveVec, oexis); // --
			NFilesVec::DeleNode(oexis);
		} else if(COPY&found_nod->action_type && EXIST&inde_nod->action_type) { //新名后改名->改名 M 1
			if(COPY & nexis->action_type) {
				if(NFilesVec::CompActioNode(pAddedVec, inde_nod, nexis)) {
					NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
					NFilesVec::AppNewNode(pAddedVec, nexis); // --
				}
			} else if(MOVE & nexis->action_type) {
				_tcscpy_s(found_nod->file_name, MAX_PATH, nexis->file_name);
			}
		} //
		// else if(MOVE&found_nod->action_type && ADDI&inde_nod->action_type) { // 新名后改名->改名 M 1 // }
		else if(MOVE&found_nod->action_type && MODIFY&inde_nod->action_type) { // 新名后修改->改名 修改 M 1
			if(NFilesVec::CompActioNode(pAddedVec, inde_nod)) {
				NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
			}
		} else if(MOVE&found_nod->action_type && DELE&inde_nod->action_type) { // 新名后删除->删除旧名 M 1
			oexis = NFilesVec::FindExistNode(pActiveVec, found_nod); // --
			oexis->action_type = DELE;
			oexis->reserved = 0;
			NFastUtili::DeleFromHmap(pActiveKHmap, found_nod);
			NFilesVec::DeleFromVec(pActiveVec, found_nod); // --
			NFilesVec::DeleNode(found_nod);
		} else if(MOVE&found_nod->action_type && EXIST&inde_nod->action_type) { // 新名后旧名->改名 M 1
			if(COPY & nexis->action_type) {
				if(NFilesVec::CompActioNode(pAddedVec, inde_nod, nexis)) {
					NFilesVec::AppNewNode(pAddedVec, inde_nod); // --
					NFilesVec::AppNewNode(pAddedVec, nexis); // --
				}
			} else if(MOVE & nexis->action_type) {
				oexis = NFilesVec::FindExistNode(pActiveVec, found_nod); // --
				if(!_tcscmp(nexis->file_name, oexis->file_name)) {
					NFastUtili::DeleFromHmap(pActiveKHmap, found_nod);
					NFilesVec::DeleFromVec(pActiveVec, found_nod); // --
					NFilesVec::DeleNode(found_nod);
					NFastUtili::DeleFromHmap(pActiveKHmap, oexis);
					NFilesVec::DeleFromVec(pActiveVec, oexis); // --
					NFilesVec::DeleNode(oexis);
				} else _tcscpy_s(found_nod->file_name, MAX_PATH, nexis->file_name);
			}
		}
		//
		found_nod = NFastUtili::FindNextValue(NULL);
	}
}

static void FinalActiveFile(FilesVec *pActiveVec, FastKHmap *pActiveKHmap, FilesVec *pAddedVec) {
    FilesVec::iterator iter;
	for(iter = pAddedVec->begin(); pAddedVec->end() != iter; ++iter) {
		NFastUtili::InseToHmap(pActiveKHmap, *iter);
	}
	NFilesVec::MoveVec(pActiveVec, pAddedVec);
}

static void HandleActiveFile(FilesVec *pActiveVec, struct attent *inde_nod, struct attent *nexis, FastKHmap *pActiveKHmap, struct FastKNode *pFoundKNode) {
	FilesVec vAddedVec;
	CompActiveFile(&vAddedVec, pActiveVec, inde_nod, nexis, pActiveKHmap, pFoundKNode);
	FinalActiveFile(pActiveVec, pActiveKHmap, &vAddedVec);
}

static void HandleNexisFile(FilesVec *pActiveVec, struct attent *inde_nod, struct attent *nexis, FastKHmap *pActiveKHmap, struct FastKNode *pNexisKNode) {
	struct attent *found_nod, *new_nod;
	//
	found_nod = NFastUtili::FindNextValue(pNexisKNode);
	while(found_nod) {
		// _LOG_DEBUG(_T("--- 3 --- pNexisKNode:%08x %x %s"), pNexisKNode, pNexisKNode->atte->action_type, pNexisKNode->atte->file_name);
		if(DELE&found_nod->action_type && COPY&nexis->action_type) { // 删除后复制->修改 M 1
			found_nod->action_type = MODIFY;
			NFilesVec::MoveToLastNode(pActiveVec, found_nod);
		} else if(DELE&found_nod->action_type && MOVE&nexis->action_type) { //
			new_nod = NFilesVec::AppNewNode(pActiveVec, inde_nod); // --
			NFastUtili::InseToHmap(pActiveKHmap, new_nod);
			//
			found_nod->action_type = MOVE;
			NFilesVec::MoveToLastNode(pActiveVec, found_nod);
		}
		//
		found_nod = NFastUtili::FindNextValue(NULL);
	}
}

//
VOID NAnchorBzl::OptimizeActiveAction(FilesVec *pActiveVec, FilesVec *pIndeVec) {
	FastKHmap mDireKHmap, mFileKHmap;
	struct FastKNode *pFoundKNode, *pNexisKNode;
    struct attent *inde_nod = NULL, *nexis;
	FilesVec::iterator iter;
//
// _LOG_DEBUG(_T("***********************************"));
	for(iter = pIndeVec->begin(); pIndeVec->end() != iter; ) {
        inde_nod = *iter;
// _LOG_DEBUG(_T("--- inde_nod:%08x %08x %s"), inde_nod, inde_nod->action_type, inde_nod->file_name);
// _LOG_DEBUG(_T("--- 1 ---"));
		if(ATTRB_IS_DIRECTORY(inde_nod->action_type)) { // handle directory
			pFoundKNode = NFastUtili::FindFromHmap(&mDireKHmap, inde_nod);
			if(pFoundKNode) {
				HandleActiveDirectory(pActiveVec, inde_nod, &mDireKHmap, pFoundKNode);
				iter = pIndeVec->erase(iter);
				NFilesVec::DeleNode(inde_nod);	
			} else { // handle directory
				pActiveVec->push_back(inde_nod);
				NFastUtili::InseToHmap(&mDireKHmap, inde_nod);
				iter = pIndeVec->erase(iter);			
			}
		} else {
			pFoundKNode = NFastUtili::FindFromHmap(&mFileKHmap, inde_nod);
			if(EXIST & inde_nod->action_type) {
				iter = pIndeVec->erase(iter);
				nexis = *iter;
				if(!((COPY|MOVE)&nexis->action_type)) {
_LOG_WARN(_T("action type list error!"));
					continue;
				}
				pNexisKNode = NFastUtili::FindFromHmap(&mFileKHmap, nexis);
// _LOG_DEBUG(_T("--- nexis:%08x %08x %s"), nexis, nexis->action_type, nexis->file_name);
				/////////////////////////////////////////////////////////////
				if(pNexisKNode) {
					HandleNexisFile(pActiveVec, inde_nod, nexis, &mFileKHmap, pNexisKNode);
					iter = pIndeVec->erase(iter);
					NFilesVec::DeleNode(nexis);
					NFilesVec::DeleNode(inde_nod);
				} else if(pFoundKNode) {
					HandleActiveFile(pActiveVec, inde_nod, nexis, &mFileKHmap, pFoundKNode);
					iter = pIndeVec->erase(iter);
					NFilesVec::DeleNode(nexis);
					NFilesVec::DeleNode(inde_nod);
				} else {
					pActiveVec->push_back(inde_nod);
					NFastUtili::InseToHmap(&mFileKHmap, inde_nod);
					pActiveVec->push_back(nexis);
					NFastUtili::InseToHmap(&mFileKHmap, nexis);
					iter = pIndeVec->erase(iter);
				}
			} else {
				if(pFoundKNode) { // handle file
					HandleActiveFile(pActiveVec, inde_nod, NULL, &mFileKHmap, pFoundKNode);
					iter = pIndeVec->erase(iter);
					NFilesVec::DeleNode(inde_nod);	
				} else { // handle file
					pActiveVec->push_back(inde_nod);
					NFastUtili::InseToHmap(&mFileKHmap, inde_nod);
					iter = pIndeVec->erase(iter);
				}
			}
		}
// _LOG_DEBUG(_T("-------------"));
// ndp::FilesVector(pActiveVec);
// _LOG_DEBUG(_T("***********************************"));
	}
//
	NFastUtili::DeleFastKHmap(&mFileKHmap);
	NFastUtili::DeleFastKHmap(&mDireKHmap);
}

//
// #define MAX_OPTIMIZE_NUM       4096
// 0:ok (DWORD)-1):error
DWORD NAnchorBzl::GetActionByAnchor(FilesVec *pActioVec, DWORD dwAnchor) {
    struct IndexValue tIndeStart, tIndeEnd;
    static FilesVec vIndeVec;
//
	if(dwAnchor == objAnchorCache.GetLastAnchor()) {
// _LOG_DEBUG(_T("local last anchor is find anchor."));	 // disable by james 20130410
		if(NFilesVec::DeepCopyVec(pActioVec, objAnchorCache.GetLastActioVec()))
			return ((DWORD)-1);
	} else {
		memset(&tIndeStart, '\0', sizeof(struct IndexValue));
		memset(&tIndeEnd, '\0', sizeof(struct IndexValue));
// _LOG_DEBUG(_T("search anchor."));	 // disable by james 20130410
		if(objAnchor.SearchAnchor(&tIndeStart, &tIndeEnd, dwAnchor))
			return ((DWORD)-1);
// _LOG_DEBUG(_T("load action."));	 // disable by james 20130410
		if(objAnchor.LoadActioByIndeValue(&vIndeVec, &tIndeStart, &tIndeEnd))
			return ((DWORD)-1);
// _LOG_DEBUG(_T("invoke optimize active action."));	 // disable by james 20130410
		OptimizeActiveAction(pActioVec, &vIndeVec);
// _LOG_DEBUG(_T("list_value:%d"), list_value);
_LOG_DEBUG(_T("invoke delete files vector."));	 // disable by james 20130410
		NFilesVec::DeleFilesVec(&vIndeVec);	
	}
//
_LOG_DEBUG(_T("invoke final."));	 // disable by james 20130410
// ndp::FilesVector(pActioVec);
//
    return 0x00;
}

DWORD NAnchorBzl::AddNewAnchor(DWORD dwNewAnchor) {
    DWORD dwAddedValue = 0;
    //
    if(objAnchor.FlushActioVec(objAnchorCache.GetLastActioVec())) dwAddedValue = ((DWORD)-1);
    // flush files cache,addition new anchor to cache.
    if(objAnchorCache.AddNewAnchor(dwNewAnchor)) dwAddedValue = ((DWORD)-1);
    // addition new anchor to file data
    if(objAnchor.AddNewAnchor(dwNewAnchor)) dwAddedValue = ((DWORD)-1);
// _LOG_DEBUG(_T("set anchor file unlocked "));	 // disable by james 20130410
    return dwAddedValue;
}

DWORD NAnchorBzl::DiscardAction() {
    DWORD dwDiscValue = 0;
	if(objAnchor.FlushActioVec(NULL)) dwDiscValue = ((DWORD)-1);
	if(objAnchorCache.AddNewAnchor(0x00)) dwDiscValue = ((DWORD)-1);
	return dwDiscValue;
}

VOID NAnchorBzl::ClearAnchorEnviro(BOOL bEraseAnchor) {
    if(bEraseAnchor) objAnchor.EraseAnchor();
    objAnchorCache.ClearActioVec();
}

//
static void SetIconDisplay(FilesVec *pAvtiveVec, const TCHAR *szDriveLetter) {
	FilesVec::iterator iter;
    TCHAR szFileName[MAX_PATH];
	//
	DRIVE_LETTE(szFileName, szDriveLetter)
	for(iter = pAvtiveVec->begin(); pAvtiveVec->end() != iter; ++iter) {
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, (*iter)->file_name);
		NShellLinkBzl::BatchIconDisplay(szFileName, 
			ATTRB_IS_DIRECTORY((*iter)->action_type), 
			CONV_STATUS_SETA((*iter)->action_type));
	}
}

DWORD NAnchorBzl::OptimizeCached(const TCHAR *szDriveLetter) {
    FilesVec vActiveVec, *pIndeVec;
//
_LOG_DEBUG(_T("get active actio."));
    pIndeVec = objAnchorCache.GetLastActioVec();
ndp::FilesVector(pIndeVec);
_LOG_DEBUG(_T("invoke optimize active action.")); // disable by james 20140207
	OptimizeActiveAction(&vActiveVec, pIndeVec);
ndp::FilesVector(&vActiveVec);
_LOG_DEBUG(_T("invoke delete files vector.")); // disable by james 20140207
    NFilesVec::DeleFilesVec(pIndeVec);
_LOG_DEBUG(_T("invoke final.")); // disable by james 20140207
    NFilesVec::ShallowCopyVec(pIndeVec, &vActiveVec);
	// insert finished, refresh explorer windows
	SetIconDisplay(pIndeVec, szDriveLetter);
	REFRESH_EXPLORER_WINDOWS(szDriveLetter)
    // flush action cache to file
	if(objAnchor.FlushActioVec(pIndeVec)) {
        _LOG_ERROR(_T("flush action to file error."));
	}
// ndp::FilesVector(pIndeVec);
    return pIndeVec->empty()? 0: 0x0001;
}

VOID NAnchorBzl::InserAnchorIcon(const TCHAR *szDriveLetter) {
	FilesVec::iterator iter;
    TCHAR szFileName[MAX_PATH];
	//
	FilesVec *pAvtiveVec = objAnchorCache.GetLastActioVec();
	DRIVE_LETTE(szFileName, szDriveLetter)
	for(iter = pAvtiveVec->begin(); pAvtiveVec->end() != iter; ++iter) {
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, (*iter)->file_name);
		NShellLinkBzl::BatchIconDisplay(szFileName, 
			ATTRB_IS_DIRECTORY((*iter)->action_type), 
			CONV_STATUS_SETA((*iter)->action_type));
	}
}

VOID NAnchorBzl::EraseAnchorIcon(const TCHAR *szDriveLetter) {
	FilesVec::iterator iter;
    TCHAR szFileName[MAX_PATH];
	//
	FilesVec *pAvtiveVec = objAnchorCache.GetLastActioVec();
	DRIVE_LETTE(szFileName, szDriveLetter)
	for(iter = pAvtiveVec->begin(); pAvtiveVec->end() != iter; ++iter) {
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, (*iter)->file_name);
		NShellLinkBzl::BatchIconDisplay(szFileName, 
			ATTRB_IS_DIRECTORY((*iter)->action_type), 
			CONV_STATUS_ERAS((*iter)->action_type));
	}
}