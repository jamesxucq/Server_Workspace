//FileOpt.h
#ifndef _FILEOPT_H_
#define _FILEOPT_H_

#include "FilesVec.h"


class CFileOpt
{
public:
	CFileOpt(void);
	~CFileOpt(void);
public:
	//目录是否存在检查：
	//VC中判断目录,文件是否存在,创建目录，求目录或文件大小
	static BOOL CheckFolderExist(wchar_t *sFolderPath);

	//文件存在性检查：
	//注意，该函数是检查当前目录下是否有该文件
	//如果想检查其他目录下是否有该文件，则在参数中输入该文件的完整路径即可
	static BOOL CheckFileExist(wchar_t *sFileName);

	//创建目录：
	static BOOL CreateFolder(wchar_t *sFolderPath);

public:
	//文件大小：
	static unsigned __int64 FileLargeSize(wchar_t *filepath);
	//当然在CFileFind里面有GetLength()函数，也可以求得。
	//static DWORD FileSizeN(CString &filepath);
	static DWORD FileSizeN(wchar_t *filepath);
	static inline DWORD FileSizeH(HANDLE hFile) { return GetFileSize(hFile, NULL); }
	static off_t FileSizeD(int fd);
	static off_t FileSizeP(FILE *fp);

	// 文件大小和修改时间
	static int FileSizeTimeN(off_t *filesize, FILETIME *lastwrite, wchar_t *filename);
public:
	//文件夹大小:
	static DWORD GetDirSize(CString &csDirPath);
	static CString ModuleBaseName();
	static bool ModuleBaseName(LPWSTR lpExePath);

	static BOOL MakeSureDirPathExists(LPCTSTR lpszDirPath);// lpszDirPath为要创建的目录，如“C:/dir1/dir2”
	static wchar_t *GetFileDirPath(const wchar_t *sFileName);
	static BOOL CreatFileDirPath(const wchar_t *sFileName);
public:
	static HANDLE CreatBackupFile(wchar_t *BackupFile, wchar_t *SourceFile);
	static HANDLE BuildTempFile();
	static bool SetDirectoryAttrib(wchar_t *sDriveRoot, wchar_t *sDirectory, bool IsSetToHidden);
	static bool CreatCacheFolder(wchar_t *sDriveRoot);
public:
	static int GetFilesVecByPath(OUT FilesAttribVec *vpFilesVec, IN wchar_t *sPath);
public:
	static bool CreateHugeFile(LPCTSTR sFileName, DWORD FileLength);
	static bool ShrinkHugeFile(LPCTSTR sFileName, DWORD FileLength);
	static bool ExpandHugeFile(LPCTSTR sFileName, DWORD FileLength);
};


#endif	/* _FILEOPT_H_ */

#ifndef TIMEOPT_H_
#define TIMEOPT_H_

class CTimeOpt
{
public:
	CTimeOpt(void);
	~CTimeOpt(void);
public:
	static int FileTimeToStringA(char *sTime, FILETIME *filetime); // 2010-11-11 10:10:10
	static wchar_t *FileTimeToStringW(FILETIME *filetime); // 2010-11-11 10:10:10

//////////////////////////////////////////////////////////////////////////////////////////////
	static int StringToFileTimeA(FILETIME *filetime, char *sTime);
	static int StringToFileTimeW(FILETIME *filetime, wchar_t *sTime); // 2010-11-11 10:10:10

//////////////////////////////////////////////////////////////////////////////////////////////
	static time_t FileTimeToTime_t(const FILETIME &ft);
	static inline LONG FileTimeCompare(FILETIME *lpFileTime1, FILETIME *lpFileTime2)
		{return (LONG)((((ULARGE_INTEGER *)lpFileTime1)->QuadPart - ((ULARGE_INTEGER *)lpFileTime2)->QuadPart ) / 10000000);}
	static inline LONG FileTimeCompareEx(FILETIME *lpFileTime1, FILETIME *lpFileTime2) {
		ULARGE_INTEGER ulFileTime1, ulFileTime2;
		ulFileTime1.LowPart = lpFileTime1->dwLowDateTime; ulFileTime1.HighPart = lpFileTime1->dwHighDateTime;
		ulFileTime2.LowPart = lpFileTime2->dwLowDateTime; ulFileTime2.HighPart = lpFileTime2->dwHighDateTime;
		return (LONG)((ulFileTime1.QuadPart - ulFileTime2.QuadPart) / 10000000);
	}


};

static BOOL CopyDirectory(wchar_t *sExistingDirPath, wchar_t *sNewDirPath, BOOL bFailIfExists=FALSE);
static BOOL DeleteDirectory(wchar_t *DirName); // 如删除 DeleteDirectory("c:\\aaa")


#endif /* TIMEOPT_H_ */