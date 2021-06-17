#include "StdAfx.h"
#include "StringUtility.h"

#include "string.h"

wchar_t *striutil::append_path(wchar_t *desti, const wchar_t *source)
{
	if (!desti || !source) return NULL;
	//_stprintf_s(szDirectory, MAX_PATH+1, _T("%s\\%s"), desti, source);
	_tcscat_s(desti, MAX_PATH+1, _T("\\"));
	_tcscat_s(desti, MAX_PATH+1, source);

	return desti;
}

wchar_t *striutil::del_last_path(wchar_t *desti, const wchar_t *source)
{
	LPTSTR token = NULL;

	if (!desti) return NULL;

	token = _tcsrchr(desti, _T('\\'));
	if (_tcscmp(token+1, source)) return desti;
	*token = _T('\0');

	return desti;
}

CString striutil::append_path(const CString &dirname, const wchar_t *subdir)
{
	wchar_t   szDir[MAX_PATH+1]; 

	if(_T('\\') == dirname.GetAt(dirname.GetLength() - 1))
		_stprintf_s(szDir, _T("%s%s"), dirname, subdir+1);
	else _stprintf_s(szDir, _T("%s%s"), dirname, subdir);

	return   CString(szDir);  
}

wchar_t *striutil::wstrsplit(wchar_t *str, const wchar_t *delim, wchar_t **context)
{
	wchar_t *tokensp, *tokenep;

	if(!delim || !context) return NULL;
	tokenep = *context;

	if(str) tokensp = str;
	else {
		if(!tokenep) return NULL;
		tokensp = tokenep + _tcslen(delim);
	}

	tokenep = _tcsstr(tokensp, delim);
	*context = tokenep;
	if(tokenep) *tokenep = _T('\0');

	return tokensp;
}

