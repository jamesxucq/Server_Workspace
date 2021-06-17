#include "StdAfx.h"

#include "clientcom/clientcom.h"
#include "./RiverFS/RiverFS.h"
#include "DebugPrinter.h"
#include "FastUtility.h"
#include "LocalBzl.h"
#include "ChksAddi.h"
#include "SHAHTable.h"

#include "ParseLkAi.h"

//
#define FOLDER_RECURS_DEFAULT   _T("folder_recurs.tmp")
static DWORD RecursParsePath(HANDLE hReally, const TCHAR *szPath);

//
// for test
static TCHAR *ChksSha1(unsigned char *sha1sum) {
    static TCHAR chksum_str[64];
    for (int inde=0; inde<SHA1_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, 41, _T("%02x"), sha1sum[inde]);
    chksum_str[40] = _T('\0');
//
	return chksum_str;
}

static TCHAR *ftim_unis(TCHAR *timestr, FILETIME *filetime) {  // Wed, 15 Nov 1995 04:58:08 GMT
    SYSTEMTIME st;
    if(!FileTimeToSystemTime(filetime, &st)) return NULL;
    struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
    _tcsftime(timestr, 32, _T("%a, %d %b %Y %H:%M:%S GMT"), &gm);
    return timestr;
}

static TCHAR *slawi(FILETIME *last_write) {
    static TCHAR timestr[64];
    ftim_unis(timestr, last_write);
	return timestr;
}

//
static DWORD CompLkAiSi(unsigned char *riv_sha1, FILETIME *riv_ctim, unsigned __int64 riv_fsiz, struct LockActio *comp_acti) { // 0x00:not found 0x01:found 0x02:not pass
	unsigned __int64 qwFileSize;
	unsigned char szSha1Chks[SHA1_DIGEST_LEN];
	FILETIME ftCreatTime;
	DWORD comp_value = 0x00;
	//
	objChksAddi.GetFileAttri(&qwFileSize, &ftCreatTime, szSha1Chks, comp_acti->ulFileInde);
	if(!memcmp(riv_ctim, &ftCreatTime, sizeof(FILETIME)) && (riv_fsiz == qwFileSize)) {
		if(memcmp(riv_sha1, szSha1Chks, SHA1_DIGEST_LEN)) comp_value = 0x02;
        else comp_value = 0x01;
	}
_LOG_DEBUG(_T("++++++++ comp_value:%d qwFileSize:%I64u ,find szFileName:%s"), comp_value, qwFileSize, comp_acti->szFileName);
_LOG_DEBUG(_T("++++++++ ftCreatTime:%s"), slawi(&ftCreatTime));
_LOG_DEBUG(_T("++++++++ szSha1Chks:%s"), ChksSha1(szSha1Chks));
	return comp_value;
}

// 0x00:not found 0x01:found 0x02:not pass
static DWORD FindAdditionSi(struct LockActio **coup_acti, FastZHmap *pFastZHmap, struct LockActio *lock_acti) {
	unordered_map <wstring, struct FastZNode*>::iterator aite;
	TCHAR *fileName = _tcsrchr(lock_acti->szFileName, _T('\\'));
	aite = pFastZHmap->find(++fileName);
	struct FastZNode *pZNode = (aite->second);
	while(lock_acti != pZNode->lock_acti) pZNode = pZNode->pNext;
	//
	unsigned char riv_sha1[SHA1_DIGEST_LEN];
	FILETIME riv_ctim;
	unsigned __int64 riv_fsiz;
	DWORD find_value = 0x00;
	//
	RiverFS::FileAttribCache(&riv_fsiz, riv_sha1, &riv_ctim, lock_acti->szFileName);
_LOG_DEBUG(_T("++++++++ szFileName:%s"), lock_acti->szFileName);
_LOG_DEBUG(_T("++++++++ riv_fsiz:%I64u"), riv_fsiz);
_LOG_DEBUG(_T("++++++++ riv_sha1:%s"), ChksSha1(riv_sha1));
_LOG_DEBUG(_T("++++++++ riv_ctim:%s"), slawi(&riv_ctim));
	for(pZNode = pZNode->pNext; pZNode; pZNode = pZNode->pNext) {
		if(ADDI & pZNode->dwActioType) {
			if(find_value = CompLkAiSi(riv_sha1, &riv_ctim, riv_fsiz, pZNode->lock_acti)){
				*coup_acti = pZNode->lock_acti;
				break;
			}		
		}
	}
    //
    return find_value;
}

