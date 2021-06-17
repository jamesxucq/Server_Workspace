#include "StdAfx.h"
#include "StringUtility.h"

#include "string.h"

TCHAR *nstriutility::append_path(TCHAR *to, const TCHAR *from) {
	if (!to || !from) return NULL;
	//_stprintf_s(szDirectory, MAX_PATH, _T("%s\\%s"), to, from);
	_tcscat_s(to, MAX_PATH, _T("\\"));
	_tcscat_s(to, MAX_PATH, from);

	return to;
}

TCHAR *nstriutility::del_last_path(TCHAR *to, const TCHAR *from) {
	LPTSTR toke = NULL;

	if (!to) return NULL;

	toke = _tcsrchr(to, _T('\\'));
	if (_tcscmp(toke+1, from)) return to;
	toke[0] = _T('\0');

	return to;
}

CString nstriutility::append_path(const CString &dirname, const TCHAR *subdir) {
	TCHAR   szDir[MAX_PATH]; 

	if(_T('\\') == dirname.GetAt(dirname.GetLength() - 1))
		_stprintf_s(szDir, _T("%s%s"), dirname, subdir+1);
	else _stprintf_s(szDir, _T("%s%s"), dirname, subdir);

	return   CString(szDir);  
}

TCHAR *nstriutility::wstrsplit(TCHAR *str, const TCHAR *delim, TCHAR **context) {
	TCHAR *toksp, *tokep;

	if(!delim || !context) return NULL;
	tokep = *context;

	if(str) toksp = str;
	else {
		if(!tokep) return NULL;
		toksp = tokep + _tcslen(delim);
	}

	tokep = _tcsstr(toksp, delim);
	*context = tokep;
	if(tokep) tokep[0] = _T('\0');

	return toksp;
}

char *nstriutility::split_status(char *str) {
	char *toksp, *tokep;

	toksp = strchr(str, ' ');
	if (!toksp) return NULL;
	*toksp = '\0';

	++toksp;
	tokep = strrchr(toksp, ' ');
	if (!tokep) return NULL;
	*tokep = '\0';

	return toksp;
}

TCHAR *nstriutility::wsplit_status(TCHAR *str) {
	TCHAR *toksp, *tokep;

	toksp = _tcschr(str, _T(' '));
	if (!toksp) return NULL;
	toksp[0] = _T('\0');

	++toksp;
	tokep = _tcsrchr(toksp, _T(' '));
	if (!tokep) return NULL;
	tokep[0] = _T('\0');

	return toksp;
}

char *nstriutility::split_value(char *str) {
	char *toksp;

	toksp = strstr(str, ": ");
	if (!toksp) return NULL;
	toksp[0] = '\0';

	return toksp + 2;
}

TCHAR *nstriutility::wsplit_value(TCHAR *str) {
	TCHAR *toksp;

	toksp = _tcsstr(str, _T(": "));
	if (!toksp) return NULL;
	toksp[0] = _T('\0');

	return toksp + 2;
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
	char *toksp, *tokep;

	if(!delim || !context) return NULL;
	tokep = *context;

	if(str) toksp = str;
	else {
		if(!tokep) return NULL;
		toksp = tokep + strlen(delim);
	}

	tokep = strstr(toksp, delim);
	*context = tokep;
	if(tokep) *tokep = '\0';

	return toksp;
}

VOID nstriutility::wsplit(CStringArray& to, CString from, CString division) {
	int toke = 0;
	int previous = 0;

	to.RemoveAll();
	while(-1 != toke){
		previous = toke;
		toke = from.Find(division,(toke+1));
		to.Add(from.Mid(previous,(toke-previous)));
	}
}

TCHAR *nstriutility::path_windows_unix(TCHAR *winpath) {
	TCHAR *toke;

	toke = winpath;
	do {
		toke = _tcschr(toke, _T('\\'));
		if(toke) *toke = _T('/');
	}while(toke);

	return winpath;
}

TCHAR *nstriutility::path_unix_windows(TCHAR *unixpath) {
	TCHAR *toke;

	toke = unixpath;
	do {
		toke = _tcschr(toke, _T('/'));
		if(toke) *toke = _T('\\');
	}while(toke);

	return unixpath;
}

TCHAR *nstriutility::GetDriveRoot(TCHAR *szDriveRoot, const TCHAR *szDirectoryPath) {
	// static TCHAR szDriveRoot[MAX_PATH];

	if(!szDriveRoot || !szDirectoryPath) return NULL;
	_tcsncpy_s(szDriveRoot, MAX_PATH, szDirectoryPath, 3);

	return szDriveRoot;
}


