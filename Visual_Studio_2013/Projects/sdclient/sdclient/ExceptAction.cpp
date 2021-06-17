#include "StdAfx.h"

#include "ExecuteTransmitBzl.h"
#include "ExceptAction.h"

CExcepActio::CExcepActio(void):
    m_hExcepActio(INVALID_HANDLE_VALUE)
{
    MKZEO(m_szExcepActio);
}

CExcepActio::~CExcepActio(void) {
}
CExcepActio objExcepActio;

DWORD CExcepActio::Initialize(TCHAR *szLocation) {
    struti::get_name(m_szExcepActio, szLocation, EXCEPT_ACTION_DEFAULT);
// _LOG_DEBUG(_T("init except file:%s"), m_szExcepActio);
    return 0x00;
}

DWORD CExcepActio::Finalize() {
    MKZEO(m_szExcepActio);
// _LOG_DEBUG(_T("final except file:%s"), m_szExcepActio);
    return 0x00;
}

VOID CExcepActio::ActioIndeAddi(struct excent *pexent) {
    struct ExcepActio *pExcepActio;
    //
    switch(TNODE_STYPE_BYTE(pexent->control_code)) {
    case STYPE_GET:
        pExcepActio = NExcepHmap::NewActioNode(pexent->szFileName1, pexent->ulTimestamp);
        NExcepHmap::InsActioNode(&m_mExcepAddition, pExcepActio);
_LOG_DEBUG(_T("set except add:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    case STYPE_PSYNC:
        pExcepActio = NExcepHmap::NewActioNode(pexent->szFileName1, pexent->ulTimestamp);
        NExcepHmap::InsActioNode(&m_mExcepModify, pExcepActio);
_LOG_DEBUG(_T("set except mod:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    case STYPE_DELETE:
        pExcepActio = NExcepHmap::NewActioNode(pexent->szFileName1, pexent->ulTimestamp);
        NExcepHmap::InsActioNode(&m_mExcepDelete, pExcepActio);
_LOG_DEBUG(_T("set except del:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    case STYPE_COPY:
        pExcepActio = NExcepHmap::NewActioNode(pexent->szFileName2, pexent->ulTimestamp);
        NExcepHmap::InsActioNode(&m_mExcepAddition, pExcepActio);
_LOG_DEBUG(_T("set except add:%s"), pexent->szFileName2); // disable by james 20130408
        break;
    case STYPE_MOVE:
        pExcepActio = NExcepHmap::NewActioNode(pexent->szFileName1, pexent->ulTimestamp);
        NExcepHmap::InsActioNode(&m_mExcepDelete, pExcepActio);
        //
        pExcepActio = NExcepHmap::NewActioNode(pexent->szFileName2, pexent->ulTimestamp);
        NExcepHmap::InsActioNode(&m_mExcepAddition, pExcepActio);
_LOG_DEBUG(_T("set except del add:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    default:
        break;
    }
}

VOID CExcepActio::ActioIndeDele(struct excent *pexent) {
    unordered_map<wstring, struct ExcepActio *>::iterator aite;
    //
    switch(TNODE_STYPE_BYTE(pexent->control_code)) {
    case STYPE_GET:
        NExcepHmap::SetActionDelete(&m_mExcepAddition, pexent->szFileName1, pexent->ulTimestamp);
_LOG_DEBUG(_T("erase except add:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    case STYPE_DELETE:
        NExcepHmap::SetActionDelete(&m_mExcepDelete, pexent->szFileName1, pexent->ulTimestamp);
_LOG_DEBUG(_T("erase except del:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    case STYPE_PSYNC:
        NExcepHmap::SetActionDelete(&m_mExcepModify, pexent->szFileName1, pexent->ulTimestamp);
_LOG_DEBUG(_T("erase except mod:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    case STYPE_COPY:
        NExcepHmap::SetActionDelete(&m_mExcepAddition, pexent->szFileName2, pexent->ulTimestamp);
_LOG_DEBUG(_T("erase except add:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    case STYPE_MOVE:
        NExcepHmap::SetActionDelete(&m_mExcepDelete, pexent->szFileName1, pexent->ulTimestamp);
        NExcepHmap::SetActionDelete(&m_mExcepAddition, pexent->szFileName2, pexent->ulTimestamp);
_LOG_DEBUG(_T("erase except del add:%s"), pexent->szFileName1); // disable by james 20130408
        break;
    }
}

DWORD CExcepActio::LoadExcepActio() {
    m_hExcepActio = CreateFile( m_szExcepActio,
                                  GENERIC_WRITE | GENERIC_READ,
                                  NULL, /* FILE_SHARE_READ */
                                  NULL,
                                  OPEN_ALWAYS,
                                  FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_WRITE_THROUGH,
                                  NULL);
    if( INVALID_HANDLE_VALUE == m_hExcepActio ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
    DWORD dwResult = SetFilePointer(m_hExcepActio, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    //
    struct excent dexent;
    DWORD dwReadSize = 0x00;
    while(ReadFile(m_hExcepActio, &dexent, sizeof(struct excent), &dwReadSize, NULL) && 0<dwReadSize) {
        switch(EXCENT_BEGIN_BYTE(dexent.control_code)) {
        case EXCEPT_ACTION_BEGIN:
            ActioIndeAddi(&dexent);
            break;
        case EXCEPT_ACTION_END:
            ActioIndeDele(&dexent);
            break;
        }
    }
    //
_LOG_DEBUG(_T("load except ok"));
    return 0x00;
}

VOID CExcepActio::DropExcepActio() {
    NExcepHmap::DeleActioHmap(&m_mExcepDelete);
    NExcepHmap::DeleActioHmap(&m_mExcepModify);
    NExcepHmap::DeleActioHmap(&m_mExcepAddition);
    //
    if(INVALID_HANDLE_VALUE != m_hExcepActio) {
        CloseHandle( m_hExcepActio );
        m_hExcepActio = INVALID_HANDLE_VALUE;
    }
    DeleteFile(m_szExcepActio);
_LOG_DEBUG(_T("clear except ok"));
}

HANDLE CExcepActio::ExcepActioInit() {
    m_hExcepActio = CreateFile( m_szExcepActio,
                                  GENERIC_WRITE | GENERIC_READ,
                                  NULL, /* FILE_SHARE_READ */
                                  NULL,
                                  OPEN_ALWAYS,
                                  FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_WRITE_THROUGH,
                                  NULL);
    if( INVALID_HANDLE_VALUE == m_hExcepActio ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
	//
	DWORD dwResult = SetFilePointer(m_hExcepActio, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle( m_hExcepActio );
		return INVALID_HANDLE_VALUE;
	}
    //
_LOG_DEBUG(_T("except action init ok"));
    return m_hExcepActio;
}

VOID CExcepActio::FinalExcepActio() {
    if(INVALID_HANDLE_VALUE != m_hExcepActio) {
        CloseHandle( m_hExcepActio );
        m_hExcepActio = INVALID_HANDLE_VALUE;
    }
_LOG_DEBUG(_T("except action final ok"));
}

VOID CExcepActio::SetExcepActio(struct TaskNode *task_node) {
    struct excent dexent;
    //
    dexent.control_code = EXCEPT_ACTION_BEGIN | TNODE_STYPE_BYTE(task_node->control_code);
// _LOG_DEBUG(_T("--------- except set:%s control_code:%08x"), task_node->szFileName1, dexent.control_code);
    _tcscpy_s(dexent.szFileName1, MAX_PATH, task_node->szFileName1);
    _tcscpy_s(dexent.szFileName2, MAX_PATH, task_node->szFileName2);
    dexent.ulTimestamp = GetTickCount();
    //
    DWORD dwWritenSize = 0x00;
    WriteFile(m_hExcepActio, &dexent, sizeof(struct excent), &dwWritenSize, NULL);
    if(sizeof(struct excent) != dwWritenSize) {
        _LOG_WARN(_T("fatal write error: %d"), GetLastError());
    }
}

VOID CExcepActio::ExcepActioErase(struct TaskNode *task_node) {
    struct excent dexent;
    //
    dexent.control_code = EXCEPT_ACTION_END | TNODE_STYPE_BYTE(task_node->control_code);
// _LOG_DEBUG(_T("--------- except erase:%s control_code:%08x"), task_node->szFileName1, dexent.control_code);
    _tcscpy_s(dexent.szFileName1, MAX_PATH, task_node->szFileName1);
    _tcscpy_s(dexent.szFileName2, MAX_PATH, task_node->szFileName2);
    dexent.ulTimestamp = GetTickCount();
    //
    DWORD dwWritenSize = 0x00;
    WriteFile(m_hExcepActio, &dexent, sizeof(struct excent), &dwWritenSize, NULL);
    if(sizeof(struct excent) != dwWritenSize) {
        _LOG_WARN(_T("fatal write error: %d"), GetLastError());
    }
}

// 用于watcher检查是否要监视
#define IS_EXCEPTED_FOUND		0x00000001
#define IS_EXCEPTED_NFOUND		0
#define ACTION_DELAY_TIME		1024
DWORD CExcepActio::ExcepAddition(const TCHAR *szFileName, DWORD ulTimestamp) {
    unordered_map<wstring, struct ExcepActio *>::iterator aite;
    struct ExcepActio *pExcepActio = NULL;
   // 同步时不需要监视
    aite = m_mExcepAddition.find(szFileName);
    if(m_mExcepAddition.end() != aite) {
        for(pExcepActio = aite->second; pExcepActio; pExcepActio = pExcepActio->pNext) {
// disable by james 20140115
// _LOG_DEBUG(_T("--------- ulSetStamp:%d ulEndStamp:%d ulTimestamp:%d"), pExcepActio->ulSetStamp, pExcepActio->ulEndStamp, ulTimestamp);
            if((pExcepActio->ulSetStamp <= ulTimestamp) && (ulTimestamp < ACTION_DELAY_TIME + pExcepActio->ulEndStamp)) {
// disable by james 20140115
// _LOG_DEBUG(_T("--------- found except addition:%s"), szFileName);
                return IS_EXCEPTED_FOUND;
            }
        }
    }
// disable by james 20140115
// _LOG_DEBUG(_T("--------- not found except addition:%s"), szFileName);
    return IS_EXCEPTED_NFOUND;
}

DWORD CExcepActio::ExcepDelete(const TCHAR *szFileName, DWORD ulTimestamp) {
    unordered_map<wstring, struct ExcepActio *>::iterator aite;
    struct ExcepActio *pExcepActio = NULL;
    // 同步时不需要监视
    aite = m_mExcepDelete.find(szFileName);
    if(m_mExcepDelete.end() != aite) {
        for(pExcepActio = aite->second; pExcepActio; pExcepActio = pExcepActio->pNext) {
// _LOG_DEBUG(_T("--------- ulSetStamp:%d ulEndStamp:%d ulTimestamp:%d"), pExcepActio->ulSetStamp, pExcepActio->ulEndStamp, ulTimestamp);
            if((pExcepActio->ulSetStamp <= ulTimestamp) && (ulTimestamp < ACTION_DELAY_TIME + pExcepActio->ulEndStamp)) {
// _LOG_DEBUG(_T("--------- found except delete:%s"), szFileName);
                return IS_EXCEPTED_FOUND;
            }
        }
    }
// _LOG_DEBUG(_T("--------- not found except delete:%s"), szFileName);
    return IS_EXCEPTED_NFOUND;
}

DWORD CExcepActio::ExcepModify(const TCHAR *szFileName, DWORD ulTimestamp) {
    unordered_map<wstring, struct ExcepActio *>::iterator aite;
    struct ExcepActio *pExcepActio = NULL;
    // 同步时不需要监视
    aite = m_mExcepModify.find(szFileName);
    if(m_mExcepModify.end() != aite) {
        for(pExcepActio = aite->second; pExcepActio; pExcepActio = pExcepActio->pNext) {
// _LOG_DEBUG(_T("--------- ulSetStamp:%d ulEndStamp:%d ulTimestamp:%d"), pExcepActio->ulSetStamp, pExcepActio->ulEndStamp, ulTimestamp);
            if((pExcepActio->ulSetStamp <= ulTimestamp) && (ulTimestamp < ACTION_DELAY_TIME + pExcepActio->ulEndStamp)) {
// _LOG_DEBUG(_T("--------- found except modify:%s"), szFileName);
                return IS_EXCEPTED_FOUND;
            }
        }
    }
// _LOG_DEBUG(_T("--------- not found except modify:%s"), szFileName);
    return IS_EXCEPTED_NFOUND;
}

struct ExcepActio *NExcepHmap::NewActioNode(TCHAR *szFileName, DWORD ulSetStamp) {
    struct ExcepActio *acti_new;
    //
    acti_new = (struct ExcepActio *)malloc(sizeof(struct ExcepActio));
	if(!acti_new) return NULL;
    memset(acti_new, '\0', sizeof(struct ExcepActio));
    _tcscpy_s(acti_new->szFileName, MAX_PATH, szFileName);
    acti_new->ulSetStamp = ulSetStamp;
    //
    return acti_new;
}

struct ExcepActio *NExcepHmap::InsActioNode(ActioHmap *pActioHmap, ExcepActio *pExcepActio) {
    unordered_map<wstring, struct ExcepActio *>::iterator aite;
    struct ExcepActio *toke;
    //
    aite = pActioHmap->find(pExcepActio->szFileName);
    if(pActioHmap->end() != aite) {
        for(toke = aite->second; toke->pNext; toke = toke->pNext);
        toke->pNext = pExcepActio;
    } else pActioHmap->insert(ActioHmap::value_type(pExcepActio->szFileName, pExcepActio));
    //
    return pExcepActio;
}

int NExcepHmap::SetActionDelete(ActioHmap *pActioHmap, TCHAR *szFileName, DWORD ulTimestamp) {
    unordered_map<wstring, struct ExcepActio *>::iterator aite;
    struct ExcepActio *toke;
    //
    aite = pActioHmap->find(szFileName);
    if(pActioHmap->end() != aite) {
        for(toke = aite->second; toke->pNext; toke = toke->pNext);
        toke->ulEndStamp = ulTimestamp;
        return 0;
    }
    //
    return -1;
}

VOID NExcepHmap::DeleActioHmap(ActioHmap *pActioHmap) {
    unordered_map<wstring, struct ExcepActio *>::iterator aite;
    struct ExcepActio *toke, *next;
    //
    for(aite = pActioHmap->begin(); pActioHmap->end() != aite; ++aite) {
        toke = aite->second;
        while(toke) {
            next = toke->pNext;
            DeleNode(toke);
            toke = next;
        }
    }
    //
    pActioHmap->clear();
}