#include "stdafx.h"
#include <winioctl.h>
#include "Shlwapi.h"
#include <tchar.h>
#include "Shlwapi.h"
//#include "iostre"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utime.h>

// #include "third_party.h"
#include "StringUtility.h"
#include "FileUtility.h"

BOOL NFileUtility::FolderExists(const TCHAR *szFolderPath)
{
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
//		true if strPath is a path to a directory
//		false otherwise.
bool NFileUtility::IsDirectory(const TCHAR *szFolderPath)
{
	DWORD dwAttrib = GetFileAttributes( szFolderPath );
	return static_cast<bool>((dwAttrib!=0xffffffff)
		&& (dwAttrib&FILE_ATTRIBUTE_DIRECTORY));
}

BOOL NFileUtility::CreationTime(FILETIME *ftCreationTime, const TCHAR *szFolderPath)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	if(!szFolderPath || !ftCreationTime) return FALSE;
	hFind = FindFirstFile(szFolderPath, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		*ftCreationTime = fileInfo.ftCreationTime;
	}
	FindClose(hFind);
	//
	return TRUE;
}

/*
BOOL NFileUtility::FileExists(TCHAR *szFileName) {
CFileFind fFind;
return fFind.FindFile(szFileName);
}
*/
BOOL NFileUtility::FileExists(TCHAR *szFileName)
{
	WIN32_FIND_DATA wfd;
	BOOL bFound = FALSE;
	//
	HANDLE hFind = FindFirstFile(szFileName, &wfd);
	if (INVALID_HANDLE_VALUE!=hFind) bFound = TRUE;
	//
	FindClose(hFind);
	return bFound;
}

BOOL NFileUtility::CreateFolder(TCHAR *szFolderPath)
{
	SECURITY_ATTRIBUTES attrib;
	attrib.bInheritHandle = FALSE;
	attrib.lpSecurityDescriptor = NULL;
	attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	//上面定义的属性可以省略
	//直接使用return ::CreateDirectory(path, NULL);即可
	return ::CreateDirectory(szFolderPath, &attrib);
}

unsigned __int64 NFileUtility::FileLargeSize(TCHAR *szFileName)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	unsigned __int64 qwLorgeSzie = 0;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		qwLorgeSzie = fileInfo.nFileSizeLow;
		qwLorgeSzie += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
	}
	FindClose(hFind);
	return qwLorgeSzie;
}

//当然在CFileFind里面有GetLength()函数，也可以求得。
/*
DWORD NFileUtility::FileSize(CString &szFileName) {
WIN32_FIND_DATA fileInfo;
HANDLE hFind;
DWORD fileSize;
//
hFind = FindFirstFile(szFileName, &fileInfo);
if(INVALID_HANDLE_VALUE != hFind)
fileSize = fileInfo.nFileSizeLow;
//
FindClose(hFind);
return fileSize;
}
*/

unsigned __int64 NFileUtility::FileSize(const TCHAR *szFileName)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	unsigned __int64 qwFileSize = 0;
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

unsigned __int64 NFileUtility::FileSizeHandle(HANDLE hFile)
{
	DWORD dwFileSizeHigh;
	unsigned __int64 qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	qwFileSize += (((unsigned __int64)dwFileSizeHigh) <<32);
	return qwFileSize;
}

__int64 NFileUtility::FileSizeStream(FILE *file_stre)
{
	_fseeki64(file_stre, 0, SEEK_END);
	return _ftelli64(file_stre);
}

DWORD NFileUtility::FileSizeTime(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, TCHAR *szFileName)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		*qwFileSize = fileInfo.nFileSizeLow;
		*qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
		*ftLastWrite = fileInfo.ftLastWriteTime;
	} else return -1;
	//
	FindClose(hFind);
	return 0;
}

DWORD NFileUtility::FileSizeTimeExt(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, TCHAR *szFileName)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		*qwFileSize = fileInfo.nFileSizeLow;
		*qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
		*ftLastWrite = fileInfo.ftLastWriteTime;
	} else return -1;
	//
	FindClose(hFind);
	return 0;
}

DWORD NFileUtility::FileLastWrite(FILETIME *ftLastWrite, TCHAR *szFileName)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) *ftLastWrite = fileInfo.ftLastWriteTime;
	else return -1;
	//
	FindClose(hFind);
	return 0;
}