char *nstriutility::name_value(char **name, char *str, const char delim) {
	char *toksp;
	static char *tokep;

	if (!str) {
		if (!tokep) return NULL;
		toksp = tokep + 1;
	} else toksp = str;

	tokep = strchr(toksp, delim);
	if (tokep) *tokep = '\0';

	if (name) *name = toksp;
	char *value = strchr(toksp, '=');
	if (value) {
		*value = '\0';
		++value;
	} else value = toksp;

	return value;
}


TCHAR* nstriconv::cstring_tchar(CString &str) {
	int iLength = str.GetLength();
	TCHAR* szRs = new TCHAR[iLength];
	lstrcpy(szRs, str.GetBuffer(iLength));
	str.ReleaseBuffer();
	return szRs;
}

TCHAR *nstriconv::cstring_tchar(TCHAR *unicode, CString &str) {
	int iLength = str.GetLength();

	lstrcpy(unicode, str.GetBuffer(iLength));
	str.ReleaseBuffer();

	return unicode;
}

char* nstriconv::unicode_utf8(const TCHAR *unicode) {
	// unicode to UTF8
	int u8length = WideCharToMultiByte(CP_UTF8, NULL, unicode, (int)wcslen(unicode), NULL, 0, NULL, NULL);

	char* utf8 = new char[u8length + 1];
	WideCharToMultiByte(CP_UTF8, NULL, unicode, (int)wcslen(unicode), utf8, u8length, NULL, NULL);
	utf8[u8length] = '\0';

	return utf8;
}

int nstriconv::unicode_utf8(char* utf8, const TCHAR* unicode) {
	// unicode to UTF8
	int u8length = WideCharToMultiByte(CP_UTF8, NULL, unicode, (int)wcslen(unicode), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_UTF8, NULL, unicode, (int)wcslen(unicode), utf8, u8length, NULL, NULL);
	utf8[u8length] = '\0';

	return u8length;
}

TCHAR* nstriconv::utf8_unicode(const char *utf8) {
	//UTF8 to Unicode
	int wlength = MultiByteToWideChar(CP_UTF8, NULL, utf8, (int)strlen(utf8), NULL, 0);

	TCHAR* unicode = new TCHAR[wlength + 1];
	MultiByteToWideChar(CP_UTF8, NULL, utf8, (int)strlen(utf8), unicode, wlength);
	unicode[wlength] = _T('\0');

	return unicode;
}

int nstriconv::utf8_unicode(TCHAR *unicode, const char *utf8) {
	//UTF8 to Unicode
	int wlength = MultiByteToWideChar(CP_UTF8, NULL, utf8, (int)strlen(utf8), NULL, 0);

	MultiByteToWideChar(CP_UTF8, NULL, utf8, (int)strlen(utf8), unicode, wlength);
	unicode[wlength] = _T('\0');

	return wlength;
}

char* nstriconv::unicode_ansi(const TCHAR* unicode) {
	// unicode to ansi
	int alength = WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), NULL, 0, NULL, NULL);

	char* ansi = new char[alength + 1];
	WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), ansi, alength, NULL, NULL);
	ansi[alength] = '\0';

	return ansi;
}

char* nstriconv::unicode_ansi_stc(const TCHAR* unicode) {
	static char ansi[MAX_PATH];
	// unicode to ansi
	int alength = WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), ansi, alength, NULL, NULL);
	ansi[alength] = '\0';

	return ansi;
}

char* nstriconv::unicode_ansi(char* ansi, const TCHAR* unicode) {
	// unicode to ansi
	int alength = WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, unicode, (int)wcslen(unicode), ansi, alength, NULL, NULL);
	ansi[alength] = '\0';

	return ansi;
}

TCHAR* nstriconv::ansi_unicode(const char* ansi) {
	// ansi to unicode
	int wlength = MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), NULL, 0);

	TCHAR* unicode = new TCHAR[wlength + 1];
	MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), unicode, wlength);
	unicode[wlength] = _T('\0');

	return unicode;
}

TCHAR* nstriconv::ansi_unicode(TCHAR* unicode, const char* ansi) {
	// ansi to unicode
	int wlength = MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), NULL, 0);
	MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), unicode, wlength);
	unicode[wlength] = _T('\0');

	return unicode;
}

