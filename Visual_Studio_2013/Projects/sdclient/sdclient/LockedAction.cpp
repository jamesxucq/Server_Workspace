#include "StdAfx.h"
#include "LockedAction.h"

CLockActio::CLockActio(void):
    m_hLockActio(INVALID_HANDLE_VALUE)
{
    MKZEO(m_szLockActio);
}

CLockActio::~CLockActio(void) {
}

CLockActio objLockActio;

DWORD CLockActio::Initialize(TCHAR *szLocation) {
    struti::get_name(m_szLockActio, szLocation, LOCKED_ACTION_DEFAULT);
    return 0x00;
}

DWORD CLockActio::Finalize() {
    MKZEO(m_szLockActio);
    return 0x00;
}

HANDLE CLockActio::LockActioIniti() {
    m_hLockActio = CreateFile( m_szLockActio,
                                GENERIC_WRITE | GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_WRITE_THROUGH,
                                NULL);
    if( INVALID_HANDLE_VALUE == m_hLockActio ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    DWORD dwResult = SetFilePointer(m_hLockActio, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return INVALID_HANDLE_VALUE;

    return m_hLockActio;
}

VOID CLockActio::FinalLockActio(BOOL bOk) {
    if(INVALID_HANDLE_VALUE != m_hLockActio) {
        CloseHandle( m_hLockActio );
        m_hLockActio = INVALID_HANDLE_VALUE;
    }
    if(bOk) DeleteFile(m_szLockActio);
}

HANDLE CLockActio::ResetLockActio() {
	if(INVALID_HANDLE_VALUE != m_hLockActio) {
		DWORD dwResult = SetFilePointer(m_hLockActio, 0, NULL, FILE_BEGIN);
		if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
			return INVALID_HANDLE_VALUE;
		}
		dwResult = SetEndOfFile(m_hLockActio);
		if(FALSE==dwResult && NO_ERROR!=GetLastError()) {	
			return INVALID_HANDLE_VALUE;
		}
	}
	//
	return m_hLockActio;
}

ULONG CLockActio::ReadNode(struct Action *pActioNode, ULONG lPosiInde) {
    DWORD slen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lPosiInde * sizeof(struct Action);
    if(!ReadFile(m_hLockActio, pActioNode, sizeof(struct Action), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_LANDE_VALU;
    }
    //
    return lPosiInde;
}

ULONG CLockActio::WriteNode(struct Action *pActioNode, ULONG lPosiInde) {
    DWORD wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lPosiInde * sizeof(struct Action);
    if(!WriteFile(m_hLockActio, pActioNode, sizeof(struct Action), &wlen, &OverLapped))
        return INVA_LANDE_VALU;
    //
    return lPosiInde;
}

DWORD CLockActio::ModifyNode(ULONG lPosiInde, DWORD dwNextType) {
    struct Action tExiActi;
    DWORD dwExisType = INVA_ATTR;
    DWORD slen, wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lPosiInde * sizeof(struct Action);
    if(!ReadFile(m_hLockActio, &tExiActi, sizeof(struct Action), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_ATTR;
    }
    //
    dwExisType = tExiActi.dwActioType;
    tExiActi.dwActioType |= dwNextType;
    //
    if(!WriteFile(m_hLockActio, &tExiActi, sizeof(struct Action), &wlen, &OverLapped))
        return INVA_ATTR;
    //
    return dwExisType;
}

//
// 剔除回收站
void CLockActio::ValidRecycled() {
    struct Action tExiActi;
    ULONG lInde;
    for(lInde = 0x00; INVA_LANDE_VALU != ReadNode(&tExiActi, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tExiActi.dwActioType) && !_tcsncmp(tExiActi.szFileName, _T("\\RECYCLER"), 9)) {
            tExiActi.dwActioType |= LATT_DISCARD;
            WriteNode(&tExiActi, lInde);
        }
// _LOG_DEBUG(_T("type:%08X name:%s"), tExiActi.dwActioType, tExiActi.szFileName);
    }
}

// 剔除重复修改 剔除增加后修改
void CLockActio::ValidAddition() {
    struct Action tExiActi;
    LkAiHmap mCacheHmap;
    LkAiHmap::iterator iter;
    ULONG lInde;
    // 剔除重复修改
    for(lInde = 0x00; INVA_LANDE_VALU != ReadNode(&tExiActi, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tExiActi.dwActioType) && (MODIFY&tExiActi.dwActioType)) {
            iter = mCacheHmap.find(tExiActi.szFileName);
            if(mCacheHmap.end() != iter) {
                tExiActi.dwActioType |= LATT_DISCARD;
                WriteNode(&tExiActi, lInde);
            } else mCacheHmap.insert(LkAiHmap::value_type(tExiActi.szFileName, lInde));
        }
    }
    mCacheHmap.clear();
    // 剔除增加后修改
    for(lInde = 0x00; INVA_LANDE_VALU != ReadNode(&tExiActi, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tExiActi.dwActioType)) {
            if(ADDI & tExiActi.dwActioType) {
                mCacheHmap.insert(LkAiHmap::value_type(tExiActi.szFileName, lInde));
            }
            if(MODIFY & tExiActi.dwActioType) {
                iter = mCacheHmap.find(tExiActi.szFileName);
                if(mCacheHmap.end() != iter) {
                    tExiActi.dwActioType |= LATT_DISCARD;
                    WriteNode(&tExiActi, lInde);
                }
            }
        }
    }
    mCacheHmap.clear();
}

