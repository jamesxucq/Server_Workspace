#include "StdAfx.h"
#include <afxmt.h>

#include "third_party.h"
#include "./RiverFS/RiverFS.h"
#include "iattb_type.h"
#include "ShellUtility.h"
#include "ShellInterface.h"
#include "Anchor.h"

#include "AnchorCache.h"

#pragma comment(lib, "..\\third party\\dbz\\dbz.lib")

//
CAnchorCache::CAnchorCache(void):
    m_dwLastAnchor((DWORD)-1)
{
}

CAnchorCache::~CAnchorCache(void) {
}

CAnchorCache objAnchorCache;

DWORD CAnchorCache::Initialize(DWORD last_anchor) {
    SetLastAnchor(last_anchor);
    return 0x00;
}

DWORD CAnchorCache::Finalize() {
    NFilesVec::DeleFilesVec(&m_vActioCache);
    return 0x00;
}

VOID CAnchorCache::FileAddition(const TCHAR *szFileName) {  //文件 add 的插入
// _LOG_DEBUG(_T("install new node file name:%s action_type:%08x"), szFileName, ADDI); // disable by james 20130408
    struct attent *atte = NFilesVec::AppNewNode(&m_vActioCache);
    _tcscpy_s(atte->file_name, MAX_PATH, szFileName);
    atte->action_type = ADDI;
}

VOID CAnchorCache::FileModify(const TCHAR *szFileName) {  //文件 modify 的插入
// _LOG_DEBUG(_T("install new node file name:%s action_type:%08x"), szFileName, MODIFY); // disable by james 20130408
    struct attent *atte = NFilesVec::AppNewNode(&m_vActioCache);
    _tcscpy_s(atte->file_name, MAX_PATH, szFileName);
    atte->action_type = MODIFY;
}

VOID CAnchorCache::FileDelete(const TCHAR *szFileName) {  //文件 delete 的插入
// _LOG_DEBUG(_T("install new node file name:%s action_type:%08x"), szFileName, DELE); // disable by james 20130408
    struct attent *atte = NFilesVec::AppNewNode(&m_vActioCache);
    _tcscpy_s(atte->file_name, MAX_PATH, szFileName);
    atte->action_type = DELE;
}

VOID CAnchorCache::FileCopy(const TCHAR *szExistName, const TCHAR *szNewName) { // 文件改名的插入
    DWORD couple_id = GetTickCount();
// _LOG_DEBUG(_T("install new node exist:%s new:%08x"), szExistName, szNewName); // disable by james 20130408
    // // install new node
    struct attent *atte = NFilesVec::AppNewNode(&m_vActioCache);
    atte->reserved = couple_id;
    _tcscpy_s(atte->file_name, MAX_PATH, szExistName);
    atte->action_type = EXIST;
    //
    atte = NFilesVec::AppNewNode(&m_vActioCache);
    atte->reserved = couple_id;
    _tcscpy_s(atte->file_name, MAX_PATH, szNewName);
    atte->action_type = COPY;
}

VOID CAnchorCache::FileMoved(const TCHAR *szExistName, const TCHAR *szNewName) {  // 文件改名的插入
    DWORD couple_id = GetTickCount();
// _LOG_DEBUG(_T("install new node exist:%s new:%08x"), szExistName, szNewName); // disable by james 20130408
    // add by james 20121028 for fix windows bug
    // // install new node
    struct attent *atte = NFilesVec::AppNewNode(&m_vActioCache);
    atte->reserved = couple_id;
    _tcscpy_s(atte->file_name, MAX_PATH, szExistName);
    atte->action_type = EXIST;
    //
    atte = NFilesVec::AppNewNode(&m_vActioCache);
    atte->reserved = couple_id;
    _tcscpy_s(atte->file_name, MAX_PATH, szNewName);
    atte->action_type = MOVE;
}

