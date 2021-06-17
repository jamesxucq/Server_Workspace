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
	void wsplit(CStringArray& desti, CString source, CString division);
	char *strsplit(char *str, const char *delim, char **context);
	TCHAR *wstrsplit(TCHAR *str, const TCHAR *delim, TCHAR **context);
	char *split_status(char *str);
	TCHAR *wsplit_status(TCHAR *str);	
	char *split_value(char *str);
	TCHAR *wsplit_value(TCHAR *str);
	CString append_path(const CString &dirname, const TCHAR *subdir);
	TCHAR *append_path(TCHAR *desti, const TCHAR *source);
	TCHAR *del_last_path(TCHAR *desti, const TCHAR *source);
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

#define FILE_DIRECTORY(FILE_NAME)											\
	TCHAR *dir_split = wcsrchr((TCHAR *)FILE_NAME, _T('\\'));			\
	if(dir_split) dir_split[0] = _T('\0');

#define REFRESH_DIRECTORY_ICONS(REFRESH_FUNCTION, FILE_NAME)				\
	TCHAR *dir_split = wcsrchr((TCHAR *)FILE_NAME, _T('\\'));			\
	if(dir_split) {															\
		dir_split[0] = _T('\0');											\
		REFRESH_FUNCTION(FILE_NAME);										\
		dir_split[0] = _T('\\');											\
	}

#endif /* STRINGUTILITY_H_ */

#ifndef STRINGCONV_H_
#define STRINGCONV_H_

namespace nstriconv {
	char* unicode_ansi(const TCHAR* szUnicode);
	char* unicode_ansi_stc(const TCHAR* szUnicode);
	char* unicode_ansi(char* szAnsi, const TCHAR* szUnicode);
	TCHAR* ansi_unicode(const char* szAnsi);
	TCHAR* ansi_unicode(TCHAR* szUnicode, const char* szAnsi);

	char* unicode_utf8(const TCHAR* szUnicode);
	int unicode_utf8(char* szDestiUTF8, const TCHAR* szUnicode);
	TCHAR* utf8_unicode(const char* szUTF8);
	int utf8_unicode(TCHAR* szUnicode, const char* szUTF8);
	//函数： TransCStringToTCHAR
	//描述：将CString 转换为 TCHAR*
	TCHAR *cstring_tchar(CString &str);
	TCHAR *cstring_tchar(TCHAR *szUnicode, CString &str);

	//函数： THCAR2Char
	//描述：将TCHAR* 转换为 char*
	inline char *tchar_char(const TCHAR* tstring)
	{return unicode_ansi(tstring);}

	//函数： cstring_char
	//描述：将CString转换为 char*
	// inline char *cstring_char(const CString &str)
	// {return unicode_ansi_stc((TCHAR*)(LPCTSTR)str);}
	// inline char *cstring_char(char* szAnsi, const CString &str)
	// {return unicode_ansi(szAnsi, (TCHAR*)(LPCTSTR)str);}
};

#endif /* STRINGCONV_H_ */

