#pragma once

#ifndef STRIUTILI_H_
#define STRIUTILI_H_

namespace struti {
	VOID wsplit(CStringArray& to, CString from, CString division);
	char *strsplit(char *str, const char *delim, char **context);
	TCHAR *wstrsplit(TCHAR *str, const TCHAR *delim, TCHAR **context);
	char *split_status(char *str);
	TCHAR *wsplit_status(TCHAR *str);	
	char *split_value(char *str);
	TCHAR *wsplit_value(TCHAR *str);
	TCHAR *appe_path(TCHAR *to, const TCHAR *from);
	TCHAR *appe_path_ext(TCHAR *to, const TCHAR *dire, const TCHAR *sub_dire);
	TCHAR *dele_last_path(TCHAR *to, const TCHAR *from);
	//
	TCHAR *get_name(TCHAR *name, const TCHAR *location, const TCHAR *sub_dire);
	char *get_name_ex(char *name, const TCHAR *location, const TCHAR *sub_dire);
	TCHAR *full_path(TCHAR *fpath, const TCHAR *path, const TCHAR *sub_dire);
	TCHAR *full_dire(TCHAR *fdire, const TCHAR *path, const TCHAR *sub_dire);
	TCHAR *full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file);
	TCHAR *get_url(TCHAR *szLinkTxt, const TCHAR *szCaption, const TCHAR *szURL);
	TCHAR *get_ipaddr(TCHAR *addr_txt, const TCHAR *address, const int port);
	VOID split_ipaddr(TCHAR *address, int *port, TCHAR *addr_txt);
	TCHAR *path_wind_unix(TCHAR *win_path);
	TCHAR *path_unix_wind(TCHAR *unix_path);
	TCHAR *get_drive_root(TCHAR *drive_letter, const TCHAR *dire_path);
	char *name_value(char **poval, char *str, const char delim);
};

#define FILE_DIRECTORY(FILE_NAME) { \
	TCHAR *szPath = _tcsrchr((TCHAR *)FILE_NAME, _T('\\')); \
	if(szPath) szPath[0] = _T('\0'); \
}

#endif /* STRIUTILI_H_ */

#ifndef STRICONV_H_
#define STRICONV_H_

namespace strcon {
	char* ustr_ansi(const TCHAR* ustr);
	char* ustr_ansi(char* ansi, const TCHAR* ustr);
	TCHAR* ansi_ustr(const char* ansi);
	TCHAR* ansi_ustr(TCHAR* ustr, const char* ansi);
	//
	char* ustr_utf8(const TCHAR* ustr);
	int ustr_utf8(char* utf8, const TCHAR* ustr);
	TCHAR* utf8_ustr(const char* utf8);
	int utf8_ustr(TCHAR* ustr, const char* utf8);
	// 将CString 转换为 TCHAR*
	TCHAR *cstr_ustr(CString &cstr);
	TCHAR *cstr_ustr(TCHAR *ustr, CString &cstr);
	// 将TCHAR* 转换为 char*
	inline char *ustr_astr(const TCHAR* ustr)
	{ return ustr_ansi(ustr); }
};

#endif /* STRICONV_H_ */

#ifndef SWDSTR_H_
#define SWDSTR_H_

namespace sdstr {
	char *strnstr(const char *phaystack, const char *pneedle, const int phaystack_len);
	char *strrstr(const char *s1, const char *s2);
	char *name_value(char **name, char *str, const char delim);
	char *s_strchr(const char *s, int c);
	char *no_quote(char *d, char *s);
	const char *strlchr(const char *str);
	const TCHAR *wcslchr(const TCHAR *str);
	const TCHAR *wcsend(const TCHAR *str);
};

#define CLEAR_LACHR(str) { \
	TCHAR *lastc = (TCHAR *)sdstr::wcslchr(str); \
	if(lastc) lastc[0] = _T('\0'); \
}

#endif /* SWDSTR_H_ */
