// lock_action.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>

#include "LockAction.h"
//
namespace NExecuteUtility
{
//
static DWORD LockActionDiscard(const TCHAR *szDriveRoot); // 0:succ 0x01:err
static DWORD ValidFileLocked(const TCHAR *szDriveRoot); // 0:succ 0x01:err
static DWORD ValidFileAttrib(const TCHAR *szDriveRoot); // 0:succ 0x01:err
DWORD ActionPreprocess(const TCHAR *szDriveRoot); // 0:pass 1:err
};


#define FLOCK_RETRY_TIMES			24 // 24*12/60 4.8m
#define FILE_LOCKED_DELAY			12000 // 12s
DWORD NExecuteUtility::ValidFileLocked(const TCHAR *szDriveRoot) // 0:succ 0x01:err
{
    struct Action tAction;
    TCHAR szFileName[MAX_PATH];
    DWORD dwLockRetry, dwFileLocked;
    //
    _tcscpy_s(szFileName, MAX_PATH, szDriveRoot);
    for(ULONG lInde = 0x00; INVA_LANDE_VAL != objLockAction.ReadNode(&tAction, lInde); lInde++) {
        if((LATT_FILE&tAction.dwActionType) && !((LATT_FINISH_ATT|LATT_LOCKED_ATT)&tAction.dwActionType)) {
            if((EXIST|DEL|MODIFY) & tAction.dwActionType) {
                tAction.dwActionType |= LATT_UNLOCK;
                objLockAction.WriteNode(&tAction, lInde);
            } else if(((MOVE|COPY|ADD)&tAction.dwActionType)) {
                _tcscpy_s(NO_ROOT(szFileName), MAX_PATH-ROOT_LENGTH, tAction.szFileName);
                //
                dwLockRetry = 0x00;
                dwFileLocked = NFileUtility::IsFileLocked(szFileName);
                while((FLOCK_RETRY_TIMES > dwLockRetry++) && (FILE_LOCKED == dwFileLocked)) {
                    Sleep(FILE_LOCKED_DELAY);
                    printf("lock retry:%u file:%s\n", dwLockRetry, szFileName);
                    dwFileLocked = NFileUtility::IsFileLocked(szFileName);
                }
                if(FILE_UNLOCK == dwFileLocked) { // unlock
                    tAction.dwActionType |= LATT_UNLOCK;
                    objLockAction.WriteNode(&tAction, lInde);
                } else if(FILE_EXLOCK == dwFileLocked) { // err
                    tAction.dwActionType |= LATT_EXLOCK;
                    objLockAction.WriteNode(&tAction, lInde);
                }
            } //
        }
    }
    //
    return 0x00;
}

// 0x00:not found 0x01:file 0x02:directory
static DWORD INodeType(TCHAR *szFilePath)
{
    DWORD dwINodeType = LATT_FILE;
    DWORD dwAttrib = GetFileAttributes( szFilePath );
    if(INVALID_FILE_ATTRIBUTES == dwAttrib) dwINodeType = 0x00;
    else if(FILE_ATTRIBUTE_DIRECTORY&dwAttrib) dwINodeType = LATT_DIRECTORY;
    return dwINodeType;
}

// 0:file 0x01:dires
static ULONG FindAttrib(struct Action *pExiAction, struct Action *pNewAction, DWORD *dwFindType, TCHAR *szFileName, ULONG lRavInde)
{
    ULONG lInde;
    DWORD dwFoundAdd = 0x00;
    int rav_len, add_len = _tcslen(szFileName);
    for(lInde = lRavInde; INVA_LANDE_VAL != objLockAction.ReadNode(pExiAction, lInde); lInde++) {
        if(EXIST & pExiAction->dwActionType) {
            rav_len = _tcslen(pExiAction->szFileName);
            if(!_tcsncmp(szFileName, pExiAction->szFileName, rav_len)) {
                if(INVA_LANDE_VAL != objLockAction.ReadNode(pNewAction, lInde + 1)) {
                    if(MOVE & pNewAction->dwActionType) {
                        dwFoundAdd = 0x01;
                        *dwFindType = (add_len == rav_len)? 0x00: 0x01;
                        break;
                    }
                    lInde++;
                }
            }
        }
    }
    if(!dwFoundAdd) lInde = INVA_LANDE_VAL;
    return lInde;
}