static DWORD CompLkAiCo(FILETIME *riv_ctim, unsigned __int64 riv_fsiz, struct LockActio *comp_acti) { // 0x00:no 0x01:0k
	FILETIME ftCreatTime;
	unsigned __int64 qwFileSize;
	objChksAddi.GetFileAttri(&qwFileSize, &ftCreatTime, NULL, comp_acti->ulFileInde);
	//
// _LOG_DEBUG(_T("++++++++ qwFileSize:%I64u ,find szFileName:%s"), qwFileSize, comp_acti->szFileName);
// _LOG_DEBUG(_T("++++++++ ftCreatTime:%s"), slawi(&ftCreatTime));
	if(!memcmp(riv_ctim, &ftCreatTime, sizeof(FILETIME)) && (riv_fsiz == qwFileSize))
		return 0x01;
	return 0x00;
}

// 0x00:not found 0x01:found 0x02:not pass
static DWORD FindAdditionCo(struct LockActio **coup_acti, hash_table *htab, struct LockActio *lock_acti) {
	unordered_map <wstring, struct FastZNode*>::iterator aite;
	unsigned char riv_sha1[SHA1_DIGEST_LEN];
	FILETIME riv_ctim;
	unsigned __int64 riv_fsiz;
	DWORD find_value = 0x00;
//
	RiverFS::FileAttribCache(&riv_fsiz, riv_sha1, &riv_ctim, lock_acti->szFileName);
// _LOG_DEBUG(_T("++++++++ szFileName:%s"), szFileName);
// _LOG_DEBUG(_T("++++++++ riv_fsiz:%I64u"), riv_fsiz);
// _LOG_DEBUG(_T("++++++++ riv_sha1:%s"), ChksSha1(riv_sha1));
// _LOG_DEBUG(_T("++++++++ riv_ctim:%s"), slawi(&riv_ctim));
	struct FastZNode *pZNode = (struct FastZNode *)hash_value(riv_sha1, htab);
	for(; pZNode; pZNode = pZNode->pNext) {
		if(CompLkAiCo(&riv_ctim, riv_fsiz, pZNode->lock_acti)){
// _LOG_DEBUG(_T("++++++++ found szFileName:%s"), pZNode->lock_acti->szFileName);
			*coup_acti = pZNode->lock_acti;
			find_value = 0x01;
			break;
		}
	}
//
    return find_value;
}

static VOID ParseFileMoveName(LkAiVec *pFileLkAi, LkAiVec *pExisAtta) {
	FastZHmap mFastZHmap;
	struct LockActio *lock_acti, *coup_acti;
	LkAiVec::iterator iter;
	// the same name, move
	NFastUtili::BuildFastZHmap(&mFastZHmap, pFileLkAi);
    for(iter=pFileLkAi->begin(); pFileLkAi->end()!=iter; ++iter) {
        lock_acti = *iter;
		if((DELE&lock_acti->dwActioType) && !lock_acti->coupleID) {
			DWORD find_value = FindAdditionSi(&coup_acti, &mFastZHmap, lock_acti);
			if(find_value) {
				lock_acti->dwActioType = LATT_FILE|EXIST;
				lock_acti->coupleID = NFileUtility::GetCoupleID();
				coup_acti->dwActioType = LATT_FILE|MOVE;
				coup_acti->coupleID = lock_acti->coupleID;
// _LOG_DEBUG(_T("++++++++ name move,szExistName:%s szNewName:%s"), lock_acti->szFileName, coup_acti->szFileName);
				if(0x02 == find_value) {
					NLkAiVec::AppNewNode(pExisAtta, coup_acti->szFileName, LATT_FILE|MODIFY, lock_acti->coupleID);	
// _LOG_DEBUG(_T("++++++++ not pass,app modify szFileName:%s"), coup_acti->szFileName);
				}
			}
		}
    }
	NFastUtili::DeleFastZHmap(&mFastZHmap);
}

