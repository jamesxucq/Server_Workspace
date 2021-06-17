#include "stdafx.h"
#include <winioctl.h>
#include "Shlwapi.h"
#include <tchar.h>
#include "Shlwapi.h"
//#include "iostream"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utime.h>

#include "StringUtility.h"
#include "FileUtility.h"

BOOL NFileUtility::FolderExists(const TCHAR *szFolderPath) {
	WIN32_FIND_DATA wfd;
	BOOL Found = FALSE;

	HANDLE hFind = FindFirstFile(szFolderPath, &wfd);
	if ((hFind!=INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) Found = TRUE;

	FindClose(hFind);
	return Found;
}

//  Returns: bool
//		true if strPath is a path to a directory
//		false otherwise.
bool NFileUtility::IsDirectory(const TCHAR *szFolderPath) {
	DWORD dwAttrib = GetFileAttributes( szFolderPath );
	return static_cast<bool>((dwAttrib!=0xffffffff) 
		&& (dwAttrib&FILE_ATTRIBUTE_DIRECTORY));
}

BOOL NFileUtility::CreationTime(FILETIME *ftCreationTime, const TCHAR *szFolderPath) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;

	if(!szFolderPath || !ftCreationTime) return FALSE;
	hFind = FindFirstFile(szFolderPath, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) {
		*ftCreationTime = fileInfo.ftCreationTime;
	}
	FindClose(hFind);

	return TRUE;
}

/*
BOOL NFileUtility::FileExists(TCHAR *szFileName) {
CFileFind fFind;
return fFind.FindFile(szFileName);
}
*/
BOOL NFileUtility::FileExists(TCHAR *szFileName) {
	WIN32_FIND_DATA wfd;
	BOOL Found = FALSE;

	HANDLE hFind = FindFirstFile(szFileName, &wfd);
	if (hFind!=INVALID_HANDLE_VALUE) Found = TRUE;

	FindClose(hFind);
	return Found;
}

BOOL NFileUtility::CreateFolder(TCHAR *szFolderPath) {
	SECURITY_ATTRIBUTES attrib;
	attrib.bInheritHandle = FALSE;
	attrib.lpSecurityDescriptor = NULL;
	attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	//上面定义的属性可以省略
	//直接使用return ::CreateDirectory(path, NULL);即可
	return ::CreateDirectory(szFolderPath, &attrib);
}

unsigned __int64 NFileUtility::FileLargeSize(TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	unsigned __int64 nLorgeSzie;

	hFind = FindFirstFile(szFileName, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) {
		nLorgeSzie = fileInfo.nFileSizeHigh;
		nLorgeSzie <<= 32;
		nLorgeSzie += fileInfo.nFileSizeLow;
	}
	FindClose(hFind);
	return nLorgeSzie;
}

//当然在CFileFind里面有GetLength()函数，也可以求得。
/*
DWORD NFileUtility::FileSize(CString &szFileName) {
WIN32_FIND_DATA fileInfo;
HANDLE hFind;
DWORD fileSize;

hFind = FindFirstFile(szFileName, &fileInfo);
if(hFind != INVALID_HANDLE_VALUE)
fileSize = fileInfo.nFileSizeLow;

FindClose(hFind);
return fileSize;
}
*/

DWORD NFileUtility::FileSize(const TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	DWORD fileSize;

	hFind = FindFirstFile(szFileName, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE)
		fileSize = fileInfo.nFileSizeLow;

	FindClose(hFind);
	return fileSize;
}

off_t NFileUtility::FileSizeDescrip(int file_descrip) {
	struct stat finfo;
	if (fstat(file_descrip, &finfo) < 0) return -1;
	return finfo.st_size;
}

/*
off_t NFileUtility::FileSizePoint(FILE *file_point) {
int file_descrip;
struct stat finfo;

if (!file_point) return -1;
if((file_descrip=_fileno(file_point)) == -1) return -1;
if (fstat(file_descrip, &finfo) < 0) return -1;

return finfo.st_size;
}
*/

