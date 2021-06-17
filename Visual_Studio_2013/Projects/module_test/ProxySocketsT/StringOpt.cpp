#include "StdAfx.h"
#include "StringOpt.h"

#include "string.h"

wchar_t *NStringOpt::appendpath(wchar_t *dest, const wchar_t *source)
{
	if (dest == NULL || source == NULL) return NULL;
	_stprintf_s(dest, MAX_PATH, _T("%s\\%s"), dest, source);

	return dest;
}

wchar_t *NStringOpt::dellastpath(wchar_t *dest, const wchar_t *source)
{
	LPTSTR pSlash = NULL;

	if (dest == NULL) return NULL;

	pSlash = _tcsrchr(dest, _T('\\'));
	if (_tcscmp(pSlash+1, source)) return dest;
	*pSlash = _T('\0');

	return dest;
}

CString NStringOpt::appendpath(const CString &dirname, const wchar_t *subdir)
{
	wchar_t   szDir[MAX_PATH]; 

	if(_T('\\') == dirname.GetAt(dirname.GetLength() - 1))
		_stprintf_s(szDir, _T("%s%s"), dirname, subdir+1);
	else _stprintf_s(szDir, _T("%s%s"), dirname, subdir);

	return   CString(szDir);  
}

wchar_t *NStringOpt::wstrsplit(wchar_t *str, const wchar_t *delim, wchar_t **context)
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

/*
wchar_t *NStringOpt::get_homepath(const wchar_t *path, const wchar_t *subdir)
{	
	static wchar_t wdes[MAX_PATH];

	if (*subdir == '~') 
		_stprintf_s(wdes, _T("%s%s"), path, subdir+1);
	else _stprintf_s(wdes, _T("%s%s"), path, subdir);

	return wdes;
}
*/

wchar_t *NStringOpt::get_homepath(wchar_t *homepath, const wchar_t *path, const wchar_t *subdir)
{	
	wchar_t wdes[MAX_PATH];

	if(!homepath) return NULL;

	if (*subdir == '~') 
		_stprintf_s(wdes, _T("%s%s"), path, subdir+1);
	else _stprintf_s(wdes, _T("%s%s"), path, subdir);
	_tcscpy_s(homepath, wcslen(wdes)+1, wdes);

	return homepath;
}

/*
wchar_t *NStringOpt::fullpath(const wchar_t *path, const wchar_t *subdir) 
{
	static wchar_t wdes[MAX_PATH];	
	_stprintf_s(wdes, _T("%s%s"), path, subdir);
	return wdes;
}
*/

wchar_t *NStringOpt::fullpath(wchar_t *fpath, const wchar_t *path, const wchar_t *subdir)
{
	if(!fpath) return NULL;
	_stprintf_s(fpath, MAX_PATH, _T("%s%s"), path, subdir);
	return fpath;
}

wchar_t *NStringOpt::fullname(wchar_t *fname, const wchar_t *path, const wchar_t *file)
{
	if(!fname) return NULL;
	_stprintf_s(fname, MAX_PATH, _T("%s\\%s"), path, file);
	return fname;
}

wchar_t *NStringOpt::GetURL(wchar_t *sLinkTxt, const wchar_t *sCaption, const wchar_t *sURL) 
{
	// static wchar_t sLinkTxt[MAX_PATH];

	if(!sLinkTxt || !sCaption || !sURL) return NULL;
	_stprintf_s(sLinkTxt, MAX_PATH, _T("<a href=\"%s\">%s</a>"), sURL, sCaption);

	return sLinkTxt;
}

char *NStringOpt::strsplit(char *str, const char *delim, char **context)
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

void NStringOpt::wsplit(CStringArray& dest, CString source, CString division)
{
	int position = 0;
	int previous = 0;

	dest.RemoveAll();
	while(-1 != position){
		previous = position;
		position = source.Find(division,(position+1));
		dest.Add(source.Mid(previous,(position-previous)));
	}
}

wchar_t *NStringOpt::winpath_unix(wchar_t *winpath)
{
	wchar_t *position;

	position = winpath;
	do {
		position = _tcschr(position, '\\');
		if(position) *position = '/';
	}while(position);

	return winpath;
}

wchar_t *NStringOpt::unixpath_windows(wchar_t *unixpath)
{
	wchar_t *position;

	position = unixpath;
	do {
		position = _tcschr(position, '/');
		if(position) *position = '\\';
	}while(position);

	return unixpath;
}

wchar_t *NStringOpt::GetDriveRoot(wchar_t *sDriveRoot, const wchar_t *sDirectoryPath)
{
	// static wchar_t sDriveRoot[MAX_PATH];

	if(!sDriveRoot || !sDirectoryPath) return NULL;
	_tcsncpy_s(sDriveRoot, MAX_PATH, sDirectoryPath, 3);

	return sDriveRoot;
}


TCHAR* NStringCon::cstring_tchar(CString &str)
{
	int iLength = str.GetLength();
	TCHAR* szRs = new TCHAR[iLength];
	lstrcpy(szRs, str.GetBuffer(iLength));
	str.ReleaseBuffer();
	return szRs;
}

wchar_t *NStringCon::cstring_tchar(wchar_t *wszString, CString &str)
{
	int iLength = str.GetLength();

	lstrcpy(wszString, str.GetBuffer(iLength));
	str.ReleaseBuffer();

	return wszString;
}

char* NStringCon::unicode_utf8(const TCHAR *wszString)
{
	// unicode to UTF8
	int u8Length = WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	char* szU8 = new char[u8Length + 1];
	WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), szU8, u8Length, NULL, NULL);
	szU8[u8Length] = '\0';

	return szU8;
}

int NStringCon::unicode_utf8(char* szDesU8, const wchar_t* wszString)
{
	// unicode to UTF8
	int u8Length = WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), szDesU8, u8Length, NULL, NULL);
	szDesU8[u8Length] = '\0';

	return u8Length;
}

wchar_t* NStringCon::utf8_unicode(const char *szU8)
{
	//UTF8 to Unicode
	int wcsLength = MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), NULL, 0);

	wchar_t* wszString = new wchar_t[wcsLength + 1];
	MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), wszString, wcsLength);
	wszString[wcsLength] = '\0';

	return wszString;
}

int NStringCon::utf8_unicode(TCHAR *wszString, const char *szU8)
{
	//UTF8 to Unicode
	int wcsLength = MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), NULL, 0);

	MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), wszString, wcsLength);
	wszString[wcsLength] = '\0';

	return wcsLength;
}

char* NStringCon::unicode_ansi(const wchar_t* wszString)
{
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	char* szAnsi = new char[ansiLength + 1];
	WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

char* NStringCon::unicode_ansi_static(const wchar_t* wszString)
{
	static char szAnsi[MAX_PATH];
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

char* NStringCon::unicode_ansi(char* szAnsi, const wchar_t* wszString)
{
	// unicode to ansi
	int ansiLength = ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szAnsi, ansiLength, NULL, NULL);
	szAnsi[ansiLength] = '\0';

	return szAnsi;
}

wchar_t* NStringCon::ansi_unicode(const char* szAnsi)
{
	// ansi to unicode
	int wcsLength = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);

	wchar_t* wszString = new wchar_t[wcsLength + 1];
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), wszString, wcsLength);
	wszString[wcsLength] = '\0';

	return wszString;
}

wchar_t* NStringCon::ansi_unicode(wchar_t* wszString, const char* szAnsi)
{
	// ansi to unicode
	int wcsLength = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), wszString, wcsLength);
	wszString[wcsLength] = '\0';

	return wszString;
}