static VOID ParseFileMoveChks(LkAiVec *pFileLkAi) {
	struct LockActio *lock_acti, *coup_acti;
	LkAiVec::iterator iter;
	// the same check sum, move
	hash_table *htab = NSHAHmap::BuildSHAHTab(pFileLkAi);
    for(iter=pFileLkAi->begin(); pFileLkAi->end()!=iter; ++iter) {
        lock_acti = *iter;
		if((DELE&lock_acti->dwActioType) && !lock_acti->coupleID) {
			if(FindAdditionCo(&coup_acti, htab, lock_acti)) {
				lock_acti->dwActioType = LATT_FILE|EXIST;
				lock_acti->coupleID = NFileUtility::GetCoupleID();
				coup_acti->dwActioType = LATT_FILE|MOVE;
				coup_acti->coupleID = lock_acti->coupleID;
// _LOG_DEBUG(_T("++++++++ chks move,szExistName:%s szNewName:%s"), lock_acti->szFileName, coup_acti->szFileName);
            }
		}
    }
	NSHAHmap::DeleSHAHTab(htab);
}

DWORD FindAdditionPy(TCHAR *szExistFile, struct LockActio *lock_acti, const TCHAR *szDriveLetter) { // 0x00:not found 0x01:found
	unsigned __int64 qwFileSize;
	unsigned char szSha1Chks[SHA1_DIGEST_LEN];
	//
	objChksAddi.GetFileAttri(&qwFileSize, NULL, szSha1Chks, lock_acti->ulFileInde);
// _LOG_DEBUG(_T("++++++++ qwFileSize:%I64u ,find szFileName:%s"), qwFileSize, lock_acti->szFileName);
// _LOG_DEBUG(_T("++++++++ szSha1Chks:%s"), ChksSha1(szSha1Chks));
	if(RiverFS::FindDupliFile(szExistFile, lock_acti->szFileName, szDriveLetter, qwFileSize, szSha1Chks))
		return 0x01;
	return 0x00;
}

static DWORD FindModifyPy(FastZHmap *pFastZHmap, TCHAR *szFileName) { // 0x00:not found 0x01:found
	unordered_map <wstring, struct FastZNode*>::iterator site;
	site = pFastZHmap->find(szFileName);
	if(pFastZHmap->end() != site) return 0x01;
	return 0x00;
}

static VOID ParseFileCopyChks(LkAiVec *pFileLkAi, LkAiVec *pModiLkAi, LkAiVec *pAttaExis, const TCHAR *szDriveLetter) {
	struct LockActio *lock_acti;
	LkAiVec::iterator iter;
	TCHAR szExistFile[MAX_PATH];
	//
	FastZHmap mFastZHmap;
	NFastUtili::BuildFastZHmapEx(&mFastZHmap, pModiLkAi);
	//  the same check sum, copy
    for(iter=pFileLkAi->begin(); pFileLkAi->end()!=iter; ++iter) {
        lock_acti = *iter;
		if((ADDI&lock_acti->dwActioType) && !lock_acti->coupleID) {
_LOG_DEBUG(_T("++++++++ szFileName:%s"), lock_acti->szFileName);
			if(FindAdditionPy(szExistFile, lock_acti, szDriveLetter)) {
				lock_acti->dwActioType = LATT_FILE|COPY;
				lock_acti->coupleID = NFileUtility::GetCoupleID();
				NLkAiVec::AppNewNode(pAttaExis, szExistFile, LATT_FILE|EXIST, lock_acti->coupleID);
_LOG_DEBUG(_T("++++++++ chks copy,szExistName:%s szNewName:%s"), szExistFile, lock_acti->szFileName);
				if(FindModifyPy(&mFastZHmap, szExistFile)) {
					NLkAiVec::AppNewNode(pAttaExis, lock_acti->szFileName, LATT_FILE|MODIFY, lock_acti->coupleID);
_LOG_DEBUG(_T("++++++++ copy have modify,app modify szFileName:%s"), lock_acti->szFileName);
				}
            }
		}
    }
	//
	NFastUtili::DeleFastZHmap(&mFastZHmap);
}

VOID NParseLkAi::ParseFileLkAi(LkAiVec *pFileLkAi, LkAiVec *pModiLkAi, LkAiVec *pAttaExis, const TCHAR *szDriveLetter) {
_LOG_DEBUG(_T("++++++++ parse file name move."));
	ParseFileMoveName(pFileLkAi, pAttaExis);
_LOG_DEBUG(_T("++++++++ parse file chks move."));
	ParseFileMoveChks(pFileLkAi);
_LOG_DEBUG(_T("++++++++ parse file chks copy."));
	ParseFileCopyChks(pFileLkAi, pModiLkAi, pAttaExis, szDriveLetter);
}

