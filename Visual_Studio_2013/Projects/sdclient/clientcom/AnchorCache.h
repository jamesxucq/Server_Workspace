#pragma once
#include <afxmt.h>

#include "FilesVec.h"

#include <vector>
using  std::vector;


class CAnchorCache {
public:
    CAnchorCache(void);
    ~CAnchorCache(void);
public:
    DWORD Initialize(DWORD last_anchor);
    DWORD Finalize();
    // anchor logic operate;
private:
    volatile DWORD m_dwLastAnchor;
    FilesVec m_vActioCache; // after last anchor modify files;
public:
    inline VOID SetLastAnchor(DWORD last_anchor) 
	{ InterlockedExchange((LONG*)&m_dwLastAnchor, last_anchor); }
    inline DWORD GetLastAnchor() {
        DWORD last_anchor;
        InterlockedExchange((LONG*)&last_anchor, m_dwLastAnchor);
        return last_anchor;
    }
public:
    VOID FileAddition(const TCHAR *szFileName); // 文件 add 的插入
    VOID FileModify(const TCHAR *szFileName); // 文件 modify 的插入
    VOID FileDelete(const TCHAR *szFileName); // 文件 delete 的插入
    VOID FileCopy(const TCHAR *szExistName, const TCHAR *szNewName);
    VOID FileMoved(const TCHAR *szExistName, const TCHAR *szNewName);
//
	VOID FolderDelete(const TCHAR *szFileName); // 目录 delete 的插入
	VOID FolderAddition(const TCHAR *szFileName); // 目录 addition 的插入
    VOID FolderMoved(const TCHAR *szExistName, const TCHAR *szNewName, HANDLE hINodeA);
public:
    inline FilesVec *GetLastActioVec() 
	{ return &m_vActioCache; }
    VOID ClearActioVec();
    DWORD AddNewAnchor(DWORD dwNewAnchor);
};

extern CAnchorCache objAnchorCache;

