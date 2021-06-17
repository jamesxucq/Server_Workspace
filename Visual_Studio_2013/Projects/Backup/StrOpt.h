#pragma once

#ifndef STROPT_H_
#define STROPT_H_


#define _mwscpy(DEST, SOURCE)			_tcscpy_s(DEST, _tcslen(SOURCE)+1, SOURCE)
#define _MwsCpyEx(DEST, SOURCE)			\
if (!SOURCE) \
	_tcscpy_s(DEST, 6, _T("00000")); \
else \
	_tcscpy_s(DEST, _tcslen(SOURCE)+1, SOURCE);
//#define _mwscat(DEST, SOURCE)			_tcscat_s(DEST, _tcslen(SOURCE)+1, SOURCE)


class CStrOpt
{
public:
	CStrOpt(void);
	~CStrOpt(void);
public:
	static void Split(CStringArray& dest, CString source, CString division);
	static char *StrSplit(char *str, const char *delim);
	static wchar_t *UniStrSplit(wchar_t *str, const wchar_t *delim);
public:
	static wchar_t *AppendPath(wchar_t *dest, const wchar_t *source);
	static wchar_t *DelLastPath(wchar_t *dest, const wchar_t *source);
public:
	static wchar_t *fhpath(wchar_t *path, wchar_t *subdir);
	static wchar_t *fhpath(wchar_t *fhpath, wchar_t *path, wchar_t *subdir);
public:
	static char *fullpath(wchar_t *path, wchar_t *subdir);
	inline static char *fullpath(char *path, char *subdir) {
		static char DesU8[MAX_PATH];
		sprintf_s(DesU8, "%s%s", path, subdir);
		return DesU8;
	}
	inline static wchar_t *FullPathEx(wchar_t *path, wchar_t *subdir) {
		static wchar_t wdes[MAX_PATH];
		_stprintf_s(wdes, _T("%s%s"), path, subdir);
		return wdes;
	}
	static wchar_t *fullpath(wchar_t *fpath, wchar_t *path, char *subdir); /*{
		wchar_t wdes[MAX_PATH];
		_stprintf_s(wdes, _T("%s/%s"), path, file);
		_mwscpy(fname, wdes);
		return fname;
	}*/
public:
	static char *fullname(wchar_t *path, char *file);
	static char *fullname(wchar_t *path, wchar_t *file);
	static wchar_t *fullname(wchar_t *fname, wchar_t *path, wchar_t *file);
	inline static char *fullname(char *path, char *file) {
		static char DesU8[MAX_PATH];
		sprintf_s(DesU8, "%s/%s", path, file);
		return DesU8;
	}
public:
	static wchar_t *DrivePath(CString &Drive);
public:
	static CString DirPath(CString &DirName, wchar_t *SubDir);
};
#endif /* STROPT_H_ */

#ifndef STRCON_H_
#define STRCON_H_

class CStrCon
{
public:
	CStrCon(void);
	~CStrCon(void);
public:
	static char* UnicodeToAnsi(wchar_t* wszString);
	static wchar_t* AnsiToUnicode(char* szAnsi);

	static char* UnicodeToUTF8(wchar_t* wszString);
	static int UnicodeToUTF8(char* szDesU8, wchar_t* wszString);
	static wchar_t* UTF8ToUnicode(char* szU8);
	static int UTF8ToUnicode(wchar_t* wszString, char* szU8);
public:
	//函数： TransCStringToTCHAR
	//描述：将CString 转换为 TCHAR*
	static TCHAR *CString2TCHAR(CString &str);
	static wchar_t *CString2TCHAR(wchar_t *wszString, CString &str);

	//函数： THCAR2Char
	//描述：将TCHAR* 转换为 char*
	inline static char *THCAR2char(TCHAR* tchStr)
	{return UnicodeToAnsi(tchStr);}

	//函数： CString2char
	//描述：将CString转换为 char*
	inline static char *CString2char(const CString &str)
	{return UnicodeToAnsi((wchar_t*)(LPCTSTR)str);}
};

#endif /* STRCON_H_ */


#ifndef TIMEEX_H_
#define TIMEEX_H_

class CTimeEx
{
public:
	CTimeEx(void);
	~CTimeEx(void);
public:
	inline static int TimeCmp(DWORD t1, DWORD t2)
	{ return t1=t2?0:t1>t2?1:-1; }
};

#endif /* TIMEEX_H_ */