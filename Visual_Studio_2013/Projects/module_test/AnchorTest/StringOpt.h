#pragma once

#ifndef STRINGOPT_H_
#define STRINGOPT_H_


#define _mywscpy_s(DEST, SOURCE)			_tcscpy_s(DEST, _tcslen(SOURCE)+1, SOURCE)

#define _mywscpy_exs(DEST, SOURCE) \
	if (!SOURCE) \
		_tcscpy_s(DEST, 6, _T("/")); \
	else _tcscpy_s(DEST, _tcslen(SOURCE)+1, SOURCE);

//#define _mywscat(DEST, SOURCE)			_tcscat_s(DEST, _tcslen(DEST), SOURCE)


class CStringOpt
{
public:
	CStringOpt(void);
	~CStringOpt(void);
public:
	static void Split(CStringArray& dest, CString source, CString division);
	static char *StrSplit(char *str, const char *delim);
	static wchar_t *UStrSplit(wchar_t *str, const wchar_t *delim);
public:
	static CString WAppendPath(CString &DirName, wchar_t *subdir);
	static wchar_t *UAppendPath(wchar_t *dest, const wchar_t *source);
	static wchar_t *UDelLastPath(wchar_t *dest, const wchar_t *source);
public:
	static wchar_t *homepath(wchar_t *path, wchar_t *subdir);
	static wchar_t *homepath(wchar_t *homepath, wchar_t *path, wchar_t *subdir);
public:
	inline static wchar_t *fullpath(wchar_t *path, wchar_t *subdir) {
		static wchar_t wdes[MAX_PATH];	
		_stprintf_s(wdes, _T("%s%s"), path, subdir);
		return wdes;
	}
	static inline wchar_t *fullpath(wchar_t *fpath, wchar_t *path, wchar_t *subdir){
		if(!fpath) return NULL;
		_stprintf_s(fpath, MAX_PATH, _T("%s%s"), path, subdir);
		return fpath;
	}
public:
	inline static wchar_t *fullname(wchar_t *fname, wchar_t *path, wchar_t *file){
		if(!fname) return NULL;
		_stprintf_s(fname, MAX_PATH, _T("%s\\%s"), path, file);
		return fname;
	}
public:
	inline static wchar_t *GetURL(wchar_t *sCaption, wchar_t *sURL) {
		static wchar_t sLinkTxt[MAX_PATH];
		_stprintf_s(sLinkTxt, MAX_PATH, _T("<a href=\"%s\">%s</a>"), sURL, sCaption);
		return sLinkTxt;
	}
public:
	//static wchar_t *fullname(wchar_t *filename, wchar_t *root, wchar_t *dir, wchar_t *file);
	static wchar_t *WindowsPath2Unix(wchar_t *path_w);
	static wchar_t *UnixPath2Windows(wchar_t *path_u);
public:
	static wchar_t *GetDriveRoot(wchar_t *sDirPath);
	//static const wchar_t *GetSubDirNext(const wchar_t *dirpath, wchar_t **strend);
};
#endif /* STRINGOPT_H_ */

#ifndef STRINGCON_H_
#define STRINGCON_H_

class CStringCon
{
public:
	CStringCon(void);
	~CStringCon(void);
public:
	static char* Unicode2Ansi(wchar_t* wszString);
	static char* Unicode2AnsiStatic(wchar_t* wszString);
	static char* Unicode2Ansi(char* szAnsi, wchar_t* wszString);
	static wchar_t* Ansi2Unicode(char* szAnsi);
	static wchar_t* Ansi2Unicode(wchar_t* wszString, char* szAnsi);

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
	{return Unicode2Ansi(tchStr);}

	//函数： CString2char
	//描述：将CString转换为 char*
	inline static char *CString2char(const CString &str)
	{return Unicode2AnsiStatic((wchar_t*)(LPCTSTR)str);}
	inline static char *CString2char(char* szAnsi, const CString &str)
	{return Unicode2Ansi(szAnsi, (wchar_t*)(LPCTSTR)str);}
};


#endif /* STRINGCON_H_ */

