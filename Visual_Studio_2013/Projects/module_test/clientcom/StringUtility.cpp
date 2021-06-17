#include "StdAfx.h"
#include "StringUtility.h"

#include "string.h"

TCHAR *nstriutility::append_path(TCHAR *desti, const TCHAR *source) {
	if (!desti || !source) return NULL;
	//_stprintf_s(szDirectory, MAX_PATH, _T("%s\\%s"), desti, source);
	_tcscat_s(desti, MAX_PATH, _T("\\"));
	_tcscat_s(desti, MAX_PATH, source);

	return desti;
}

TCHAR *nstriutility::del_last_path(TCHAR *desti, const TCHAR *source) {
	LPTSTR token = NULL;

	if (!desti) return NULL;

	token = _tcsrchr(desti, _T('\\'));
	if (_tcscmp(token+1, source)) return desti;
	token[0] = _T('\0');

	return desti;
}

CString nstriutility::append_path(const CString &dirname, const TCHAR *subdir) {
	TCHAR   szDir[MAX_PATH]; 

	if(_T('\\') == dirname.GetAt(dirname.GetLength() - 1))
		_stprintf_s(szDir, _T("%s%s"), dirname, subdir+1);
	else _stprintf_s(szDir, _T("%s%s"), dirname, subdir);

	return   CString(szDir);  
}

TCHAR *nstriutility::wstrsplit(TCHAR *str, const TCHAR *delim, TCHAR **context) {
	TCHAR *tokensp, *tokenep;

	if(!delim || !context) return NULL;
	tokenep = *context;

	if(str) tokensp = str;
	else {
		if(!tokenep) return NULL;
		tokensp = tokenep + _tcslen(delim);
	}

	tokenep = _tcsstr(tokensp, delim);
	*context = tokenep;
	if(tokenep) tokenep[0] = _T('\0');

	return tokensp;
}

char *nstriutility::split_status(char *str) {
	char *tokensp, *tokenep;

	tokensp = strchr(str, ' ');
	if (!tokensp) return NULL;
	*tokensp = '\0';

	++tokensp;
	tokenep = strrchr(tokensp, ' ');
	if (!tokenep) return NULL;
	*tokenep = '\0';

	return tokensp;
}

TCHAR *nstriutility::wsplit_status(TCHAR *str) {
	TCHAR *tokensp, *tokenep;

	tokensp = _tcschr(str, _T(' '));
	if (!tokensp) return NULL;
	tokensp[0] = _T('\0');

	++tokensp;
	tokenep = _tcsrchr(tokensp, _T(' '));
	if (!tokenep) return NULL;
	tokenep[0] = _T('\0');

	return tokensp;
}

char *nstriutility::split_value(char *str) {
	char *tokensp;

	tokensp = strstr(str, ": ");
	if (!tokensp) return NULL;
	tokensp[0] = '\0';

	return tokensp + 2;
}

TCHAR *nstriutility::wsplit_value(TCHAR *str) {
	TCHAR *tokensp;

	tokensp = _tcsstr(str, _T(": "));
	if (!tokensp) return NULL;
	tokensp[0] = _T('\0');

	return tokensp + 2;
}

/*
TCHAR *nstriutility::get_name(const TCHAR *location, const TCHAR *subdir) {
static TCHAR wdesti[MAX_PATH];

if (*subdir == '~') 
_stprintf_s(wdesti, _T("%s%s"), location, subdir+1);
else _stprintf_s(wdesti, _T("%s%s"), location, subdir);

return wdesti;
}
*/

TCHAR *nstriutility::get_name(TCHAR *name, const TCHAR *location, const TCHAR *subdir) {
	TCHAR wdesti[MAX_PATH];

	if(!name) return NULL;

	if (*subdir == _T('~')) 
		_stprintf_s(wdesti, _T("%s%s"), location, subdir+1);
	else _stprintf_s(wdesti, _T("%s%s"), location, subdir);
	_tcscpy_s(name, wcslen(wdesti)+1, wdesti);

	return name;
}

/*
TCHAR *nstriutility::full_path(const TCHAR *location, const TCHAR *subdir) {
static TCHAR wdesti[MAX_PATH];	
_stprintf_s(wdesti, _T("%s%s"), location, subdir);
return wdesti;
}
*/

TCHAR *nstriutility::full_path(TCHAR *fpath, const TCHAR *path, const TCHAR *subdir) {
	if(!fpath) return NULL;
	_stprintf_s(fpath, MAX_PATH, _T("%s%s"), path, subdir);
	return fpath;
}

TCHAR *nstriutility::full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file) {
	if(!file_name) return NULL;
	_stprintf_s(file_name, MAX_PATH, _T("%s\\%s"), path, file);
	return file_name;
}

TCHAR *nstriutility::GetURL(TCHAR *szLinkTxt, const TCHAR *szCaption, const TCHAR *szURL) {
	// static TCHAR szLinkTxt[MAX_PATH];

	if(!szLinkTxt || !szCaption || !szURL) return NULL;
	_stprintf_s(szLinkTxt, MAX_PATH, _T("<a href=\"%s\">%s</a>"), szURL, szCaption);

	return szLinkTxt;
}

