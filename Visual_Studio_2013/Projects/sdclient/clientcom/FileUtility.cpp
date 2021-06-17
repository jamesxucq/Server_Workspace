#include "stdafx.h"
#include <winioctl.h>
#include "Shlwapi.h"
#include <tchar.h>
#include "Shlwapi.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utime.h>

#include "third_party.h"
#include "StringUtility.h"
#include "FileUtility.h"

//
BOOL NFileUtility::CreationTime(FILETIME *ftCreatTime, const TCHAR *szFolderPath) {
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    //
    if(!szFolderPath || !ftCreatTime) return FALSE;
    hFind = FindFirstFile(szFolderPath, &fileInfo);
    if(INVALID_HANDLE_VALUE != hFind) {
		memcpy(ftCreatTime, &fileInfo.ftCreationTime, sizeof(FILETIME));
    }
    FindClose(hFind);
    //
    return TRUE;
}

BOOL NFileUtility::FileExist(TCHAR *szFileName) {
    WIN32_FIND_DATA wfd;
    BOOL bFound = FALSE;
    //
    HANDLE hFind = FindFirstFile(szFileName, &wfd);
    if (INVALID_HANDLE_VALUE != hFind) bFound = TRUE;
    //
    FindClose(hFind);
    return bFound;
}


ULONG NFileUtility::MateriExist(HANDLE hFileMateri) {
    ULONG lOffset = SetFilePointer(hFileMateri, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lOffset && NO_ERROR!=GetLastError())
		lOffset = 0;
	return lOffset;
}

unsigned __int64 NFileUtility::FileSize(const TCHAR *szFileName) {
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    unsigned __int64 qwFileSize = 0x00;
    //
    hFind = FindFirstFile(szFileName, &fileInfo);
    if(INVALID_HANDLE_VALUE != hFind) {
        qwFileSize = fileInfo.nFileSizeLow;
        qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
    }
    //
    FindClose(hFind);
    return qwFileSize;
}

unsigned __int64 NFileUtility::FileSizeHandle(HANDLE hFileHand) {
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileSize = GetFileSize(hFileHand, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError())
        return ((unsigned __int64)-1);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    return qwFileSize;
}

__int64 NFileUtility::FileSizeStream(FILE *file_stre) {
    _fseeki64(file_stre, 0, SEEK_END);
    return _ftelli64(file_stre);
}

DWORD NFileUtility::FileSizeTime(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, TCHAR *szFileName) {
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    //
    hFind = FindFirstFile(szFileName, &fileInfo);
    if(INVALID_HANDLE_VALUE != hFind) {
        *qwFileSize = fileInfo.nFileSizeLow;
        *qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
		memcpy(ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME));
	} else { return ((DWORD)-1); }
    //
    FindClose(hFind);
    return 0x00;
}

DWORD NFileUtility::FileLastWrite(FILETIME *ftLastWrite, TCHAR *szFileName) {
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    //
    hFind = FindFirstFile(szFileName, &fileInfo);
    if(INVALID_HANDLE_VALUE != hFind) memcpy(ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME));
	else { return ((DWORD)-1); }
    //
    FindClose(hFind);
    return 0x00;
}

TCHAR *NFileUtility::ModuleBaseName() {
    static TCHAR szBasePath[MAX_PATH];
    //
	if(_T(':') != szBasePath[1]) {
        if (!GetModuleFileName(NULL, szBasePath, MAX_PATH))
            return NULL;
        if (!PathRemoveFileSpec(szBasePath))
            return NULL;
    }
    //
    return szBasePath;
}

TCHAR *NFileUtility::ModuleBaseName(TCHAR *lpFilePath) {
    static TCHAR szBasePath[MAX_PATH];
    if(!lpFilePath) return NULL;
    //
	if(_T(':') != szBasePath[1]) {
		if (!GetModuleFileName(NULL, szBasePath, MAX_PATH)) return NULL;
		TCHAR *Token = _tcsrchr(szBasePath, _T('\\'));
		if (!Token) return NULL;
		*(Token) = _T('\0');	
	}
    _tcscpy_s(lpFilePath, MAX_PATH, szBasePath);
    //
    return lpFilePath;
}

TCHAR *NFileUtility::ModuleBaseNameEx(TCHAR *lpFilePath) {
    static TCHAR szBasePath[MAX_PATH];
    if(!lpFilePath) return NULL;
    //
	if(_T(':') != szBasePath[1]) {
		if (!GetModuleFileName(NULL, szBasePath, MAX_PATH)) return NULL;
		TCHAR *Token = _tcsrchr(szBasePath, _T('\\'));
		if (!Token) return NULL;
		*(++Token) = _T('\0');	
	}
    _tcscpy_s(lpFilePath, MAX_PATH, szBasePath);
    //
    return lpFilePath;
}

TCHAR *NFileUtility::SystemDriverPath(TCHAR *lpFilePath) {
    static TCHAR szSystemPath[MAX_PATH];
    if(!lpFilePath) return NULL;
    //
	if(_T(':') != szSystemPath[1]) {
		if(!GetEnvironmentVariable(_T("SYSTEMROOT"), szSystemPath, MAX_PATH)) 
			return NULL;
		_tcscat_s(szSystemPath, _T("\\system32\\drivers"));
	}
    _tcscpy_s(lpFilePath, MAX_PATH, szSystemPath);
    //
    return lpFilePath;
}

