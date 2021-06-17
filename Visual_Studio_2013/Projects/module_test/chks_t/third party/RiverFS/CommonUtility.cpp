#include "StdAfx.h"

#include "../Logger.h"
#include "md5.h"
#include "../tiny_map64.h"
#include "CommonUtility.h"

#define MAX_MAP_SIZE (256*1024)
#define CHKS_CHUNK 4096

TCHAR *CommonUtility::full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file) {
	if(!file_name) return NULL;
	_stprintf_s(file_name, MAX_PATH, _T("%s\\%s"), path, file);
	return file_name;
}

TCHAR *CommonUtility::get_name(TCHAR *name, const TCHAR *location, const TCHAR *subdir) {
	TCHAR wdesti[MAX_PATH];
	//
	if (*subdir == _T('~')) 
		_stprintf_s(wdesti, _T("%s%s"), location, subdir+1);
	else _stprintf_s(wdesti, _T("%s%s"), location, subdir);
	_tcscpy_s(name, wcslen(wdesti)+1, wdesti);
	//
	return name;
}

TCHAR *CommonUtility::GetFileFolderPath(TCHAR *szDirectoryPath, const TCHAR *szFileName)
{
    TCHAR *toke = NULL;
    //
    if(!szFileName || !szDirectoryPath) return NULL;
    MKZERO(szDirectoryPath);
    _tcscpy_s(szDirectoryPath, MAX_PATH, szFileName);
    toke = _tcsrchr(szDirectoryPath, _T('\\'));
    if(toke) *toke = _T('\0');
    //
    return szDirectoryPath;
}

BOOL CommonUtility::FileExists(TCHAR *szFileName) {
	WIN32_FIND_DATA wfd;
	BOOL bFound = FALSE;
	//
	HANDLE hFind = FindFirstFile(szFileName, &wfd);
	if (INVALID_HANDLE_VALUE!=hFind) bFound = TRUE;
	//
	FindClose(hFind);
	return bFound;
}

TCHAR *CommonUtility::append_path(TCHAR *to, const TCHAR *from) {
	//_stprintf_s(szDirectory, MAX_PATH, _T("%s\\%s"), to, from);
	_tcscat_s(to, MAX_PATH, _T("\\"));
	_tcscat_s(to, MAX_PATH, from);
	//
	return to;
}

TCHAR *CommonUtility::del_last_path(TCHAR *to, const TCHAR *from) {
	LPTSTR toke = NULL;
	//
	toke = _tcsrchr(to, _T('\\'));
	if (_tcscmp(toke+1, from)) return to;
	toke[0] = _T('\0');
	//
	return to;
}

char* CommonUtility::unicode_ansi(char* ansi, const TCHAR* unicode) {
	// unicode to ansi
	int alength = WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), ansi, alength, NULL, NULL);
	ansi[alength] = '\0';
	//
	return ansi;
}

unsigned __int64 CommonUtility::FileSize(const TCHAR *szFileName)
{
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    unsigned __int64 qwFileSize = 0;
//
    hFind = FindFirstFile(szFileName, &fileInfo);
    if(INVALID_HANDLE_VALUE != hFind) {
        qwFileSize = fileInfo.nFileSizeLow;
        qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
    }
//
    FindClose(hFind);
    return qwFileSize;
}

DWORD CommonUtility::FileSizeTimeExt(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, const TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		*qwFileSize = fileInfo.nFileSizeLow;
		*qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
		*ftLastWrite = fileInfo.ftLastWriteTime;
	} else return ((DWORD)-1);
	//
	FindClose(hFind);
	return 0;
}

DWORD CommonUtility::FileLastWrite(FILETIME *ftLastWrite, const TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) *ftLastWrite = fileInfo.ftLastWriteTime;
	else return ((DWORD)-1);
	//
	FindClose(hFind);
	return 0;
}

BOOL CommonUtility::CreationTime(FILETIME *ftCreationTime, const TCHAR *szFolderPath)
{
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    //
    if(!szFolderPath || !ftCreationTime) return FALSE;
    hFind = FindFirstFile(szFolderPath, &fileInfo);
    if(INVALID_HANDLE_VALUE != hFind) {
        *ftCreationTime = fileInfo.ftCreationTime;
    }
    FindClose(hFind);
    //
    return TRUE;
}

VOID CommonUtility::file_chks(unsigned char *md5_chks, const TCHAR *file_name) {
	HANDLE hFile;
	struct tiny_map64 *buf;
	unsigned __int64 i, qwFileSize;
	DWORD dwFileSizeHigh;
	MD5_CTX md;
	int remainder;
	//
	memset(md5_chks, '\0', MD5_DIGEST_LEN);
	hFile = CreateFile( file_name, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( INVALID_HANDLE_VALUE == hFile ) {
		TRACE( _T("create file failed: %d\n"), GetLastError() );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return;
	} 
	//
	qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
	buf = (struct tiny_map64 *) map_file64(hFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
	MD5Init(&md);
	for (i = 0; i + CHKS_CHUNK <= qwFileSize; i += CHKS_CHUNK) {
		MD5Update(&md, (unsigned char *) map_ptr64(buf, i, CHKS_CHUNK), CHKS_CHUNK);
	}
	remainder = (int) (qwFileSize - i);
	if (remainder > 0)
		MD5Update(&md, (unsigned char *) map_ptr64(buf, i, remainder), remainder);
	MD5Final(&md);
	memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
	//
	unmap_file64(buf);
	if(INVALID_HANDLE_VALUE != hFile) {
		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
	}
}

VOID CommonUtility::chunk_chks(unsigned char *md5_chks, HANDLE hFile, unsigned __int64 qwFileSize, unsigned __int64 offset) {
	struct tiny_map64 *buf;
	unsigned __int64 i, offset_end;
	MD5_CTX md;
	int remainder;
//
	memset(md5_chks, '\0', MD5_DIGEST_LEN);
	buf = (struct tiny_map64 *) map_file64(hFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
	offset_end = offset + CHUNK_SIZE;
	if (offset_end > qwFileSize) offset_end = qwFileSize;
//
	MD5Init(&md);
	for (i = offset; i + CHKS_CHUNK <= offset_end; i += CHKS_CHUNK) {
		MD5Update(&md, (unsigned char *) map_ptr64(buf, i, CHKS_CHUNK), CHKS_CHUNK);
	}
	remainder = (int) (offset_end - i);
	if (remainder > 0)
		MD5Update(&md, (unsigned char *) map_ptr64(buf, i, remainder), remainder);
	MD5Final(&md);
	memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
//
	unmap_file64(buf);
}