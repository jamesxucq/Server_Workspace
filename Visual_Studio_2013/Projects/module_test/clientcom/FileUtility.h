//FileUtility.h
#ifndef _FILEUTILITY_H_
#define _FILEUTILITY_H_

#include "FilesVec.h"


namespace NFileUtility {
	//目录是否存在检查：
	//VC中判断目录,文件是否存在,创建目录，求目录或文件大小
	BOOL FolderExists(const TCHAR *szFolderPath);
	/////////////////////////////////////////////////////////////////////////////////
	//文件存在性检查：
	//注意，该函数是检查当前目录下是否有该文件
	//如果想检查其他目录下是否有该文件，则在参数中输入该文件的完整路径即可
	BOOL FileExists(TCHAR *szFileName);
	bool IsDirectory(const TCHAR *szFolderPath);
	BOOL CreationTime(FILETIME *ftCreationTime, const TCHAR *szFolderPath);
	/////////////////////////////////////////////////////////////////////////////////
	//创建目录：
	BOOL CreateFolder(TCHAR *szFolderPath);
	/////////////////////////////////////////////////////////////////////////////////
	//文件大小：
	unsigned __int64 FileLargeSize(TCHAR *szFileName);
	//当然在CFileFind里面有GetLength()函数，也可以求得。
	// DWORD FileSize(CString &szFileName);
	DWORD FileSize(const TCHAR *szFileName);
	inline DWORD FileSizeHandle(HANDLE hFile) { return GetFileSize(hFile, NULL); }
	off_t FileSizeDescrip(int file_descrip);
	off_t FileSizePoint(FILE *file_point);
	/////////////////////////////////////////////////////////////////////////////////
	// 文件大小和修改时间
	DWORD FileSizeTime(off_t *dwFileSize, FILETIME *lastwrite, TCHAR *szFileName);
	DWORD FileSizeTimeExt(DWORD *dwFileSize, FILETIME *lastwrite, TCHAR *szFileName);
	DWORD FileLastWrite(FILETIME *ftLastWrite, TCHAR *szFileName);
	/////////////////////////////////////////////////////////////////////////////////
	//文件夹大小:
	DWORD GetDirectorySize(CString &szDirectoryPath);
	TCHAR *ModuleBaseName();
	TCHAR *ModuleBaseName(LPWSTR lpFilePath);
	TCHAR *SystemDriverPath(LPWSTR lpFilePath);
	/////////////////////////////////////////////////////////////////////////////////
	BOOL MakeSureDirectoryPathExists(LPCTSTR lpszDirectoryPath);// lpszDirectoryPath为要创建的目录，如“C:/dir1/dir2”
	TCHAR *GetFileDirectoryPath(TCHAR *szDirectoryPath, const TCHAR *szFileName);
	BOOL CreatFileDirectoryPath(const TCHAR *szFileName);
	/////////////////////////////////////////////////////////////////////////////////
	HANDLE CreatBackupChunk(TCHAR *szBackupChunk, TCHAR *szSourceFile, size_t offset);
	HANDLE CreatBackupFile(TCHAR *szBackupFile, TCHAR *szSourceFile, size_t offset);
	HANDLE BuildCsumsFile(OUT TCHAR *szCsumsFile);
	/////////////////////////////////////////////////////////////////////////////////
	HANDLE BuildVerifyFile(OUT TCHAR *szVerifyFile);
	HANDLE BuildComplexFile(OUT TCHAR *szComplexFile);
	HANDLE OpenLocalFile(OUT TCHAR *szFileLocal);
	/////////////////////////////////////////////////////////////////////////////////
	DWORD SetDirectoryAttrib(const TCHAR *szDriveRoot, TCHAR *szDirectory, bool IsSetToHidden);
	DWORD CreatAttribFolder(const TCHAR *szDriveRoot);
	/////////////////////////////////////////////////////////////////////////////////
	DWORD GetFilesByDirectory(OUT FilesVec *pFilesVec, IN const TCHAR *szPath);
	/////////////////////////////////////////////////////////////////////////////////
	DWORD CreateHugeFile(LPCTSTR szFileName, DWORD FileLength);
	DWORD ShrinkHugeFile(LPCTSTR szFileName, DWORD FileLength);
	DWORD ExpandHugeFile(LPCTSTR szFileName, DWORD FileLength);
	/////////////////////////////////////////////////////////////////////////////////
	DWORD FtruncateHandle(HANDLE hFile, off_t FileLength);
	DWORD SetFileAttrib(const TCHAR *szFileName, off_t FileLength, FILETIME *ftLastWrite);
	DWORD SetFileLastWrite(const TCHAR *szFileName, FILETIME *ftLastWrite);
#define FILE_LOCKED		0x0001
#define FILE_UNLOCK		0
	DWORD IsFileLocked(const TCHAR *szFileName); // -1:error 0:not lock 1:locked
	/////////////////////////////////////////////////////////////////////////////////
	DWORD RandomCoupleID();
	BOOL EmptyDirectory(TCHAR *szDirectory); // 如 EmptyDirectory("c:\\aaa")
	/////////////////////////////////////////////////////////////////////////////////
};

static BOOL CopyDirectory(TCHAR *szExistingDirectory, TCHAR *szNewDirectory, BOOL bFailIfExists=FALSE);
static BOOL DeleteDirectory(TCHAR *szDirectory); // 如删除 DeleteDirectory("c:\\aaa")

#endif	/* _FILEUTILITY_H_ */

#ifndef TIME_CONV_H_
#define TIME_CONV_H_

namespace ntimeconv {
	char *filetime_ansi(char *timestr, FILETIME *filetime); // 2010-11-11 10:10:10
	TCHAR *filetime_unicode(TCHAR *timestr, FILETIME *filetime); // 2010-11-11 10:10:10

	//////////////////////////////////////////////////////////////////////////////////////////////
	DWORD ansi_filetime(FILETIME *filetime, char *timestr);
	DWORD unicode_filetime(FILETIME *filetime, TCHAR *timestr); // 2010-11-11 10:10:10

	//////////////////////////////////////////////////////////////////////////////////////////////
	time_t filetime_timet(const FILETIME &ft);
	inline LONG filetime_compare(FILETIME *filetime1, FILETIME *filetime2)
	{return (LONG)((((ULARGE_INTEGER *)filetime1)->QuadPart - ((ULARGE_INTEGER *)filetime2)->QuadPart ) / 10000000);}
	LONG filetime_compare_ext(FILETIME *filetime1, FILETIME *filetime2) ;
};

#endif /* TIME_CONV_H_ */