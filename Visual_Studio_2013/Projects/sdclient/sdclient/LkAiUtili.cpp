#include "StdAfx.h"

// #include "Logger.h"
#include "clientcom/clientcom.h"
#include "./RiverFS/RiverFS.h"
#include "LockedAction.h"
#include "DebugPrinter.h"
#include "AnchorBzl.h"
#include "ShellLinkBzl.h"
#include "FastUtility.h"
#include "ExceptAction.h"
#include "ExecuteTransmitBzl.h"
#include "LocalBzl.h"
#include "ParseLkAi.h"

#include "LkAiUtili.h"


// ����Ƿ���Ҫ����
#define EXCEPTED_SYSTEM_FOUND		0x00000001
#define IS_EXCEPTED_NFOUND			0x00
static DWORD ExcepSystem(const TCHAR *szFileName, FileFobdVec *pFobdVec) {
    // �û���ϵͳ���ò���Ҫ����
    FileFobdVec::iterator iter;
    for(iter=pFobdVec->begin(); pFobdVec->end()!=iter; ++iter) {
        if(!_tcsncmp(szFileName, (*iter)->szFilePath, _tcslen((*iter)->szFilePath))) {
            return EXCEPTED_SYSTEM_FOUND;
        }
    }
    return IS_EXCEPTED_NFOUND;
}

// ���ͬ�������Ƿ�Ҫ����
static DWORD ExcepTransmit(struct LockActio *lock_acti) {
	TCHAR szFileName[MAX_PATH];
	DWORD dwExcepFound = 0x00;
	// add by james 20151009
	if(LATT_FILE & lock_acti->dwActioType) {
		switch(FILE_ACTION_BYTE(lock_acti->dwActioType)) {
		case ADDI:
			dwExcepFound = objExcepActio.ExcepAddition(lock_acti->szFileName, lock_acti->ulTimestamp);
			break;
		case MODIFY:
			dwExcepFound = objExcepActio.ExcepModify(lock_acti->szFileName, lock_acti->ulTimestamp);
			break;
		case DELE:
			dwExcepFound = objExcepActio.ExcepDelete(lock_acti->szFileName, lock_acti->ulTimestamp);
			break;
		}
	} else if(LATT_DIRECTORY & lock_acti->dwActioType) {
		_tcscpy_s(szFileName, lock_acti->szFileName);
		_tcscat_s(szFileName, _T("\\"));
		//
		switch(FILE_ACTION_BYTE(lock_acti->dwActioType)) {
		case ADDI:
			dwExcepFound = objExcepActio.ExcepAddition(szFileName, lock_acti->ulTimestamp);
			break;
		case MODIFY:
			dwExcepFound = objExcepActio.ExcepModify(szFileName, lock_acti->ulTimestamp);
			break;
		case DELE:
			dwExcepFound = objExcepActio.ExcepDelete(szFileName, lock_acti->ulTimestamp);
			break;
		}
	}
	//
	return dwExcepFound;
}

// ���ϵͳ�Ƿ�Ҫ����
static VOID ValidExcepAction(LkAiVec *pLkAiVec) {  // 0:succ
    LkAiVec::iterator iter;
    struct LockActio *lock_acti;
    //
    objExcepActio.LoadExcepActio(); // prepa except inde
	FileFobdVec *pFobdVec = NFileFobd::GetFileFobdPoint();
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ) {
        lock_acti = *iter;
		// ���ͬ�������Ƿ�Ҫ����,��������޳�
		if(ExcepTransmit(lock_acti)) {
			NLkAiVec::DeleNode(lock_acti);
			iter = pLkAiVec->erase(iter);
		} else {
			// �û���ϵͳ���ò���Ҫ����
			lock_acti->dwExcepFound |= ExcepSystem(lock_acti->szFileName, pFobdVec);
			++iter;
// _LOG_DEBUG(_T("szFileName:%s dwActioType:%08x dwExcepFound:%08x"), lock_acti->szFileName, lock_acti->dwActioType, lock_acti->dwExcepFound);
		}
    }
    objExcepActio.DropExcepActio(); // delete except inde
}