//
static VOID PushBackLkAi(LkAiVec *pLkAiVec, struct FastZNode *pZNode, struct LockActio *lock_acti) {
	struct LockActio *mvex_acti = NULL;
	struct LockActio *modi_acti = NULL;
	struct FastZNode *znod_inde;
	//
// _LOG_DEBUG(_T("++++++++ pZNode:%08X"), pZNode);
	for(znod_inde = pZNode; znod_inde; znod_inde = znod_inde->pNext) {
// _LOG_DEBUG(_T("++++++++ at:%08X add:%s"), znod_inde->dwActioType, znod_inde->lock_acti->szFileName);
		if((MOVE|EXIST) & znod_inde->dwActioType) mvex_acti = znod_inde->lock_acti;
		else if(MODIFY & znod_inde->dwActioType) modi_acti = znod_inde->lock_acti;
	}
// _LOG_DEBUG(_T("++++++++ mvex_acti:%08X"), mvex_acti);
	if(mvex_acti) {
		if(EXIST & lock_acti->dwActioType) {
			pLkAiVec->push_back(lock_acti);
			pLkAiVec->push_back(mvex_acti);		
		} else if(COPY & lock_acti->dwActioType) {
			pLkAiVec->push_back(mvex_acti);	
			pLkAiVec->push_back(lock_acti);
		}
// _LOG_DEBUG(_T("++++++++ szExistName:%s szNewName:%s"), lock_acti->szFileName, mvex_acti->szFileName);
		if(modi_acti) {
// _LOG_DEBUG(_T("++++++++ szModiName:%s"), modi_acti->szFileName);
			pLkAiVec->push_back(modi_acti);
		}	
	}
}

static VOID BuildActiveLkAiVec(LkAiVec *pLkAiVec, LkAiVec *pToLkAi, FastNHmap *pFastNHmap) {
	struct LockActio *lock_acti;
	LkAiVec::iterator iter;
	unordered_map <DWORD, struct FastZNode*>::iterator site;
	//
    for(iter=pToLkAi->begin(); pToLkAi->end()!=iter; ++iter) {
		lock_acti = (*iter);
// _LOG_DEBUG(_T("++++++++ szFileName:%s coupleID:%d"), lock_acti->szFileName, lock_acti->coupleID);
		if(lock_acti->coupleID) {
			site = pFastNHmap->find(lock_acti->coupleID);
			if(pFastNHmap->end() != site)
				PushBackLkAi(pLkAiVec, (site)->second, lock_acti);
		} else pLkAiVec->push_back(lock_acti);
    }
	//
}

VOID NParseLkAi::CleanLkAiNode(LkAiVec *pLkAiVec, LkAiVec *pAttaExis) {
	FastNHmap mFastNHmap;
	LkAiVec vToLkAi;
	//
	NLkAiVec::MoveLkAiVec(&vToLkAi, pAttaExis, pLkAiVec);
_LOG_DEBUG(_T("------- vToLkAi"));
// ndp::ActionPrinter(&vToLkAi);// disable by james 20140115
_LOG_DEBUG(_T("------- pAttaExis"));
// ndp::ActionPrinter(pAttaExis);// disable by james 20140115
	NFastUtili::BuildFastNHmap(&mFastNHmap, pAttaExis);
	//
	BuildActiveLkAiVec(pLkAiVec, &vToLkAi, &mFastNHmap);
	NFastUtili::DeleFastNHmap(&mFastNHmap);
	vToLkAi.clear(); 
}

// 解析LkAiVec生成copy move
VOID NParseLkAi::ParseAnchoLkAi(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter) {
    struct LockActio *lock_acti;
    LkAiVec::iterator iter;
	// 解析文件
_LOG_DEBUG(_T("++++++++++++++++ parse file lkai."));
	LkAiVec vFileLkAi, vModiLkAi;
	LkAiVec vExisAtta;
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
        lock_acti = *iter;
		if(LATT_FILE & lock_acti->dwActioType) {
			if((DELE|ADDI) & lock_acti->dwActioType) vFileLkAi.push_back(lock_acti);
			else vModiLkAi.push_back(lock_acti);
		}
    } 