char *nstriutility::strsplit(char *str, const char *delim, char **context) {
	char *tokensp, *tokenep;

	if(!delim || !context) return NULL;
	tokenep = *context;

	if(str) tokensp = str;
	else {
		if(!tokenep) return NULL;
		tokensp = tokenep + strlen(delim);
	}

	tokenep = strstr(tokensp, delim);
	*context = tokenep;
	if(tokenep) *tokenep = '\0';

	return tokensp;
}

void nstriutility::wsplit(CStringArray& desti, CString source, CString division) {
	int position = 0;
	int previous = 0;

	desti.RemoveAll();
	while(-1 != position){
		previous = position;
		position = source.Find(division,(position+1));
		desti.Add(source.Mid(previous,(position-previous)));
	}
}

TCHAR *nstriutility::path_windows_unix(TCHAR *winpath) {
	TCHAR *position;

	position = winpath;
	do {
		position = _tcschr(position, _T('\\'));
		if(position) *position = _T('/');
	}while(position);

	return winpath;
}

TCHAR *nstriutility::path_unix_windows(TCHAR *unixpath) {
	TCHAR *position;

	position = unixpath;
	do {
		position = _tcschr(position, _T('/'));
		if(position) *position = _T('\\');
	}while(position);

	return unixpath;
}

TCHAR *nstriutility::GetDriveRoot(TCHAR *szDriveRoot, const TCHAR *szDirectoryPath) {
	// static TCHAR szDriveRoot[MAX_PATH];

	if(!szDriveRoot || !szDirectoryPath) return NULL;
	_tcsncpy_s(szDriveRoot, MAX_PATH, szDirectoryPath, 3);

	return szDriveRoot;
}


char *nstriutility::name_value(char **name, char *str, const char delim) {
    char *tokensp;
    static char *tokenep;

    if (!str) {
        if (!tokenep) return NULL;
        tokensp = tokenep + 1;
    } else tokensp = str;

    tokenep = strchr(tokensp, delim);
    if (tokenep) *tokenep = '\0';

    if (name) *name = tokensp;
    char *value = strchr(tokensp, '=');
    if (value) {
        *value = '\0';
        ++value;
    } else value = tokensp;

    return value;
}


TCHAR* nstriconv::cstring_tchar(CString &str) {
	int iLength = str.GetLength();
	TCHAR* szRs = new TCHAR[iLength];
	lstrcpy(szRs, str.GetBuffer(iLength));
	str.ReleaseBuffer();
	return szRs;
}

TCHAR *nstriconv::cstring_tchar(TCHAR *szUnicode, CString &str) {
	int iLength = str.GetLength();

	lstrcpy(szUnicode, str.GetBuffer(iLength));
	str.ReleaseBuffer();

	return szUnicode;
}

char* nstriconv::unicode_utf8(const TCHAR *szUnicode) {
	// unicode to UTF8
	int u8Length = WideCharToMultiByte(CP_UTF8, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);

	char* szUTF8 = new char[u8Length + 1];
	WideCharToMultiByte(CP_UTF8, NULL, szUnicode, (int)wcslen(szUnicode), szUTF8, u8Length, NULL, NULL);
	szUTF8[u8Length] = '\0';

	return szUTF8;
}

int nstriconv::unicode_utf8(char* szDestiUTF8, const TCHAR* szUnicode) {
	// unicode to UTF8
	int u8Length = WideCharToMultiByte(CP_UTF8, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_UTF8, NULL, szUnicode, (int)wcslen(szUnicode), szDestiUTF8, u8Length, NULL, NULL);
	szDestiUTF8[u8Length] = '\0';

	return u8Length;
}

TCHAR* nstriconv::utf8_unicode(const char *szUTF8) {
	//UTF8 to Unicode
	int wcsLength = MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), NULL, 0);

	TCHAR* szUnicode = new TCHAR[wcsLength + 1];
	MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), szUnicode, wcsLength);
	szUnicode[wcsLength] = _T('\0');

	return szUnicode;
}

int nstriconv::utf8_unicode(TCHAR *szUnicode, const char *szUTF8) {
	//UTF8 to Unicode
	int wcsLength = MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), NULL, 0);

	MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), szUnicode, wcsLength);
	szUnicode[wcsLength] = _T('\0');

	return wcsLength;
}

char* nstriconv::unicode_ansi(const TCHAR* szUnicode) {
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);

	char* szAnsi = new char[ansiLength + 1];
	WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

char* nstriconv::unicode_ansi_stc(const TCHAR* szUnicode) {
	static char szAnsi[MAX_PATH];
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

char* nstriconv::unicode_ansi(char* szAnsi, const TCHAR* szUnicode) {
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

TCHAR* nstriconv::ansi_unicode(const char* szAnsi) {
	// ansi to unicode
	int wcsLength = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);

	TCHAR* szUnicode = new TCHAR[wcsLength + 1];
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), szUnicode, wcsLength);
	szUnicode[wcsLength] = _T('\0');

	return szUnicode;
}

TCHAR* nstriconv::ansi_unicode(TCHAR* szUnicode, const char* szAnsi) {
	// ansi to unicode
	int wcsLength = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), szUnicode, wcsLength);
	szUnicode[wcsLength] = _T('\0');

	return szUnicode;
}