//
// discard modified directory
DWORD CLockActio::ValidPreproFinished() { // 0:succ 0x01:err
    struct Action tExiActi;
    DWORD dwWildTatol = 0x00;
    //
    for(ULONG lInde = 0x00; INVA_LANDE_VALU != ReadNode(&tExiActi, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tExiActi.dwActioType) && (LATT_FILE_ATT&tExiActi.dwActioType)) {
            if(LATT_DIRECTORY & tExiActi.dwActioType) {
                if((DELE|ADDI) & tExiActi.dwActioType)
                    tExiActi.dwActioType |= LATT_FINISHED;
                else if(MODIFY & tExiActi.dwActioType) // discard modified directory
                    tExiActi.dwActioType |= LATT_DISCARD;
                WriteNode(&tExiActi, lInde);
            }else if(LATT_FILE & tExiActi.dwActioType) {
                if((LATT_UNLOCK|LATT_LOCKED) & tExiActi.dwActioType) {
                    tExiActi.dwActioType |= LATT_FINISHED;
                    WriteNode(&tExiActi, lInde);
                } else dwWildTatol++;
            }
        }
    }
    //
    return dwWildTatol;
}

//
DWORD CLockActio::LoadLockActio(LkAiVec *pLkAiVec) {
    struct Action tExiActi;
    struct LockActio *lock_acti;
    DWORD dwReadSize = 0x00;
_LOG_DEBUG(_T("--------- in load lock action!"));
    if(INVALID_HANDLE_VALUE == m_hLockActio) return 0x02;
    //
    memset(&tExiActi, '\0', sizeof(struct Action));
    DWORD dwResult = SetFilePointer(m_hLockActio, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    while(ReadFile(m_hLockActio, &tExiActi, sizeof(struct Action), &dwReadSize, NULL) && sizeof(struct Action) == dwReadSize) {
        if(LATT_FINISHED & tExiActi.dwActioType) {
            lock_acti = NLkAiVec::AppNewNode(pLkAiVec);
            // memcpy(action, &tExiActi, sizeof(struct Action));
			lock_acti->dwActioType = ACTI_ATTRIB_BYTE(tExiActi.dwActioType);
            _tcscpy_s(lock_acti->szFileName, MAX_PATH, tExiActi.szFileName);
            lock_acti->ulTimestamp = tExiActi.ulTimestamp;
        }
    }
    //
    dwResult = SetFilePointer(m_hLockActio, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    //
    return 0x00; // succ
}
//
struct LockActio *NLkAiVec::AppNewNode(LkAiVec *pLkAiVec) {
    struct LockActio *acti_new;
    acti_new = (struct LockActio *)malloc(sizeof(struct LockActio));
	if(!acti_new) return NULL;
    memset(acti_new, '\0', sizeof(struct LockActio));
	acti_new->ulFileInde = INVA_INDE_VALU;
    pLkAiVec->push_back(acti_new);
    return acti_new;
}

struct LockActio *NLkAiVec::AppNewNode(LkAiVec *pLkAiVec, TCHAR *szFileName, DWORD dwActioType, DWORD coupleID) {
    struct LockActio *acti_new;
    acti_new = (struct LockActio *)malloc(sizeof(struct LockActio));
	if(!acti_new) return NULL;
    memset(acti_new, '\0', sizeof(struct LockActio));
	//
	acti_new->dwActioType = dwActioType;
	_tcscpy_s(acti_new->szFileName, MAX_PATH, szFileName);
	acti_new->coupleID = coupleID;
	acti_new->ulFileInde = INVA_INDE_VALU;
	//
    pLkAiVec->push_back(acti_new);
    return acti_new;
}

struct LockActio *NLkAiVec::InsPrevNewNode(LkAiVec *pLkAiVec, struct LockActio *lock_acti) {
	static LkAiVec::iterator iter;
	struct LockActio *acti_new = NULL;
	//
	if(lock_acti) {
		for(iter = pLkAiVec->begin(); pLkAiVec->end() != iter; ++iter)
			if(lock_acti == (*iter)) break;
	} else {
		acti_new = (struct LockActio *)malloc(sizeof(struct LockActio));
		if(!acti_new) return NULL;
		memset(acti_new, '\0', sizeof(struct LockActio));
		acti_new->ulFileInde = INVA_INDE_VALU;
		iter = pLkAiVec->insert(iter, acti_new);
	}
	//
	return acti_new;
}

VOID NLkAiVec::DeleLkAiVec(LkAiVec *pLkAiVec) {
    LkAiVec::iterator iter;
    for(iter = pLkAiVec->begin(); pLkAiVec->end() != iter; ++iter)
        DeleNode(*iter);
    pLkAiVec->clear();
}

VOID NLkAiVec::MoveLkAiVec(LkAiVec *pToVec, LkAiVec *pExiVec, LkAiVec *pFromVec) {
	struct LockActio *lock_acti;
    LkAiVec::iterator iter;
//
	for(iter = pFromVec->begin(); pFromVec->end() != iter; ++iter) {
		lock_acti = (*iter);
		if(MOVE & lock_acti->dwActioType) pExiVec->push_back(lock_acti);
		else pToVec->push_back(lock_acti);
	}
//
    pFromVec->clear();
}