off_t NFileUtility::FileSizePoint(FILE *file_point) {
	if (!file_point) return -1;

	fseek(file_point, 0, SEEK_END);
	return ftell(file_point); 
}

DWORD NFileUtility::FileSizeTime(off_t *dwFileSize, FILETIME *lastwrite, TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;

	if(!dwFileSize || !lastwrite || !szFileName) return -1;
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) {
		*dwFileSize = fileInfo.nFileSizeLow;
		*lastwrite = fileInfo.ftLastWriteTime;
	} else return -1;

	FindClose(hFind);
	return 0;
}

DWORD NFileUtility::FileSizeTimeExt(DWORD *dwFileSize, FILETIME *lastwrite, TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;

	if(!dwFileSize || !lastwrite || !szFileName) return -1;
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) {
		*dwFileSize = fileInfo.nFileSizeLow;
		*lastwrite = fileInfo.ftLastWriteTime;
	} else return -1;

	FindClose(hFind);
	return 0;
}

DWORD NFileUtility::FileLastWrite(FILETIME *ftLastWrite, TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;

	if(!ftLastWrite || !szFileName) return -1;
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) *ftLastWrite = fileInfo.ftLastWriteTime;
	else return -1;

	FindClose(hFind);
	return 0;
}

DWORD NFileUtility::GetDirectorySize(CString &szDirectoryPath) {
	CString csFilePath;
	DWORD    dwDirectorySize = 0;

	csFilePath += szDirectoryPath;
	csFilePath += "\\*.*";

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

TCHAR *NFileUtility::ModuleBaseName() {
	static TCHAR szDirectoryPath[MAX_PATH];

	if('\0' == szDirectoryPath[0]) {
		if (!GetModuleFileName(NULL, szDirectoryPath, MAX_PATH)) 
			return NULL;
		if (!PathRemoveFileSpec(szDirectoryPath)) 
			return NULL;
	}

	return szDirectoryPath;
}

TCHAR *NFileUtility::ModuleBaseName(LPWSTR lpFilePath) {
	TCHAR szDirectoryPath[MAX_PATH];

	if(!lpFilePath) return NULL;

	if (!GetModuleFileName(NULL, szDirectoryPath, MAX_PATH)) return NULL;
	if (!PathRemoveFileSpec(szDirectoryPath)) return NULL;

	//_tcscpy_s(lpFilePath, pFileNPath);
	_tcscpy_s(lpFilePath, wcslen(szDirectoryPath)+1, szDirectoryPath);

	return lpFilePath;
}

TCHAR *NFileUtility::SystemDriverPath(LPWSTR lpFilePath) {
	TCHAR szDirectoryPath[MAX_PATH];

	if(!lpFilePath) return NULL;

	if(!GetEnvironmentVariable(_T("SystemRoot"), szDirectoryPath, MAX_PATH)) return NULL;
	_tcscat_s(szDirectoryPath, _T("\\system32\\drivers"));

	//_tcscpy_s(lpFilePath, pFileNPath);
	_tcscpy_s(lpFilePath, wcslen(szDirectoryPath)+1, szDirectoryPath);

	return lpFilePath;
}

BOOL NFileUtility::MakeSureDirectoryPathExists(LPCTSTR lpszDirectoryPath) { // lpszDirectoryPath为要创建的目录，如“C:\dir1\dir2”
	CString csDirectoryPath = lpszDirectoryPath;

	int iPosition = 0;
	while((iPosition = csDirectoryPath.Find('\\', iPosition+1)) != -1)
		CreateDirectory(csDirectoryPath.Left(iPosition), NULL);

	return CreateDirectory(csDirectoryPath, NULL);
}

TCHAR *NFileUtility::GetFileDirectoryPath(TCHAR *szDirectoryPath, const TCHAR *szFileName) {
	// static TCHAR szDirectoryPath[MAX_PATH];
	TCHAR *pend = NULL;

	if(!szFileName || !szDirectoryPath) return NULL;

	memset(szDirectoryPath, '\0', MAX_PATH);
	_tcscpy_s(szDirectoryPath, MAX_PATH, szFileName);
	pend = _tcsrchr(szDirectoryPath, _T('\\'));
	if(pend) pend[0] = _T('\0');

	return szDirectoryPath;
}

BOOL NFileUtility::CreatFileDirectoryPath(const TCHAR *szFileName) {
	TCHAR szDirectoryPath[MAX_PATH];

	if(!szFileName) return FALSE;
	if(!GetFileDirectoryPath(szDirectoryPath, szFileName)) return FALSE;

	return MakeSureDirectoryPathExists(szDirectoryPath);
}

#define BUF_SIZE 0x2000
#define BUILD_CHUNK_SIZE						((int)1 << 22) // 4M
HANDLE NFileUtility::CreatBackupChunk(TCHAR *szBackupChunk, TCHAR *szSourceFile, size_t offset) {
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	HANDLE hChunkBackup, hSourceFile;
	DWORD dwWritenSize = 0, dwReadSize = 0;
	char szBuffer [BUF_SIZE];

	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
	if(szBackupChunk) _tcscpy_s(szBackupChunk, MAX_PATH, szFileName);

	hChunkBackup = CreateFile( szFileName, 
		GENERIC_WRITE,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hChunkBackup == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return INVALID_HANDLE_VALUE;
	}

	hSourceFile = CreateFile( szSourceFile, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hSourceFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return INVALID_HANDLE_VALUE;
	}
	DWORD result = SetFilePointer(hSourceFile, offset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return INVALID_HANDLE_VALUE;

	DWORD dwBackupCount = 0;
	while(ReadFile(hSourceFile, szBuffer, BUF_SIZE, &dwReadSize, NULL) && dwReadSize>0) {
		WriteFile(hChunkBackup, szBuffer, dwReadSize, &dwWritenSize, NULL);
		if(dwReadSize != dwWritenSize) {
			TRACE(_T("fatal write error: %x!!!"), GetLastError());
			return INVALID_HANDLE_VALUE;		
		}
		dwBackupCount += dwWritenSize;
		if(BUILD_CHUNK_SIZE ==dwBackupCount) break;
	}

	CloseHandle(hSourceFile);
	return hChunkBackup;
}

HANDLE NFileUtility::CreatBackupFile(TCHAR *szBackupFile, TCHAR *szSourceFile, size_t offset) {
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	HANDLE hFileBackup, hSourceFile;
	DWORD dwWritenSize = 0, dwReadSize = 0;
	char szBuffer [BUF_SIZE];

	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
	if(szBackupFile) _tcscpy_s(szBackupFile, MAX_PATH, szFileName);

	hFileBackup = CreateFile( szFileName, 
		GENERIC_WRITE,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFileBackup == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return INVALID_HANDLE_VALUE;
	}

	hSourceFile = CreateFile( szSourceFile, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hSourceFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return INVALID_HANDLE_VALUE;
	}
	DWORD result = SetFilePointer(hSourceFile, offset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return INVALID_HANDLE_VALUE;

	while(ReadFile(hSourceFile, szBuffer, BUF_SIZE, &dwReadSize, NULL) && dwReadSize>0) {
		WriteFile(hFileBackup, szBuffer, dwReadSize, &dwWritenSize, NULL);
		if(dwReadSize != dwWritenSize) {
			TRACE(_T("fatal write error: %x!!!"), GetLastError());
			return INVALID_HANDLE_VALUE;		
		}
	}

	CloseHandle(hSourceFile);
	return hFileBackup;
}

HANDLE NFileUtility::BuildCsumsFile(OUT TCHAR *szCsumsFile) {
	HANDLE hFile;
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];

	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
	hFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return INVALID_HANDLE_VALUE;
	} 
	if(szCsumsFile) _tcscpy_s(szCsumsFile, MAX_PATH, szFileName);

	return hFile;
}

HANDLE NFileUtility::BuildVerifyFile(OUT TCHAR *szVerifyFile) {
	HANDLE hFile;
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];

	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
	hFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return INVALID_HANDLE_VALUE;
	} 
	if(szVerifyFile) _tcscpy_s(szVerifyFile, MAX_PATH, szFileName);

	return hFile;
}

