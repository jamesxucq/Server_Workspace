//FileUtility.h
#ifndef _FILEUTILITY_H_
#define _FILEUTILITY_H_

#include "FilesVec.h"


namespace NFileUtility {
	//Ŀ¼�Ƿ���ڼ�飺
	//VC���ж�Ŀ¼,�ļ��Ƿ����,����Ŀ¼����Ŀ¼���ļ���С
	BOOL FolderExists(const TCHAR *szFolderPath);
	/////////////////////////////////////////////////////////////////////////////////
	//�ļ������Լ�飺
	//ע�⣬�ú����Ǽ�鵱ǰĿ¼���Ƿ��и��ļ�
	//�����������Ŀ¼���Ƿ��и��ļ������ڲ�����������ļ�������·������
	BOOL FileExists(TCHAR *szFileName);
	bool IsDirectory(const TCHAR *szFolderPath);
	BOOL CreationTime(FILETIME *ftCreationTime, const TCHAR *szFolderPath);
	/////////////////////////////////////////////////////////////////////////////////
	//����Ŀ¼��
	BOOL CreateFolder(TCHAR *szFolderPath);
	/////////////////////////////////////////////////////////////////////////////////
	//�ļ���С��
	unsigned __int64 FileLargeSize(TCHAR *szFileName);
	//��Ȼ��CFileFind������GetLength()������Ҳ������á�
	// DWORD FileSize(CString &szFileName);
	DWORD FileSize(const TCHAR *szFileName);
	inline DWORD FileSizeHandle(HANDLE hFile) { return GetFileSize(hFile, NULL); }
	off_t FileSizeDescrip(int file_descrip);
	off_t FileSizePoint(FILE *file_point);
	/////////////////////////////////////////////////////////////////////////////////
	// �ļ���С���޸�ʱ��
	DWORD FileSizeTime(off_t *dwFileSize, FILETIME *lastwrite, TCHAR *szFileName);
	DWORD FileSizeTimeExt(DWORD *dwFileSize, FILETIME *lastwrite, TCHAR *szFileName);
	DWORD FileLastWrite(FILETIME *ftLastWrite, TCHAR *szFileName);
	/////////////////////////////////////////////////////////////////////////////////
	//�ļ��д�С:
	DWORD GetDirectorySize(CString &szDirectoryPath);
	TCHAR *ModuleBaseName();
	TCHAR *ModuleBaseName(LPWSTR lpFilePath);
	TCHAR *SystemDriverPath(LPWSTR lpFilePath);
	/////////////////////////////////////////////////////////////////////////////////
	BOOL MakeSureDirectoryPathExists(LPCTSTR lpszDirectoryPath);// lpszDirectoryPathΪҪ������Ŀ¼���硰C:/dir1/dir2��
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
	BOOL EmptyDirectory(TCHAR *szDirectory); // �� EmptyDirectory("c:\\aaa")
	/////////////////////////////////////////////////////////////////////////////////
};

static BOOL CopyDirectory(TCHAR *szExistingDirectory, TCHAR *szNewDirectory, BOOL bFailIfExists=FALSE);
static BOOL DeleteDirectory(TCHAR *szDirectory); // ��ɾ�� DeleteDirectory("c:\\aaa")

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