static DWORD TraverseAttribute(TCHAR *szFileName, ULONG lAddInde, const TCHAR *szDriveRoot) // 0:err 0x01:succ file 0x02:dire
{
    struct Action tExiAction, tNewAction;
    TCHAR szFilePath[MAX_PATH];
    DWORD dwFindType = 0x00; // 0:file 0x01:dires
    DWORD dwTraverValue = 0x00;
    //
    ULONG lFindInde = FindAttrib(&tExiAction, &tNewAction, &dwFindType, szFileName, lAddInde + 1);
    if(INVA_LANDE_VAL != lFindInde) {
        if(dwFindType) { // dire
            _tcscpy_s(szFilePath, MAX_PATH, tNewAction.szFileName);
            _tcscat_s(szFilePath, MAX_PATH, szFileName + _tcslen(tExiAction.szFileName));
        } else _tcscpy_s(szFilePath, MAX_PATH, tNewAction.szFileName); // file
        dwTraverValue = TraverseAttribute(szFilePath, lFindInde + 1, szDriveRoot);
        if(dwTraverValue) {
            if(dwFindType) {
                tExiAction.dwActionType |= LATT_DIRECTORY;
                objLockAction.WriteNode(&tExiAction, lFindInde); //
                tNewAction.dwActionType |= LATT_DIRECTORY;
                objLockAction.WriteNode(&tNewAction, lFindInde + 1);
            } else {
                tExiAction.dwActionType |= dwTraverValue;
                objLockAction.WriteNode(&tExiAction, lFindInde); //
                tNewAction.dwActionType |= dwTraverValue;
                objLockAction.WriteNode(&tNewAction, lFindInde + 1);
            }
        }
    } else { //
        _tcscpy_s(szFilePath, MAX_PATH, szDriveRoot);
        _tcscpy_s(NO_ROOT(szFilePath), MAX_PATH-ROOT_LENGTH, szFileName);
        dwTraverValue = INodeType(szFilePath);
    }
    //
    return dwTraverValue;
}

DWORD NExecuteUtility::ValidFileAttrib(const TCHAR *szDriveRoot) // 0:err 0x01:succ
{
    struct Action tExiAction, tNewAction;
    ULONG lExiInde= INVA_LANDE_VAL;
    DWORD dwAttriAdd = 0x00;
    //
    for(ULONG lInde = 0x00; INVA_LANDE_VAL != objLockAction.ReadNode(&tExiAction, lInde); lInde++) {
        if(!((LATT_FINISH_ATT|LATT_FILE_ATT) & tExiAction.dwActionType)) {
            if(EXIST & tExiAction.dwActionType) {
                if(INVA_LANDE_VAL == objLockAction.ReadNode(&tNewAction, ++lInde)) {
                    dwAttriAdd = 0x00;
                    break;
                }
                dwAttriAdd = TraverseAttribute(tNewAction.szFileName, lInde, szDriveRoot);
                if(!dwAttriAdd) break;
                //
                tNewAction.dwActionType |= dwAttriAdd;
                objLockAction.WriteNode(&tNewAction, lInde); //
                tExiAction.dwActionType |= dwAttriAdd;
                objLockAction.WriteNode(&tExiAction, lInde - 1);
            } else {
                dwAttriAdd = TraverseAttribute(tExiAction.szFileName, lInde, szDriveRoot);
                if(!dwAttriAdd) break;
                tExiAction.dwActionType |= dwAttriAdd;
                objLockAction.WriteNode(&tExiAction, lInde);
            }
        }
    }
    //
    return dwAttriAdd;
}