//
VOID CAnchorCache::FolderDelete(const TCHAR *szFileName) {
    struct attent *atte = NFilesVec::AppNewNode(&m_vActioCache); 
    _tcscpy_s(atte->file_name, MAX_PATH, szFileName);
    _tcscat_s(atte->file_name, MAX_PATH, _T("\\"));
    atte->action_type = DELE|DIRECTORY;
}

VOID CAnchorCache::FolderAddition(const TCHAR *szFileName) {
    struct attent *atte = NFilesVec::AppNewNode(&m_vActioCache);
    _tcscpy_s(atte->file_name, MAX_PATH, szFileName);
    _tcscat_s(atte->file_name, MAX_PATH, _T("\\"));
    atte->action_type = ADDI|DIRECTORY;
}

static VOID MoveFileApp(FilesVec *pActioCache, const TCHAR *szExistName, const TCHAR *szNewName) {
    DWORD couple_id = GetTickCount();
// _LOG_DEBUG(_T("install new node exist:%s new:%s"), szExistName, szNewName);
    struct attent *atte = NFilesVec::AppNewNode(pActioCache);
    atte->reserved = couple_id;
    _tcscpy_s(atte->file_name, MAX_PATH, szExistName);
    atte->action_type = EXIST;
    //
    atte = NFilesVec::AppNewNode(pActioCache);
    atte->reserved = couple_id;
    _tcscpy_s(atte->file_name, MAX_PATH, szNewName);
    atte->action_type = MOVE;
}

static VOID DeleFolderApp(FilesVec *pActioCache, const TCHAR *szExistName, const TCHAR *szNewName) {
// _LOG_DEBUG(_T("install new node dele:%s"), szFileName);
    struct attent *atte = NFilesVec::AppNewNode(pActioCache);
    _tcscpy_s(atte->file_name, MAX_PATH, szExistName);
    _tcscat_s(atte->file_name, MAX_PATH, _T("\\"));
    atte->action_type = DELE|DIRECTORY;
	//
    atte = NFilesVec::AppNewNode(pActioCache);
    _tcscpy_s(atte->file_name, MAX_PATH, szNewName);
    _tcscat_s(atte->file_name, MAX_PATH, _T("\\"));
    atte->action_type = ADDI|DIRECTORY;
}

// 目录改名转换成文件的改名和目录的删除
VOID CAnchorCache::FolderMoved(const TCHAR *szExistName, const TCHAR *szNewName, HANDLE hINodeA) {  // 目录改名的插入
	struct INodeUtili::INodeA tINodeA;
	TCHAR szExistPath[MAX_PATH], szNewPath[MAX_PATH];
	//
	INodeUtili::ResetINodeHandle(hINodeA, FALSE);
	while(!INodeUtili::INodeANext(hINodeA, &tINodeA)) {
// _LOG_DEBUG(_T("read node dwINodeType:%08X szINodeName:%s"), tINodeA.dwINodeType, tINodeA.szINodeName);
		if(INTYPE_FILE == tINodeA.dwINodeType) {
			// _stprintf_s(szExistPath, MAX_PATH, _T("%s\\%s"), szExistName, tINodeA.szINodeName);
			_tcscpy_s(szExistPath, MAX_PATH, szExistName);
			_tcscat_s(szExistPath, MAX_PATH, tINodeA.szINodeName);
			_tcscpy_s(szNewPath, MAX_PATH, szNewName);
			_tcscat_s(szNewPath, MAX_PATH, tINodeA.szINodeName);
			// 
			MoveFileApp(&m_vActioCache, szExistPath, szNewPath);
		}
	}
	// 删除根目录,所有的目录都删除
	DeleFolderApp(&m_vActioCache, szExistName, szNewName);
}

//

VOID CAnchorCache::ClearActioVec() {
    NFilesVec::DeleFilesVec(&m_vActioCache);
    InterlockedExchange((LONG*)&m_dwLastAnchor, 0x00);
}

DWORD CAnchorCache::AddNewAnchor(DWORD dwNewAnchor) {
    NFilesVec::DeleFilesVec(&m_vActioCache);
    if(dwNewAnchor) SetLastAnchor(dwNewAnchor);
    return 0x00;
}