char *striutil::split_value(char *str) {
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

wchar_t *striutil::wsplit_value(wchar_t *str){
	wchar_t *tokensp, *tokenep;

	tokensp = _tcschr(str, _T(' '));
	if (!tokensp) return NULL;
	*tokensp = _T('\0');

	++tokensp;
	tokenep = _tcsrchr(tokensp, _T(' '));
	if (!tokenep) return NULL;
	*tokenep = _T('\0');

	return tokensp;
}

char *striutil::split_line(char *str) {
	char *tokensp;

	tokensp = strstr(str, ": ");
	if (!tokensp) return NULL;
	*tokensp = '\0';

	return tokensp + 2;
}

wchar_t *striutil::wsplit_line(wchar_t *str) {
	wchar_t *tokensp;

	tokensp = _tcsstr(str, _T(": "));
	if (!tokensp) return NULL;
	*tokensp = _T('\0');

	return tokensp + 2;
}

wchar_t *striutil::get_name(wchar_t *name, const wchar_t *location, const wchar_t *subdir)
{	
	wchar_t wdesti[MAX_PATH+1];

	if(!name) return NULL;

	if (*subdir == _T('~')) 
		_stprintf_s(wdesti, _T("%s%s"), location, subdir+1);
	else _stprintf_s(wdesti, _T("%s%s"), location, subdir);
	_tcscpy_s(name, wcslen(wdesti)+1, wdesti);

	return name;
}

wchar_t *striutil::full_path(wchar_t *fpath, const wchar_t *path, const wchar_t *subdir)
{
	if(!fpath) return NULL;
	_stprintf_s(fpath, MAX_PATH+1, _T("%s%s"), path, subdir);
	return fpath;
}

wchar_t *striutil::full_name(wchar_t *fname, const wchar_t *path, const wchar_t *file)
{
	if(!fname) return NULL;
	_stprintf_s(fname, MAX_PATH+1, _T("%s\\%s"), path, file);
	return fname;
}

wchar_t *striutil::GetURL(wchar_t *szLinkTxt, const wchar_t *szCaption, const wchar_t *szURL) 
{
	// static wchar_t szLinkTxt[MAX_PATH+1];

	if(!szLinkTxt || !szCaption || !szURL) return NULL;
	_stprintf_s(szLinkTxt, MAX_PATH+1, _T("<a href=\"%s\">%s</a>"), szURL, szCaption);

	return szLinkTxt;
}

char *striutil::strsplit(char *str, const char *delim, char **context)
{
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

void striutil::wsplit(CStringArray& desti, CString source, CString division)
{
	int position = 0;
	int previous = 0;

	desti.RemoveAll();
	while(-1 != position){
		previous = position;
		position = source.Find(division,(position+1));
		desti.Add(source.Mid(previous,(position-previous)));
	}
}

wchar_t *striutil::path_windows_unix(wchar_t *winpath)
{
	wchar_t *position;

	position = winpath;
	do {
		position = _tcschr(position, _T('\\'));
		if(position) *position = _T('/');
	}while(position);

	return winpath;
}

wchar_t *striutil::path_unix_windows(wchar_t *unixpath)
{
	wchar_t *position;

	position = unixpath;
	do {
		position = _tcschr(position, _T('/'));
		if(position) *position = _T('\\');
	}while(position);

	return unixpath;
}

wchar_t *striutil::GetDriveRoot(wchar_t *szDriveRoot, const wchar_t *szDirectoryPath)
{
	// static wchar_t szDriveRoot[MAX_PATH+1];

	if(!szDriveRoot || !szDirectoryPath) return NULL;
	_tcsncpy_s(szDriveRoot, MAX_PATH+1, szDirectoryPath, 3);

	return szDriveRoot;
}

int striutil::address_split(serv_address *address, char * addr_txt) {
	char *token = NULL;

	if (!address || !addr_txt) return -1;
	memset(address, '\0', sizeof (serv_address));

	token = strchr(addr_txt, ':');
	if (token){
		strncpy_s(address->sin_addr, addr_txt, token - addr_txt);
		address->sin_port = atoi(++token);
	}else strcpy_s(address->sin_addr, addr_txt);

	return 0;
}

int striutil::port_split(int *port_array, int array_inde, char *port_txt) {
	char *toksp = NULL, *tokep = NULL;
	int inde;
//
	if (!port_txt || !port_array) return -1;
	for(inde = 0; array_inde > inde; inde++) 
		port_array[inde] = 0;
//
	toksp = port_txt;
	tokep = strchr(toksp, ';');
	for(inde = 0; tokep; inde++) {
		*tokep = '\0';
		port_array[inde] = atoi(toksp);
		toksp = tokep + 1;
		tokep = strchr(toksp, ';');
	}
	port_array[inde] = atoi(toksp);
//
	return 0;
}

TCHAR* striconv::cstring_tchar(CString &str)
{
	int iLength = str.GetLength();
	TCHAR* szRs = new TCHAR[iLength];
	lstrcpy(szRs, str.GetBuffer(iLength));
	str.ReleaseBuffer();
	return szRs;
}

wchar_t *striconv::cstring_tchar(wchar_t *szUnicode, CString &str)
{
	int iLength = str.GetLength();

	lstrcpy(szUnicode, str.GetBuffer(iLength));
	str.ReleaseBuffer();

	return szUnicode;
}

char* striconv::unicode_utf8(const TCHAR *szUnicode)
{
	// unicode to UTF8
	int u8Length = WideCharToMultiByte(CP_UTF8, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);

	char* szUTF8 = new char[u8Length + 1];
	WideCharToMultiByte(CP_UTF8, NULL, szUnicode, (int)wcslen(szUnicode), szUTF8, u8Length, NULL, NULL);
	szUTF8[u8Length] = '\0';

	return szUTF8;
}

int striconv::unicode_utf8(char* szDestiUTF8, const wchar_t* szUnicode)
{
	// unicode to UTF8
	int u8Length = WideCharToMultiByte(CP_UTF8, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_UTF8, NULL, szUnicode, (int)wcslen(szUnicode), szDestiUTF8, u8Length, NULL, NULL);
	szDestiUTF8[u8Length] = '\0';

	return u8Length;
}

wchar_t* striconv::utf8_unicode(const char *szUTF8)
{
	//UTF8 to Unicode
	int wcsLength = MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), NULL, 0);

	wchar_t* szUnicode = new wchar_t[wcsLength + 1];
	MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), szUnicode, wcsLength);
	szUnicode[wcsLength] = _T('\0');

	return szUnicode;
}

int striconv::utf8_unicode(TCHAR *szUnicode, const char *szUTF8)
{
	//UTF8 to Unicode
	int wcsLength = MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), NULL, 0);

	MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), szUnicode, wcsLength);
	szUnicode[wcsLength] = _T('\0');

	return wcsLength;
}

char* striconv::unicode_ansi(const wchar_t* szUnicode)
{
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);

	char* szAnsi = new char[ansiLength + 1];
	WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

char* striconv::unicode_ansi_stc(const wchar_t* szUnicode)
{
	static char szAnsi[MAX_PATH+1];
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

char* striconv::unicode_ansi(char* szAnsi, const wchar_t* szUnicode)
{
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, szUnicode, (int)wcslen(szUnicode), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

wchar_t* striconv::ansi_unicode(const char* szAnsi)
{
	// ansi to unicode
	int wcsLength = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);

	wchar_t* szUnicode = new wchar_t[wcsLength + 1];
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), szUnicode, wcsLength);
	szUnicode[wcsLength] = _T('\0');

	return szUnicode;
}

wchar_t* striconv::ansi_unicode(wchar_t* szUnicode, const char* szAnsi)
{
	// ansi to unicode
	int wcsLength = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), szUnicode, wcsLength);
	szUnicode[wcsLength] = _T('\0');

	return szUnicode;
}

