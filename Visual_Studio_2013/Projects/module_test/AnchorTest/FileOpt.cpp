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

#include "StringOpt.h"
#include "FileOpt.h"

CFileOpt::CFileOpt(void)
{
}

CFileOpt::~CFileOpt(void)
{
}

BOOL CFileOpt::CheckFolderExist(wchar_t *sFolderPath)
{
	WIN32_FIND_DATA wfd;
	BOOL rValue = FALSE;
	HANDLE hFind = FindFirstFile(sFolderPath, &wfd);
	if ((hFind!=INVALID_HANDLE_VALUE) &&
		(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = TRUE;
	}
	FindClose(hFind);
	return rValue;
}


BOOL CFileOpt::CheckFileExist(wchar_t *sFileName)
{
	CFileFind fFind;
	return fFind.FindFile(sFileName);
}


BOOL CFileOpt::CreateFolder(wchar_t *sFolderPath)
{
	SECURITY_ATTRIBUTES attrib;
	attrib.bInheritHandle = FALSE;
	attrib.lpSecurityDescriptor = NULL;
	attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	//上面定义的属性可以省略
	//直接使用return ::CreateDirectory(path, NULL);即可
	return ::CreateDirectory(sFolderPath, &attrib);
}

unsigned __int64 CFileOpt::FileLargeSize(wchar_t *filepath)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	unsigned __int64 iLorgeSzie;

	hFind = FindFirstFile(filepath, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) {
		iLorgeSzie = fileInfo.nFileSizeHigh;
		iLorgeSzie <<= 32;
		iLorgeSzie += fileInfo.nFileSizeLow;
	}
	FindClose(hFind);
	return iLorgeSzie;
}

//当然在CFileFind里面有GetLength()函数，也可以求得。
/*
DWORD CFileOpt::FileSizeN(CString &filepath)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	DWORD fileSize;

	hFind = FindFirstFile(filepath, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE)
		fileSize = fileInfo.nFileSizeLow;

	FindClose(hFind);
	return fileSize;
}
*/

DWORD CFileOpt::FileSizeN(wchar_t *filepath)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	DWORD fileSize;

	hFind = FindFirstFile(filepath, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE)
		fileSize = fileInfo.nFileSizeLow;

	FindClose(hFind);
	return fileSize;
}

off_t CFileOpt::FileSizeD(int fd)
{
	struct stat finfo;
	if (fstat(fd, &finfo) < 0) return -1;
	return finfo.st_size;
}

/*
off_t CFileOpt::FileSizeP(FILE *fp)
{
	int fd;
	struct stat finfo;

	if (!fp) return -1;
	if((fd=_fileno(fp)) == -1) return -1;
	if (fstat(fd, &finfo) < 0) return -1;

	return finfo.st_size;
}
*/

off_t CFileOpt::FileSizeP(FILE *fp)
{
	if (!fp) return -1;
 
	fseek(fp, 0, SEEK_END);
	return ftell(fp); 
}

int CFileOpt::FileSizeTimeN(off_t *filesize, FILETIME *lastwrite, wchar_t *filename)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;

	if(!filesize || !lastwrite || !filename) return -1;
	hFind = FindFirstFile(filename, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) {
		*filesize = fileInfo.nFileSizeLow;
		*lastwrite = fileInfo.ftLastWriteTime;
	}
	else return -1;

	FindClose(hFind);
	return 0;
}

DWORD CFileOpt::GetDirSize(CString &csDirPath)
{
	CString csFilePath;
	DWORD    dwDirSize = 0;

	csFilePath += csDirPath;
	csFilePath += "\\*.*";

	CFileFind finder;
	BOOL bFind = finder.FindFile(csFilePath);
	while (bFind) {
		bFind = finder.FindNextFile();
		if (!finder.IsDots()) {
			CString csTempPath = finder.GetFilePath();
			if (!finder.IsDirectory()) {
				dwDirSize += (DWORD)finder.GetLength();
			} else {
				dwDirSize += GetDirSize(csTempPath);
			}
		}
	}
	finder.Close();
	return dwDirSize;
}