DWORD NFileUtility::GetDirectorySize(CString &szDirectoryPath)
{
	CString csFilePath;
	DWORD    dwDirectorySize = 0;
	//
	csFilePath += szDirectoryPath;
	csFilePath += "\\*.*";
	//
	CFileFind finder;
	BOOL bFind = finder.FindFile(csFilePath);
	while (bFind) {
		bFind = finder.FindNextFile();
		if (!finder.IsDots()) {
			CString sTempPath = finder.GetFilePath();
			if (!finder.IsDirectory()) {
				dwDirectorySize += (DWORD)finder.GetLength();
			} else {
				dwDirectorySize += GetDirectorySize(sTempPath);
			}
		}
	}
	finder.Close();
	return dwDirectorySize;
}

TCHAR *NFileUtility::ModuleBaseName()
{
	static TCHAR szDirectoryPath[MAX_PATH];
	//
	if('\0' == szDirectoryPath[0]) {
		if (!GetModuleFileName(NULL, szDirectoryPath, MAX_PATH))
			return NULL;
		if (!PathRemoveFileSpec(szDirectoryPath))
			return NULL;
	}
	//
	return szDirectoryPath;
}

TCHAR *NFileUtility::ModuleBaseName(TCHAR *lpFilePath)
{
	TCHAR szDirectoryPath[MAX_PATH];
	//
	if(!lpFilePath) return NULL;
	//
	if (!GetModuleFileName(NULL, szDirectoryPath, MAX_PATH)) return NULL;
	if (!PathRemoveFileSpec(szDirectoryPath)) return NULL;
	//
	//_tcscpy_s(lpFilePath, pFileNPath);
	_tcscpy_s(lpFilePath, wcslen(szDirectoryPath)+1, szDirectoryPath);
	//
	return lpFilePath;
}

TCHAR *NFileUtility::SystemDriverPath(TCHAR *lpFilePath)
{
	TCHAR szDirectoryPath[MAX_PATH];
	//
	if(!lpFilePath) return NULL;
	//
	if(!GetEnvironmentVariable(_T("SystemRoot"), szDirectoryPath, MAX_PATH)) return NULL;
	_tcscat_s(szDirectoryPath, _T("\\system32\\drivers"));
	//
	//_tcscpy_s(lpFilePath, pFileNPath);
	_tcscpy_s(lpFilePath, wcslen(szDirectoryPath)+1, szDirectoryPath);
	//
	return lpFilePath;
}

BOOL NFileUtility::MakeFolderExists(LPCTSTR szDirectoryPath)   // szDirectoryPath为要创建的目录，如“C:\dir1\dir2”
{
	CString csDirectoryPath = szDirectoryPath;
	//
	int iToken = 0;
	while((iToken = csDirectoryPath.Find('\\', iToken+1)) != -1)
		CreateDirectory(csDirectoryPath.Left(iToken), NULL);
	//
	return CreateDirectory(csDirectoryPath, NULL);
}

BOOL NFileUtility::RecuMakeFolder(LPCTSTR szDirectoryPath)
{
	TCHAR szParent[MAX_PATH];
	//
	if (FolderExists(szDirectoryPath) || CreateDirectory(szDirectoryPath, NULL)) return TRUE;
	GetFileFolderPath(szParent, szDirectoryPath);
	if (!RecuMakeFolder(szParent) || !CreateDirectory(szDirectoryPath, NULL))
		return FALSE;
	//
	return TRUE;
}

TCHAR *NFileUtility::GetFileFolderPath(TCHAR *szDirectoryPath, const TCHAR *szFileName)
{
	TCHAR *toke = NULL;
	//
	if(!szFileName || !szDirectoryPath) return NULL;
	MKZERO(szDirectoryPath);
	_tcscpy_s(szDirectoryPath, MAX_PATH, szFileName);
	toke = _tcsrchr(szDirectoryPath, _T('\\'));
	if(toke) *toke = _T('\0');
	//
	return szDirectoryPath;
}

