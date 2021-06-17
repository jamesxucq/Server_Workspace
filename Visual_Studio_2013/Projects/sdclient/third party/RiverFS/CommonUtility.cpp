#include "StdAfx.h"

#include "../Logger.h"
#include "md5.h"
#include "../tiny_map64.h"
#include "CommonUtility.h"

#define MAX_MAP_SIZE 0x80000 // 512K
#define CHKS_CHUNK 4096

TCHAR *comutil::full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file) {
	if(!file_name) return NULL;
	_stprintf_s(file_name, MAX_PATH, _T("%s\\%s"), path, file);
	return file_name;
}

TCHAR *comutil::get_name(TCHAR *name, const TCHAR *location, const TCHAR *sub_dire) {
	TCHAR wdesti[MAX_PATH];
	//
	if (*sub_dire == _T('~')) 
		_stprintf_s(wdesti, _T("%s%s"), location, sub_dire+1);
	else _stprintf_s(wdesti, _T("%s%s"), location, sub_dire);
	_tcscpy_s(name, MAX_PATH, wdesti);
	//
	return name;
}

TCHAR *comutil::GetFileFolderPathW(TCHAR *szDirePath, const TCHAR *szFileName) {
    TCHAR *toke = NULL;
    //
    if(!szFileName || !szDirePath) return NULL;
    MKZEO(szDirePath);
    _tcscpy_s(szDirePath, MAX_PATH, szFileName);
    toke = _tcsrchr(szDirePath, _T('\\'));
    if(toke) *toke = _T('\0');
    //
    return szDirePath;
}

BOOL comutil::FileExist(TCHAR *szFileName) {
	WIN32_FIND_DATA wfd;
	BOOL bFound = FALSE;
	//
	HANDLE hFind = FindFirstFile(szFileName, &wfd);
	if (INVALID_HANDLE_VALUE!=hFind) bFound = TRUE;
	//
	FindClose(hFind);
	return bFound;
}

TCHAR *comutil::appe_path(TCHAR *to, const TCHAR *from) {
	//_stprintf_s(szDirectory, MAX_PATH, _T("%s\\%s"), to, from);
	_tcscat_s(to, MAX_PATH, _T("\\"));
	_tcscat_s(to, MAX_PATH, from);
	//
	return to;
}

TCHAR *comutil::dele_last_path(TCHAR *to, const TCHAR *from) {
	LPTSTR toke = NULL;
	//
	toke = _tcsrchr(to, _T('\\'));
	if (_tcscmp(toke+1, from)) return to;
	toke[0] = _T('\0');
	//
	return to;
}

char* comutil::ustr_ansi(char* ansi, const TCHAR* unicode) {
	// unicode to ansi
	int alen = WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), ansi, alen, NULL, NULL);
	ansi[alen] = '\0';
	//
	return ansi;
}

const TCHAR *comutil::wcslchr(const TCHAR *str) {
	const TCHAR *char_ptr;
	const unsigned long int *longword_ptr;
	unsigned long int longword, himagic, lomagic;
//
	for (char_ptr = str; ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0; ++char_ptr) {
		if (*char_ptr == '\0')
			return (char_ptr - 1);
	}
//
	longword_ptr = (unsigned long int *) char_ptr;
	himagic = 0x80008000L;
	lomagic = 0x01000100L;
	if (sizeof (longword) > 4) {
		himagic = ((himagic << 16) << 16) | himagic;
		lomagic = ((lomagic << 16) << 16) | lomagic;
	}
	if (sizeof (longword) > 8) abort ();
//
	for (;;) {
		longword = *longword_ptr++;
		if (((longword - lomagic) & ~longword & himagic) != 0)
		{
			const TCHAR *cp = (const TCHAR *) (longword_ptr - 1);
			//
			if (cp[0] == 0)
				return (cp - 1);
			if (cp[1] == 0)
				return cp;

			if (sizeof (longword) > 4)
			{
				if (cp[2] == 0)
					return (cp + 1);
				if (cp[3] == 0)
					return (cp + 2);
			}
		}
	}
}