HANDLE NFileUtility::BuildComplexFile(OUT TCHAR *szComplexFile) {
	HANDLE hFile;
	TCHAR szDirectory[MAX_PATH];
	TCHAR szFileName[MAX_PATH];

	GetTempPath(MAX_PATH, szDirectory);
	GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
	hFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return INVALID_HANDLE_VALUE;
	} 
	if(szComplexFile) _tcscpy_s(szComplexFile, MAX_PATH, szFileName);

	return hFile;
}

HANDLE NFileUtility::OpenLocalFile(OUT TCHAR *szFileLocal) {
	HANDLE hFile;

	if(!szFileLocal) return INVALID_HANDLE_VALUE;
	hFile = CreateFile( szFileLocal, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return INVALID_HANDLE_VALUE;
	} 

	return hFile;
}

DWORD NFileUtility::CreatAttribFolder(const TCHAR *szDriveRoot) {
	TCHAR szAttribFolder[MAX_PATH];

	if(!szDriveRoot) return -1;
	_stprintf_s(szAttribFolder, _T("%s\\%s"), szDriveRoot, _T(".sdclient.attrib"));

	CreateDirectory(szAttribFolder, NULL);
	if(!SetFileAttributes(szAttribFolder, FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN)) 
		return -1;

	return 0;
}