//
// 0:file 0x01:dires
static ULONG FindAddition(struct Action *pExiAction, struct Action *pNewAction, DWORD *dwFindType, TCHAR *szFileName, ULONG lRavInde)
{
    ULONG lInde;
    DWORD dwFoundAdd = 0x00;
    int rav_len, add_len = _tcslen(pExiAction->szFileName);
    for(lInde = lRavInde; INVA_LANDE_VAL != objLockAction.ReadNode(pExiAction, lInde); lInde++) {
        if((EXIST|DEL) & pExiAction->dwActionType) {
            rav_len = _tcslen(pExiAction->szFileName);
            if(!_tcsncmp(szFileName, pExiAction->szFileName, rav_len)) {
                if((EXIST&pExiAction->dwActionType) && (INVA_LANDE_VAL!=objLockAction.ReadNode(pNewAction, lInde + 1))) {
                    if(MOVE & pNewAction->dwActionType) {
                        dwFoundAdd = 0x01;
                        *dwFindType = (add_len == rav_len)? 0x00: 0x01;
                        break;
                    }
                    lInde++;
                } else if(DEL & pExiAction->dwActionType) {
                    dwFoundAdd = 0x01;
                    *dwFindType = (add_len == rav_len)? 0x00: 0x01;
                    break;
                }
            }
        }
    }
    if(!dwFoundAdd) lInde = INVA_LANDE_VAL;
    return lInde;
}

static DWORD TraverseAddition(TCHAR *szFileName, ULONG lAddInde, const TCHAR *szDriveRoot) // 0:err 0x01:succ 0x02:del
{
    struct Action tExiAction, tNewAction;
    TCHAR szFilePath[MAX_PATH];
    DWORD dwFindType = 0x00; // 0:file 0x01:dires
    DWORD dwTraverValue = 0x00;
    //
    ULONG lFindInde = FindAddition(&tExiAction, &tNewAction, &dwFindType, szFileName, lAddInde + 1);
    if(INVA_LANDE_VAL != lFindInde) {
        if(EXIST & tExiAction.dwActionType) {
            if(dwFindType) { // dire
                _tcscpy_s(szFilePath, MAX_PATH, tNewAction.szFileName);
                _tcscat_s(szFilePath, MAX_PATH, szFileName + _tcslen(tExiAction.szFileName));
            } else _tcscpy_s(szFilePath, MAX_PATH, tNewAction.szFileName); // file
            dwTraverValue = TraverseAddition(szFilePath, lFindInde + 1, szDriveRoot);
            if(0x02 == dwTraverValue) {
                tExiAction.dwActionType |= LATT_DISCARD;
                objLockAction.WriteNode(&tExiAction, lFindInde); //
                tNewAction.dwActionType |= LATT_DISCARD;
                objLockAction.WriteNode(&tNewAction, lFindInde + 1);
            } //
        } else if(DEL & tExiAction.dwActionType) {
            tExiAction.dwActionType |= LATT_DISCARD;
            objLockAction.WriteNode(&tExiAction, lFindInde);
            dwTraverValue = 0x02;
        }
    } else { //
        _tcscpy_s(szFilePath, MAX_PATH, szDriveRoot);
        _tcscpy_s(NO_ROOT(szFilePath), MAX_PATH-ROOT_LENGTH, szFileName);
        if(NFileUtility::FileExists(szFilePath)) dwTraverValue = 0x01;
    }
    //
    return dwTraverValue;
}

