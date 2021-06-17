#include "StdAfx.h"

#include "third_party.h"
#include "StringUtility.h"
#include "FilesVec.h"
#include "FileUtility.h"
#include "iattb_type.h"

#include "RecursiveUtility.h"

#define MAX_RECURSIVE_NUM       4096

struct recu_head {
    unsigned __int64 hand_inde;
    TCHAR drive[MAX_PATH];
};

//
int read_head(struct recu_head *repa, HANDLE hPathFile) {
    DWORD dwResult = SetFilePointer(hPathFile, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD slen;
    if(!ReadFile(hPathFile, repa, sizeof(struct recu_head), &slen, NULL) || !slen) {
        return -1;
    }
    return 0;
}

int write_head(HANDLE hPathFile, struct recu_head *repa) {
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

int read_path(TCHAR *path, HANDLE hPathFile, unsigned __int64 offset) {
    LONG lFileSizeHigh = (DWORD)(offset>>32);
    DWORD dwResult = SetFilePointer(hPathFile, (DWORD)(offset&0xFFFFFFFF), &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD slen;
    if(!ReadFile(hPathFile, path, MAX_PATH, &slen, NULL) || !slen) {
        return -1;
    }
    return 0;
}

int write_path(HANDLE hPathFile, TCHAR *path) {
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

int write_path_ext(HANDLE hPathFile, TCHAR *szPathString, WIN32_FIND_DATA *pFindData) {
	TCHAR path[MAX_PATH];
	struti::full_dire(path, szPathString, pFindData->cFileName);
// _LOG_DEBUG(_T("app dire:%s"), path);
	//
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

int read_latt(struct attent *latt, HANDLE hEntryFile, unsigned __int64 offset) {
    LONG lFileSizeHigh = (DWORD)(offset>>32);
    DWORD dwResult = SetFilePointer(hEntryFile, (DWORD)(offset&0xFFFFFFFF), &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD slen;
    if(!ReadFile(hEntryFile, latt, sizeof(struct attent), &slen, NULL) || !slen) {
        return -1;
    }
    return 0;
}

int write_latt(HANDLE hEntryFile, struct attent *latt) {
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

int write_latt_file(HANDLE hEntryFile, TCHAR *szPathString, WIN32_FIND_DATA *pFindData) {
    struct attent latt;
	latt.action_type = RECURSIVE;
    struti::full_path(latt.file_name, szPathString, pFindData->cFileName);
// _LOG_DEBUG(_T("add file:%s"), latt.file_name);
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

int write_latt_dire(HANDLE hEntryFile, TCHAR *szPathString, WIN32_FIND_DATA *pFindData) {
    struct attent latt;
	latt.action_type = RECURSIVE | DIRECTORY;
    struti::full_dire(latt.file_name, szPathString, pFindData->cFileName);
// _LOG_DEBUG(_T("add dire:%s"), latt.file_name);
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

HANDLE RecursiveUtility::CreatRecursiveEnviro(TCHAR *szDriveLetter) {
    struct recu_head repa;
//
    BOOL bPathExist = NFileUtility::FileExist(szPathFile);
    HANDLE hPathFile = NFileUtility::BuildCacheFile(szPathFile, RECURSIVE_FILE_PATH);
    if(!bPathExist) {
        repa.hand_inde = sizeof (struct recu_head);
        _stprintf_s(repa.drive, _T("%s\\"), szDriveLetter);
        write_head(hPathFile, &repa);
        write_path(hPathFile, repa.drive);
    }
//
    return hPathFile;
}

/* Limits and constants. */
#define TYPE_FILE 0x01		/* Used in ls, rm, and lsFP */
#define TYPE_DIRE 0x02
#define TYPE_DOT 0x04

/* Return file type from the find data structure.
Types supported:
TYPE_FILE:	If this is a file
TYPE_DIRE:	If this is a directory other than . or ..
TYPE_DOT:	If this is . or .. directory */
static DWORD FileType (LPWIN32_FIND_DATA pFileData) {
    DWORD dwFileType = TYPE_FILE;
    if (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        if (!lstrcmp(pFileData->cFileName, _T (".")) || !lstrcmp(pFileData->cFileName, _T ("..")))
            dwFileType = TYPE_DOT;
        else dwFileType = TYPE_DIRE;

    return dwFileType;
}

// 0:ok 0x01:error 0x02:processing
#define ROOTLEN	0x02
DWORD RecursiveUtility::HandRecursiveFile(HANDLE hPathFile, HANDLE hEntryFile, DWORD dwMaxRecursive) {
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
    TCHAR szPathString[MAX_PATH], szPathName[MAX_PATH];
    DWORD dwFileType;
    DWORD dwRecursiveTatol = 0x00;
    int haval = 0x02;
    //
    for (; dwMaxRecursive > dwRecursiveTatol; repa.hand_inde += MAX_PATH) {
        if (read_path(szPathString, hPathFile, repa.hand_inde)) {
            haval = 0x00;
            break;
        }
        _stprintf_s(szPathName, _T("%s*"), szPathString);
// _LOG_DEBUG(_T("find dir:%s"), szPathName);
        HANDLE SearchHandle = FindFirstFile (szPathName, &FindData);
        if(INVALID_HANDLE_VALUE == SearchHandle) {
// _LOG_DEBUG("opendir");
            continue;
        }
        do {
            dwFileType = FileType (&FindData);
            //
            if (dwFileType & TYPE_FILE) {
                dwRecursiveTatol++;
                write_latt_file(hEntryFile, szPathString+ROOTLEN, &FindData);
// _LOG_DEBUG(_T("recu add file:%s"), FindData.cFileName);
            } else if (dwFileType & TYPE_DIRE) {
				write_latt_dire(hEntryFile, szPathString+ROOTLEN, &FindData);
// _LOG_DEBUG(_T("recu add dir:%s"), hPathFile);
                write_path_ext(hPathFile, szPathString, &FindData);
            }
        } while (FindNextFile (SearchHandle, &FindData));
        FindClose (SearchHandle);
    }
    //
    write_head(hPathFile, &repa);
    return haval;
}

//
DWORD RecursiveUtility::RecursiveFile(HANDLE hEntryFile, TCHAR *szDriveLetter) { // 0:ok 0x01:error 0x02:processing
    HANDLE hPathFile = INVALID_HANDLE_VALUE;
    //
    hPathFile = CreatRecursiveEnviro(szDriveLetter);
    DWORD recurs_value = HandRecursiveFile(hPathFile, hEntryFile, MAX_RECURSIVE_NUM);
    //
    if(INVALID_HANDLE_VALUE != hPathFile) {
        CloseHandle( hPathFile );
        hPathFile = INVALID_HANDLE_VALUE;
    }
    if(0x02 != recurs_value) DeleteFile(szPathFile);
    //
    return recurs_value;
}