BOOL NFileUtility::MakeFileFolder(const TCHAR *szFileName)
{
	TCHAR szDirectoryPath[MAX_PATH];
	//
	if(!szFileName) return FALSE;
	if(!GetFileFolderPath(szDirectoryPath, szFileName)) return FALSE;
	//
	// Fix windows API FolderExists bug 20130226
	if(0x02 == _tcslen(szDirectoryPath)) {
		if(IsDirectory(szDirectoryPath)) return TRUE;
		else return FALSE;
	}
	//
	return RecuMakeFolder(szDirectoryPath);
}

#define BUFF_SIZE 0x2000
#define BUILD_CHUNK_SIZE						((int)1 << 22) // 4M
HANDLE NFileUtility::CreatBackupChunk(TCHAR *szBackupChunk, TCHAR *szFromFile, size_t offset)
{
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	HANDLE hChunkBackup, hFromFile;
	DWORD dwWritenSize = 0, dwReadSize = 0;
	char szBuffer [BUFF_SIZE];
	//
	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
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
		TRACE( _T("create file failed: %d"), GetLastError() );
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
		TRACE( _T("create file failed: %d"), GetLastError() );
		return INVALID_HANDLE_VALUE;
	}
	DWORD dwResult = SetFilePointer(hFromFile, offset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
		return INVALID_HANDLE_VALUE;
	//
	DWORD backup_tally = 0;
	while(ReadFile(hFromFile, szBuffer, BUFF_SIZE, &dwReadSize, NULL) && 0 < dwReadSize) {
		WriteFile(hChunkBackup, szBuffer, dwReadSize, &dwWritenSize, NULL);
		if(dwReadSize != dwWritenSize) {
			TRACE(_T("fatal write error: %d"), GetLastError());
			return INVALID_HANDLE_VALUE;
		}
		backup_tally += dwWritenSize;
		if(BUILD_CHUNK_SIZE ==backup_tally) break;
	}
	//
	CloseHandle(hFromFile);
	return hChunkBackup;
}

HANDLE NFileUtility::CreatBackupFile(TCHAR *szBackupFile, TCHAR *szFromFile, size_t offset)
{
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	HANDLE hFileBackup, hFromFile;
	DWORD dwWritenSize = 0, dwReadSize = 0;
	char szBuffer [BUFF_SIZE];
	//
	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
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
		TRACE( _T("create file failed: %d"), GetLastError() );
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
		TRACE( _T("create file failed: %d"), GetLastError() );
		return INVALID_HANDLE_VALUE;
	}
	DWORD dwResult = SetFilePointer(hFromFile, offset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
		return INVALID_HANDLE_VALUE;
	//
	while(ReadFile(hFromFile, szBuffer, BUFF_SIZE, &dwReadSize, NULL) && 0 < dwReadSize) {
		WriteFile(hFileBackup, szBuffer, dwReadSize, &dwWritenSize, NULL);
		if(dwReadSize != dwWritenSize) {
			TRACE(_T("fatal write error: %d"), GetLastError());
			return INVALID_HANDLE_VALUE;
		}
	}
	//
	CloseHandle(hFromFile);
	return hFileBackup;
}

HANDLE NFileUtility::BuildChksFile(TCHAR *szChksFile)
{
	HANDLE hFileChks;
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	//
	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
	hFileChks = CreateFile( szFileName,
		GENERIC_WRITE | GENERIC_READ,
		NULL, /* FILE_SHARE_READ */
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileChks ) {
		TRACE( _T("create file failed: %d"), GetLastError() );
		return INVALID_HANDLE_VALUE;
	}
	if(szChksFile) _tcscpy_s(szChksFile, MAX_PATH, szFileName);
	//
	return hFileChks;
}

HANDLE NFileUtility::BuildVerifyFile(TCHAR *szVerifyFile)
{
	HANDLE hFileVerify;
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	//
	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
	hFileVerify = CreateFile( szFileName,
		GENERIC_WRITE | GENERIC_READ,
		NULL, /* FILE_SHARE_READ */
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileVerify ) {
		TRACE( _T("create file failed: %d"), GetLastError() );
		return INVALID_HANDLE_VALUE;
	}
	if(szVerifyFile) _tcscpy_s(szVerifyFile, MAX_PATH, szFileName);
	//
	return hFileVerify;
}