static DWORD ValidInvaAdd(const TCHAR *szDriveRoot) // 0:err 0x01:succ 0x02:delete
{
    struct Action tAction;
    DWORD dwInvaAdd = 0x00;
    // 剔除增加后删除
    for(ULONG lInde = 0x00; INVA_LANDE_VAL != objLockAction.ReadNode(&tAction, lInde); lInde++) {
        if(!(LATT_FINISH_ATT&tAction.dwActionType) && (ADD&tAction.dwActionType)) {
            dwInvaAdd = TraverseAddition(tAction.szFileName, lInde, szDriveRoot);
            if(0x02 == dwInvaAdd) {
                tAction.dwActionType |= LATT_DISCARD;
                objLockAction.WriteNode(&tAction, lInde);
            }
            if(!dwInvaAdd) break;
        }
    }
    //
    return dwInvaAdd;
}

//
DWORD NExecuteUtility::LockActionDiscard(const TCHAR *szDriveRoot)   // 0:err 0x01:succ
{
    objLockAction.ValidRecycled();
    objLockAction.ValidAddition();
    return ValidInvaAdd(szDriveRoot);
}

DWORD NExecuteUtility::ActionPreprocess(const TCHAR *szDriveRoot)   // 0:pass 1:undone 2:discard 3:attribute
{
    // 检查文件是否被丢弃或者已经完成
    printf("--------- valid file discard ---------\n");
    if(!LockActionDiscard(szDriveRoot)) return 0x02;
    // 检查是否是文件
    printf("--------- valid file attrib ---------\n");
    if(!ValidFileAttrib(szDriveRoot)) return 0x03;
    // 检查文件是否被锁定
    printf("--------- valid file locked ---------\n");
    ValidFileLocked(szDriveRoot);
    printf("--------- valid file finished ---------\n");
    objLockAction.ValidPreproFinished();
    //
    printf("--------- valid file wild ---------\n");
    return objLockAction.WildActionNode(); // 1:undone
}

//
void print_data()
{
    struct Action tAction;
    for(ULONG lInde = 0x00; INVA_LANDE_VAL != objLockAction.ReadNode(&tAction, lInde); lInde++) {
        wprintf(L"lInde:%d dwActionType:%04X szFileName:%s\n", lInde, tAction.dwActionType>>16, tAction.szFileName);
    }
}

void ready_file_data(HANDLE hReady) // file
{
    struct Action tAction;
    DWORD dwWritenSize;
    //
    tAction.dwActionType = ADD;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\RECYCLER\\recursive.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MODIFY;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\system.c"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MODIFY;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\system.c"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = ADD;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\recursive.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MODIFY;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\recursive.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = ADD;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\system.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = DEL;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\system.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    //
    tAction.dwActionType = MODIFY;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\sys.c"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = EXIST;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\sys.c"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MOVE;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\system.c"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MODIFY;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\system.c"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    //
    tAction.dwActionType = ADD;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\web\\system.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = EXIST;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\web"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MOVE;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\Video"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MODIFY;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\Video\\system.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);

//
}
//
void ready_dire_data(HANDLE hReady) // file
{
    struct Action tAction;
    DWORD dwWritenSize;
    //
    tAction.dwActionType = ADD;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\web\\system.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = EXIST;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\web"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MOVE;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\x"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = EXIST;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\x\\system.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MOVE;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\Video\\system.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
    //
    tAction.dwActionType = MODIFY;
    _tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\Video\\system.iso"));
    WriteFile(hReady, &tAction, sizeof(struct Action), &dwWritenSize, NULL);
//
}
//
int _tmain(int argc, _TCHAR* argv[])
{
    //
    objLockAction.Initialize(_T("E:\\live"));
    //
    HANDLE hLockAction = objLockAction.LockActionInitial();
    // ready_file_data(hLockAction);
    ready_dire_data(hLockAction);
    //
    DWORD x = NExecuteUtility::ActionPreprocess(_T("E:"));
    print_data();
    printf("--------------------\n");
    printf("action preprocess:%d\n", x);
    //
    objLockAction.FinalLockAction();
    objLockAction.Finalize();
    //
    return 0;
}