CString CFileOpt::ModuleBaseName()
{   
	CString csFilePath;

	GetModuleFileName(NULL, csFilePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	csFilePath.ReleaseBuffer();

	int iPosIndex;
	iPosIndex = csFilePath.ReverseFind('\\');
	csFilePath = csFilePath.Left(iPosIndex);

	return csFilePath;
}

bool CFileOpt::ModuleBaseName(LPWSTR lpExePath) 
{   
	wchar_t sFileNPath[MAX_PATH];
	BOOL boolReturn = FALSE;

	boolReturn = GetModuleFileName(NULL, sFileNPath, MAX_PATH);
	if (!boolReturn) return false;
	boolReturn = PathRemoveFileSpec(sFileNPath);
	if (!boolReturn) return false;

	//_tcscpy(pExePath, pFileNPath);
	wcscpy_s(lpExePath, wcslen(sFileNPath)+1, sFileNPath);
	
	return true;
}

BOOL CFileOpt::MakeSureDirPathExists(LPCTSTR lpszDirPath)// lpszDirPath为要创建的目录，如“C:\dir1\dir2”

{       
	CString csDirPath = lpszDirPath;

	int nPos = 0;
	while((nPos = csDirPath.Find('\\', nPos+1)) != -1)
		CreateDirectory(csDirPath.Left(nPos), NULL);

	return CreateDirectory(csDirPath, NULL);
}

wchar_t *CFileOpt::GetFileDirPath(const wchar_t *sFileName)
{
	static wchar_t sDirPath[MAX_PATH];
	wchar_t *pend = NULL;

	if(!sFileName) return NULL;

	memset(sDirPath, '\0', MAX_PATH);
	_tcscpy_s(sDirPath, sFileName);
	pend = _tcsrchr(sDirPath, _T('\\'));
	*pend = _T('\0');

	return sDirPath;
}

BOOL CFileOpt::CreatFileDirPath(const wchar_t *sFileName)
{
	wchar_t sDirPath[MAX_PATH];

	if(!sFileName) return FALSE;

	memset(sDirPath, '\0', MAX_PATH);
	_tcscpy_s(sDirPath, GetFileDirPath(sFileName));

	return MakeSureDirPathExists(sDirPath);
}

HANDLE CFileOpt::CreatBackupFile(wchar_t *BackupFile, wchar_t *SourceFile)
{
	HANDLE hFileBackup;
	wchar_t szDirName[MAX_PATH];
	wchar_t sFileName[MAX_PATH];

	GetTempPath(MAX_PATH, szDirName);
	GetTempFileName(szDirName, _T("sdisk"), 0, sFileName);
	if(BackupFile) _tcscpy_s(BackupFile, MAX_PATH, sFileName);

	if(!CopyFile(SourceFile, sFileName, FALSE))
		return INVALID_HANDLE_VALUE;
    hFileBackup = CreateFile( sFileName, 
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ, 
						NULL, 
						CREATE_ALWAYS, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL);
    if( hFileBackup == INVALID_HANDLE_VALUE ) {
        TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return INVALID_HANDLE_VALUE;
    }

	return hFileBackup;
}

HANDLE CFileOpt::BuildTempFile()
{
	HANDLE hFile;
	wchar_t szDirName[MAX_PATH];
	wchar_t sFileName[MAX_PATH];

	GetTempPath(MAX_PATH, szDirName);
	GetTempFileName(szDirName, _T("sdisk"), 0, sFileName);
	//if(_tfopen_s(&fpt, sFileName, _T("wb+"))) return NULL;
	//fseek(fpt, 0, SEEK_SET);
    hFile = CreateFile( sFileName, 
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ, 
						NULL, 
						CREATE_ALWAYS, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL);
    if( hFile == INVALID_HANDLE_VALUE ) {
        TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return INVALID_HANDLE_VALUE;
    } 

	return hFile;
}

bool CFileOpt::CreatCacheFolder(wchar_t *sDriveRoot)
{
	wchar_t sCacheFolder[MAX_PATH];

	if(!sDriveRoot) return false;
	_stprintf_s(sCacheFolder, _T("%s\\%s"), sDriveRoot, _T(".sdisk.cache"));

	CreateDirectory(sCacheFolder, NULL);
	if(!SetFileAttributes(sCacheFolder, FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN)) 
		return false;

	return true;
}

bool CFileOpt::SetDirectoryAttrib(wchar_t *sDriveRoot, wchar_t *sDirectory, bool IsSetToHidden)
{
	wchar_t sDirectoryName[MAX_PATH];

	if(!sDriveRoot) return false;
	_stprintf_s(sDirectoryName, _T("%s\\%s"), sDriveRoot, sDirectory);

	if(IsSetToHidden) {
		if(!SetFileAttributes(sDirectoryName, FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY)) 
			return false;
	} else {
		if(!SetFileAttributes(sDirectoryName, FILE_ATTRIBUTE_NORMAL)) 
			return false;
	}

	return true;
}


/* Limits and constants. */
#define TYPE_FILE 1		/* Used in ls, rm, and lsFP */
#define TYPE_DIR 2
#define TYPE_DOT 3

static BOOL TraverseDirectory (FilesAttribVec *, TCHAR *, TCHAR *);
DWORD FileType (LPWIN32_FIND_DATA);
static BOOL ProcessItem (FilesAttribVec *, wchar_t *, LPWIN32_FIND_DATA );
wchar_t sDriveRoot[MAX_PATH];

int CFileOpt::GetFilesVecByPath(OUT FilesAttribVec *vpFilesVec, IN wchar_t *sPath)
{
	BOOL ok = TRUE;
	wchar_t PathName [MAX_PATH], CuPath [MAX_PATH];
	wchar_t PathStr [MAX_PATH] = _T("");

	if(!vpFilesVec || !sPath) return -1;
	if(!GetCurrentDirectory (MAX_PATH, CuPath)) return -1;
	//_mywscpy_s (PathName, pDriveRoot);
	_tcscpy_s(sDriveRoot, sPath);
	_stprintf_s(PathName, _T("%s\\"), sPath);
	if(!SetCurrentDirectory (PathName)) return -1;

	ok = TraverseDirectory (vpFilesVec, _T ("*"), PathStr);
	if(!SetCurrentDirectory (CuPath)) return -1;

	return ok ? 0:-1;
}


/*	Traverse a directory - Carrying out an implementation-specific
	"action" for every name encountered. The action in this version is "Delete". */
/*	PathName: Relative or absolute pathname to traverse. */
static BOOL TraverseDirectory (FilesAttribVec *vpFilesVec, TCHAR *PathName, TCHAR *PathStr)
{
	HANDLE SearchHandle;
	WIN32_FIND_DATA FindData;
	DWORD FType, iPass;

	/*	Open up the directory search handle and get the
		first file name to satisfy the pathname.
		Make two passes. The first processes the files
		and the second processes the directories. */

	for (iPass = 1; iPass <= 2; iPass++)
	{
		SearchHandle = FindFirstFile (PathName, &FindData);
		if (SearchHandle == INVALID_HANDLE_VALUE)
			/* A deleted file will not be found on the second pass */
			return iPass == 2; 

		/* Scan the directory and its subdirectories
			for files satisfying the pattern. */
		do
		{
			/* For each file located, get the type. Delete files on pass 1.
			On pass 2, recursively process the subdirectory
			contents, if the recursive option is set. */

			FType = FileType (&FindData);
			if (iPass == 1) /* Delete a file. */
				ProcessItem (vpFilesVec, PathStr, &FindData);

			/* Traverse the subdirectory on the second pass. */
			if (FType == TYPE_DIR && iPass == 2)
			{
//printf("----------------%s\n", PathStr);
				SetCurrentDirectory (FindData.cFileName);
				CStringOpt::UAppendPath(PathStr, FindData.cFileName);

				TraverseDirectory (vpFilesVec, _T ("*"), PathStr);

				CStringOpt::UDelLastPath(PathStr, FindData.cFileName);
				SetCurrentDirectory (_T (".."));
			}
			/* Get the next file or directory name. */
		}
		while (FindNextFile (SearchHandle, &FindData));
		FindClose (SearchHandle);
	}
	return TRUE;
}


/* Return file type from the find data structure.
	Types supported:
		TYPE_FILE:	If this is a file
		TYPE_DIR:	If this is a directory other than . or ..
		TYPE_DOT:	If this is . or .. directory */
static DWORD FileType (LPWIN32_FIND_DATA pFileData)
{
	BOOL IsDir;
	DWORD FType;
	FType = TYPE_FILE;
	IsDir = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	if (IsDir)
		if (lstrcmp(pFileData->cFileName, _T ("."))==0 || lstrcmp(pFileData->cFileName, _T (".."))==0)
			FType = TYPE_DOT;
		else FType = TYPE_DIR;

	return FType;
}


/* Function to process (delete, in this case) the file or directory.
	Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
	a \ or the null string. sFileName has no wildcards at this point. */
static BOOL ProcessItem (FilesAttribVec *vpFilesVec, wchar_t *dirname, LPWIN32_FIND_DATA pFileData)
{
	DWORD FType;

	FType = FileType (pFileData);
	if (FType != TYPE_FILE && FType != TYPE_DIR) return FALSE;

	if (FType == TYPE_DIR){
//_tprintf (_T ("send directory %s\n"), pFileData->cFileName);	
		;
	}
	else if (FType == TYPE_FILE){
//_tprintf (_T ("send file %s\n"), pFileData->cFileName);
		CFilesVec::AddNewFilesNode(vpFilesVec, sDriveRoot, dirname, pFileData);
	}

	return TRUE;
}

bool CFileOpt::CreateHugeFile(LPCTSTR sFileName, DWORD FileLength)  
{
	HANDLE hFile;
    HANDLE hMapFile; 

    hFile = CreateFile( sFileName, 
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ, 
						NULL, 
						CREATE_ALWAYS, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL);
    if( hFile == INVALID_HANDLE_VALUE ) {
        TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return false;
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
        return false;
    } 

    CloseHandle( hMapFile );
    CloseHandle( hFile );

	return true;
}

bool CFileOpt::ShrinkHugeFile(LPCTSTR sFileName, DWORD FileLength)
{
	HANDLE hFile;

    hFile = CreateFile( sFileName, 
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ, 
						NULL, 
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL);
    if( hFile == INVALID_HANDLE_VALUE ) {
        TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return false;
    } 

	unsigned __int64 i64FileSize = FileLength<<30;
	LONG lDistanceToMoveHigh = (i64FileSize>>32)&0xffffffff;
	DWORD retValue = SetFilePointer(hFile, (DWORD)i64FileSize&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return false;
	retValue = SetEndOfFile(hFile);
	if(0x00==retValue && NO_ERROR!=GetLastError()) 
		return false;
    CloseHandle( hFile );

	return true;

}

bool CFileOpt::ExpandHugeFile(LPCTSTR sFileName, DWORD FileLength)
{
	HANDLE hFile;
    HANDLE hMapFile; 

    hFile = CreateFile( sFileName, 
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ, 
						NULL, 
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL);
    if( hFile == INVALID_HANDLE_VALUE ) {
        TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return false;
    } 

	unsigned __int64 i64FileSize = FileLength<<30;
    hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, (DWORD)(i64FileSize>>32)&0xffffffff, (DWORD)i64FileSize&0xffffffff, NULL);
    if(hMapFile == NULL) {
        TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        CloseHandle( hFile );
        return false;
    } 

    CloseHandle( hMapFile );
    CloseHandle( hFile );

	return true;
}


CTimeOpt::CTimeOpt(void)
{
}

CTimeOpt::~CTimeOpt(void)
{
}

#ifndef TIME_STRING_LEN
#define TIME_STRING_LEN 20
#endif

int CTimeOpt::FileTimeToStringA(char *sTime, FILETIME *filetime) // 2010-11-11 10:10:10
{
	SYSTEMTIME st;

	if(!sTime || !filetime) return -1;
	if(!FileTimeToSystemTime(filetime, &st)) return -1;
	//sprintf_s(sTime,TIME_STRING_LEN,"%4d-%2d-%2d %2d:%2d:%2d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	sprintf_s(sTime,TIME_STRING_LEN,"%d-%d-%d %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

	return 0;
}

wchar_t *CTimeOpt::FileTimeToStringW(FILETIME *filetime) // 11-11-2010 10:10:10
{
	static wchar_t sTime[TIME_STRING_LEN];
	SYSTEMTIME st;

	if(!filetime) return NULL;
	if(!FileTimeToSystemTime(filetime, &st)) return NULL;
	//_stprintf_s(sTime,TIME_STRING_LEN,_T("%4d-%2d-%2d %2d:%2d:%2d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	_stprintf_s(sTime,TIME_STRING_LEN,_T("%d-%d-%d %d:%d:%d"),st.wMonth,st.wDay,st.wYear,st.wHour,st.wMinute,st.wSecond);

	return sTime;
}

//11-11-2010 10:10:10
int CTimeOpt::StringToFileTimeA(FILETIME *filetime, char *sTime)
{
	char *s = "- :";
	char *pos, *prev;
	SYSTEMTIME st;

	if(!sTime || !filetime) return -1;

	memset(&st, '\0', sizeof(SYSTEMTIME));
	//sscanf_s(sTime,"%2d-%2d-%4d %2d:%2d:%2d",&st.wMonth,&st.wDay,&st.wYear,&st.wHour,&st.wMinute,&st.wSecond);
	prev = sTime; int count = 0;
	do {
		pos = strpbrk(prev, s);
		if(pos) *pos = '\0';
		switch(count) {
		case 0:
			st.wMonth = atoi(prev);
			break;
		case 1:
			st.wDay = atoi(prev);
			break;
		case 2:
			st.wYear = atoi(prev);
			break;
		case 3:
			st.wHour = atoi(prev);
			break;
		case 4:
			st.wMinute = atoi(prev);
			break;
		case 5:
			st.wSecond = atoi(prev);
			break;
		}
		++count;
		if(pos) prev = pos + 1;
	} while(pos);
	if(!SystemTimeToFileTime(&st, filetime)) 
		return -1; 

	return 0;
}

int CTimeOpt::StringToFileTimeW(FILETIME *filetime, wchar_t *sTime)
{
	wchar_t *s = _T("- :");
	wchar_t *pos, *prev;
	SYSTEMTIME st;


	if(!sTime || !filetime) return -1;

	memset(&st, '\0', sizeof(SYSTEMTIME));
	//_stscanf_s(sTime,_T("%2d-%2d-%4d %2d:%2d:%2d"),&st.wMonth,&st.wDay,&st.wYear,&st.wHour,&st.wMinute,&st.wSecond);
	prev = sTime; int count = 0;
	do {
		pos = _tcspbrk(prev, s);
		if(pos) *pos = _T('\0');
		switch(count) {
		case 0:
			st.wMonth = _ttoi(prev);
			break;
		case 1:
			st.wDay = _ttoi(prev);
			break;
		case 2:
			st.wYear = _ttoi(prev);
			break;
		case 3:
			st.wHour = _ttoi(prev);
			break;
		case 4:
			st.wMinute = _ttoi(prev);
			break;
		case 5:
			st.wSecond = _ttoi(prev);
			break;
		}
		++count;
		if(pos) prev = pos + 1;
	} while(pos);
	if(!SystemTimeToFileTime(&st, filetime)) 
		return -1; 

	return 0;
}

time_t CTimeOpt::FileTimeToTime_t(const FILETIME &ft)
{  
	ULARGE_INTEGER ui;  
	ui.LowPart = ft.dwLowDateTime;  
	ui.HighPart = ft.dwHighDateTime;  
	return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);  
}   