_LOG_DEBUG(_T("------- file before vFileLkAi"));
// ndp::ActionPrinter(&vFileLkAi);// disable by james 20140115
	ParseFileLkAi(&vFileLkAi, &vModiLkAi, &vExisAtta, szDriveLetter);
_LOG_DEBUG(_T("------- file after vFileLkAi"));
// ndp::ActionPrinter(&vFileLkAi);// disable by james 20140115
_LOG_DEBUG(_T("------- vModiLkAi"));
// ndp::ActionPrinter(&vModiLkAi);// disable by james 20140115
	vModiLkAi.clear();
	vFileLkAi.clear();
	// 调整顺序
_LOG_DEBUG(_T("++++++++++++++++ adjust lkai poision."));
_LOG_DEBUG(_T("------- vExisAtta"));
// ndp::ActionPrinter(&vExisAtta);// disable by james 20140115
	CleanLkAiNode(pLkAiVec, &vExisAtta);
	vExisAtta.clear(); 
//
_LOG_DEBUG(_T("------- final vFileLkAi"));
// ndp::ActionPrinter(pLkAiVec);// disable by james 20140115
}
	
//
//
static VOID FileLockedTip(TCHAR *szFileName) {
    _LOG_WARN(_T("instert anchor, file locked:%s"), szFileName);
    TCHAR szMessage[MAX_TEXT_LEN];
    _stprintf_s(szMessage, _T("文件%s被锁定,请关闭!"), szFileName);
    NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, szMessage);
}

//
#define FLOCK_RETRY_TIMES			48 // 48*12/60 9.6m
#define FILE_LOCKED_DELAY			12000 // 12s

static DWORD LkAiAddiChks(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter) { // 0x00:ok !0:exception
    LkAiVec::iterator iter;
    struct LockActio *lock_acti;
    TCHAR szFileName[MAX_PATH];
    DWORD dwParseValue = 0x00;
    //
	DRIVE_LETTE(szFileName, szDriveLetter)
	struct FileID tFileID = {0, 0, 0, 0, NULL};
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
        lock_acti = *iter;
// _LOG_DEBUG(_T("lock_acti->szFileName:%s lock_acti->dwActioType:%08X"), lock_acti->szFileName, lock_acti->dwActioType);
        _tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, lock_acti->szFileName);
		//
		if(ADDI & lock_acti->dwActioType) {
			if(LATT_DIRECTORY & lock_acti->dwActioType) {
				RiverFS::InitFolderID(&tFileID, szFileName);
				lock_acti->ulFileInde = objChksAddi.AppFolderAttri(&tFileID);
			} else {
				DWORD dwLockRetry = 0x00;
				HANDLE hFileLocked;
				DWORD dwFileLocked = NFileUtility::LockReadFile(&hFileLocked, szFileName);
				while((FLOCK_RETRY_TIMES > dwLockRetry++) && (FILE_LOCKED == dwFileLocked)) {
					Sleep(FILE_LOCKED_DELAY);
					dwFileLocked = NFileUtility::LockReadFile(&hFileLocked, szFileName);
				}
				//
				if(FILE_UNLOCK == dwFileLocked) {
					RiverFS::InitFileID(&tFileID, szFileName);
					lock_acti->ulFileInde = objChksAddi.AppFileAttri(&tFileID);
					CloseFileID(tFileID);
					NFileUtility::UnlockFile(hFileLocked);
				} else if(dwFileLocked) {
					dwParseValue = ((DWORD)-1);
					FileLockedTip(szFileName);
					break;
				}
			}
		}
    }
	//
	return dwParseValue;
}