HANDLE NFileUtility::BuildComplexFile(TCHAR *szComplexFile)
{
	HANDLE hFileComplex;
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	//
	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
	hFileComplex = CreateFile( szFileName,
		GENERIC_WRITE | GENERIC_READ,
		NULL, /* FILE_SHARE_READ */
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileComplex ) {
		TRACE( _T("create file failed: %d"), GetLastError() );
		return INVALID_HANDLE_VALUE;
	}
	if(szComplexFile) _tcscpy_s(szComplexFile, MAX_PATH, szFileName);
	//
	return hFileComplex;
}

HANDLE NFileUtility::OpenLocalFile(TCHAR *szFileLocal)
{
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
		TRACE( _T("create file failed: %d"), GetLastError() );
		return INVALID_HANDLE_VALUE;
	}
	//
	return hFileLocal;
}

DWORD NFileUtility::CreatAttribFolder(const TCHAR *szDriveRoot)
{
	TCHAR szAttribFolder[MAX_PATH];
	//
	if(!szDriveRoot) return -1;
	_stprintf_s(szAttribFolder, _T("%s\\%s"), szDriveRoot, _T(".sdclient.attrib"));
	//
	CreateDirectory(szAttribFolder, NULL);
	if(!SetFileAttributes(szAttribFolder, FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN))
		return -1;
	//
	return 0;
}

DWORD NFileUtility::SetDirectoryAttrib(const TCHAR *szDriveRoot, TCHAR *szDirectory, bool IsSetToHidden)
{
	TCHAR szDirectoryName[MAX_PATH];
	//
	if(!szDriveRoot) return -1;
	_stprintf_s(szDirectoryName, _T("%s\\%s"), szDriveRoot, szDirectory);
	if(IsSetToHidden) {
		if(!SetFileAttributes(szDirectoryName, FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY))
			return -1;
	} else {
		if(!SetFileAttributes(szDirectoryName, FILE_ATTRIBUTE_NORMAL))
			return -1;
	}
	//
	return 0;
}

DWORD NFileUtility::IsFileLocked(const TCHAR *szFileName)   // -1:error 0:not lock 1:locked
{
	HANDLE hFileLocked;
	//
	if(!szFileName) return -1;
	hFileLocked = CreateFile( szFileName,
		/* GENERIC_WRITE | */ GENERIC_READ,
		FILE_SHARE_READ /* | FILE_SHARE_WRITE */,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileLocked ) {
		if(ERROR_SHARING_VIOLATION == GetLastError()) return 0x01;
		return -1;
	}
	//
	if(INVALID_HANDLE_VALUE != hFileLocked) {
		CloseHandle( hFileLocked );
		hFileLocked = INVALID_HANDLE_VALUE;
	}
	//
	return 0;
}

BOOL NFileUtility::EmptyDirectory(TCHAR *szDirectory)   // 如 EmptyDirectory("c:\\aaa")
{
	CFileFind FileFinder;
	TCHAR szFindDirectory[MAX_PATH];
	//
	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szDirectory);
	BOOL bFound = (BOOL)FileFinder.FindFile(szFindDirectory);
	while(bFound) {
		bFound = (BOOL)FileFinder.FindNextFile();
		if(!FileFinder.IsDots()) {
			TCHAR foundFileName[MAX_PATH];
			_tcscpy_s(foundFileName, FileFinder.GetFileName().GetBuffer(MAX_PATH));
			if(FileFinder.IsDirectory()) {
				TCHAR foundDirectory[MAX_PATH];
				_stprintf_s(foundDirectory, _T("%s\\%s"), szDirectory, foundFileName);
				DeleteDirectory(foundDirectory);
			} else {
				TCHAR foundFilePath[MAX_PATH];
				_stprintf_s(foundFilePath, _T("%s\\%s"), szDirectory, foundFileName);
				DeleteFile(foundFilePath);
			}
		}
	}
	FileFinder.Close();
	//
	return TRUE;
}