//int  CTimeOpt::FileTimeCompare(FILETIME *lpFileTime1, FILETIME *lpFileTime2)


static BOOL CopyDirectory(wchar_t *sExistingDirPath, wchar_t *sNewDirPath, BOOL bFailIfExists)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	wchar_t FindDirString[MAX_PATH];

	_stprintf_s(FindDirString, _T("%s\\*.*"), sExistingDirPath);
	hFind = FindFirstFile(FindDirString, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE) return FALSE;

	CreateDirectory(sNewDirPath, NULL);
	do {
		wchar_t newdst[MAX_PATH];
		_tcscpy_s(newdst, sNewDirPath);
		if(newdst[_tcslen(newdst)] != _T('\\')) _tcscat_s(newdst, _T("\\"));
		_tcscat_s(newdst, FindFileData.cFileName);

		wchar_t newsrc[MAX_PATH];
		_tcscpy_s(newsrc, sExistingDirPath);
		if(newsrc[_tcslen(newsrc)] != _T('\\')) _tcscat_s(newsrc, _T("\\"));
		_tcscat_s(newsrc, FindFileData.cFileName);
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(_tcscmp(FindFileData.cFileName,_T("."))!=0 && _tcscmp(FindFileData.cFileName,_T(".."))!=0)
			{
				CopyDirectory(newsrc, newdst, bFailIfExists);
			}
		}
		else
		{
			CopyFile(newsrc, newdst, bFailIfExists);
		}
	}while(FindNextFile(hFind, &FindFileData));
	FindClose(hFind); 

	return TRUE;
}