DWORD NFileUtility::SetDirectoryAttrib(const TCHAR *szDriveRoot, TCHAR *szDirectory, bool IsSetToHidden) {
	TCHAR szDirectoryName[MAX_PATH];

	if(!szDriveRoot) return -1;
	_stprintf_s(szDirectoryName, _T("%s\\%s"), szDriveRoot, szDirectory);

	if(IsSetToHidden) {
		if(!SetFileAttributes(szDirectoryName, FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY)) 
			return -1;
	} else {
		if(!SetFileAttributes(szDirectoryName, FILE_ATTRIBUTE_NORMAL)) 
			return -1;
	}

	return 0;
}

DWORD NFileUtility::IsFileLocked(const TCHAR *szFileName) { // -1:error 0:not lock 1:locked
	HANDLE hFileLocked;

	if(!szFileName) return -1;
	hFileLocked = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFileLocked == INVALID_HANDLE_VALUE ) {
		if(ERROR_SHARING_VIOLATION == GetLastError()) return 1;
		return -1;
	} 

	if(hFileLocked != INVALID_HANDLE_VALUE) {
		CloseHandle( hFileLocked );
		hFileLocked = INVALID_HANDLE_VALUE;
	}

	return 0;
}

BOOL NFileUtility::EmptyDirectory(TCHAR *szDirectory) { // 如 EmptyDirectory("c:\\aaa")
	CFileFind FileFinder;
	TCHAR szFindDirectory[MAX_PATH];

	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szDirectory);
	BOOL IsFound = (BOOL)FileFinder.FindFile(szFindDirectory);
	while(IsFound) {
		IsFound = (BOOL)FileFinder.FindNextFile();
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

	return TRUE;
}


/* Limits and constants. */
#define TYPE_FILE 0x01		/* Used in ls, rm, and lsFP */
#define TYPE_DIR 0x02
#define TYPE_DOT 0x04

static BOOL TraverseDirectory (FilesVec *, TCHAR *, TCHAR *);
static DWORD FileType (LPWIN32_FIND_DATA);
static BOOL ProcessItem (FilesVec *, TCHAR *, LPWIN32_FIND_DATA );

