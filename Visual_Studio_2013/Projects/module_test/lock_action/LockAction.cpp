#include "StdAfx.h"
#include "LockAction.h"

CLockAction::CLockAction(void):
    m_hLockAction(INVALID_HANDLE_VALUE)
{
    MKZERO(m_szLockAction);
}

CLockAction::~CLockAction(void)
{
}

CLockAction objLockAction;

DWORD CLockAction::Initialize(TCHAR *szLocation)
{
    nstriutility::get_name(m_szLockAction, szLocation, LOCKED_ACTION_DEFAULT);
    return 0;
}

DWORD CLockAction::Finalize()
{
    MKZERO(m_szLockAction);
    return 0;
}

HANDLE CLockAction::LockActionInitial()
{
    m_hLockAction = CreateFile( m_szLockAction,
                                GENERIC_WRITE | GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                NULL);
    if( INVALID_HANDLE_VALUE == m_hLockAction ) {
        printf( "create file failed: %d\n", GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    DWORD dwResult = SetFilePointer(m_hLockAction, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return INVALID_HANDLE_VALUE;

    return m_hLockAction;
}

VOID CLockAction::FinalLockAction()
{
    if(INVALID_HANDLE_VALUE != m_hLockAction) {
        CloseHandle( m_hLockAction );
        m_hLockAction = INVALID_HANDLE_VALUE;
    }
    DeleteFile(m_szLockAction);
}

ULONG CLockAction::ReadNode(struct Action *pAction, ULONG lPosiInde)
{
    DWORD rlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lPosiInde * sizeof(struct Action);
    if(!ReadFile(m_hLockAction, pAction, sizeof(struct Action), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_LANDE_VAL;
    }
    //
    return lPosiInde;
}

ULONG CLockAction::WriteNode(struct Action *pAction, ULONG lPosiInde)
{
    DWORD wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lPosiInde * sizeof(struct Action);
    if(!WriteFile(m_hLockAction, pAction, sizeof(struct Action), &wlen, &OverLapped))
        return INVA_LANDE_VAL;
    //
    return lPosiInde;
}

DWORD CLockAction::ModifyNode(ULONG lPosiInde, DWORD dwNextType)
{
    struct Action tAction;
    DWORD dwExisType = INVA_ATTR;
    DWORD rlen, wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lPosiInde * sizeof(struct Action);
    if(!ReadFile(m_hLockAction, &tAction, sizeof(struct Action), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_ATTR;
    }
    //
    dwExisType = tAction.dwActionType;
    tAction.dwActionType |= dwNextType;
    //
    if(!WriteFile(m_hLockAction, &tAction, sizeof(struct Action), &wlen, &OverLapped))
        return INVA_ATTR;
    //
    return dwExisType;
}

//
void CLockAction::ValidRecycled()
{
    struct Action tAction;
    ULONG lInde;
    //
    for(lInde = 0x00; INVA_LANDE_VAL != ReadNode(&tAction, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tAction.dwActionType) && !_tcsncmp(tAction.szFileName, _T("\\RECYCLER"), 9)) {
            tAction.dwActionType |= LATT_DISCARD;
            WriteNode(&tAction, lInde);
        }
    }
}

void CLockAction::ValidAddition()
{
    struct Action tAction;
    LockActHmap mCacheHmap;
    LockActHmap::iterator iter;
    ULONG lInde;
    // 剔除重复修改
    for(lInde = 0x00; INVA_LANDE_VAL != ReadNode(&tAction, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tAction.dwActionType) && (MODIFY&tAction.dwActionType)) {
            iter = mCacheHmap.find(tAction.szFileName);
            if(iter != mCacheHmap.end()) {
                tAction.dwActionType |= LATT_DISCARD;
                WriteNode(&tAction, lInde);
            } else mCacheHmap.insert(LockActHmap::value_type(tAction.szFileName, lInde));
        }
    }
    mCacheHmap.clear();
    // 剔除增加后修改
    for(lInde = 0x00; INVA_LANDE_VAL != ReadNode(&tAction, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tAction.dwActionType)) {
            if(ADD & tAction.dwActionType) {
                mCacheHmap.insert(LockActHmap::value_type(tAction.szFileName, lInde));
            }
            if(MODIFY & tAction.dwActionType) {
                iter = mCacheHmap.find(tAction.szFileName);
                if(iter != mCacheHmap.end()) {
                    tAction.dwActionType |= LATT_DISCARD;
                    WriteNode(&tAction, lInde);
                }
            }
        }
    }
    mCacheHmap.clear();
}

//
// except modified directory
DWORD CLockAction::ValidPreproFinished() // 0:succ 0x01:err
{
    struct Action tAction;
    //
    for(ULONG lInde = 0x00; INVA_LANDE_VAL != ReadNode(&tAction, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tAction.dwActionType) && (LATT_FILE_ATT&tAction.dwActionType)) {
            if(LATT_DIRECTORY & tAction.dwActionType) {
                if((MOVE|COPY|EXIST|DEL|ADD) & tAction.dwActionType)
                    tAction.dwActionType |= LATT_FINISHED;
                else if(MODIFY & tAction.dwActionType) // except modified directory
                    tAction.dwActionType |= LATT_DISCARD;
                WriteNode(&tAction, lInde);
            }
            //
            if(LATT_FILE & tAction.dwActionType) {
                if((LATT_UNLOCK|LATT_EXLOCK) & tAction.dwActionType) {
                    tAction.dwActionType |= LATT_FINISHED;
                    WriteNode(&tAction, lInde);
                }
            }
        }
    }
    //
    return 0x00;
}
//
DWORD CLockAction::WildActionNode()
{
    struct Action tAction;
    DWORD dwWildTally = 0x00;
    //
    for(ULONG lInde = 0x00; INVA_LANDE_VAL != ReadNode(&tAction, lInde); lInde++) {
        if(!(LATT_FINISH_ATT & tAction.dwActionType)) {
            dwWildTally = 0x01;
            break;
        }
    }
    //
    return dwWildTally;
}
//
DWORD CLockAction::LoadLockAction(ActionVec *pLockAction)
{
    struct Action tAction;
    struct LockAction *lock_action;
    DWORD dwReadSize = 0;
    printf("--------- in load lock action!\n");
    if(INVALID_HANDLE_VALUE == m_hLockAction) return 0x02;
    //
    memset(&tAction, '\0', sizeof(struct Action));
    DWORD dwResult = SetFilePointer(m_hLockAction, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return -1;
    while(ReadFile(m_hLockAction, &tAction, sizeof(struct Action), &dwReadSize, NULL) && sizeof(struct Action) == dwReadSize) {
        if(LATT_FINISHED & tAction.dwActionType) {
            lock_action = NActionVec::AppendNewNode(pLockAction);
            // memcpy(action, &tAction, sizeof(struct Action));
            if(LATT_DIRECTORY & tAction.dwActionType) {
                lock_action->dwActionType = DIRECTORY | ACTION_TYPE(tAction.dwActionType);
            } else lock_action->dwActionType = ACTION_TYPE(tAction.dwActionType);
            _tcscpy_s(lock_action->szFileName, MAX_PATH, tAction.szFileName);
            lock_action->ulTimestamp = tAction.ulTimestamp;
        }
    }
    //
    dwResult = SetFilePointer(m_hLockAction, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return -1;
    //
    return 0; //succ
}
//
struct LockAction *NActionVec::AppendNewNode(ActionVec *pActionVec)
{
    struct LockAction *action_new;
    action_new = (struct LockAction *)malloc(sizeof(struct LockAction));
    memset(action_new, '\0', sizeof(struct LockAction));
    pActionVec->push_back(action_new);
    return action_new;
}

VOID NActionVec::DeleteActionVec(ActionVec *pActionVec)
{
    ActionVec::iterator iter;
    for(iter = pActionVec->begin(); iter != pActionVec->end(); ++iter)
        DeleteNode(*iter);
    pActionVec->clear();
}

