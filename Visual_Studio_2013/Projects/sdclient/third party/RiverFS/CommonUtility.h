#pragma once
//
#define NO_LETT(FILE_PATH) (FILE_PATH) + 2
#define DRIVE_LETTE(FILE_PATH, LETTE)	FILE_PATH[0] = LETTE[0]; FILE_PATH[1] = LETTE[1];
#define MKZEO(TEXT)	TEXT[0] = _T('\0')
#define ISZEO(TEXT) _T('\0') == TEXT[0]
// #define NOZEO(TEXT) _T('\0') != TEXT[0]
#define LETT_LENGTH	2
#define ROOT_LENGTH	1
#define MD5_DIGEST_LEN 16
#define SHA1_DIGEST_LEN 20
#define CHUNK_SIZE ((int)1 << 22) // 4M
#define POW2N_MOD(nua, nub) (nua & (nub -1)) // x=2^n a=a%x-->a=a&(x-1)

//
namespace comutil {
	TCHAR *get_name(TCHAR *name, const TCHAR *location, const TCHAR *sub_dire);
	TCHAR *full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file);
	TCHAR *appe_path(TCHAR *to, const TCHAR *from);
	TCHAR *dele_last_path(TCHAR *to, const TCHAR *from);
	char* ustr_ansi(char* ansi, const TCHAR* unicode);
	const TCHAR *wcslchr(const TCHAR *str);
	//
	TCHAR *GetFileFolderPathW(TCHAR *szDirePath, const TCHAR *szFileName);
	BOOL FileExist(TCHAR *szFileName);
	unsigned __int64 FileSize(const TCHAR *szFileName);
	BOOL FileSizeTime(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, const TCHAR *szFileName);
	BOOL FileSizeCLTime(unsigned __int64 *qwFileSize, FILETIME *ftCreatTime, FILETIME *ftLastWrite, const TCHAR *szFileName);
	BOOL FileLastWrite(FILETIME *ftLastWrite, const TCHAR *szFileName);
	BOOL FileCLTime(FILETIME *ftCreatTime, FILETIME *ftLastWrite, const TCHAR *szFileName);
	BOOL CreationTime(FILETIME *ftCreatTime, const TCHAR *szFolderPath);
	//
	VOID file_chks(unsigned char *md5_chks, const TCHAR *file_name);
	VOID chunk_chks(unsigned char *md5_chks, HANDLE hFile, unsigned __int64 qwFileSize, unsigned __int64 offset);
	// for debug
	TCHAR *ftim_unis(FILETIME *filetime);
};

// (vleng / CHUNK_SIZE);
#define CHKS_LENGTH(CHKS_LEN, FILE_SIZE) \
	CHKS_LEN = (unsigned int)(FILE_SIZE >> 22); \
    if (POW2N_MOD(FILE_SIZE, CHUNK_SIZE)) CHKS_LEN++;

// absolute path
#define ABSOLU_PATH(ABSO_PATH, LETTER, RELA_PATH) \
	DRIVE_LETTE(ABSO_PATH, LETTER); \
	_tcscpy_s(NO_LETT(ABSO_PATH), MAX_PATH-LETT_LENGTH, RELA_PATH);

//
#define ERASE_LACHR(str) { \
	TCHAR *lastc = (TCHAR *)comutil::wcslchr(str); \
	if(lastc) lastc[0] = _T('\0'); \
}

//
#define INVA_INDE_VALU		((ULONG)-1)
#define ROOT_INDE_VALU		0
// list
#define LIST_INDE_SIBLING	0x0001
#define LIST_INDE_RECYCLED	0x0002
// file
#define FILE_INDE_ISONYM	0x0001
#define FILE_INDE_RECYCLED	FILE_INDE_ISONYM
#define FILE_INDE_SIBLING	0x0002
#define FILE_INDE_AIDLE		0x0003
// folder
#define FODE_INDE_SIBLING	0x0001
#define FODE_INDE_RECYCLED	FODE_INDE_SIBLING
#define FODE_INDE_LEAF		0x0002
#define FODE_INDE_CHILD		0x0003
//