// ���ϵͳ�Ƿ�Ҫת��
DWORD NLkAiUtili::InitActionConve(const TCHAR *szDriveLetter, const TCHAR *szLocation) { // >0:finish 0:nothing
    LkAiVec vLkAiVec;
    DWORD dwLockedValue;
_LOG_DEBUG(_T("--------- in lock action conv ---------"));
    objLockActio.LoadLockActio(&vLkAiVec); // �Ӵ��̶����ڴ�
_LOG_DEBUG(_T("--------- load lock action ---------"));
    // ndp::ActionPrinter(&vLkAiVec);// disable by james 20140115
    //
    ValidExcepAction(&vLkAiVec); // ���ͬ������/ϵͳ�Ƿ�Ҫ����
_LOG_DEBUG(_T("--------- valid except action ---------"));
    // ndp::ActionPrinter(&vLkAiVec);// disable by james 20140115
	NParseLkAi::InitiLkAiAddi(&vLkAiVec, szDriveLetter, szLocation);
	NParseLkAi::ParseAnchoLkAi(&vLkAiVec, szDriveLetter); // ����LkAiVec����copy move
    dwLockedValue = NAnchorBzl::InsAnchorAction(&vLkAiVec, szDriveLetter); // ���뵽anchor cache
    //
	NParseLkAi::FinalLkAiAddi();
    NLkAiVec::DeleLkAiVec(&vLkAiVec); // ��������ڴ�
    //
    return dwLockedValue;
}

DWORD NLkAiUtili::ValidateProcessFinished() {
    DWORD dwFinished = 0x00;
//
    DWORD dwProcessStatus = NExecTransBzl::GetTransmitNextStatus();
    if(PROCESS_STATUS_WAITING != dwProcessStatus) {
        NExecTransBzl::SetTransmitProcessStatus(dwProcessStatus);
        dwFinished = 0x01;
_LOG_DEBUG(_T("process_status:%08X"), dwProcessStatus); // disable by james 20140410
    }
//
    return dwFinished;
}

VOID NLkAiUtili::ClearActioDisplay(const TCHAR *szDriveLetter) {
    struct Action tExiActi;
    TCHAR szFileName[MAX_PATH];
    //
	DRIVE_LETTE(szFileName, szDriveLetter)
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != objLockActio.ReadNode(&tExiActi, lInde); lInde++) {
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, tExiActi.szFileName);
        BATCH_ICON_DISPLAY(szFileName, CONV_ACTION_ERAS(tExiActi.dwActioType));
    }
}

DWORD NLkAiUtili::LockActionConve(const TCHAR *szDriveLetter, const TCHAR *szLocation) { // >0:finish 0:exception
    LkAiVec vLkAiVec;
    DWORD dwLockedValue = 0x00;
    //
_LOG_DEBUG(_T("--------- in lock action conv ---------"));
    objLockActio.LoadLockActio(&vLkAiVec); // �Ӵ��̶����ڴ�
_LOG_DEBUG(_T("--------- load lock action ---------"));
// ndp::ActionPrinter(&vLkAiVec);// disable by james 20140115
    //
    ValidExcepAction(&vLkAiVec); // ���ͬ������ /ϵͳ�Ƿ�Ҫ����
_LOG_DEBUG(_T("--------- valid except action ---------"));
	if(!NParseLkAi::AnchoLkAiAddi(&vLkAiVec, szDriveLetter, szLocation)) {
		NParseLkAi::ParseAnchoLkAi(&vLkAiVec, szDriveLetter); // ����LkAiVec����copy move
_LOG_DEBUG(_T("--------- locked action parser ---------"));
// ndp::ActionPrinter(&vLkAiVec);// disable by james 20140115
		NAnchorBzl::EraseAnchorIcon(szDriveLetter); // ����last action cache
		dwLockedValue = NAnchorBzl::InsAnchorAction(&vLkAiVec, szDriveLetter); // ���뵽anchor cache
_LOG_DEBUG(_T("--------- insert anchor action ---------"));
	}
	NParseLkAi::FinalLkAiAddi();
    NLkAiVec::DeleLkAiVec(&vLkAiVec); // ��������ڴ�
    //
    return dwLockedValue;
}