DWORD NFileUtility::GetFilesByDirectory(OUT FilesVec *pFilesVec, IN const TCHAR *szPath) {
	BOOL ok = TRUE;
	TCHAR szSearchName [MAX_PATH], szCurrentPath [MAX_PATH];
	TCHAR szPathString [MAX_PATH] = _T("");

	if(!pFilesVec || !szPath) return -1;
	if(!GetCurrentDirectory (MAX_PATH, szCurrentPath)) return -1;

	_stprintf_s(szSearchName, _T("%s\\"), szPath);
	if(!SetCurrentDirectory (szSearchName)) return -1;

	ok = TraverseDirectory (pFilesVec, _T ("*"), szPathString);
	if(!SetCurrentDirectory (szCurrentPath)) return -1;

	return ok ? 0: -1;
}


/*	Traverse a directory - Carrying out an implementation-specific
"action" for every name encountered. The action in this version is "Delete". */
/*	PathName: Relative or absolute pathname to traverse. */
static BOOL TraverseDirectory (FilesVec *pFilesVec, TCHAR *szPathName, TCHAR *szPathString) {
	HANDLE SearchHandle;
	WIN32_FIND_DATA FindData;
	DWORD dwFileType, dwPass;

	/*	Open up the directory search handle and get the
	first file name to satisfy the pathname.
	Make two passes. The first processes the files
	and the second processes the directories. */

	for (dwPass = 0; dwPass < 2; dwPass++) {
		SearchHandle = FindFirstFile (szPathName, &FindData);
		/* A deleted file will not be found on the second pass */
		if (SearchHandle == INVALID_HANDLE_VALUE) return TRUE;

		/* Scan the directory and its subdirectories
		for files satisfying the pattern. */
		do {
			/* For each file located, get the type. Delete files on pass 1.
			On pass 2, recursively process the subdirectory
			contents, if the recursive option is set. */

			dwFileType = FileType (&FindData); /* Delete a file. */
			if (!dwPass) ProcessItem (pFilesVec, szPathString, &FindData);

			/* Traverse the subdirectory on the second pass. */
			if (dwFileType&TYPE_DIR && dwPass) {
				//printf("----------------%s\n", szPathString);
				SetCurrentDirectory (FindData.cFileName);
				nstriutility::append_path(szPathString, FindData.cFileName);
				////////////////////////////////////////////////////////////
				TraverseDirectory (pFilesVec, _T ("*"), szPathString);
				////////////////////////////////////////////////////////////
				nstriutility::del_last_path(szPathString, FindData.cFileName);
				SetCurrentDirectory (_T (".."));
			}
			/* Get the next file or directory name. */
		} while (FindNextFile (SearchHandle, &FindData));
		FindClose (SearchHandle);
	}
	return TRUE;
}


/* Return file type from the find data structure.
Types supported:
TYPE_FILE:	If this is a file
TYPE_DIR:	If this is a directory other than . or ..
TYPE_DOT:	If this is . or .. directory */
static DWORD FileType (LPWIN32_FIND_DATA pFileData) {
	DWORD dwFileType = TYPE_FILE;
	if (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		if (!lstrcmp(pFileData->cFileName, _T (".")) || !lstrcmp(pFileData->cFileName, _T ("..")))
			dwFileType = TYPE_DOT;
		else dwFileType = TYPE_DIR;

		return dwFileType;
}


/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
static BOOL ProcessItem (FilesVec *pFilesVec, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData) {
	DWORD dwFileType;

	dwFileType = FileType (pFileData);
	if (!(dwFileType&TYPE_FILE) && !(dwFileType&TYPE_DIR)) return FALSE;

	if (dwFileType & TYPE_DIR){
		//_tprintf (_T ("send directory %s\n"), pFileData->szFileName);	
	} else if (dwFileType & TYPE_FILE){
		//_tprintf (_T ("send file %s\n"), pFileData->szFileName);
		NFilesVec::AddNewNode(pFilesVec, szPathString, pFileData);
	}

	return TRUE;
}

DWORD NFileUtility::CreateHugeFile(LPCTSTR szFileName, DWORD FileLength) {
	HANDLE hFile;
	HANDLE hMapFile; 

	hFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} 

	// NTFS稀疏文件优化，FAT32下不起作用  
	DWORD dwBytesReturned = 0; 
	DeviceIoControl(hFile, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dwBytesReturned, NULL); 

	unsigned __int64 i64FileSize = FileLength<<30;
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, (DWORD)(i64FileSize>>32)&0xffffffff, (DWORD)i64FileSize&0xffffffff, NULL);
	if(hMapFile == NULL) {
		TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		CloseHandle( hFile );
		return -1;
	} 

	CloseHandle( hMapFile );
	CloseHandle( hFile );

	return 0;
}

