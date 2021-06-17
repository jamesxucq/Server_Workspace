#include "StdAfx.h"
#include "StrOpt.h"

#include "string.h"
#include "EnvMac.h"

CStrOpt::CStrOpt(void)
{
}

CStrOpt::~CStrOpt(void)
{
}

wchar_t *CStrOpt::AppendPath(wchar_t *dest, const wchar_t *source)
{
	wchar_t PathBuff[MAX_PATH];

	if (dest == NULL || source == NULL)
		return NULL;

	_mwscpy(PathBuff, dest);
	_tcscat_s(PathBuff, _T("/"));
	_tcscat_s(PathBuff, source);
	_mwscpy(dest, PathBuff);

	return dest;
}

wchar_t *CStrOpt::DelLastPath(wchar_t *dest, const wchar_t *source)
{
	LPTSTR pSlash = NULL;

	if (dest == NULL)
		return NULL;

	pSlash = _tcsrchr(dest, _T('/'));
	if (_tcscmp(pSlash+1, source))
		return dest;
	*pSlash = _T('\0');

	return dest;
}


wchar_t *CStrOpt::UniStrSplit(wchar_t *str, const wchar_t *delim)
{
	wchar_t *tokensp;
	static wchar_t *tokenep;

	if(!str){
		if(!tokenep) return NULL;
		tokensp = tokenep + _tcslen(delim);
	}
	else
		tokensp = str;

	tokenep = _tcsstr(tokensp, delim);
	if(!tokenep)
		return tokensp;
	*tokenep = _T('\0');

	return tokensp;
}

wchar_t *CStrOpt::fhpath(wchar_t *path, wchar_t *subdir)
{	
	static wchar_t wdes[MAX_PATH];

	if (*subdir == '~')
		_stprintf_s(wdes, _T("%s%s"), path, subdir+1);
	else
		_stprintf_s(wdes, _T("%s%s"), path, subdir);

	return wdes;
}

wchar_t *CStrOpt::fhpath(wchar_t *fhpath, wchar_t *path, wchar_t *subdir)
{	
	wchar_t wdes[MAX_PATH];

	if(!fhpath) return NULL;

	if (*subdir == '~')
		_stprintf_s(wdes, _T("%s%s"), path, subdir+1);
	else
		_stprintf_s(wdes, _T("%s%s"), path, subdir);
	wcscpy_s(fhpath, wcslen(wdes)+1, wdes);

	return fhpath;
}


wchar_t *CStrOpt::fullpath(wchar_t *fpath, wchar_t *path, char *subdir)
{
	wchar_t wfpath[MAX_PATH];	
	wchar_t wsubdir[MAX_PATH];

	if(!fpath) return NULL;

	CStrCon::UTF8ToUnicode(wsubdir, subdir);
	_stprintf_s(wfpath, _T("%s%s"), path, wsubdir);
	_mwscpy(fpath, wfpath);

	return fpath;
}

char *fullpath(wchar_t *path, wchar_t *subdir) 
{
	static char DesU8[MAX_PATH];	
	wchar_t wdes[MAX_PATH];

	_stprintf_s(wdes, _T("%s%s"), path, subdir);
	CStrCon::UnicodeToUTF8(DesU8, wdes);

	return DesU8;
}

char *fullname(wchar_t *path, char *file) 
{
	static char DesU8[MAX_PATH];	
	
	CStrCon::UnicodeToUTF8(DesU8, path);
	strcat_s(DesU8, file);
	
	return DesU8;
}

char *CStrOpt::fullname(wchar_t *path, wchar_t *file)
{
	static char DesU8[MAX_PATH];	
	wchar_t wdes[MAX_PATH];

	_stprintf_s(wdes, _T("%s/%s"), path, file);
	CStrCon::UnicodeToUTF8(DesU8, wdes);

	return DesU8;
}

wchar_t *DrivePath(CString &Drive) 
{
	static wchar_t wdes[MINSTRLEN];

	CStrCon::CString2TCHAR(wdes, Drive);
	_tcscat_s(wdes, _T("\\"));
	
	return wdes;
}

char *CStrOpt::StrSplit(char *str, const char *delim)
{
	char *tokensp;
	static char *tokenep;

	if(!str){
		if(!tokenep) return NULL;
		tokensp = tokenep + strlen(delim);
	}
	else
		tokensp = str;

	tokenep = strstr(tokensp, delim);
	if(!tokenep) return tokensp;
	*tokenep = '\0';

	return tokensp;
}

void CStrOpt::Split(CStringArray& dest, CString source, CString division)
{
	int pos = 0;
	int pre_pos = 0;

	dest.RemoveAll();
	while(-1 != pos){
		pre_pos = pos;
		pos = source.Find(division,(pos+1));
		dest.Add(source.Mid(pre_pos,(pos-pre_pos)));
	}
}


CString CStrOpt::DirPath(CString &DirName, wchar_t *SubDir)
{
	wchar_t   szDir[MAX_PATH]; 

	if(_T('\\') == DirName.GetAt(DirName.GetLength() - 1))
		_stprintf_s(szDir, _T("%s%s"), DirName, SubDir+1);
	else 
		_stprintf_s(szDir, _T("%s%s"), DirName, SubDir);

	return   CString(szDir);  
}

CStrCon::CStrCon(void)
{
}

CStrCon::~CStrCon(void)
{
}


TCHAR* CStrCon::CString2TCHAR(CString &str)
{
	int iLen = str.GetLength();
	TCHAR* szRs = new TCHAR[iLen];
	lstrcpy(szRs, str.GetBuffer(iLen));
	str.ReleaseBuffer();
	return szRs;
}

wchar_t *CStrCon::CString2TCHAR(wchar_t *wszString, CString &str)
{
	int iLen = str.GetLength();

	lstrcpy(wszString, str.GetBuffer(iLen));
	str.ReleaseBuffer();

	return wszString;
}

char* CStrCon::UnicodeToUTF8(TCHAR *wszString)
{
	// unicode to UTF8
	int u8Len = WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	char* szU8 = new char[u8Len + 1];
	WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), szU8, u8Len, NULL, NULL);
	szU8[u8Len] = '\0';

	return szU8;
}

int CStrCon::UnicodeToUTF8(char* szDesU8, wchar_t* wszString)
{
	// unicode to UTF8
	int u8Len = WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), szDesU8, u8Len, NULL, NULL);
	szDesU8[u8Len] = '\0';

	return u8Len;
}

wchar_t* CStrCon::UTF8ToUnicode(char *szU8)
{
	//UTF8 to Unicode
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), NULL, 0);

	wchar_t* wszString = new wchar_t[wcsLen + 1];
	MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	return wszString;
}

int CStrCon::UTF8ToUnicode(TCHAR *wszString, char *szU8)
{
	//UTF8 to Unicode
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), NULL, 0);

	MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	return wcsLen;
}

char* CStrCon::UnicodeToAnsi(wchar_t* wszString)
{
	// unicode to ansi
	int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	char* szAnsi = new char[ansiLen + 1];
	WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';

	return szAnsi;
}

wchar_t* CStrCon::AnsiToUnicode(char* szAnsi)
{
	// ansi to unicode
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);

	wchar_t* wszString = new wchar_t[wcsLen + 1];
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	return wszString;
}



CTimeEx::CTimeEx(void)
{
}

CTimeEx::~CTimeEx(void)
{
}