//
static DWORD ValidActionLocked(const TCHAR *szDriveLetter) { // 0:succ -1:excep
    struct Action tExiActi;
    TCHAR szFileName[MAX_PATH];
	DWORD dwLockValue = 0x00;
    //
	DRIVE_LETTE(szFileName, szDriveLetter)
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != objLockActio.ReadNode(&tExiActi, lInde); lInde++) {
        if((LATT_FILE&tExiActi.dwActioType) && !((LATT_FINISH_ATT|LATT_LOCKED_ATT)&tExiActi.dwActioType)) {
            if((DELE|MODIFY) & tExiActi.dwActioType) {
                tExiActi.dwActioType |= LATT_UNLOCK;
                objLockActio.WriteNode(&tExiActi, lInde);
            } else if(ADDI & tExiActi.dwActioType) {
                _tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, tExiActi.szFileName);
                DWORD dwFileLocked = NFileUtility::IsFileLocked(szFileName);
				if(FILE_LOCKED == dwFileLocked) {
					dwLockValue = ((DWORD)-1);
					break;
				} else if(FILE_UNLOCK == dwFileLocked) { // unlock
                    tExiActi.dwActioType |= LATT_UNLOCK;
                    objLockActio.WriteNode(&tExiActi, lInde);
                } else if(FILE_LOCKEO == dwFileLocked) { // err
                    tExiActi.dwActioType |= LATT_DISCARD;
                    objLockActio.WriteNode(&tExiActi, lInde);
                }
            } //
        }
    }
    //
    return dwLockValue;
}

//
// 0x00:not found 0x01:file 0x02:directory
static DWORD INodeTypeSY(TCHAR *szFilePath) {
    DWORD dwINodeType = LATT_FILE;
    DWORD dwAttrib = GetFileAttributes( szFilePath );
    if(INVALID_FILE_ATTRIBUTES == dwAttrib) dwINodeType = INVA_ATTR;
    else if(FILE_ATTRIBUTE_DIRECTORY&dwAttrib) dwINodeType = LATT_DIRECTORY;
    return dwINodeType;
}

// synthesis
static VOID GetSynthesisAttrib(const TCHAR *szDriveLetter) {
	struct Action tExiActi;
	TCHAR szFilePath[MAX_PATH];
	DWORD dwFileAttrib = 0x00;
	//
	DRIVE_LETTE(szFilePath, szDriveLetter)
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != objLockActio.ReadNode(&tExiActi, lInde); lInde++) {
        if(!((LATT_FINISH_ATT|LATT_FILE_ATT) & tExiActi.dwActioType)) {
            if((MODIFY|ADDI) & tExiActi.dwActioType) {
				_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, tExiActi.szFileName);
				dwFileAttrib = INodeTypeSY(szFilePath);
                if(!dwFileAttrib) continue;
				//
                tExiActi.dwActioType |= dwFileAttrib;
                objLockActio.WriteNode(&tExiActi, lInde);
            }
        }
    }
	//
}

// 0x00:not found 0x01:file 0x02:directory
static DWORD INodeTypeIN(TCHAR *szFilePath) {
    DWORD dwINodeType = INVA_ATTR;
    if(RiverFS::FileExist(szFilePath)) dwINodeType = LATT_FILE;
    else if(IS_DIRECTORY(szFilePath)) dwINodeType = LATT_DIRECTORY;
// _LOG_DEBUG(_T("szFilePath:%s, dwINodeType:%X"), szFilePath, dwINodeType);
    return dwINodeType;
}

// inherited
static VOID GetInheritedAttrib() {
	struct Action tExiActi;
	DWORD dwFileAttrib = 0x00;
	//
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != objLockActio.ReadNode(&tExiActi, lInde); lInde++) {
        if(!((LATT_FINISH_ATT|LATT_FILE_ATT) & tExiActi.dwActioType)) {
            if((MODIFY|DELE) & tExiActi.dwActioType) {
				dwFileAttrib = INodeTypeIN(tExiActi.szFileName);
                if(!dwFileAttrib) continue;
				//
                tExiActi.dwActioType |= dwFileAttrib;
                objLockActio.WriteNode(&tExiActi, lInde);
            }
        }
    }
	//
}

//
static VOID DiscardUnknownAttrib() {
    struct Action tExiActi;
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != objLockActio.ReadNode(&tExiActi, lInde); lInde++) {
        if(!((LATT_FINISH_ATT|LATT_FILE_ATT) & tExiActi.dwActioType)) {
            tExiActi.dwActioType |= LATT_DISCARD;
            objLockActio.WriteNode(&tExiActi, lInde);
_LOG_DEBUG(_T("----- type:%08X name:%s"), tExiActi.dwActioType , tExiActi.szFileName);
        }
    }
}

// for debut
void print_all_action() {
    struct Action tExiActi;
_LOG_DEBUG(_T("-------------------------"));
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != objLockActio.ReadNode(&tExiActi, lInde); lInde++) {
_LOG_DEBUG(_T("lInde:%d type:%08X name:%s"), lInde, tExiActi.dwActioType , tExiActi.szFileName);
    }
}