TCHAR *NFileUtility::ApplicaDataPath(TCHAR *szFilePath, const TCHAR *szFileName) {
    static TCHAR szApplicaPath[MAX_PATH];
    if(!szFileName) return NULL;
    //
	if(_T(':') != szApplicaPath[1]) {
		if(!GetEnvironmentVariable(_T("APPDATA"), szApplicaPath, MAX_PATH)) 
			return NULL;
		_tcscat_s(szApplicaPath, _T("\\sdclient"));
		CreateDirectory(szApplicaPath, NULL);
	}
    _stprintf_s(szFilePath, MAX_PATH, _T("%s\\%s"), szApplicaPath, szFileName);
    //
    return szFilePath;
}

#define CREAT_TEMP_FILE(FILE_NAME) { \
    static TCHAR szTempPath[MAX_PATH]; \
    if(_T(':') != szTempPath[1]) \
		GetTempPath(MAX_PATH, szTempPath); \
    GetTempFileName(szTempPath, _T("sdclient"), 0, FILE_NAME); \
}

#define BUFF_SIZE 0x2000
#define BUILD_CHUNK_SIZE						((int)1 << 22) // 4M
HANDLE NFileUtility::CreatBackupChunk(TCHAR *szBackupChunk, TCHAR *szFromFile, long Offset) {
    TCHAR szFileName[MAX_PATH];
    HANDLE hChunkBackup, hFromFile;
    DWORD dwWritenSize = 0x00, dwReadSize = 0x00;
    char szBuffer [BUFF_SIZE];
    //
	CREAT_TEMP_FILE(szFileName)
    if(szBackupChunk) _tcscpy_s(szBackupChunk, MAX_PATH, szFileName);
    //
    hChunkBackup = CreateFile( szFileName,
                               GENERIC_WRITE,
                               NULL, /* FILE_SHARE_READ */
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_TEMPORARY,
                               NULL);
    if( INVALID_HANDLE_VALUE == hChunkBackup ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    //
    hFromFile = CreateFile( szFromFile,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
    if( INVALID_HANDLE_VALUE == hFromFile ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		CloseHandle(hChunkBackup);
        return INVALID_HANDLE_VALUE;
    }
    DWORD dwResult = SetFilePointer(hFromFile, Offset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hChunkBackup);
		CloseHandle(hFromFile);
        return INVALID_HANDLE_VALUE;
	}
    //
    DWORD dwBackupTatol = 0x00;
    while(ReadFile(hFromFile, szBuffer, BUFF_SIZE, &dwReadSize, NULL) && 0<dwReadSize) {
        WriteFile(hChunkBackup, szBuffer, dwReadSize, &dwWritenSize, NULL);
        if(dwReadSize != dwWritenSize) {
            _LOG_WARN(_T("fatal write error: %d"), GetLastError());
			CloseHandle(hChunkBackup);
			CloseHandle(hFromFile);
            return INVALID_HANDLE_VALUE;
        }
        dwBackupTatol += dwWritenSize;
        if(BUILD_CHUNK_SIZE ==dwBackupTatol) break;
    }
    //
    CloseHandle(hFromFile);
    return hChunkBackup;
}

HANDLE NFileUtility::BackupFile(HANDLE hFromFile) {
    TCHAR szFileName[MAX_PATH];
    HANDLE hFileBackup;
    DWORD dwWritenSize = 0x00, dwReadSize = 0x00;
    char szBuffer [BUFF_SIZE];
    //
	CREAT_TEMP_FILE(szFileName)
    hFileBackup = CreateFile( szFileName,
                              GENERIC_WRITE,
                              NULL, /* FILE_SHARE_READ */
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                              NULL);
    if( INVALID_HANDLE_VALUE == hFileBackup ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
	//
    DWORD dwResult = SetFilePointer(hFromFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hFileBackup);
        return INVALID_HANDLE_VALUE;
	}
    //
    while(ReadFile(hFromFile, szBuffer, BUFF_SIZE, &dwReadSize, NULL) && 0<dwReadSize) {
        WriteFile(hFileBackup, szBuffer, dwReadSize, &dwWritenSize, NULL);
        if(dwReadSize != dwWritenSize) {
            _LOG_WARN(_T("fatal write error: %d"), GetLastError());
			CloseHandle(hFileBackup);
            return INVALID_HANDLE_VALUE;
        }
    }
    //
    return hFileBackup;
}

HANDLE NFileUtility::CreatBackupFile(TCHAR *szBackupFile, TCHAR *szFromFile, long Offset) {
    TCHAR szFileName[MAX_PATH];
    HANDLE hFileBackup, hFromFile;
    DWORD dwWritenSize = 0x00, dwReadSize = 0x00;
    char szBuffer [BUFF_SIZE];
    //
	CREAT_TEMP_FILE(szFileName)
    if(szBackupFile) _tcscpy_s(szBackupFile, MAX_PATH, szFileName);
    //
    hFileBackup = CreateFile( szFileName,
                              GENERIC_WRITE,
                              NULL, /* FILE_SHARE_READ */
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY,
                              NULL);
    if( INVALID_HANDLE_VALUE == hFileBackup ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    //
    hFromFile = CreateFile( szFromFile,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
    if( INVALID_HANDLE_VALUE == hFromFile ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		CloseHandle(hFileBackup);
        return INVALID_HANDLE_VALUE;
    }
    DWORD dwResult = SetFilePointer(hFromFile, Offset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hFileBackup);
		CloseHandle(hFromFile);
        return INVALID_HANDLE_VALUE;
	}
    //
    while(ReadFile(hFromFile, szBuffer, BUFF_SIZE, &dwReadSize, NULL) && 0<dwReadSize) {
        WriteFile(hFileBackup, szBuffer, dwReadSize, &dwWritenSize, NULL);
        if(dwReadSize != dwWritenSize) {
            _LOG_WARN(_T("fatal write error: %d"), GetLastError());
			CloseHandle(hFileBackup);
			CloseHandle(hFromFile);
            return INVALID_HANDLE_VALUE;
        }
    }
    //
    CloseHandle(hFromFile);
    return hFileBackup;
}

HANDLE NFileUtility::BuildChksFile(TCHAR *szChksFile) {
    HANDLE hFileChks;
    TCHAR szFileName[MAX_PATH];
    //
	CREAT_TEMP_FILE(szFileName)
    hFileChks = CreateFile( szFileName,
                            GENERIC_WRITE | GENERIC_READ,
                            NULL, /* FILE_SHARE_READ */
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                            NULL);
    if( INVALID_HANDLE_VALUE == hFileChks ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    if(szChksFile) _tcscpy_s(szChksFile, MAX_PATH, szFileName);
    //
    return hFileChks;
}

HANDLE NFileUtility::BuildVerifyFile(TCHAR *szVerifyFile) {
    HANDLE hFileVerify;
    TCHAR szFileName[MAX_PATH];
    //
	CREAT_TEMP_FILE(szFileName)
    hFileVerify = CreateFile( szFileName,
                              GENERIC_WRITE | GENERIC_READ,
                              NULL, /* FILE_SHARE_READ */
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                              NULL);
    if( INVALID_HANDLE_VALUE == hFileVerify ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    if(szVerifyFile) _tcscpy_s(szVerifyFile, MAX_PATH, szFileName);
    //
    return hFileVerify;
}

HANDLE NFileUtility::BuildComplexFile(TCHAR *szComplexFile) {
    HANDLE hFileCompl;
    TCHAR szFileName[MAX_PATH];
    //
	CREAT_TEMP_FILE(szFileName)
    hFileCompl = CreateFile( szFileName,
                               GENERIC_WRITE | GENERIC_READ,
                               NULL, /* FILE_SHARE_READ */
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                               NULL);
    if( INVALID_HANDLE_VALUE == hFileCompl ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    if(szComplexFile) _tcscpy_s(szComplexFile, MAX_PATH, szFileName);
    //
    return hFileCompl;
}

HANDLE NFileUtility::OpenLocalFile(TCHAR *szFileLocal) {
    HANDLE hFileLocal;
    //
    if(!szFileLocal) return INVALID_HANDLE_VALUE;
    hFileLocal = CreateFile( szFileLocal,
                             GENERIC_WRITE | GENERIC_READ,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
    if( INVALID_HANDLE_VALUE == hFileLocal ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    //
    return hFileLocal;
}

DWORD NFileUtility::IsFileLocked(const TCHAR *szFileName) {  // -1:error 0:not lock 1:locked
    HANDLE hFileLocked;
    //
    if(!szFileName) return ((DWORD)-1);
    hFileLocked = CreateFile( szFileName,
                              /* GENERIC_WRITE | */ GENERIC_READ,
                              FILE_SHARE_READ /* | FILE_SHARE_WRITE */,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    if( INVALID_HANDLE_VALUE == hFileLocked ) {
		if(ERROR_SHARING_VIOLATION==GetLastError() || ERROR_LOCK_VIOLATION==GetLastError()) {
			return FILE_LOCKED;
		}
        return FILE_LOCKEO;
    }
    //
    if(INVALID_HANDLE_VALUE != hFileLocked) {
        CloseHandle( hFileLocked );
        hFileLocked = INVALID_HANDLE_VALUE;
    }
    //
    return FILE_UNLOCK;
}

DWORD NFileUtility::LockReadFile(HANDLE *hReadLocked, const TCHAR *szFileName) {  // -1:error 0:not lock 1:locked
    HANDLE hLockRead;
    //
    if(!szFileName) return ((DWORD)-1);
    hLockRead = CreateFile( szFileName,
                              /* GENERIC_WRITE | */ GENERIC_READ,
                              FILE_SHARE_READ /* | FILE_SHARE_WRITE */,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    if( INVALID_HANDLE_VALUE == hLockRead ) {
		*hReadLocked = INVALID_HANDLE_VALUE;
		if(ERROR_SHARING_VIOLATION==GetLastError() || ERROR_LOCK_VIOLATION==GetLastError()) {
			return FILE_LOCKED;
		}
		return FILE_LOCKEO;
    }
    //
	*hReadLocked = hLockRead;
    return FILE_UNLOCK;
}

VOID NFileUtility::UnlockFile(HANDLE hFileLocked) {
	if(INVALID_HANDLE_VALUE != hFileLocked)
        CloseHandle( hFileLocked );
}

DWORD NFileUtility::CreateHugeFile(LPCTSTR szFileName, DWORD dwFileLength) {
    HANDLE hFileHuge;
    HANDLE hMapFile;
    //
    hFileHuge = CreateFile( szFileName,
                            GENERIC_WRITE | GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
    if( INVALID_HANDLE_VALUE == hFileHuge ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
    //
    // NTFS稀疏文件优化，FAT32下不起作用
    DWORD dwBytesReturned = 0;
    DeviceIoControl(hFileHuge, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
    //
    unsigned __int64 qwFileSize = ((unsigned __int64)dwFileLength) << 30;
    hMapFile = CreateFileMapping(hFileHuge, NULL, PAGE_READWRITE, (DWORD)(qwFileSize>>32)&0xffffffff, (DWORD)qwFileSize&0xffffffff, NULL);
    if(NULL == hMapFile) {
        _LOG_WARN( _T("create file mapping failed.") );
        CloseHandle( hFileHuge );
        return ((DWORD)-1);
    }
    //
    CloseHandle( hMapFile );
    CloseHandle( hFileHuge );
//
    return 0x00;
}

DWORD NFileUtility::ShrinkHugeFile(LPCTSTR szFileName, DWORD dwFileLength) {
    HANDLE hFileHuge;
    //
    hFileHuge = CreateFile( szFileName,
                            GENERIC_WRITE | GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
    if( INVALID_HANDLE_VALUE == hFileHuge ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
    //
    unsigned __int64 qwFileSize = ((unsigned __int64)dwFileLength) << 30;
    LONG lDistanceToMoveHigh = (qwFileSize>>32)&0xffffffff;
    DWORD dwResult = SetFilePointer(hFileHuge, (DWORD)qwFileSize&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle( hFileHuge );
        return ((DWORD)-1);
	}
    dwResult = SetEndOfFile(hFileHuge);
	if(FALSE==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle( hFileHuge );	
        return ((DWORD)-1);
	}
    CloseHandle( hFileHuge );
    //
    return 0x00;
}

DWORD NFileUtility::ExpandHugeFile(LPCTSTR szFileName, DWORD dwFileLength) {
    HANDLE hFileHuge;
    HANDLE hMapFile;
    //
    hFileHuge = CreateFile( szFileName,
                            GENERIC_WRITE | GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
    if( INVALID_HANDLE_VALUE == hFileHuge ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
    //
    unsigned __int64 qwFileSize = ((unsigned __int64)dwFileLength) << 30;
    hMapFile = CreateFileMapping(hFileHuge, NULL, PAGE_READWRITE, (DWORD)(qwFileSize>>32)&0xffffffff, (DWORD)qwFileSize&0xffffffff, NULL);
    if(NULL == hMapFile) {
        _LOG_WARN( _T("create file mapping failed.") );
        CloseHandle( hFileHuge );
        return ((DWORD)-1);
    }
    //
    CloseHandle( hMapFile );
    CloseHandle( hFileHuge );
    //
    return 0x00;
}

DWORD NFileUtility::TruncateHandle(HANDLE hFileHand, unsigned __int64 qwFileLength) {
    LONG lDistanceToMoveHigh = (qwFileLength>>32)&0xffffffff;
    DWORD dwResult = SetFilePointer(hFileHand, (LONG)qwFileLength&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    dwResult = SetEndOfFile(hFileHand);
    if(FALSE==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    //
    return 0x00;
}

DWORD NFileUtility::SetFileSize(const TCHAR *szFileName, unsigned __int64 qwFileLength) {
    HANDLE hFileSize = CreateFile( szFileName,
                                   GENERIC_WRITE | GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if( INVALID_HANDLE_VALUE == hFileSize ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
    //
    LONG lDistanceToMoveHigh = (qwFileLength>>32)&0xffffffff;
    DWORD dwResult = SetFilePointer(hFileSize, (LONG)qwFileLength&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hFileSize);
        return ((DWORD)-1);
	}
    dwResult = SetEndOfFile(hFileSize);
	if(FALSE==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hFileSize);
        return ((DWORD)-1);
	}
    //
    CloseHandle(hFileSize);
    return 0x00;
}

DWORD NFileUtility::SetFileLawi(const TCHAR *szFileName, FILETIME *ftLastWrite) {
    HANDLE hFileLawi = CreateFile( szFileName,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if( INVALID_HANDLE_VALUE == hFileLawi ) {
_LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
    SetFileTime(hFileLawi, NULL, NULL, ftLastWrite); // add by james 20110910
    CloseHandle(hFileLawi);
// _LOG_DEBUG( _T("set name:%s lawi:%I64u"), szFileName, ftLastWrite );
    //
    return 0x00;
}

DWORD NFileUtility::SetFileSizeLawi(const TCHAR *szFileName, unsigned __int64 qwFileLength, FILETIME *ftLastWrite) {
    HANDLE hFileArib = CreateFile( szFileName,
                                   GENERIC_WRITE | GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if( INVALID_HANDLE_VALUE == hFileArib ) {
_LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
    //
    LONG lDistanceToMoveHigh = (qwFileLength>>32)&0xffffffff;
    DWORD dwResult = SetFilePointer(hFileArib, (LONG)qwFileLength&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hFileArib);
        return ((DWORD)-1);
	}
    dwResult = SetEndOfFile(hFileArib);
	if(FALSE==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hFileArib);
        return ((DWORD)-1);
	}
    //
    SetFileTime(hFileArib, NULL, NULL, ftLastWrite); // add by james 20110910
    CloseHandle(hFileArib);
// _LOG_DEBUG( _T("set name:%s lawi:%I64u"), szFileName, ftLastWrite );
    return 0x00;
}


DWORD NFileUtility::GetCoupleID() {
	static DWORD dwTotal;
    return GetTickCount() + (dwTotal++);
}

DWORD NFileUtility::RandomCoupleID() {
    srand((unsigned int) time(NULL));
    return rand();
}

DWORD NFileUtility::BlockCopyFile(LPCWSTR lpExistFile, LPCWSTR lpNewFile, DWORD dwMaxLen) {
    HANDLE hNewFile, hExistFile;
    DWORD dwWritenSize = 0x00, dwReadSize = 0x00;
    char szBuffer [BUFF_SIZE];
    //
    hNewFile = CreateFile( lpNewFile,
                          GENERIC_WRITE,
                          NULL, /* FILE_SHARE_READ */
                          NULL,
                          CREATE_ALWAYS,
                          FILE_ATTRIBUTE_TEMPORARY,
                          NULL );
    if( INVALID_HANDLE_VALUE == hNewFile ) {
_LOG_WARN( _T("create new file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return BCOPY_NOT_FOUND;
		} else { return BCOPY_VIOLATION; }
    }
	unsigned __int64 qwFileSize = NFileUtility::FileSizeHandle(hNewFile);
    //
    hExistFile = CreateFile( lpExistFile,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL );
    if( INVALID_HANDLE_VALUE == hExistFile ) {
_LOG_WARN( _T("create exist file failed: %d"), GetLastError() );
		CloseHandle(hNewFile);
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return BCOPY_NOT_FOUND;
		} else { return BCOPY_VIOLATION; }
    }
	if(qwFileSize > NFileUtility::FileSizeHandle(hExistFile)) {
		CloseHandle(hExistFile);
		CloseHandle(hNewFile);
		return BCOPY_EXCEPT;
	}
	//
    LONG lDistanceToMoveHigh = (qwFileSize>>32)&0xffffffff;
    DWORD dwResult = SetFilePointer(hExistFile, (LONG)qwFileSize&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hExistFile);
		CloseHandle(hNewFile);	
        return BCOPY_EXCEPT;
	}
    //
	DWORD dwStepValue = BCOPY_SUCCESS;
    DWORD dwCopyTatol = 0x00;
    while(ReadFile(hExistFile, szBuffer, BUFF_SIZE, &dwReadSize, NULL) && 0<dwReadSize) {
        WriteFile(hNewFile, szBuffer, dwReadSize, &dwWritenSize, NULL);
        if(dwReadSize != dwWritenSize) {
_LOG_WARN(_T("fatal write error: %d"), GetLastError());
			CloseHandle(hExistFile);
			CloseHandle(hNewFile);
            return BCOPY_EXCEPT;
        }
        dwCopyTatol += dwWritenSize;
		if(dwMaxLen ==dwCopyTatol) {
			dwStepValue = BCOPY_CONTINUE;
			break;
		}
    }
    //
    CloseHandle(hExistFile);
    CloseHandle(hNewFile);
//
	return dwStepValue;
}

HANDLE NFileUtility::BuildCacheFile(const TCHAR *szFileName) {
    static TCHAR szCacheDirectory[MAX_PATH];
    TCHAR szBuildFile[MAX_PATH];
    //
    if(_T(':') != szCacheDirectory[1]) {
        if(!GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH))
            return INVALID_HANDLE_VALUE;
        _tcscat_s(szCacheDirectory, _T("\\sdclient\\cache"));
		NDireUtility::RecursMakeFolderW(szCacheDirectory);
    }
    _stprintf_s(szBuildFile, MAX_PATH, _T("%s\\%s"), szCacheDirectory, szFileName);
    //
    HANDLE hFileCache = CreateFile( szBuildFile,
                                    GENERIC_WRITE | GENERIC_READ,
                                    NULL, /* FILE_SHARE_READ */
                                    NULL,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileCache ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    //
    return hFileCache;
}

HANDLE NFileUtility::BuildCacheFile(TCHAR *szCacheFile, const TCHAR *szFileName) {
    static TCHAR szCacheDirectory[MAX_PATH];
    TCHAR szBuildFile[MAX_PATH];
    //
    if(_T(':') != szCacheDirectory[1]) {
        if(!GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH))
            return INVALID_HANDLE_VALUE;
        _tcscat_s(szCacheDirectory, _T("\\sdclient\\cache"));
		NDireUtility::RecursMakeFolderW(szCacheDirectory);
    }
    _stprintf_s(szBuildFile, MAX_PATH, _T("%s\\%s"), szCacheDirectory, szFileName);
    if(szCacheFile) _tcscpy_s(szCacheFile, MAX_PATH, szBuildFile);
    //
    HANDLE hFileCache = CreateFile( szBuildFile,
                                    GENERIC_WRITE | GENERIC_READ,
                                    NULL, /* FILE_SHARE_READ */
                                    NULL,
                                    OPEN_ALWAYS, // CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_TEMPORARY, // | FILE_FLAG_DELETE_ON_CLOSE,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileCache ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    //
    return hFileCache;
}

char *NFileUtility::build_cache_name(char *cache_file, char *file_name) {
    static char cache_directory[MAX_PATH];
    char build_file[MAX_PATH];
    //
    if(':' != cache_directory[1]) {
        if(!GetEnvironmentVariableA("APPDATA", cache_directory, MAX_PATH))
            return NULL;
        strcat_s(cache_directory, "\\sdclient\\cache");
		NDireUtility::RecursMakeFolderA(cache_directory);
    }
    sprintf_s(build_file, MAX_PATH, "%s\\%s", cache_directory, file_name);
    if(cache_file) strcpy_s(cache_file, MAX_PATH, build_file);
    //
    return NULL;
}
//
//

char *timcon::ftim_ansi(char *timestr, FILETIME *filetime) {  // Wed, 15 Nov 1995 04:58:08 GMT
    SYSTEMTIME st;
    if(!FileTimeToSystemTime(filetime, &st)) return NULL;
    struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
    strftime(timestr, 32, "%a, %d %b %Y %H:%M:%S GMT", &gm);
    return timestr;
}

TCHAR *timcon::ftim_unis(TCHAR *timestr, FILETIME *filetime) {  // Wed, 15 Nov 1995 04:58:08 GMT
    SYSTEMTIME st;
    if(!FileTimeToSystemTime(filetime, &st)) return NULL;
    struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
    _tcsftime(timestr, 32, _T("%a, %d %b %Y %H:%M:%S GMT"), &gm);
    return timestr;
}

//
static const char *abday[7] = {
    "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};

static const char *abmon[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

// Wed, 15 Nov 1995 04:58:08 GMT
DWORD timcon::ansi_ftim(FILETIME *filetime, char *timestr) {
    char *s = ", :";
    char *toke, *prev;
    char time_str[32];
    SYSTEMTIME st;
    //
    memset(&st, '\0', sizeof(SYSTEMTIME));
    strcpy_s(time_str, timestr);
    // sscanf_s(timestr,"%2d-%2d-%4d %2d:%2d:%2d",&st.wMonth,&st.wDay,&st.wYear,&st.wHour,&st.wMinute,&st.wSecond);
    prev = time_str;
    int inde, count = 0;
    do {
        toke = strpbrk(prev, s);
        if(toke) toke[0] = '\0';
        switch(count) {
        case 0:
            for(inde = 0; inde < 7; inde++) {
                if(!strcmp(abday[inde], prev)) st.wDayOfWeek = ++inde;
            }
            break;
        case 2:
            st.wDay = atoi(prev);
            break;
        case 3:
            for(inde = 0; inde < 12; inde++) {
                if(!strcmp(abmon[inde], prev)) st.wMonth = ++inde;
            }
            break;
        case 4:
            st.wYear = atoi(prev);
            break;
        case 5:
            st.wHour = atoi(prev);
            break;
        case 6:
            st.wMinute = atoi(prev);
            break;
        case 7:
            st.wSecond = atoi(prev);
            break;
        }
        ++count;
        if(toke) prev = toke + 1;
    } while(toke);
    if(!SystemTimeToFileTime(&st, filetime))
        return ((DWORD)-1);
    //
    return 0x00;
}

//
static const TCHAR *Abmon[12] = {
    _T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"),
    _T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec")
};

// Wed, 15 Nov 1995 04:58:08 GMT
DWORD timcon::unis_ftim(FILETIME *filetime, TCHAR *timestr) {
    TCHAR *s = _T(" :");
    TCHAR *toke, *prev;
    TCHAR time_str[32];
    SYSTEMTIME st;
    //
    memset(&st, '\0', sizeof(SYSTEMTIME));
    _tcscpy_s(time_str, timestr);
    // _stscanf_s(timestr,_T("%2d-%2d-%4d %2d:%2d:%2d"),&st.wMonth,&st.wDay,&st.wYear,&st.wHour,&st.wMinute,&st.wSecond);
    prev = time_str;
    int inde, count = 0;
    do {
        toke = _tcspbrk(prev, s);
        if(toke) toke[0] = _T('\0');
        switch(count) {
        case 1:
            st.wDay = _ttoi(prev);
            break;
        case 2:
            for(inde = 0; inde < 12; inde++) {
                if(!_tcscmp(Abmon[inde], prev)) st.wMonth = ++inde;
            }
            break;
        case 3:
            st.wYear = _ttoi(prev);
            break;
        case 4:
            st.wHour = _ttoi(prev);
            break;
        case 5:
            st.wMinute = _ttoi(prev);
            break;
        case 6:
            st.wSecond = _ttoi(prev);
            break;
        }
        ++count;
        if(toke) prev = toke + 1;
    } while(toke);
    if(!SystemTimeToFileTime(&st, filetime))
        return ((DWORD)-1);
    //
    return 0x00;
}

time_t timcon::ftim_timt(const FILETIME &ft) {
    ULARGE_INTEGER ui;
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

BOOL NDireUtility::CopyDirectory(TCHAR *szExistDirectory, TCHAR *szNewDirectory, BOOL bFailIfExist) {
    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    TCHAR szFindDirectory[MAX_PATH];
    //
    _stprintf_s(szFindDirectory, _T("%s\\*.*"), szExistDirectory);
    hFind = FindFirstFile(szFindDirectory, &wfd);
    if(INVALID_HANDLE_VALUE == hFind) return FALSE;
    //
    CreateDirectory(szNewDirectory, NULL);
    do {
        TCHAR destFilePath[MAX_PATH];
        _tcscpy_s(destFilePath, szNewDirectory);
        if(destFilePath[_tcslen(destFilePath)] != _T('\\')) _tcscat_s(destFilePath, _T("\\"));
        _tcscat_s(destFilePath, wfd.cFileName);

        TCHAR foundFilePath[MAX_PATH];
        _tcscpy_s(foundFilePath, szExistDirectory);
        if(foundFilePath[_tcslen(foundFilePath)] != _T('\\')) _tcscat_s(foundFilePath, _T("\\"));
        _tcscat_s(foundFilePath, wfd.cFileName);
		//
        if(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes) {
            if(_tcscmp(wfd.cFileName,_T(".")) && _tcscmp(wfd.cFileName,_T(".."))) {
                CopyDirectory(foundFilePath, destFilePath, bFailIfExist);
            }
        } else {
            CopyFile(foundFilePath, destFilePath, bFailIfExist);
        }
    } while(FindNextFile(hFind, &wfd));
    FindClose(hFind);
    //
    return TRUE;
}

BOOL NDireUtility::DeleDirectoryEx(CString &csDirectory) {  // 如删除 DeleDirectory("c:\\aaa")
    CFileFind FileFinder;
    CString csFindDirectory;
    //
    csFindDirectory = csDirectory + _T("\\*.*");
    BOOL bFound = (BOOL)FileFinder.FindFile(csFindDirectory);
    while(bFound) {
        bFound = (BOOL)FileFinder.FindNextFile();
        if(!FileFinder.IsDots()) {
            if(FileFinder.IsDirectory()) {
                CString foundDirectory;
				foundDirectory = csDirectory + _T("\\") + FileFinder.GetFileName();
                DeleDirectoryEx(foundDirectory);
            } else {
                CString foundFilePath;
				foundFilePath = csDirectory + _T("\\") + FileFinder.GetFileName();
                DeleteFile(foundFilePath);
            }
        }
    }
    FileFinder.Close();
    if(!RemoveDirectory(csDirectory)) {
// MessageBox(0,"删除目录失败!","警告信息",MB_OK);//比如没有找到文件夹,删除失败，可把此句删除
		if(ERROR_SHARING_VIOLATION==GetLastError() || ERROR_LOCK_VIOLATION==GetLastError()) {
			return FALSE;
		}
    }
    //
    return TRUE;
}

BOOL NDireUtility::DeleDirectory(TCHAR *szDirectory) {  // 如删除 DeleDirectory("c:\\aaa")
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	TCHAR szFindDirectory[MAX_PATH];
	//
	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szDirectory);
	hFind = FindFirstFile(szFindDirectory, &wfd);
	if(INVALID_HANDLE_VALUE == hFind) return FALSE;
	while(FindNextFile(hFind, &wfd)) {
		if(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes) {
			if(_tcscmp(wfd.cFileName,_T(".")) && _tcscmp(wfd.cFileName,_T(".."))) {
				TCHAR foundDirectory[MAX_PATH];
				_stprintf_s(foundDirectory, _T("%s\\%s"), szDirectory, wfd.cFileName);
				DeleDirectory(foundDirectory);
			}
		} else {
			TCHAR foundFilePath[MAX_PATH];
			_stprintf_s(foundFilePath, _T("%s\\%s"), szDirectory, wfd.cFileName);
			DeleteFile(foundFilePath);
		}
	}
	FindClose(hFind);
	if(!RemoveDirectory(szDirectory)) {
		// MessageBox(0,"删除目录失败!","警告信息",MB_OK);//比如没有找到文件夹,删除失败，可把此句删除
		if(ERROR_SHARING_VIOLATION==GetLastError() || ERROR_LOCK_VIOLATION==GetLastError()) {
			return FALSE;
		}
	}
	//
	return TRUE;
}

BOOL NDireUtility::FolderExistA(const char *szFolderPath) {
    WIN32_FIND_DATAA wfda;
    BOOL bFound = FALSE;
    //
    HANDLE hFind = FindFirstFileA(szFolderPath, &wfda);
    if ((INVALID_HANDLE_VALUE!=hFind) && (wfda.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) bFound = TRUE;
    //
    FindClose(hFind);
    return bFound;
}

BOOL NDireUtility::FolderExistW(const TCHAR *szFolderPath) {
    WIN32_FIND_DATA wfd;
    BOOL bFound = FALSE;
    //
    HANDLE hFind = FindFirstFile(szFolderPath, &wfd);
    if ((INVALID_HANDLE_VALUE!=hFind) && (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) bFound = TRUE;
    //
    FindClose(hFind);
    return bFound;
}

//  Returns: bool
//		true if csPath is a path to a directory
//		false otherwise.
bool NDireUtility::IsDirectory(const TCHAR *szFolderPath) {
    DWORD dwAttrib = GetFileAttributes( szFolderPath );
    return static_cast<bool>((INVALID_FILE_ATTRIBUTES!=dwAttrib)
                             && (FILE_ATTRIBUTE_DIRECTORY&dwAttrib));
}

BOOL NDireUtility::CreateFolder(TCHAR *szFolderPath) {
    SECURITY_ATTRIBUTES attrib;
    attrib.bInheritHandle = FALSE;
    attrib.lpSecurityDescriptor = NULL;
    attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
    //上面定义的属性可以省略
    //直接使用return ::CreateDirectory(path, NULL);即可
    return ::CreateDirectory(szFolderPath, &attrib);
}

DWORD NDireUtility::GetDirectorySize(CString &csDirePath) {
    CString csFilePath;
    DWORD    dwDireSize = 0;
    csFilePath += csDirePath + _T("\\*.*");
    //
    CFileFind fileFind;
    BOOL bFind = fileFind.FindFile(csFilePath);
    while (bFind) {
        bFind = fileFind.FindNextFile();
        if (!fileFind.IsDots()) {
            CString csTempPath = fileFind.GetFilePath();
            if (!fileFind.IsDirectory()) {
                dwDireSize += (DWORD)fileFind.GetLength();
            } else {
                dwDireSize += GetDirectorySize(csTempPath);
            }
        }
    }
    fileFind.Close();
    return dwDireSize;
}

BOOL NDireUtility::MakeFolderExist(LPCTSTR szDirePath) {  // szDirePath为要创建的目录，如“C:\dir1\dir2”
    CString csDirePath = szDirePath;
    //
    int iToken = 0;
    while((iToken = csDirePath.Find('\\', iToken+1)) != -1)
        CreateDirectory(csDirePath.Left(iToken), NULL);
    //
    return CreateDirectory(csDirePath, NULL);
}

BOOL NDireUtility::RecursMakeFolderW(LPCTSTR szDirePath) {
    TCHAR szParent[MAX_PATH];
    //
    if (FolderExistW(szDirePath) || CreateDirectory(szDirePath, NULL)) 
		return TRUE;
    GetFileFolderPathW(szParent, szDirePath);
    if (!RecursMakeFolderW(szParent) || !CreateDirectory(szDirePath, NULL))
        return FALSE;
    //
    return TRUE;
}

BOOL NDireUtility::RecursMakeFolderA(char *szDirePath) {
    char szParent[MAX_PATH];
    //
    if (FolderExistA(szDirePath) || CreateDirectoryA(szDirePath, NULL)) 
		return TRUE;
    GetFileFolderPathA(szParent, szDirePath);
    if (!RecursMakeFolderA(szParent) || !CreateDirectoryA(szDirePath, NULL))
        return FALSE;
    //
    return TRUE;
}

char *NDireUtility::GetFileFolderPathA(char *szDirePath, const char *szFileName) {
    char *toke = NULL;
    //
    if(!szFileName || !szDirePath) return NULL;
    mkzeo(szDirePath);
    strcpy_s(szDirePath, MAX_PATH, szFileName);
    toke = strrchr(szDirePath, _T('\\'));
    if(toke) *toke = _T('\0');
    //
    return szDirePath;
}

TCHAR *NDireUtility::GetFileFolderPathW(TCHAR *szDirePath, const TCHAR *szFileName) {
    TCHAR *toke = NULL;
    //
    if(!szFileName || !szDirePath) return NULL;
    MKZEO(szDirePath);
    _tcscpy_s(szDirePath, MAX_PATH, szFileName);
    toke = _tcsrchr(szDirePath, _T('\\'));
    if(toke) *toke = _T('\0');
    //
    return szDirePath;
}

BOOL NDireUtility::MakeFileFolder(const TCHAR *szFileName) {
    TCHAR szDirePath[MAX_PATH];
    //
    if(!szFileName) return FALSE;
    if(!GetFileFolderPathW(szDirePath, szFileName)) return FALSE;
    //
    // Fix windows API FolderExistW bug 20130226
    if(0x02 == _tcslen(szDirePath)) {
        if(IsDirectory(szDirePath)) return TRUE;
		else {
			return FALSE;
		}
    }
    //
    return RecursMakeFolderW(szDirePath);
}

DWORD NDireUtility::SetDirectoryAttrib(const TCHAR *szDriveLetter, TCHAR *szDirectory, bool IsSetToHidden) {
    TCHAR szPathName[MAX_PATH];
    //
    if(!szDriveLetter) return ((DWORD)-1);
    _stprintf_s(szPathName, _T("%s\\%s"), szDriveLetter, szDirectory);
    if(IsSetToHidden) {
        // FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY
        if(!SetFileAttributes(szPathName, FILE_ATTRIBUTE_READONLY))
            return ((DWORD)-1);
    } else {
        if(!SetFileAttributes(szPathName, FILE_ATTRIBUTE_NORMAL))
            return ((DWORD)-1);
    }
    //
    return 0x00;
}

DWORD NDireUtility::CreatAttribFolder(const TCHAR *szDriveLetter) {
    TCHAR szAttribFolder[MAX_PATH];
    //
    if(!szDriveLetter) return ((DWORD)-1);
    _stprintf_s(szAttribFolder, _T("%s\\%s"), szDriveLetter, _T(".sdclient.attrib"));
    //
    CreateDirectory(szAttribFolder, NULL);
    if(!SetFileAttributes(szAttribFolder, FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN))
        return ((DWORD)-1);
    //
    return 0x00;
}

BOOL NDireUtility::EmptyDirectory(TCHAR *szDirectory) {  // 如 EmptyDirectory("c:\\aaa")
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	TCHAR szFindDirectory[MAX_PATH];
	//
	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szDirectory);
	hFind = FindFirstFile(szFindDirectory, &wfd);
	if(INVALID_HANDLE_VALUE == hFind) return FALSE;
	while(FindNextFile(hFind, &wfd)) {
		if(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes) {
			if(_tcscmp(wfd.cFileName,_T(".")) && _tcscmp(wfd.cFileName,_T(".."))) {
				TCHAR foundDirectory[MAX_PATH];
				_stprintf_s(foundDirectory, _T("%s\\%s"), szDirectory, wfd.cFileName);
				DeleDirectory(foundDirectory);
			}
		} else {
			TCHAR foundFilePath[MAX_PATH];
			_stprintf_s(foundFilePath, _T("%s\\%s"), szDirectory, wfd.cFileName);
			DeleteFile(foundFilePath);
		}
	}
	FindClose(hFind);
    //
    return TRUE;
}

VOID NDireUtility::CCacheDirectory() {
    TCHAR szCacheDirectory[MAX_PATH];
    //
    if(GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH)) {
        _tcscat_s(szCacheDirectory, _T("\\sdclient\\cache"));
        // empty cache directory;
        NDireUtility::EmptyDirectory(szCacheDirectory);
    }
}