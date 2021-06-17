// FileUtility.h
#ifndef _FILEUTILITY_H_
#define _FILEUTILITY_H_

#include "FilesVec.h"


namespace NFileUtility {
	// �ļ������Լ��
	// ע�⣬�ú����Ǽ�鵱ǰĿ¼���Ƿ��и��ļ�
	// �����������Ŀ¼���Ƿ��и��ļ������ڲ�����������ļ�������·������
	BOOL FileExist(TCHAR *szFileName);
	ULONG MateriExist(HANDLE hFileMateri);
	BOOL CreationTime(FILETIME *ftCreatTime, const TCHAR *szFolderPath);
	// �ļ���С:
	unsigned __int64 FileSize(const TCHAR *szFileName);
	unsigned __int64 FileSizeHandle(HANDLE hFileHand);
	__int64 FileSizeStream(FILE *file_stre);
	// �ļ���С���޸�ʱ��
	DWORD FileSizeTime(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, TCHAR *szFileName);
	DWORD FileLastWrite(FILETIME *ftLastWrite, TCHAR *szFileName);
//
	TCHAR *ModuleBaseName();
	TCHAR *ModuleBaseName(TCHAR *lpFilePath);
	TCHAR *ModuleBaseNameEx(TCHAR *lpFilePath);
	TCHAR *SystemDriverPath(TCHAR *lpFilePath);
	TCHAR *ApplicaDataPath(TCHAR *szFilePath, const TCHAR *szFileName);
	//
	HANDLE BackupFile(HANDLE hFromFile);
	HANDLE CreatBackupChunk(TCHAR *szBackupChunk, TCHAR *szFromFile, long Offset);
	HANDLE CreatBackupFile(TCHAR *szBackupFile, TCHAR *szFromFile, long Offset);
	HANDLE BuildChksFile(TCHAR *szChksFile);
	//
	HANDLE BuildVerifyFile(TCHAR *szVerifyFile);
	HANDLE BuildComplexFile(TCHAR *szComplexFile);
	HANDLE OpenLocalFile(TCHAR *szFileLocal);
	//
	DWORD CreateHugeFile(LPCTSTR szFileName, DWORD dwFileLength);
	DWORD ShrinkHugeFile(LPCTSTR szFileName, DWORD dwFileLength);
	DWORD ExpandHugeFile(LPCTSTR szFileName, DWORD dwFileLength);
	//
	DWORD TruncateHandle(HANDLE hFileHand, unsigned __int64 qwFileLength);
	DWORD SetFileSize(const TCHAR *szFileName, unsigned __int64 qwFileLength);
	DWORD SetFileLawi(const TCHAR *szFileName, FILETIME *ftLastWrite);
	DWORD SetFileSizeLawi(const TCHAR *szFileName, unsigned __int64 qwFileLength, FILETIME *ftLastWrite);
	//
#define FILE_LOCKEO		((DWORD)-1)
#define FILE_UNLOCK		0x0000
#define FILE_LOCKED		0x0001
	DWORD IsFileLocked(const TCHAR *szFileName); // ((DWORD)-1):error 0:not lock 1:locked
	DWORD LockReadFile(HANDLE *hReadLocked, const TCHAR *szFileName); // ((DWORD)-1):error 0:not lock 1:locked
	VOID UnlockFile(HANDLE hFileLocked);
	//
#define BCOPY_EXCEPT		((DWORD)-1)
#define BCOPY_SUCCESS		0x0000
#define BCOPY_CONTINUE		0x0001
#define BCOPY_NOT_FOUND		0x0010
#define BCOPY_VIOLATION		0x0020
	DWORD BlockCopyFile(LPCWSTR lpExistFile, LPCWSTR lpNewFile, DWORD dwMaxLen);
	DWORD GetCoupleID();
	DWORD RandomCoupleID();
//
	HANDLE BuildCacheFile(const TCHAR *szFileName);
	HANDLE BuildCacheFile(TCHAR *szCacheFile, const TCHAR *szFileName);
	char *build_cache_name(char *cache_file, char *file_name);
};

namespace NDireUtility {
	// Ŀ¼�Ƿ���ڼ�飺
	// VC���ж�Ŀ¼,�ļ��Ƿ����,����Ŀ¼����Ŀ¼���ļ���С
	BOOL FolderExistA(const char *szFolderPath);
	BOOL FolderExistW(const TCHAR *szFolderPath);
	bool IsDirectory(const TCHAR *szFolderPath);
	// ����Ŀ¼��
	BOOL CreateFolder(TCHAR *szFolderPath);
	// �ļ��д�С:
	DWORD GetDirectorySize(CString &csDirePath);
	//
	BOOL MakeFolderExist(LPCTSTR szDirePath);// szDirePathΪҪ������Ŀ¼���硰C:/dir1/dir2��
	BOOL RecursMakeFolderA(char *szDirePath);
	BOOL RecursMakeFolderW(LPCTSTR szDirePath);
	char *GetFileFolderPathA(char *szDirePath, const char *szFileName);
	TCHAR *GetFileFolderPathW(TCHAR *szDirePath, const TCHAR *szFileName);
	BOOL MakeFileFolder(const TCHAR *szFileName);
	//
	DWORD SetDirectoryAttrib(const TCHAR *szDriveLetter, TCHAR *szDirectory, bool IsSetToHidden);
	DWORD CreatAttribFolder(const TCHAR *szDriveLetter);
	BOOL EmptyDirectory(TCHAR *szDirectory); // �� EmptyDirectory("c:\\aaa")
	//
	VOID CCacheDirectory();
	//
	BOOL CopyDirectory(TCHAR *szExistDirectory, TCHAR *szNewDirectory, BOOL bFailIfExist=FALSE);
	BOOL DeleDirectory(TCHAR *szDirectory); // ��ɾ�� DeleDirectory("c:\\aaa")
	BOOL DeleDirectoryEx(CString &csDirectory); 
};

#endif	/* _FILEUTILITY_H_ */

#ifndef TIME_CONV_H_
#define TIME_CONV_H_

namespace timcon {
	char *ftim_ansi(char *timestr, FILETIME *filetime); // 2010-11-11 10:10:10
	TCHAR *ftim_unis(TCHAR *timestr, FILETIME *filetime); // 2010-11-11 10:10:10
	//
	DWORD ansi_ftim(FILETIME *filetime, char *timestr);
	DWORD unis_ftim(FILETIME *filetime, TCHAR *timestr); // 2010-11-11 10:10:10
	//
	time_t ftim_timt(const FILETIME &ft);
	inline LONGLONG ftim_comp(FILETIME *filetime1, FILETIME *filetime2)
	{ return (LONGLONG)(((ULARGE_INTEGER *)filetime1)->QuadPart - ((ULARGE_INTEGER *)filetime2)->QuadPart); }
};

#endif /* TIME_CONV_H_ */