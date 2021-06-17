#pragma once
//
#define NO_ROOT(FILE_PATH)				(FILE_PATH) + 2
#define PATH_ROOT(FILE_PATH, ROOT)		FILE_PATH[0] = ROOT[0]; FILE_PATH[1] = _T(':');
#define MKZERO(text)					text[0] = _T('\0')
#define ROOT_LENGTH						2
#define MD5_DIGEST_LEN					16
#define SHA1_DIGEST_LEN					20
#define CHUNK_SIZE						((int)1 << 22) // 4M
#define POW2N_MOD(nua, nub)  (nua & (nub -1)) // x=2^n a=a%x-->a=a&(x-1)

//
namespace CommonUtility
{
	//
	TCHAR *get_name(TCHAR *name, const TCHAR *location, const TCHAR *subdir);
	TCHAR *full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file);
	TCHAR *append_path(TCHAR *to, const TCHAR *from);
	TCHAR *del_last_path(TCHAR *to, const TCHAR *from);
	char* unicode_ansi(char* ansi, const TCHAR* unicode);
	//
	TCHAR *GetFileFolderPath(TCHAR *szDirectoryPath, const TCHAR *szFileName);
	BOOL FileExists(TCHAR *szFileName);
	unsigned __int64 FileSize(const TCHAR *szFileName);
	DWORD FileSizeTimeExt(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, const TCHAR *szFileName);
	DWORD FileLastWrite(FILETIME *ftLastWrite, const TCHAR *szFileName);
	BOOL CreationTime(FILETIME *ftCreationTime, const TCHAR *szFolderPath);
	//
	VOID file_chks(unsigned char *md5_chks, const TCHAR *file_name);
	VOID chunk_chks(unsigned char *md5_chks, HANDLE hFile, unsigned __int64 qwFileSize, unsigned __int64 offset);
	//
};

//
#define INVA_INDE_VALU		((ULONG)-1)
#define ROOT_INDE_VALU		0
// list
#define LIST_INDEX_SIBLING	0x0001
#define LIST_INDEX_RECYCLED	0x0002
// file
#define FILE_INDEX_ISONYM	0x0001
#define FILE_INDEX_RECYCLED	FILE_INDEX_ISONYM
#define FILE_INDEX_SIBLING	0x0002
// folder
#define FODE_INDEX_SIBLING	0x0001
#define FODE_INDEX_RECYCLED	FODE_INDEX_SIBLING
#define FODE_INDEX_LEAF		0x0002
#define FODE_INDEX_CHILD	0x0004
//