static BOOL DeleteDirectory(wchar_t *DirName) // 如删除 DeleteDirectory("c:\\aaa")
{
	CFileFind FileFinder;
    wchar_t FindDirString[MAX_PATH];

	_stprintf_s(FindDirString, _T("%s\\*.*"), DirName);
    BOOL IsFinded = (BOOL)FileFinder.FindFile(FindDirString);
    while(IsFinded)
    {
		IsFinded = (BOOL)FileFinder.FindNextFile();
		if(!FileFinder.IsDots())
        {
			wchar_t foundFileName[MAX_PATH];
            _tcscpy_s(foundFileName, FileFinder.GetFileName().GetBuffer(MAX_PATH));
            if(FileFinder.IsDirectory())
            {
				wchar_t foundDirPath[MAX_PATH];
                _stprintf_s(foundDirPath, _T("%s\\%s"), DirName, foundFileName);
                DeleteDirectory(foundDirPath);
            }
            else
            {
				wchar_t foundFilePath[MAX_PATH];
                _stprintf_s(foundFilePath, _T("%s\\%s"), DirName, foundFileName);
                DeleteFile(foundFilePath);
            }
        }
    }
    FileFinder.Close();
    if(!RemoveDirectory(DirName))
    {
        //MessageBox(0,"删除目录失败!","警告信息",MB_OK);//比如没有找到文件夹,删除失败，可把此句删除
        return FALSE;
    }

    return TRUE;
}