DWORD NFileUtility::ShrinkHugeFile(LPCTSTR szFileName, DWORD FileLength) {
	HANDLE hFile;

	hFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} 

	unsigned __int64 i64FileSize = FileLength<<30;
	LONG lDistanceToMoveHigh = (i64FileSize>>32)&0xffffffff;
	DWORD result = SetFilePointer(hFile, (DWORD)i64FileSize&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	result = SetEndOfFile(hFile);
	if(FALSE==result && NO_ERROR!=GetLastError()) 
		return -1;
	CloseHandle( hFile );

	return 0;

}

DWORD NFileUtility::ExpandHugeFile(LPCTSTR szFileName, DWORD FileLength) {
	HANDLE hFile;
	HANDLE hMapFile; 

	hFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} 

	unsigned __int64 i64FileSize = FileLength<<30;
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, (DWORD)(i64FileSize>>32)&0xffffffff, (DWORD)i64FileSize&0xffffffff, NULL);
	if(hMapFile == NULL) {
		TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		CloseHandle( hFile );
		return -1;
	} 

	CloseHandle( hMapFile );
	CloseHandle( hFile );

	return 0;
}

DWORD NFileUtility::FtruncateHandle(HANDLE hFile, off_t FileLength) {
	DWORD result = SetFilePointer(hFile, FileLength, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	result = SetEndOfFile(hFile);
	if(FALSE==result && NO_ERROR!=GetLastError()) 
		return -1;

	return 0;
}

DWORD NFileUtility::SetFileAttrib(const TCHAR *szFileName, off_t FileLength, FILETIME *ftLastWrite) {
	HANDLE hFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		return -1;
	}

	DWORD result = SetFilePointer(hFile, FileLength, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	result = SetEndOfFile(hFile);
	if(FALSE==result && NO_ERROR!=GetLastError()) 
		return -1;
	SetFileTime(hFile, NULL, NULL, ftLastWrite); // add by james 20110910

	CloseHandle(hFile);

	return 0;
}

DWORD NFileUtility::SetFileLastWrite(const TCHAR *szFileName, FILETIME *ftLastWrite) {
	HANDLE hFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		return -1;
	}
	SetFileTime(hFile, NULL, NULL, ftLastWrite); // add by james 20110910
	CloseHandle(hFile);

	return 0;
}

DWORD NFileUtility::RandomCoupleID() {
	srand((unsigned int) time(NULL));
	return rand();
}

char *ntimeconv::filetime_ansi(char *timestr, FILETIME *filetime) { // Wed, 15 Nov 1995 04:58:08 GMT
	SYSTEMTIME st;

	if(!timestr || !filetime) return NULL;

	if(!FileTimeToSystemTime(filetime, &st)) return NULL;
	struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
	strftime(timestr, TIME_LENGTH, "%a, %d %b %Y %H:%M:%S GMT", &gm);

	return timestr;
}

