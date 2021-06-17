#include "StdAfx.h"

#include "md5.h"
#include "../FileIO.h"
#include "CommonUtility.h"

#define MAX_MAP_SIZE (256*1024)
#define CSUM_CHUNK 64

bool CommonUtility::IsDirectory(FILETIME *ftCreationTime, const TCHAR *szFolderPath) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	DWORD dwAttrib = 0x00000000;

	if(!szFolderPath || !ftCreationTime) return false;
	hFind = FindFirstFile(szFolderPath, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) {
		*ftCreationTime = fileInfo.ftCreationTime;
		dwAttrib = fileInfo.dwFileAttributes;
	}
	FindClose(hFind);

	return static_cast<bool>((dwAttrib!=0xffffffff) 
		&& (dwAttrib&FILE_ATTRIBUTE_DIRECTORY));
}

TCHAR *CommonUtility::full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file) {
	if(!file_name) return NULL;
	_stprintf_s(file_name, MAX_PATH, _T("%s\\%s"), path, file);
	return file_name;
}

TCHAR *CommonUtility::get_name(TCHAR *name, const TCHAR *location, const TCHAR *subdir) {
	TCHAR wdesti[MAX_PATH];

	if(!name) return NULL;

	if (*subdir == _T('~')) 
		_stprintf_s(wdesti, _T("%s%s"), location, subdir+1);
	else _stprintf_s(wdesti, _T("%s%s"), location, subdir);
	_tcscpy_s(name, wcslen(wdesti)+1, wdesti);

	return name;
}

BOOL CommonUtility::FileExists(TCHAR *szFileName) {
	WIN32_FIND_DATA wfd;
	BOOL Found = FALSE;

	HANDLE hFind = FindFirstFile(szFileName, &wfd);
	if (hFind!=INVALID_HANDLE_VALUE) Found = TRUE;

	FindClose(hFind);
	return Found;
}

DWORD CommonUtility::FileSize(const TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	DWORD fileSize;

	hFind = FindFirstFile(szFileName, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE)
		fileSize = fileInfo.nFileSizeLow;

	FindClose(hFind);
	return fileSize;
}

TCHAR *CommonUtility::append_path(TCHAR *desti, const TCHAR *source) {
	if (!desti || !source) return NULL;
	//_stprintf_s(szDirectory, MAX_PATH, _T("%s\\%s"), desti, source);
	_tcscat_s(desti, MAX_PATH, _T("\\"));
	_tcscat_s(desti, MAX_PATH, source);

	return desti;
}

TCHAR *CommonUtility::del_last_path(TCHAR *desti, const TCHAR *source) {
	LPTSTR token = NULL;

	if (!desti) return NULL;

	token = _tcsrchr(desti, _T('\\'));
	if (_tcscmp(token+1, source)) return desti;
	token[0] = _T('\0');

	return desti;
}

char* CommonUtility::unicode_ansi(char* szAnsi, const TCHAR* szUnicode) {
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

DWORD CommonUtility::FileSizeTimeExt(DWORD *nFileSize, FILETIME *lastwrite, TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;

	if(!nFileSize || !lastwrite || !szFileName) return -1;
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(hFind != INVALID_HANDLE_VALUE) {
		*nFileSize = fileInfo.nFileSizeLow;
		*lastwrite = fileInfo.ftLastWriteTime;
	}
	else return -1;

	FindClose(hFind);
	return 0;
}

void CommonUtility::file_csum(unsigned char *sum, const TCHAR *file_name, off_t offset) {
	HANDLE hFile;
	struct tiny_map *buf;
	off_t i, nFileSize;
	MD5_CTX md;
	int remainder;

	memset(sum, '\0', DIGEST_LENGTH);
	hFile = CreateFile( file_name, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return;
	} 

	DWORD dwFileOffset = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwFileOffset && NO_ERROR!=GetLastError()) 
		return;
	nFileSize = GetFileSize(hFile, NULL);

	buf = (struct tiny_map *) map_file(hFile, nFileSize, MAX_MAP_SIZE, CSUM_CHUNK);
	MD5Init(&md);
	for (i = offset; i + CSUM_CHUNK <= nFileSize; i += CSUM_CHUNK) {
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, CSUM_CHUNK), CSUM_CHUNK);
	}
	remainder = (int) (nFileSize - i);
	if (remainder > 0)
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, remainder), remainder);
	MD5Final(&md);
	memcpy(sum, md.digest, DIGEST_LENGTH);

	unmap_file(buf);
	if(hFile != INVALID_HANDLE_VALUE) {
		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
	}
}