//
unsigned __int64 comutil::FileSize(const TCHAR *szFileName) {
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    unsigned __int64 qwFileSize = 0x00;
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

BOOL comutil::FileSizeTime(unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, const TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		*qwFileSize = fileInfo.nFileSizeLow;
		*qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
		memcpy(ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME));
	} else { return FALSE; }
	//
	FindClose(hFind);
	return TRUE;
}

BOOL comutil::FileSizeCLTime(unsigned __int64 *qwFileSize, FILETIME *ftCreatTime, FILETIME *ftLastWrite, const TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		*qwFileSize = fileInfo.nFileSizeLow;
		*qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
		memcpy(ftCreatTime, &fileInfo.ftCreationTime, sizeof(FILETIME));
		memcpy(ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME));
	} else { return FALSE; }
	//
	FindClose(hFind);
	return TRUE;
}

BOOL comutil::FileLastWrite(FILETIME *ftLastWrite, const TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) memcpy(ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME));
	else { return FALSE; }
	//
	FindClose(hFind);
	return TRUE;
}

BOOL comutil::FileCLTime(FILETIME *ftCreatTime, FILETIME *ftLastWrite, const TCHAR *szFileName) {
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	//
	hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		memcpy(ftCreatTime, &fileInfo.ftCreationTime, sizeof(FILETIME));
		memcpy(ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME));
	} else { return FALSE; }
	//
	FindClose(hFind);
	return TRUE;
}

BOOL comutil::CreationTime(FILETIME *ftCreatTime, const TCHAR *szFolderPath) {
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    //
    if(!szFolderPath || !ftCreatTime) return FALSE;
    hFind = FindFirstFile(szFolderPath, &fileInfo);
    if(INVALID_HANDLE_VALUE != hFind) {
		memcpy(ftCreatTime, &fileInfo.ftCreationTime, sizeof(FILETIME));
    }
    FindClose(hFind);
    //
    return TRUE;
}

VOID comutil::file_chks(unsigned char *md5_chks, const TCHAR *file_name) {
	HANDLE hFile;
	struct tiny_map64 *buf;
	unsigned __int64 inde, qwFileSize;
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
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
		return;
	} 
	//
	qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError()) {
        CloseHandle( hFile );
        return;
    }
	qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
	buf = (struct tiny_map64 *) map_file64(hFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
	MD5Init(&md);
	for (inde = 0; inde + CHKS_CHUNK <= qwFileSize; inde += CHKS_CHUNK) {
		MD5Update(&md, map_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
	}
	remainder = (int) (qwFileSize - inde);
	if (remainder > 0)
		MD5Update(&md, map_ptr64(buf, inde, remainder), remainder);
	MD5Final(&md);
	memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
	//
	unmap_file64(buf);
	if(INVALID_HANDLE_VALUE != hFile) {
		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
	}
}

VOID comutil::chunk_chks(unsigned char *md5_chks, HANDLE hFile, unsigned __int64 qwFileSize, unsigned __int64 offset) {
	struct tiny_map64 *buf;
	unsigned __int64 inde, offset_end;
	MD5_CTX md;
	int remainder;
//
	memset(md5_chks, '\0', MD5_DIGEST_LEN);
	buf = (struct tiny_map64 *) map_file64(hFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
	offset_end = offset + CHUNK_SIZE;
	if (offset_end > qwFileSize) offset_end = qwFileSize;
//
	MD5Init(&md);
	for (inde = offset; inde + CHKS_CHUNK <= offset_end; inde += CHKS_CHUNK) {
		MD5Update(&md, map_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
	}
	remainder = (int) (offset_end - inde);
	if (remainder > 0)
		MD5Update(&md, map_ptr64(buf, inde, remainder), remainder);
	MD5Final(&md);
	memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
//
	unmap_file64(buf);
}

TCHAR *comutil::ftim_unis(FILETIME *filetime) {  // Wed, 15 Nov 1995 04:58:08 GMT
	static TCHAR timestr[64];
    SYSTEMTIME st;
    if(!FileTimeToSystemTime(filetime, &st)) return NULL;
    struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
    _tcsftime(timestr, 32, _T("%a, %d %b %Y %H:%M:%S GMT"), &gm);
    return timestr;
}