DWORD NFileUtility::CreateHugeFile(LPCTSTR szFileName, DWORD dwFileLength)
{
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
		TRACE( _T("create file failed: %d"), GetLastError() );
		return -1;
	}
	//
	// NTFS稀疏文件优化，FAT32下不起作用
	DWORD dwBytesReturned = 0;
	DeviceIoControl(hFileHuge, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
	//
	unsigned __int64 qwFileSize = ((unsigned __int64)dwFileLength) << 30;
	hMapFile = CreateFileMapping(hFileHuge, NULL, PAGE_READWRITE, (DWORD)(qwFileSize>>32)&0xffffffff, (DWORD)qwFileSize&0xffffffff, NULL);
	if(NULL == hMapFile) {
		TRACE( _T("create file mapping failed.") );
		CloseHandle( hFileHuge );
		return -1;
	}
	//
	CloseHandle( hMapFile );
	CloseHandle( hFileHuge );

	return 0;
}

DWORD NFileUtility::ShrinkHugeFile(LPCTSTR szFileName, DWORD dwFileLength)
{
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
		TRACE( _T("create file failed: %d"), GetLastError() );
		return -1;
	}
	//
	unsigned __int64 qwFileSize = ((unsigned __int64)dwFileLength) << 30;
	LONG lDistanceToMoveHigh = (qwFileSize>>32)&0xffffffff;
	DWORD dwResult = SetFilePointer(hFileHuge, (DWORD)qwFileSize&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
		return -1;
	dwResult = SetEndOfFile(hFileHuge);
	if(FALSE==dwResult && NO_ERROR!=GetLastError())
		return -1;
	CloseHandle( hFileHuge );
	//
	return 0;
}

DWORD NFileUtility::ExpandHugeFile(LPCTSTR szFileName, DWORD dwFileLength)
{
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
		TRACE( _T("create file failed: %d"), GetLastError() );
		return -1;
	}
	//
	unsigned __int64 qwFileSize = ((unsigned __int64)dwFileLength) << 30;
	hMapFile = CreateFileMapping(hFileHuge, NULL, PAGE_READWRITE, (DWORD)(qwFileSize>>32)&0xffffffff, (DWORD)qwFileSize&0xffffffff, NULL);
	if(NULL == hMapFile) {
		TRACE( _T("create file mapping failed.") );
		CloseHandle( hFileHuge );
		return -1;
	}
	//
	CloseHandle( hMapFile );
	CloseHandle( hFileHuge );
	//
	return 0;
}

DWORD NFileUtility::FtruncateHandle(HANDLE hFile, unsigned __int64 qwFileLength)
{
	LONG lDistanceToMoveHigh = (qwFileLength>>32)&0xffffffff;
	DWORD dwResult = SetFilePointer(hFile, (LONG)qwFileLength&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
		return -1;
	dwResult = SetEndOfFile(hFile);
	if(FALSE==dwResult && NO_ERROR!=GetLastError())
		return -1;
	//
	return 0;
}

DWORD NFileUtility::SetFileAttrib(const TCHAR *szFileName, unsigned __int64 qwFileLength, FILETIME *ftLastWrite)
{
	HANDLE hFileAttb = CreateFile( szFileName,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileAttb ) {
		TRACE( _T("create file failed: %d"), GetLastError() );
		return -1;
	}
	//
	LONG lDistanceToMoveHigh = (qwFileLength>>32)&0xffffffff;
	DWORD dwResult = SetFilePointer(hFileAttb, (LONG)qwFileLength&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
		return -1;
	dwResult = SetEndOfFile(hFileAttb);
	if(FALSE==dwResult && NO_ERROR!=GetLastError())
		return -1;
	SetFileTime(hFileAttb, NULL, NULL, ftLastWrite); // add by james 20110910
	//
	CloseHandle(hFileAttb);
	//
	return 0;
}

DWORD NFileUtility::SetFileLastWrite(const TCHAR *szFileName, FILETIME *ftLastWrite)
{
	HANDLE hFileLast = CreateFile( szFileName,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileLast ) {
		TRACE( _T("create file failed: %d"), GetLastError() );
		return -1;
	}
	SetFileTime(hFileLast, NULL, NULL, ftLastWrite); // add by james 20110910
	CloseHandle(hFileLast);
	//
	return 0;
}

DWORD NFileUtility::RandomCoupleID()
{
	srand((unsigned int) time(NULL));
	return rand();
}

HANDLE NFileUtility::BuildCacheFile(TCHAR *szFileName)
{
    static TCHAR szCacheDirectory[MAX_PATH];
	TCHAR szBuildFile[MAX_PATH];
    //
    if(_T(':') != szCacheDirectory[1]) {
        if(!GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH))
            return NULL;
        _tcscat_s(szCacheDirectory, _T("\\sdclient"));
        CreateDirectory(szCacheDirectory, NULL);
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
		TRACE( _T("create file failed: %d"), GetLastError() );
		return INVALID_HANDLE_VALUE;
	}
	//
	return hFileCache;
}

HANDLE NFileUtility::BuildCacheFile(TCHAR *szCacheFile, TCHAR *szFileName)
{
    static TCHAR szCacheDirectory[MAX_PATH];
	TCHAR szBuildFile[MAX_PATH];
    //
    if(_T(':') != szCacheDirectory[1]) {
        if(!GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH))
            return NULL;
        _tcscat_s(szCacheDirectory, _T("\\sdclient"));
        CreateDirectory(szCacheDirectory, NULL);
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
		TRACE( _T("create file failed: %d"), GetLastError() );
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
        strcat_s(cache_directory, "\\sdclient");
        CreateDirectoryA(cache_directory, NULL);
    }
    sprintf_s(build_file, MAX_PATH, "%s\\%s", cache_directory, file_name);
	if(cache_file) strcpy_s(cache_file, MAX_PATH, build_file);
	//
	return 0;
}

