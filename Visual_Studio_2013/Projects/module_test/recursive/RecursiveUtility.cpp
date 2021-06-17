#include "StdAfx.h"

#include "StringUtility.h"
#include "FilesVec.h"
#include "FileUtility.h"

#include "RecursiveUtility.h"

#define MAX_RECURSIVE_NUM       4096

struct recu_head {
    unsigned __int64 hand_inde;
    TCHAR drive[MAX_PATH];
};

//
int read_head(struct recu_head *repa, HANDLE hPathFile)
{
    DWORD dwResult = SetFilePointer(hPathFile, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD rlen;
    if(!ReadFile(hPathFile, repa, sizeof(struct recu_head), &rlen, NULL) || 0 >= rlen) {
        return -1;
    }
    return 0;
}

int write_head(HANDLE hPathFile, struct recu_head *repa)
{
    DWORD dwResult = SetFilePointer(hPathFile, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD wlen;
    if(!WriteFile(hPathFile, repa, sizeof(struct recu_head), &wlen, NULL)) {
        return -1;
    }
    return 0;
}

int read_path(TCHAR *path, HANDLE hPathFile, unsigned __int64 offset)
{
    LONG lFileSizeHigh = (DWORD)(offset>>32);
    DWORD dwResult = SetFilePointer(hPathFile, (DWORD)(offset&0xFFFFFFFF), &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD rlen;
    if(!ReadFile(hPathFile, path, MAX_PATH, &rlen, NULL) || 0 >= rlen) {
        return -1;
    }
    return 0;
}

int write_path(HANDLE hPathFile, TCHAR *path)
{
    DWORD dwResult = SetFilePointer(hPathFile, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD wlen;
    if(!WriteFile(hPathFile, path, MAX_PATH, &wlen, NULL)) {
        return -1;
    }
    return 0;
}

int read_latt(struct attent *latt, HANDLE hEntryFile, unsigned __int64 offset)
{
    LONG lFileSizeHigh = (DWORD)(offset>>32);
    DWORD dwResult = SetFilePointer(hEntryFile, (DWORD)(offset&0xFFFFFFFF), &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD rlen;
    if(!ReadFile(hEntryFile, latt, sizeof(struct attent), &rlen, NULL) || 0 >= rlen) {
        return -1;
    }
    return 0;
}

int write_latt(HANDLE hEntryFile, struct attent *latt)
{
    DWORD dwResult = SetFilePointer(hEntryFile, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD wlen;
    if(!WriteFile(hEntryFile, latt, sizeof (struct attent), &wlen, NULL)) {
        return -1;
    }
    return 0;
}

int write_latt_ext(HANDLE hEntryFile, TCHAR *szPathString, WIN32_FIND_DATA *pFindData)
{
    struct attent latt;
    nstriutility::full_path(latt.file_name, szPathString, pFindData->cFileName);
    latt.file_size = pFindData->nFileSizeLow;
    latt.file_size += ((unsigned __int64)pFindData->nFileSizeHigh) << 32;
    latt.last_write = pFindData->ftLastWriteTime;
    //
    DWORD dwResult = SetFilePointer(hEntryFile, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD wlen;
    if(!WriteFile(hEntryFile, &latt, sizeof (struct attent), &wlen, NULL)) {
        return -1;
    }
    return 0;
}

//

#define RECURSIVE_FILE_PATH   _T("recursive_file.path")
static TCHAR szPathFile[MAX_PATH];

HANDLE RecursiveUtility::CreatRecursiveEnviro(TCHAR *szDrive)
{
    struct recu_head repa;
//
    BOOL bPathExists = NFileUtility::FileExists(szPathFile);
    HANDLE hPathFile = NFileUtility::BuildCacheFile(szPathFile, RECURSIVE_FILE_PATH);
    if(!bPathExists) {
        repa.hand_inde = sizeof (struct recu_head);
        _stprintf_s(repa.drive, _T("%s\\"), szDrive);
        write_head(hPathFile, &repa);
        write_path(hPathFile, repa.drive);
    }
//
    return hPathFile;
}

/* Limits and constants. */
#define TYPE_FILE 0x01		/* Used in ls, rm, and lsFP */
#define TYPE_DIR 0x02
#define TYPE_DOT 0x04

/* Return file type from the find data structure.
Types supported:
TYPE_FILE:	If this is a file
TYPE_DIR:	If this is a directory other than . or ..
TYPE_DOT:	If this is . or .. directory */
static DWORD FileType (LPWIN32_FIND_DATA pFileData)
{
    DWORD dwFileType = TYPE_FILE;
    if (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        if (!lstrcmp(pFileData->cFileName, _T (".")) || !lstrcmp(pFileData->cFileName, _T ("..")))
            dwFileType = TYPE_DOT;
        else dwFileType = TYPE_DIR;

    return dwFileType;
}

// 0:ok 0x01:error 0x02:processing 0x03:not_found
DWORD RecursiveUtility::HandRecursiveFile(HANDLE hPathFile, HANDLE hEntryFile, DWORD dwMaxRecursive)
{
    DWORD dwResult = SetFilePointer(hPathFile, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return 0x01;
    }
    dwResult = SetFilePointer(hEntryFile, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return 0x01;
    }
    //
    struct recu_head repa;
    read_head(&repa, hPathFile);
    //
    WIN32_FIND_DATA FindData;
    TCHAR szPathString[MAX_PATH], szPathName[MAX_PATH], szSubPath[MAX_PATH];
    DWORD dwFileType;
    DWORD dwRecursiveTally = 0;
    int haval = 0x02;
    //
    for (; dwMaxRecursive > dwRecursiveTally; repa.hand_inde += MAX_PATH) {
        if (read_path(szPathString, hPathFile, repa.hand_inde)) {
            haval = 0x00;
            break;
        }
        _stprintf_s(szPathName, _T("%s*"), szPathString);
        TRACE(_T("find dir:%s\n"), szPathName);
        HANDLE SearchHandle = FindFirstFile (szPathName, &FindData);
        if(INVALID_HANDLE_VALUE == SearchHandle) {
            TRACE("opendir\n");
            return 0x03;
        }
        do {
            dwFileType = FileType (&FindData);
            //
            if (dwFileType & TYPE_FILE) {
                dwRecursiveTally++;
                write_latt_ext(hEntryFile, szPathString, &FindData);
                TRACE(_T("add file:%s\n"), FindData.cFileName);
            } else if (dwFileType & TYPE_DIR) {
                _stprintf_s(szSubPath, _T("%s%s\\"), szPathString, FindData.cFileName);
                TRACE(_T("add dir:%s\n"), szSubPath);
                write_path(hPathFile, szSubPath);
            }
        } while (FindNextFile (SearchHandle, &FindData));
        FindClose (SearchHandle);
    }
    //
    write_head(hPathFile, &repa);
    return haval;
}

//
DWORD RecursiveUtility::RecursiveFile(HANDLE hEntryFile, TCHAR *szDrive) // 0:ok 0x01:error 0x02:processing 0x03:not_found
{
    HANDLE hPathFile = INVALID_HANDLE_VALUE;
    //
    hPathFile = CreatRecursiveEnviro(szDrive);
    DWORD recu_value = HandRecursiveFile(hPathFile, hEntryFile, MAX_RECURSIVE_NUM);
    //
    if(INVALID_HANDLE_VALUE != hPathFile) {
        CloseHandle( hPathFile );
        hPathFile = INVALID_HANDLE_VALUE;
    }
    if(0x02 != recu_value) DeleteFile(szPathFile);
    //
    return recu_value;
}
