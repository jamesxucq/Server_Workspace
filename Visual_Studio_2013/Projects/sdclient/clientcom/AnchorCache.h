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
    VOID FileAddition(const TCHAR *szFileName); // �ļ� add �Ĳ���
    VOID FileModify(const TCHAR *szFileName); // �ļ� modify �Ĳ���
    VOID FileDelete(const TCHAR *szFileName); // �ļ� delete �Ĳ���
    VOID FileCopy(const TCHAR *szExistName, const TCHAR *szNewName);
    VOID FileMoved(const TCHAR *szExistName, const TCHAR *szNewName);
//
	VOID FolderDelete(const TCHAR *szFileName); // Ŀ¼ delete �Ĳ���
	VOID FolderAddition(const TCHAR *szFileName); // Ŀ¼ addition �Ĳ���
    VOID FolderMoved(const TCHAR *szExistName, const TCHAR *szNewName, HANDLE hINodeA);
public:
    inline FilesVec *GetLastActioVec() 
	{ return &m_vActioCache; }
    VOID ClearActioVec();
    DWORD AddNewAnchor(DWORD dwNewAnchor);
};

extern CAnchorCache objAnchorCache;

