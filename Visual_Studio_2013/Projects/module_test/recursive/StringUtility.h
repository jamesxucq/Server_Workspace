#pragma once

#ifndef STRINGUTILITY_H_
#define STRINGUTILITY_H_

#define _mywscpy_s(DEST, SOURCE)			_tcscpy_s(DEST, _tcslen(SOURCE)+1, SOURCE)
#define _mywscpy_exs(DEST, SOURCE) \
	if (!SOURCE) \
	_tcscpy_s(DEST, 6, _T("/")); \
	else _tcscpy_s(DEST, _tcslen(SOURCE)+1, SOURCE);
//#define _mywscat(DEST, SOURCE)			_tcscat_s(DEST, _tcslen(DEST), SOURCE)

namespace nstriutility {
	VOID wsplit(CStringArray& to, CString from, CString division);
	char *strsplit(char *str, const char *delim, char **context);
	TCHAR *wstrsplit(TCHAR *str, const TCHAR *delim, TCHAR **context);
	char *split_status(char *str);
	TCHAR *wsplit_status(TCHAR *str);	
	char *split_value(char *str);
	TCHAR *wsplit_value(TCHAR *str);
	CString append_path(const CString &dirname, const TCHAR *subdir);
	TCHAR *append_path(TCHAR *to, const TCHAR *from);
	TCHAR *del_last_path(TCHAR *to, const TCHAR *from);
	//  TCHAR *get_name(const TCHAR *location, const TCHAR *subdir);
	TCHAR *get_name(TCHAR *name, const TCHAR *location, const TCHAR *subdir);
	//  TCHAR *full_path(const TCHAR *location, const TCHAR *subdir);
	TCHAR *full_path(TCHAR *fpath, const TCHAR *path, const TCHAR *subdir);
	//  TCHAR *full_name(TCHAR *file_name, TCHAR *root, TCHAR *dir, TCHAR *file);
	TCHAR *full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file);
	TCHAR *GetURL(TCHAR *szLinkTxt, const TCHAR *szCaption, const TCHAR *szURL);
	TCHAR *path_windows_unix(TCHAR *winpath);
	TCHAR *path_unix_windows(TCHAR *unixpath);
	TCHAR *GetDriveRoot(TCHAR *szDriveRoot, const TCHAR *szDirectoryPath);
	//const TCHAR *GetSubdirNext(const TCHAR *dirpath, TCHAR **strend);
	char *name_value(char **name, char *str, const char delim);
};

#define FILE_DIRECTORY(FILE_NAME) \
	TCHAR *dir_split = wcsrchr((TCHAR *)FILE_NAME, _T('\\')); \
	if(dir_split) dir_split[0] = _T('\0');

#define REFRESH_DIRECTORY_ICONS(REFRESH_FUNCTION, FILE_NAME) \
	TCHAR *dir_split = wcsrchr((TCHAR *)FILE_NAME, _T('\\')); \
	if(dir_split) { \
	dir_split[0] = _T('\0'); \
	REFRESH_FUNCTION(FILE_NAME); \
	dir_split[0] = _T('\\'); \
	}

#endif /* STRINGUTILITY_H_ */

#ifndef STRINGCONV_H_
#define STRINGCONV_H_

namespace nstriconv {
	char* unicode_ansi(const TCHAR* unicode);
	char* unicode_ansi_stc(const TCHAR* unicode);
	char* unicode_ansi(char* ansi, const TCHAR* unicode);
	TCHAR* ansi_unicode(const char* ansi);
	TCHAR* ansi_unicode(TCHAR* unicode, const char* ansi);

	char* unicode_utf8(const TCHAR* unicode);
	int unicode_utf8(char* utf8, const TCHAR* unicode);
	TCHAR* utf8_unicode(const char* utf8);
	int utf8_unicode(TCHAR* unicode, const char* utf8);
	//函数： TransCStringToTCHAR
	//描述：将CString 转换为 TCHAR*
	TCHAR *cstring_tchar(CString &str);
	TCHAR *cstring_tchar(TCHAR *unicode, CString &str);

	//函数： THCAR2Char
	//描述：将TCHAR* 转换为 char*
	inline char *tchar_char(const TCHAR* tstring)
	{ return unicode_ansi(tstring); }

	//函数： cstring_char
	//描述：将CString转换为 char*
	// inline char *cstring_char(const CString &str)
	// { return unicode_ansi_stc((TCHAR*)(LPCTSTR)str); }
	// inline char *cstring_char(char* ansi, const CString &str)
	// { return unicode_ansi(ansi, (TCHAR*)(LPCTSTR)str); }
};

#endif /* STRINGCONV_H_ */

