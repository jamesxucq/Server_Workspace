#include "StdAfx.h"
#include "StringOpt.h"

#include "string.h"

CStringOpt::CStringOpt(void)
{
}

CStringOpt::~CStringOpt(void)
{
}

wchar_t *CStringOpt::UAppendPath(wchar_t *dest, const wchar_t *source)
{
	if (dest == NULL || source == NULL) return NULL;
	_stprintf_s(dest, MAX_PATH, _T("%s\\%s"), dest, source);

	return dest;
}

wchar_t *CStringOpt::UDelLastPath(wchar_t *dest, const wchar_t *source)
{
	LPTSTR pSlash = NULL;

	if (dest == NULL) return NULL;

	pSlash = _tcsrchr(dest, _T('\\'));
	if (_tcscmp(pSlash+1, source)) return dest;
	*pSlash = _T('\0');

	return dest;
}

CString CStringOpt::WAppendPath(CString &DirName, wchar_t *subdir)
{
	wchar_t   szDir[MAX_PATH]; 

	if(_T('\\') == DirName.GetAt(DirName.GetLength() - 1))
		_stprintf_s(szDir, _T("%s%s"), DirName, subdir+1);
	else _stprintf_s(szDir, _T("%s%s"), DirName, subdir);

	return   CString(szDir);  
}

wchar_t *CStringOpt::UStrSplit(wchar_t *str, const wchar_t *delim)
{
	wchar_t *tokensp;
	static wchar_t *tokenep;

	if(!str){
		if(!tokenep) return NULL;
		tokensp = tokenep + _tcslen(delim);
	}
	else tokensp = str;

	tokenep = _tcsstr(tokensp, delim);
	if(!tokenep) return tokensp;
	*tokenep = _T('\0');

	return tokensp;
}

wchar_t *CStringOpt::homepath(wchar_t *path, wchar_t *subdir)
{	
	static wchar_t wdes[MAX_PATH];

	if (*subdir == '~') 
		_stprintf_s(wdes, _T("%s%s"), path, subdir+1);
	else _stprintf_s(wdes, _T("%s%s"), path, subdir);

	return wdes;
}

wchar_t *CStringOpt::homepath(wchar_t *homepath, wchar_t *path, wchar_t *subdir)
{	
	wchar_t wdes[MAX_PATH];

	if(!homepath) return NULL;

	if (*subdir == '~') 
		_stprintf_s(wdes, _T("%s%s"), path, subdir+1);
	else _stprintf_s(wdes, _T("%s%s"), path, subdir);
	wcscpy_s(homepath, wcslen(wdes)+1, wdes);

	return homepath;
}

//wchar_t *CStringOpt::fullname_u(wchar_t *fname, wchar_t *path, wchar_t *file)
//wchar_t *CStringOpt::fullname(wchar_t *fname, wchar_t *path, wchar_t *file)


char *CStringOpt::StrSplit(char *str, const char *delim)
{
	char *tokensp;
	static char *tokenep;

	if(!str){
		if(!tokenep) return NULL;
		tokensp = tokenep + strlen(delim);
	}
	else tokensp = str;

	tokenep = strstr(tokensp, delim);
	if(!tokenep) return tokensp;
	*tokenep = '\0';

	return tokensp;
}

void CStringOpt::Split(CStringArray& dest, CString source, CString division)
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

wchar_t *CStringOpt::WindowsPath2Unix(wchar_t *path_w)
{
	wchar_t *pPos;

	pPos = path_w;
	do {
		pPos = _tcschr(pPos, '\\');
		if(pPos) *pPos = '/';
	}while(pPos);

	return path_w;
}

wchar_t *CStringOpt::UnixPath2Windows(wchar_t *path_u)
{
	wchar_t *pPos;

	pPos = path_u;
	do {
		pPos = _tcschr(pPos, '/');
		if(pPos) *pPos = '\\';
	}while(pPos);

	return path_u;
}

wchar_t *CStringOpt::GetDriveRoot(wchar_t *sDirPath)
{
	static wchar_t sDriveRoot[MAX_PATH];

	if(!sDirPath) return NULL;
	_tcsncpy_s(sDriveRoot, sDirPath, 3);

	return sDriveRoot;
}


CStringCon::CStringCon(void)
{
}

CStringCon::~CStringCon(void)
{
}

TCHAR* CStringCon::CString2TCHAR(CString &str)
{
	int iLen = str.GetLength();
	TCHAR* szRs = new TCHAR[iLen];
	lstrcpy(szRs, str.GetBuffer(iLen));
	str.ReleaseBuffer();
	return szRs;
}

wchar_t *CStringCon::CString2TCHAR(wchar_t *wszString, CString &str)
{
	int iLen = str.GetLength();

	lstrcpy(wszString, str.GetBuffer(iLen));
	str.ReleaseBuffer();

	return wszString;
}

char* CStringCon::UnicodeToUTF8(TCHAR *wszString)
{
	// unicode to UTF8
	int u8Len = WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	char* szU8 = new char[u8Len + 1];
	WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), szU8, u8Len, NULL, NULL);
	szU8[u8Len] = '\0';

	return szU8;
}

int CStringCon::UnicodeToUTF8(char* szDesU8, wchar_t* wszString)
{
	// unicode to UTF8
	int u8Len = WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), szDesU8, u8Len, NULL, NULL);
	szDesU8[u8Len] = '\0';

	return u8Len;
}

wchar_t* CStringCon::UTF8ToUnicode(char *szU8)
{
	//UTF8 to Unicode
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), NULL, 0);

	wchar_t* wszString = new wchar_t[wcsLen + 1];
	MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	return wszString;
}

int CStringCon::UTF8ToUnicode(TCHAR *wszString, char *szU8)
{
	//UTF8 to Unicode
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), NULL, 0);

	MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	return wcsLen;
}

char* CStringCon::Unicode2Ansi(wchar_t* wszString)
{
	// unicode to ansi
	int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	char* szAnsi = new char[ansiLen + 1];
	WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';

	return szAnsi;
}

char* CStringCon::Unicode2AnsiStatic(wchar_t* wszString)
{
	static char szAnsi[MAX_PATH];
	// unicode to ansi
	int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);

	WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';

	return szAnsi;
}

char* CStringCon::Unicode2Ansi(char* szAnsi, wchar_t* wszString)
{
	// unicode to ansi
	int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';

	return szAnsi;
}

wchar_t* CStringCon::Ansi2Unicode(char* szAnsi)
{
	// ansi to unicode
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);

	wchar_t* wszString = new wchar_t[wcsLen + 1];
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	return wszString;
}

wchar_t* CStringCon::Ansi2Unicode(wchar_t* wszString, char* szAnsi)
{
	// ansi to unicode
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);
	MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	return wszString;
}