char *ntimeconv::filetime_ansi(char *timestr, FILETIME *filetime)   // Wed, 15 Nov 1995 04:58:08 GMT
{
	SYSTEMTIME st;
	if(!FileTimeToSystemTime(filetime, &st)) return NULL;
	struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
	strftime(timestr, TIME_LENGTH, "%a, %d %b %Y %H:%M:%S GMT", &gm);
	return timestr;
}

TCHAR *ntimeconv::filetime_unicode(TCHAR *timestr, FILETIME *filetime)   // Wed, 15 Nov 1995 04:58:08 GMT
{
	SYSTEMTIME st;
	if(!FileTimeToSystemTime(filetime, &st)) return NULL;
	struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
	_tcsftime(timestr, TIME_LENGTH, _T("%a, %d %b %Y %H:%M:%S GMT"), &gm);
	return timestr;
}

//Wed, 15 Nov 1995 04:58:08 GMT
DWORD ntimeconv::ansi_filetime(FILETIME *filetime, char *timestr)
{
	char *s = "/ :";
	char *toke, *previous;
	SYSTEMTIME st;
	//
	memset(&st, '\0', sizeof(SYSTEMTIME));
	//sscanf_s(timestr,"%2d-%2d-%4d %2d:%2d:%2d",&st.wMonth,&st.wDay,&st.wYear,&st.wHour,&st.wMinute,&st.wSecond);
	previous = timestr;
	int count = 0;
	do {
		toke = strpbrk(previous, s);
		if(toke) *toke = '\0';
		switch(count) {
		case 0:
			st.wMonth = atoi(previous);
			break;
		case 1:
			st.wDay = atoi(previous);
			break;
		case 2:
			st.wYear = atoi(previous);
			break;
		case 3:
			st.wHour = atoi(previous);
			break;
		case 4:
			st.wMinute = atoi(previous);
			break;
		case 5:
			st.wSecond = atoi(previous);
			break;
		}
		++count;
		if(toke) previous = toke + 1;
	} while(toke);
	if(!SystemTimeToFileTime(&st, filetime))
		return -1;
	//
	return 0;
}

//Wed, 15 Nov 1995 04:58:08 GMT
DWORD ntimeconv::unicode_filetime(FILETIME *filetime, TCHAR *timestr)
{
	TCHAR *s = _T("/ :");
	TCHAR *toke, *previous;
	SYSTEMTIME st;
	//
	memset(&st, '\0', sizeof(SYSTEMTIME));
	//_stscanf_s(timestr,_T("%2d-%2d-%4d %2d:%2d:%2d"),&st.wMonth,&st.wDay,&st.wYear,&st.wHour,&st.wMinute,&st.wSecond);
	previous = timestr;
	int count = 0;
	do {
		toke = _tcspbrk(previous, s);
		if(toke) toke[0] = _T('\0');
		switch(count) {
		case 0:
			st.wMonth = _ttoi(previous);
			break;
		case 1:
			st.wDay = _ttoi(previous);
			break;
		case 2:
			st.wYear = _ttoi(previous);
			break;
		case 3:
			st.wHour = _ttoi(previous);
			break;
		case 4:
			st.wMinute = _ttoi(previous);
			break;
		case 5:
			st.wSecond = _ttoi(previous);
			break;
		}
		++count;
		if(toke) previous = toke + 1;
	} while(toke);
	if(!SystemTimeToFileTime(&st, filetime))
		return -1;
	//
	return 0;
}