//
static VOID InseAddiLkAi(LkAiVec *pLkAiVec, FastZHmap *pFastZHmap, HANDLE hINodeA, struct LockActio *lock_acti) {
	struct INodeUtili::INodeA tINodeA;
	struct LockActio *inse_acti;
	//
	INodeUtili::ResetINodeHandle(hINodeA, FALSE);
	inse_acti = NLkAiVec::InsPrevNewNode(pLkAiVec, lock_acti);
	while(!INodeUtili::INodeANext(hINodeA, &tINodeA)) {
// _LOG_DEBUG(_T("read node dwINodeType:%08X szINodeName:%s"), tINodeA.dwINodeType, tINodeA.szINodeName);
		if(!NFastUtili::FindAddiZHmap(pFastZHmap, tINodeA.szINodeName)) {
			if(INTYPE_FILE == tINodeA.dwINodeType) {
				inse_acti = NLkAiVec::InsPrevNewNode(pLkAiVec, NULL);
				inse_acti->dwActioType = LATT_FILE|ADDI;
		        _tcscpy_s(inse_acti->szFileName, MAX_PATH, tINodeA.szINodeName);
// _LOG_DEBUG(_T("++++++++ app file, szFileName:%s"), tINodeA.szINodeName);
			} else {
				inse_acti = NLkAiVec::InsPrevNewNode(pLkAiVec, NULL);
				inse_acti->dwActioType = LATT_DIRECTORY|ADDI;
		        _tcscpy_s(inse_acti->szFileName, MAX_PATH, tINodeA.szINodeName);
// _LOG_DEBUG(_T("++++++++ app dire, szFileName:%s"), tINodeA.szINodeName);
			}
			NFastUtili::AddNewZNode(pFastZHmap, inse_acti);
		}
	}
	//
}

static VOID InseDeleLkAi(LkAiVec *pLkAiVec, FastZHmap *pFastZHmap, HANDLE hINodeA, struct LockActio *lock_acti) {
	struct INodeUtili::INodeA tINodeA;
	struct LockActio *inse_acti;
	//
	INodeUtili::ResetINodeHandle(hINodeA, FALSE);
	inse_acti = NLkAiVec::InsPrevNewNode(pLkAiVec, lock_acti);
	while(!INodeUtili::INodeANext(hINodeA, &tINodeA)) {
_LOG_DEBUG(_T("read node dwINodeType:%08X szINodeName:%s"), tINodeA.dwINodeType, tINodeA.szINodeName);
		if(!NFastUtili::FindDeleZHmap(pFastZHmap, tINodeA.szINodeName)) {
			if(INTYPE_FILE == tINodeA.dwINodeType) {
				inse_acti = NLkAiVec::InsPrevNewNode(pLkAiVec, NULL);
				inse_acti->dwActioType = LATT_FILE|DELE;
		        _tcscpy_s(inse_acti->szFileName, MAX_PATH, tINodeA.szINodeName);
_LOG_DEBUG(_T("++++++++ dele file, szFileName:%s"), tINodeA.szINodeName);
			} else {
				inse_acti = NLkAiVec::InsPrevNewNode(pLkAiVec, NULL);
				inse_acti->dwActioType = LATT_DIRECTORY|DELE;
		        _tcscpy_s(inse_acti->szFileName, MAX_PATH, tINodeA.szINodeName);
_LOG_DEBUG(_T("++++++++ dele dire, szFileName:%s"), tINodeA.szINodeName);
			}
			NFastUtili::AddNewZNode(pFastZHmap, inse_acti);
		}
	}
	//
}

VOID ParseFolderLkAi(LkAiVec *pLkAiVec, LkAiVec *pFolderLkAi, const TCHAR *szDriveLetter) {
	FastZHmap mFastZHmap;
	TCHAR szFolderName[MAX_PATH];
	struct LockActio *lock_acti;
	LkAiVec::iterator iter;
	//
	DRIVE_LETTE(szFolderName, szDriveLetter)
	HANDLE hINodeA = NFileUtility::BuildCacheFile(FOLDER_RECURS_DEFAULT);
	NFastUtili::BuildFastZHmapEx(&mFastZHmap, pLkAiVec);
	//
    for(iter=pFolderLkAi->begin(); pFolderLkAi->end()!=iter; ++iter) {
        lock_acti = *iter;
_LOG_DEBUG(_T("++++++++ lock_acti->szFileName:%s"), lock_acti->szFileName);
		if(ADDI & lock_acti->dwActioType) {
			_tcscpy_s(NO_LETT(szFolderName), MAX_PATH-LETT_LENGTH, lock_acti->szFileName);
			INodeUtili::ResetINodeHandle(hINodeA, TRUE);
_LOG_DEBUG(_T("++++++++ recurs find local path."));
			RecursParsePath(hINodeA, szFolderName);
			InseAddiLkAi(pLkAiVec, &mFastZHmap, hINodeA, lock_acti);
		} else if(DELE & lock_acti->dwActioType) {
			INodeUtili::ResetINodeHandle(hINodeA, TRUE);
_LOG_DEBUG(_T("++++++++ recurs find river path."));
			RiverFS::FolderRecursINodeA(hINodeA, lock_acti->szFileName);
			InseDeleLkAi(pLkAiVec, &mFastZHmap, hINodeA, lock_acti);
		}
    }
	CloseHandle( hINodeA );
	NFastUtili::DeleFastZHmap(&mFastZHmap);
}