// for debug
void print_ok_action() {
    struct Action tExiActi;
_LOG_DEBUG(_T("-------------------------"));
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != objLockActio.ReadNode(&tExiActi, lInde); lInde++) {
        if(!(LATT_DISCARD & tExiActi.dwActioType)) {
_LOG_DEBUG(_T("lInde:%d type:%08X name:%s"), lInde, tExiActi.dwActioType , tExiActi.szFileName);
        }
    }
}

//
static VOID FillFileAttrib(const TCHAR *szDriveLetter) {
// _LOG_DEBUG(_T("init print action"));
// print_all_action();
_LOG_DEBUG(_T("inherited"));
	GetInheritedAttrib();
// print_all_action();
_LOG_DEBUG(_T("synthesis"));
    GetSynthesisAttrib(szDriveLetter);
// print_all_action();
_LOG_DEBUG(_T("discard"));
    DiscardUnknownAttrib();
// print_all_action();
}

//
// 0:file 0x01:dire
static ULONG FindAddition(FastAHmap *pFastAHmap, TCHAR *szFileName, ULONG lRavInde) {
	unordered_map <wstring, struct FastANode*>::iterator aite;
	struct FastANode *pANode;
	ULONG lActioInde = INVA_LANDE_VALU;
	//
	aite = pFastAHmap->find(szFileName);
	if(pFastAHmap->end() != aite) {
		pANode = (aite->second);
		while(pANode && (lRavInde>pANode->lActioInde))
			pANode = pANode->pNext;
		for(; pANode && !(DELE&pANode->dwActioType); pANode = pANode->pNext);
		if(pANode) lActioInde = pANode->lActioInde;
	}
	//
	return lActioInde;
}

static DWORD TraverseAddition(FastAHmap *pFastAHmap, TCHAR *szFileName, ULONG lAddInde) { // 0:err 0x01:succ 0x02:del
    DWORD dwTraveValue = 0x00;
	//
    ULONG lFindInde = FindAddition(pFastAHmap, szFileName, lAddInde + 1);
    if(INVA_LANDE_VALU != lFindInde) {
        objLockActio.ModifyNode(lFindInde, LATT_DISCARD);
        dwTraveValue = 0x01;
    }
    //
// _LOG_DEBUG(_T("dwTraveValue:%u"), dwTraveValue);
    return dwTraveValue;
}

// �޳����Ӻ�ɾ��
static VOID ValidInvaAdded() {
	FastAHmap mFastHmap;
	NFastUtili::BuildFastAHmap(&mFastHmap, &objLockActio);
	//
    struct Action tExiActi;
_LOG_DEBUG(_T("--- need simple inva add"));
    // �޳����Ӻ�ɾ��
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != objLockActio.ReadNode(&tExiActi, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tExiActi.dwActioType) && (ADDI&tExiActi.dwActioType)) {
            if(TraverseAddition(&mFastHmap, tExiActi.szFileName, lInde)) {
                tExiActi.dwActioType |= LATT_DISCARD;
                objLockActio.WriteNode(&tExiActi, lInde);
            }
        }
    }
    // ���ټ���hash map
	NFastUtili::DeleFastAHmap(&mFastHmap);
}

//
static VOID LockActioDiscard(const TCHAR *szDriveLetter) {  // 0:err 0x01:succ
 // �޳�����վ
_LOG_DEBUG(_T("--------- valid recycled"));
    objLockActio.ValidRecycled();
 // �޳��ظ��޸� �޳����Ӻ��޸�
_LOG_DEBUG(_T("--------- valid addition"));
    objLockActio.ValidAddition();
// print_all_action();
// �޳����Ӻ�ɾ��
	ValidInvaAdded();
}

DWORD NLkAiUtili::ActioPrepaProcess(const TCHAR *szDriveLetter) {  // 0:pass >0:undone
    // ����ļ��Ƿ񱻶��������Ѿ����
_LOG_DEBUG(_T("--------- valid file discard ---------"));
	LockActioDiscard(szDriveLetter);
// print_all_action();
    // ����Ƿ����ļ�
_LOG_DEBUG(_T("--------- valid file attrib ---------"));
	FillFileAttrib(szDriveLetter);
    // ����ļ��Ƿ�����
_LOG_DEBUG(_T("--------- valid file locked ---------"));
    if(ValidActionLocked(szDriveLetter)) return ((DWORD)-1);
_LOG_DEBUG(_T("--------- valid file finished ---------"));
    DWORD dwWildTatol = objLockActio.ValidPreproFinished();
// print_all_action();
// print_ok_action();
    return dwWildTatol; // >0:undone
}