time_t ntimeconv::filetime_timet(const FILETIME &ft)
{
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

//int  ntimeconv::filetime_compare(FILETIME *filetime1, FILETIME *filetime2)
LONG ntimeconv::filetime_compare_ext(FILETIME *filetime1, FILETIME *filetime2)
{
	ULARGE_INTEGER ulFileTime1, ulFileTime2;
	ulFileTime1.LowPart = filetime1->dwLowDateTime;
	ulFileTime1.HighPart = filetime1->dwHighDateTime;
	ulFileTime2.LowPart = filetime2->dwLowDateTime;
	ulFileTime2.HighPart = filetime2->dwHighDateTime;
	return (LONG)((ulFileTime1.QuadPart - ulFileTime2.QuadPart) / 10000000);
}

BOOL CopyDirectory(TCHAR *szExistingDirectory, TCHAR *szNewDirectory, BOOL bFailIfExists)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	TCHAR szFindDirectory[MAX_PATH];
	//
	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szExistingDirectory);
	hFind = FindFirstFile(szFindDirectory, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind) return FALSE;
	//
	CreateDirectory(szNewDirectory, NULL);
	do {
		TCHAR newdst[MAX_PATH];
		_tcscpy_s(newdst, szNewDirectory);
		if(newdst[_tcslen(newdst)] != _T('\\')) _tcscat_s(newdst, _T("\\"));
		_tcscat_s(newdst, FindFileData.cFileName);

		TCHAR newsrc[MAX_PATH];
		_tcscpy_s(newsrc, szExistingDirectory);
		if(newsrc[_tcslen(newsrc)] != _T('\\')) _tcscat_s(newsrc, _T("\\"));
		_tcscat_s(newsrc, FindFileData.cFileName);
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(_tcscmp(FindFileData.cFileName,_T("."))!=0 && _tcscmp(FindFileData.cFileName,_T(".."))!=0) {
				CopyDirectory(newsrc, newdst, bFailIfExists);
			}
		} else {
			CopyFile(newsrc, newdst, bFailIfExists);
		}
	} while(FindNextFile(hFind, &FindFileData));
	FindClose(hFind);
	//
	return TRUE;
}

BOOL DeleteDirectory(TCHAR *szDirectory)   // 如删除 DeleteDirectory("c:\\aaa")
{
	CFileFind FileFinder;
	TCHAR szFindDirectory[MAX_PATH];
	//
	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szDirectory);
	BOOL bFound = (BOOL)FileFinder.FindFile(szFindDirectory);
	while(bFound) {
		bFound = (BOOL)FileFinder.FindNextFile();
		if(!FileFinder.IsDots()) {
			TCHAR foundFileName[MAX_PATH];
			_tcscpy_s(foundFileName, FileFinder.GetFileName().GetBuffer(MAX_PATH));
			if(FileFinder.IsDirectory()) {
				TCHAR foundDirectory[MAX_PATH];
				_stprintf_s(foundDirectory, _T("%s\\%s"), szDirectory, foundFileName);
				DeleteDirectory(foundDirectory);
			} else {
				TCHAR foundFilePath[MAX_PATH];
				_stprintf_s(foundFilePath, _T("%s\\%s"), szDirectory, foundFileName);
				DeleteFile(foundFilePath);
			}
		}
	}
	FileFinder.Close();
	if(!RemoveDirectory(szDirectory)) {
		//MessageBox(0,"删除目录失败!","警告信息",MB_OK);//比如没有找到文件夹,删除失败，可把此句删除
		return FALSE;
	}
	//
	return TRUE;
}