TCHAR *ntimeconv::filetime_unicode(TCHAR *timestr, FILETIME *filetime) { // Wed, 15 Nov 1995 04:58:08 GMT
	SYSTEMTIME st;

	if(!timestr || !filetime) return NULL;

	if(!FileTimeToSystemTime(filetime, &st)) return NULL;
	struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
	_tcsftime(timestr, TIME_LENGTH, _T("%a, %d %b %Y %H:%M:%S GMT"), &gm);

	return timestr;
}

//Wed, 15 Nov 1995 04:58:08 GMT
DWORD ntimeconv::ansi_filetime(FILETIME *filetime, char *timestr) {
	char *s = "/ :";
	char *position, *previous;
	SYSTEMTIME st;

	if(!timestr || !filetime) return -1;
	memset(&st, '\0', sizeof(SYSTEMTIME));
	//sscanf_s(timestr,"%2d-%2d-%4d %2d:%2d:%2d",&st.wMonth,&st.wDay,&st.wYear,&st.wHour,&st.wMinute,&st.wSecond);
	previous = timestr; int count = 0;
	do {
		position = strpbrk(previous, s);
		if(position) *position = '\0';
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
		if(position) previous = position + 1;
	} while(position);
	if(!SystemTimeToFileTime(&st, filetime)) 
		return -1; 

	return 0;
}

//Wed, 15 Nov 1995 04:58:08 GMT
DWORD ntimeconv::unicode_filetime(FILETIME *filetime, TCHAR *timestr) {
	TCHAR *s = _T("/ :");
	TCHAR *position, *previous;
	SYSTEMTIME st;

	if(!timestr || !filetime) return -1;
	memset(&st, '\0', sizeof(SYSTEMTIME));
	//_stscanf_s(timestr,_T("%2d-%2d-%4d %2d:%2d:%2d"),&st.wMonth,&st.wDay,&st.wYear,&st.wHour,&st.wMinute,&st.wSecond);
	previous = timestr; int count = 0;
	do {
		position = _tcspbrk(previous, s);
		if(position) position[0] = _T('\0');
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
		if(position) previous = position + 1;
	} while(position);
	if(!SystemTimeToFileTime(&st, filetime)) 
		return -1; 

	return 0;
}

time_t ntimeconv::filetime_timet(const FILETIME &ft) {  
	ULARGE_INTEGER ui;  
	ui.LowPart = ft.dwLowDateTime;  
	ui.HighPart = ft.dwHighDateTime;  
	return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);  
}   

//int  ntimeconv::filetime_compare(FILETIME *filetime1, FILETIME *filetime2)
LONG ntimeconv::filetime_compare_ext(FILETIME *filetime1, FILETIME *filetime2) {
	ULARGE_INTEGER ulFileTime1, ulFileTime2;
	ulFileTime1.LowPart = filetime1->dwLowDateTime; ulFileTime1.HighPart = filetime1->dwHighDateTime;
	ulFileTime2.LowPart = filetime2->dwLowDateTime; ulFileTime2.HighPart = filetime2->dwHighDateTime;
	return (LONG)((ulFileTime1.QuadPart - ulFileTime2.QuadPart) / 10000000);
}

static BOOL CopyDirectory(TCHAR *szExistingDirectory, TCHAR *szNewDirectory, BOOL bFailIfExists) {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	TCHAR szFindDirectory[MAX_PATH];

	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szExistingDirectory);
	hFind = FindFirstFile(szFindDirectory, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE) return FALSE;

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
	}while(FindNextFile(hFind, &FindFileData));
	FindClose(hFind); 

	return TRUE;
}

static BOOL DeleteDirectory(TCHAR *szDirectory) { // 如删除 DeleteDirectory("c:\\aaa")
	CFileFind FileFinder;
	TCHAR szFindDirectory[MAX_PATH];

	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szDirectory);
	BOOL IsFound = (BOOL)FileFinder.FindFile(szFindDirectory);
	while(IsFound) {
		IsFound = (BOOL)FileFinder.FindNextFile();
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

	return TRUE;
}
