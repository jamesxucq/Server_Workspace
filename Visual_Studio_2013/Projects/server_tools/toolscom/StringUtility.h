#pragma once

#ifndef STRIUTILI_H_
#define STRIUTILI_H_

#include "tools_macro.h"

typedef struct {
	char sin_addr[HOSTNAME_LENGTH];
	int sin_port;
} serv_address;

namespace striutil
{
	void wsplit(CStringArray& desti, CString source, CString division);
	char *strsplit(char *str, const char *delim, char **context);
	wchar_t *wstrsplit(wchar_t *str, const wchar_t *delim, wchar_t **context);
	char *split_value(char *str);
	wchar_t *wsplit_value(wchar_t *str);	
	char *split_line(char *str);
	wchar_t *wsplit_line(wchar_t *str);
	CString append_path(const CString &dirname, const wchar_t *subdir);
	wchar_t *append_path(wchar_t *desti, const wchar_t *source);
	wchar_t *del_last_path(wchar_t *desti, const wchar_t *source);
	wchar_t *get_name(wchar_t *name, const wchar_t *location, const wchar_t *subdir);
	wchar_t *full_path(wchar_t *fpath, const wchar_t *path, const wchar_t *subdir);
	wchar_t *full_name(wchar_t *fname, const wchar_t *path, const wchar_t *file);
	wchar_t *GetURL(wchar_t *szLinkTxt, const wchar_t *szCaption, const wchar_t *szURL);
	wchar_t *path_windows_unix(wchar_t *winpath);
	wchar_t *path_unix_windows(wchar_t *unixpath);
	wchar_t *GetDriveRoot(wchar_t *szDriveRoot, const wchar_t *szDirectoryPath);
//
	int address_split(serv_address *address, char *addr_txt);
	int port_split(int *port_array, int array_inde, char *port_txt);
};
#endif /* STRIUTILI_H_ */

#ifndef STRICONV_H_
#define STRICONV_H_

namespace striconv
{
	char* unicode_ansi(const wchar_t* szUnicode);
	char* unicode_ansi_stc(const wchar_t* szUnicode);
	char* unicode_ansi(char* szAnsi, const wchar_t* szUnicode);
	wchar_t* ansi_unicode(const char* szAnsi);
	wchar_t* ansi_unicode(wchar_t* szUnicode, const char* szAnsi);
	char* unicode_utf8(const wchar_t* szUnicode);
	int unicode_utf8(char* szDestiUTF8, const wchar_t* szUnicode);
	wchar_t* utf8_unicode(const char* szUTF8);
	int utf8_unicode(wchar_t* szUnicode, const char* szUTF8);
	// 将CString 转换为 TCHAR*
	TCHAR *cstring_tchar(CString &str);
	wchar_t *cstring_tchar(wchar_t *szUnicode, CString &str);
	// 将TCHAR* 转换为 char*
	inline char *tchar_char(const TCHAR* tstring)
	{return unicode_ansi(tstring);}
	// 将CString转换为 char*
	inline char *cstring_char(const CString &str)
	{return unicode_ansi_stc((wchar_t*)(LPCTSTR)str);}
	inline char *cstring_char(char* szAnsi, const CString &str)
	{return unicode_ansi(szAnsi, (wchar_t*)(LPCTSTR)str);}
};


#endif /* STRICONV_H_ */

