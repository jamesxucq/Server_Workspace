#pragma once

#ifndef STRINGOPT_H_
#define STRINGOPT_H_


#define _mywscpy_s(DEST, SOURCE)			_tcscpy_s(DEST, _tcslen(SOURCE)+1, SOURCE)

#define _mywscpy_exs(DEST, SOURCE) \
	if (!SOURCE) \
		_tcscpy_s(DEST, 6, _T("/")); \
	else _tcscpy_s(DEST, _tcslen(SOURCE)+1, SOURCE);

//#define _mywscat(DEST, SOURCE)			_tcscat_s(DEST, _tcslen(DEST), SOURCE)


namespace NStringOpt
{
	void wsplit(CStringArray& dest, CString source, CString division);
	char *strsplit(char *str, const char *delim, char **context);
	wchar_t *wstrsplit(wchar_t *str, const wchar_t *delim, wchar_t **context);
	CString appendpath(const CString &dirname, const wchar_t *subdir);
	wchar_t *appendpath(wchar_t *dest, const wchar_t *source);
	wchar_t *dellastpath(wchar_t *dest, const wchar_t *source);
	//  wchar_t *get_homepath(const wchar_t *path, const wchar_t *subdir);
	wchar_t *get_homepath(wchar_t *homepath, const wchar_t *path, const wchar_t *subdir);
	//  wchar_t *fullpath(const wchar_t *path, const wchar_t *subdir);
	wchar_t *fullpath(wchar_t *fpath, const wchar_t *path, const wchar_t *subdir);
	//  wchar_t *fullname(wchar_t *strFileName, wchar_t *root, wchar_t *dir, wchar_t *file);
	wchar_t *fullname(wchar_t *fname, const wchar_t *path, const wchar_t *file);
	wchar_t *GetURL(wchar_t *sLinkTxt, const wchar_t *sCaption, const wchar_t *sURL);
	wchar_t *winpath_unix(wchar_t *winpath);
	wchar_t *unixpath_windows(wchar_t *unixpath);
	wchar_t *GetDriveRoot(wchar_t *sDriveRoot, const wchar_t *sDirectoryPath);
	//const wchar_t *GetSubDirNext(const wchar_t *dirpath, wchar_t **strend);
};


/*
	wchar_t NotifyChar;
	
	NotifyChar = *(++char_ptr);
*/

#define FILE_DIRECTORY(FILE_NAME) \
	wchar_t *dir_split = wcsrchr((wchar_t *)FILE_NAME, _T('\\')); \
	if(dir_split) *(++dir_split) = _T('\0');


#endif /* STRINGOPT_H_ */

#ifndef STRINGCON_H_
#define STRINGCON_H_

namespace NStringCon
{
	char* unicode_ansi(const wchar_t* wszString);
	char* unicode_ansi_static(const wchar_t* wszString);
	char* unicode_ansi(char* szAnsi, const wchar_t* wszString);
	wchar_t* ansi_unicode(const char* szAnsi);
	wchar_t* ansi_unicode(wchar_t* wszString, const char* szAnsi);

	char* unicode_utf8(const wchar_t* wszString);
	int unicode_utf8(char* szDesU8, const wchar_t* wszString);
	wchar_t* utf8_unicode(const char* szU8);
	int utf8_unicode(wchar_t* wszString, const char* szU8);
	//函数： TransCStringToTCHAR
	//描述：将CString 转换为 TCHAR*
	TCHAR *cstring_tchar(CString &str);
	wchar_t *cstring_tchar(wchar_t *wszString, CString &str);

	//函数： THCAR2Char
	//描述：将TCHAR* 转换为 char*
	inline char *tchar_char(const TCHAR* tchStr)
	{return unicode_ansi(tchStr);}

	//函数： cstring_char
	//描述：将CString转换为 char*
	// inline char *cstring_char(const CString &str)
	// {return unicode_ansi_static((wchar_t*)(LPCTSTR)str);}
	// inline char *cstring_char(char* szAnsi, const CString &str)
	// {return unicode_ansi(szAnsi, (wchar_t*)(LPCTSTR)str);}
};


#endif /* STRINGCON_H_ */