static VOID RecursFolderLkAi(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter) {
    struct LockActio *lock_acti;
    LkAiVec::iterator iter;
	// 解析目录 
_LOG_DEBUG(_T("------- init pLkAiVec"));
// ndp::ActionPrinter(pLkAiVec);// disable by james 20140115
_LOG_DEBUG(_T("++++++++++++++++ parse directoryy lkai."));
	LkAiVec vFolderLkAi;
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
        lock_acti = *iter;
		if(LATT_DIRECTORY & lock_acti->dwActioType) vFolderLkAi.push_back(lock_acti);
    }
// _LOG_DEBUG(_T("------- dire before vFileLkAi"));
	ParseFolderLkAi(pLkAiVec, &vFolderLkAi, szDriveLetter);
	vFolderLkAi.clear(); 
// _LOG_DEBUG(_T("------- dire before vFileLkAi"));
}

DWORD NParseLkAi::AnchoLkAiAddi(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter, const TCHAR *szLocation) { // 0x00:ok !0:exception
	objChksAddi.Initialize(szLocation);
	//
	RecursFolderLkAi(pLkAiVec, szDriveLetter);
	if(LkAiAddiChks(pLkAiVec, szDriveLetter))
		return ((DWORD)-1);
	//
	return 0x00;
}

DWORD NParseLkAi::InitiLkAiAddi(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter, const TCHAR *szLocation) { // 0x00:ok !0:exception
    LkAiVec::iterator iter;
    struct LockActio *lock_acti;
    TCHAR szFileName[MAX_PATH];
    //
	DRIVE_LETTE(szFileName, szDriveLetter)
	objChksAddi.Initialize(szLocation);
	//
	struct FileID tFileID = {0, 0, 0, 0, NULL};
	DWORD dwLockRetry = 0x00;
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
        lock_acti = *iter;
_LOG_DEBUG(_T("lock_acti->szFileName:%s lock_acti->dwActioType:%08X"), lock_acti->szFileName, lock_acti->dwActioType);
        _tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, lock_acti->szFileName);
		//
		if(ADDI & lock_acti->dwActioType) {
			if(LATT_DIRECTORY & lock_acti->dwActioType) {
				RiverFS::InitFolderID(&tFileID, szFileName);
				lock_acti->ulFileInde = objChksAddi.AppFolderAttri(&tFileID);
			} else {
				HANDLE hFileLocked;
				DWORD dwFileLocked = NFileUtility::LockReadFile(&hFileLocked, szFileName);
				if(FILE_UNLOCK == dwFileLocked) {
					RiverFS::InitFileID(&tFileID, szFileName);
					lock_acti->ulFileInde = objChksAddi.AppFileAttri(&tFileID);
					CloseFileID(tFileID);
					NFileUtility::UnlockFile(hFileLocked);
				} else FileLockedTip(szFileName);
			}
		}
    }
	//
	return 0x00;
}

VOID NParseLkAi::FinalLkAiAddi() {
	objChksAddi.Finalize();
}


//
/* Limits and constants. */
#define TYPE_FILE		0x01		/* Used in ls, rm, and lsFP */
#define TYPE_DIRE		0x02
#define TYPE_DOT		0x04
#define TYPE_RECY		0x08

static BOOL TraverseDirectory (HANDLE, TCHAR *, TCHAR *);
static DWORD FileType (LPWIN32_FIND_DATA);
static DWORD FileItem (HANDLE, TCHAR *, LPWIN32_FIND_DATA);
static DWORD FolderItem (HANDLE, TCHAR *, LPWIN32_FIND_DATA);

