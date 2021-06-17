//FileUtility.h
#ifndef _FILEUTILITY_H_
#define _FILEUTILITY_H_

#include "FilesVec.h"


namespace NFileUtility {
	//Ŀ¼�Ƿ���ڼ�飺
	//VC���ж�Ŀ¼,�ļ��Ƿ����,����Ŀ¼����Ŀ¼���ļ���С
	BOOL FolderExists(const TCHAR *szFolderPath);
	//
	//�ļ������Լ�飺
	//ע�⣬�ú����Ǽ�鵱ǰĿ¼���Ƿ��и��ļ�
	//�����������Ŀ¼���Ƿ��и��ļ������ڲ�����������ļ�������·������
	BOOL FileExists(TCHAR *szFileName);
	bool IsDirectory(const TCHAR *szFolderPath);
	BOOL CreationTime(FILETIME *ftCreationTime, const TCHAR *szFolderPath);
	//
	//����Ŀ¼��
	BOOL CreateFolder(TCHAR *szFolderPath);
	//
	//�ļ���С��
	unsigned __int64 FileLargeSize(TCHAR *szFileName);
	//��Ȼ��CFileFind������GetLength()������Ҳ������á�
	unsigned __int64 FileSize(const TCHAR *szFileName);
	unsigned __int64 FileSizeHandle(HANDLE hFile);
	__int64 FileSizeStream(FILE *file_stre);
	//
	// �ļ���С���޸�ʱ��
	DWORD FileSizeTime(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, TCHAR *szFileName);
	DWORD FileSizeTimeExt(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, TCHAR *szFileName);
	DWORD FileLastWrite(FILETIME *ftLastWrite, TCHAR *szFileName);
	//
	//�ļ��д�С:
	DWORD GetDirectorySize(CString &szDirectoryPath);
	TCHAR *ModuleBaseName();
	TCHAR *ModuleBaseName(TCHAR *lpFilePath);
	TCHAR *SystemDriverPath(TCHAR *lpFilePath);
	//
	BOOL MakeFolderExists(LPCTSTR szDirectoryPath);// szDirectoryPathΪҪ������Ŀ¼���硰C:/dir1/dir2��
	BOOL RecuMakeFolder(LPCTSTR szDirectoryPath);
	TCHAR *GetFileFolderPath(TCHAR *szDirectoryPath, const TCHAR *szFileName);
	BOOL MakeFileFolder(const TCHAR *szFileName);
	//
	HANDLE CreatBackupChunk(TCHAR *szBackupChunk, TCHAR *szFromFile, size_t offset);
	HANDLE CreatBackupFile(TCHAR *szBackupFile, TCHAR *szFromFile, size_t offset);
	HANDLE BuildChksFile(TCHAR *szChksFile);
	//
	HANDLE BuildVerifyFile(TCHAR *szVerifyFile);
	HANDLE BuildComplexFile(TCHAR *szComplexFile);
	HANDLE OpenLocalFile(TCHAR *szFileLocal);
	//
	DWORD SetDirectoryAttrib(const TCHAR *szDriveRoot, TCHAR *szDirectory, bool IsSetToHidden);
	DWORD CreatAttribFolder(const TCHAR *szDriveRoot);
	//
	DWORD CreateHugeFile(LPCTSTR szFileName, DWORD dwFileLength);
	DWORD ShrinkHugeFile(LPCTSTR szFileName, DWORD dwFileLength);
	DWORD ExpandHugeFile(LPCTSTR szFileName, DWORD dwFileLength);
	//
	DWORD FtruncateHandle(HANDLE hFile, unsigned __int64 qwFileLength);
	DWORD SetFileAttrib(const TCHAR *szFileName, unsigned __int64 qwFileLength, FILETIME *ftLastWrite);
	DWORD SetFileLastWrite(const TCHAR *szFileName, FILETIME *ftLastWrite);
#define FILE_LOCKED		0x0001
#define FILE_UNLOCK		0
	DWORD IsFileLocked(const TCHAR *szFileName); // -1:error 0:not lock 1:locked
	//
	DWORD RandomCoupleID();
	BOOL EmptyDirectory(TCHAR *szDirectory); // �� EmptyDirectory("c:\\aaa")
	HANDLE BuildCacheFile(TCHAR *szFileName);
	HANDLE BuildCacheFile(TCHAR *szCacheFile, TCHAR *szFileName);
	char *build_cache_name(char *cache_file, char *file_name);
};

BOOL CopyDirectory(TCHAR *szExistingDirectory, TCHAR *szNewDirectory, BOOL bFailIfExists=FALSE);
BOOL DeleteDirectory(TCHAR *szDirectory); // ��ɾ�� DeleteDirectory("c:\\aaa")

#endif	/* _FILEUTILITY_H_ */

#ifndef TIME_CONV_H_
#define TIME_CONV_H_

namespace ntimeconv {
	char *filetime_ansi(char *timestr, FILETIME *filetime); // 2010-11-11 10:10:10
	TCHAR *filetime_unicode(TCHAR *timestr, FILETIME *filetime); // 2010-11-11 10:10:10
	//
	DWORD ansi_filetime(FILETIME *filetime, char *timestr);
	DWORD unicode_filetime(FILETIME *filetime, TCHAR *timestr); // 2010-11-11 10:10:10
	//
	time_t filetime_timet(const FILETIME &ft);
	inline LONG filetime_compare(FILETIME *filetime1, FILETIME *filetime2)
	{return (LONG)((((ULARGE_INTEGER *)filetime1)->QuadPart - ((ULARGE_INTEGER *)filetime2)->QuadPart ) / 10000000); }
	LONG filetime_compare_ext(FILETIME *filetime1, FILETIME *filetime2) ;
};

#endif /* TIME_CONV_H_ */