static DWORD RecursParsePath(HANDLE hReally, const TCHAR *szPath) {
    BOOL ok = TRUE;
    TCHAR szSearchName [MAX_PATH], szCurrentPath [MAX_PATH];
    TCHAR szPathString [MAX_PATH] = _T("");
    //
    if(!szPath) return ((DWORD)-1);
    if(!GetCurrentDirectory (MAX_PATH, szCurrentPath)) return ((DWORD)-1);
    //
    _stprintf_s(szSearchName, _T("%s\\"), szPath);
	_tcscpy_s(szPathString, MAX_PATH, NO_LETT(szPath));
    //
_LOG_DEBUG(_T("---- szPath:%s szSearchName:%s "), szPath, szSearchName);
    if(!SetCurrentDirectory (szSearchName)) return ((DWORD)-1);
    ok = TraverseDirectory (hReally, _T ("*"), szPathString);
    if(!SetCurrentDirectory (szCurrentPath)) return ((DWORD)-1);
    //
    return ok ? 0: -1;
}

/*	Traverse a directory - Carrying out an implementation-specific
"action" for every name encountered. The action in this version is "Delete". */
/*	PathName: Relative or absolute pathname to traverse. */
static BOOL TraverseDirectory (HANDLE hReally, TCHAR *szPathName, TCHAR *szPathString) {
    HANDLE SearchHandle;
    WIN32_FIND_DATA FindData;
    DWORD dwFileType;
    DWORD lFolderInde;

    /*	Open up the directory search handle and get the
    first file name to satisfy the pathname.
    Make two passes. The first processes the files
    and the second processes the directories. */

    SearchHandle = FindFirstFile (szPathName, &FindData);
    /* A deleted file will not be found on the second pass */
    if (INVALID_HANDLE_VALUE == SearchHandle) return TRUE;

    /* Scan the directory and its subdirectories
    for files satisfying the pattern. */
    do {
        /* For each file located, get the type. Delete files on pass 1.
        On pass 2, recursively process the subdirectory
        contents, if the recursive option is set. */

        dwFileType = FileType (&FindData); /* Delete a file. */
        if(TYPE_FILE & dwFileType) {
            FileItem (hReally, szPathString, &FindData);
        } else if (TYPE_DIRE & dwFileType) {
            lFolderInde = FolderItem (hReally, szPathString, &FindData);
        }

        /* Traverse the subdirectory on the second pass. */
        if (TYPE_DIRE & dwFileType) {
            // fprintf(stderr, "----------------%s\n", szPathString);
            SetCurrentDirectory (FindData.cFileName);
            struti::appe_path(szPathString, FindData.cFileName);
            //
            TraverseDirectory (hReally, _T ("*"), szPathString);
            //
            struti::dele_last_path(szPathString, FindData.cFileName);
            SetCurrentDirectory (_T (".."));
        }
        /* Get the next file or directory name. */
    } while (FindNextFile (SearchHandle, &FindData));
    FindClose (SearchHandle);

    return TRUE;
}

/* Return file type from the find data structure.
Types supported:
TYPE_FILE:	If this is a file
TYPE_DIRE:	If this is a directory other than . or ..
TYPE_DOT:	If this is . or .. directory */
static DWORD FileType (LPWIN32_FIND_DATA pFileData) {
    DWORD dwFileType = TYPE_FILE;
    if (FILE_ATTRIBUTE_DIRECTORY & pFileData->dwFileAttributes)
        if (!lstrcmp(pFileData->cFileName, _T (".")) || !lstrcmp(pFileData->cFileName, _T ("..")))
            dwFileType = TYPE_DOT;
        else if(!_tcsncmp(pFileData->cFileName, _T ("RECYCLER"), 0x08)) dwFileType = TYPE_RECY;
        else dwFileType = TYPE_DIRE;
    //
    return dwFileType;
}

/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
static DWORD FileItem (HANDLE hReally, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData) {
	TCHAR szFileName[MAX_PATH];
	struti::full_name(szFileName, szPathString, pFileData->cFileName);
	INodeUtili::INodeAApp(hReally, szFileName, INTYPE_FILE);
// _LOG_DEBUG( _T("---- prepa file add:%s"), szFileName);
    return 0x00;
}

/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
static DWORD FolderItem (HANDLE hReally, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData) {
	TCHAR szFilderName[MAX_PATH];
	struti::full_name(szFilderName, szPathString, pFileData->cFileName);
	INodeUtili::INodeAApp(hReally, szFilderName, INTYPE_DIRECTORY);
// _LOG_DEBUG( _T("---- prepa folder add:%s"), szFilderName);
    return 0x00;
}
