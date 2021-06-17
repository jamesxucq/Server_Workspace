#include "StdAfx.h"
#include "StringUtility.h"

#include "string.h"

TCHAR *struti::appe_path(TCHAR *to, const TCHAR *from) {
    if (!to || !from) return NULL;
    //_stprintf_s(szDirectory, MAX_PATH, _T("%s\\%s"), to, from);
    _tcscat_s(to, MAX_PATH, _T("\\"));
    _tcscat_s(to, MAX_PATH, from);
    //
    return to;
}

TCHAR *struti::dele_last_path(TCHAR *to, const TCHAR *from) {
    LPTSTR toke = NULL;
    //
    if (!to) return NULL;
    //
    toke = _tcsrchr(to, _T('\\'));
    if (_tcscmp(toke+1, from)) return to;
    toke[0] = _T('\0');
    //
    return to;
}

/*
CString struti::appe_path(const CString &dirpath, const TCHAR *sub_dire) {
    TCHAR   szDire[MAX_PATH];
    //
    if(_T('\\') == dirpath.GetAt(dirpath.GetLength() - 1))
        _stprintf_s(szDire, _T("%s%s"), dirpath, sub_dire+1);
    else _stprintf_s(szDire, _T("%s%s"), dirpath, sub_dire);
    //
    return   CString(szDire);
}
*/

TCHAR *struti::appe_path_ext(TCHAR *to, const TCHAR *dire, const TCHAR *sub_dire) {
	if(_T('\\') == dire[_tcslen(dire) - 1])
        _stprintf_s(to, MAX_PATH, _T("%s%s"), dire, sub_dire+1);
    else _stprintf_s(to, MAX_PATH, _T("%s%s"), dire, sub_dire);
    //
    return to;
}

TCHAR *struti::wstrsplit(TCHAR *str, const TCHAR *delim, TCHAR **context) {
    TCHAR *toksp, *tokep;
    //
    if(!delim || !context) return NULL;
    tokep = *context;
    //
    if(str) toksp = str;
    else {
        if(!tokep) return NULL;
        toksp = tokep + _tcslen(delim);
    }
    //
    tokep = _tcsstr(toksp, delim);
    *context = tokep;
    if(tokep) tokep[0] = _T('\0');
    //
    return toksp;
}

char *struti::split_status(char *str) {
    char *toksp, *tokep;
    //
    toksp = strchr(str, ' ');
    if (!toksp) return NULL;
    *toksp = '\0';
    //
    ++toksp;
    tokep = strrchr(toksp, ' ');
    if (!tokep) return NULL;
    *tokep = '\0';
    //
    return toksp;
}

TCHAR *struti::wsplit_status(TCHAR *str) {
    TCHAR *toksp, *tokep;
    //
    toksp = _tcschr(str, _T(' '));
    if (!toksp) return NULL;
    toksp[0] = _T('\0');
    //
    ++toksp;
    tokep = _tcsrchr(toksp, _T(' '));
    if (!tokep) return NULL;
    tokep[0] = _T('\0');
    //
    return toksp;
}

char *struti::split_value(char *str) {
    char *toksp;
    //
    toksp = strstr(str, ": ");
    if (!toksp) return NULL;
    toksp[0] = '\0';
    //
    return toksp + 2;
}

TCHAR *struti::wsplit_value(TCHAR *str) {
    TCHAR *toksp;
    //
    toksp = _tcsstr(str, _T(": "));
    if (!toksp) return NULL;
    toksp[0] = _T('\0');
    //
    return toksp + 2;
}

TCHAR *struti::get_name(TCHAR *name, const TCHAR *location, const TCHAR *sub_dire) {
    TCHAR wdesti[MAX_PATH];
    if(!name) return NULL;
    //
    if (_T('~') == sub_dire[0]) _stprintf_s(wdesti, _T("%s%s"), location, sub_dire+1);
	else if(_T('\\') == sub_dire[0]) _stprintf_s(wdesti, _T("%s%s"), location, sub_dire);
	else _stprintf_s(wdesti, _T("%s\\%s"), location, sub_dire);
    _tcscpy_s(name, MAX_PATH, wdesti);
    //
    return name;
}

char *struti::get_name_ex(char *name, const TCHAR *location, const TCHAR *sub_dire) {
    if(!name) return NULL;
    //
    TCHAR ustr[MAX_PATH];
    if (_T('~') == sub_dire[0]) _stprintf_s(ustr, _T("%s%s"), location, sub_dire+1);
	else if(_T('\\') == sub_dire[0]) _stprintf_s(ustr, _T("%s%s"), location, sub_dire);
	else _stprintf_s(ustr, _T("%s\\%s"), location, sub_dire);
	//
	char ansi[MAX_PATH];
    int alen = WideCharToMultiByte(CP_ACP, NULL, ustr, (int)wcslen(ustr), NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, NULL, ustr, (int)wcslen(ustr), ansi, alen, NULL, NULL);
    ansi[alen] = '\0';
    //
    strcpy_s(name, MAX_PATH, ansi);
    return name;
}

TCHAR *struti::full_path(TCHAR *fpath, const TCHAR *path, const TCHAR *sub_dire) {
    if(!fpath) return NULL;
    _stprintf_s(fpath, MAX_PATH, _T("%s%s"), path, sub_dire);
    return fpath;
}

TCHAR *struti::full_dire(TCHAR *fdire, const TCHAR *path, const TCHAR *sub_dire) {
    if(!fdire) return NULL;
    _stprintf_s(fdire, MAX_PATH, _T("%s%s\\"), path, sub_dire);
    return fdire;
}

TCHAR *struti::full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file) {
    if(!file_name) return NULL;
    _stprintf_s(file_name, MAX_PATH, _T("%s\\%s"), path, file);
    return file_name;
}

TCHAR *struti::get_url(TCHAR *szLinkTxt, const TCHAR *szCaption, const TCHAR *szURL) {
    if(!szLinkTxt || !szCaption || !szURL) return NULL;
    _stprintf_s(szLinkTxt, MAX_PATH, _T("<a href=\"%s\">%s</a>"), szURL, szCaption);
    //
    return szLinkTxt;
}

TCHAR *struti::get_ipaddr(TCHAR *addr_txt, const TCHAR *address, const int port) {
    if(!addr_txt || !address) return NULL;
	_stprintf_s(addr_txt, MAX_PATH, _T("%s:%d"), address, port);
    //
    return addr_txt;
}

VOID struti::split_ipaddr(TCHAR *address, int *port, TCHAR *addr_txt) {
    TCHAR *toke = NULL;
    toke = _tcschr(addr_txt, _T(':'));
    if (toke) {
        _tcsncpy(address, addr_txt, toke - addr_txt);
        *port = _ttoi(++toke);
	} else {
		_tcscpy(address, addr_txt);
		*port = 0;
	}
}


char *struti::strsplit(char *str, const char *delim, char **context) {
    char *toksp, *tokep;
    //
    if(!delim || !context) return NULL;
    tokep = *context;
    //
    if(str) toksp = str;
    else {
        if(!tokep) return NULL;
        toksp = tokep + strlen(delim);
    }
    //
    tokep = strstr(toksp, delim);
    *context = tokep;
    if(tokep) *tokep = '\0';
    //
    return toksp;
}

VOID struti::wsplit(CStringArray& to, CString from, CString division) {
    int toke = 0;
    int prev = 0;
    //
    to.RemoveAll();
    while(-1 != toke) {
        prev = toke;
        toke = from.Find(division,(toke+1));
        to.Add(from.Mid(prev,(toke-prev)));
    }
}

TCHAR *struti::path_wind_unix(TCHAR *win_path) {
    TCHAR *toke;
    //
    toke = win_path;
    do {
        toke = _tcschr(toke, _T('\\'));
        if(toke) *toke = _T('/');
    } while(toke);
    //
    return win_path;
}

TCHAR *struti::path_unix_wind(TCHAR *unix_path) {
    TCHAR *toke;
    //
    toke = unix_path;
    do {
        toke = _tcschr(toke, _T('/'));
        if(toke) *toke = _T('\\');
    } while(toke);
    //
    return unix_path;
}

TCHAR *struti::get_drive_root(TCHAR *drive_letter, const TCHAR *dire_path) {
    if(!drive_letter || !dire_path) return NULL;
    _tcsncpy_s(drive_letter, MAX_PATH, dire_path, 3);
    //
    return drive_letter;
}


char *struti::name_value(char **poval, char *str, const char delim) {
    char *toksp, *tokep;
    //
    if (!str) {
        if (!(*poval)) return NULL;
        toksp = (*poval) + 1;
    } else toksp = str;
    //
    *poval = tokep = strchr(toksp, delim);
    if (tokep) *tokep = '\0';
    //
    char *value = strchr(toksp, '=');
    if (value) {
        *value = '\0';
        ++value;
    } else value = toksp;
    //
    return value;
}


TCHAR* strcon::cstr_ustr(CString &cstr) {
    int len = cstr.GetLength();
    TCHAR* rtstr = new TCHAR[len];
    lstrcpy(rtstr, cstr.GetBuffer(len));
    cstr.ReleaseBuffer();
    return rtstr;
}

TCHAR *strcon::cstr_ustr(TCHAR *ustr, CString &cstr) {
    int len = cstr.GetLength();
    lstrcpy(ustr, cstr.GetBuffer(len));
    cstr.ReleaseBuffer();
    return ustr;
}

char* strcon::ustr_utf8(const TCHAR *ustr) {
    // unicode to UTF8
    int u8len = WideCharToMultiByte(CP_UTF8, NULL, ustr, (int)wcslen(ustr), NULL, 0, NULL, NULL);
    //
    char* utf8 = new char[u8len + 1];
    WideCharToMultiByte(CP_UTF8, NULL, ustr, (int)wcslen(ustr), utf8, u8len, NULL, NULL);
    utf8[u8len] = '\0';
    //
    return utf8;
}

int strcon::ustr_utf8(char* utf8, const TCHAR* ustr) {
    // unicode to UTF8
    int u8len = WideCharToMultiByte(CP_UTF8, NULL, ustr, (int)wcslen(ustr), NULL, 0, NULL, NULL);
    //
    WideCharToMultiByte(CP_UTF8, NULL, ustr, (int)wcslen(ustr), utf8, u8len, NULL, NULL);
    utf8[u8len] = '\0';
    //
    return u8len;
}

TCHAR* strcon::utf8_ustr(const char *utf8) {
    // UTF8 to Unicode
    int wlen = MultiByteToWideChar(CP_UTF8, NULL, utf8, (int)strlen(utf8), NULL, 0);
    //
    TCHAR* ustr = new TCHAR[wlen + 1];
    MultiByteToWideChar(CP_UTF8, NULL, utf8, (int)strlen(utf8), ustr, wlen);
    ustr[wlen] = _T('\0');
    //
    return ustr;
}

int strcon::utf8_ustr(TCHAR *ustr, const char *utf8) {
    // UTF8 to Unicode
    int wlen = MultiByteToWideChar(CP_UTF8, NULL, utf8, (int)strlen(utf8), NULL, 0);
    //
    MultiByteToWideChar(CP_UTF8, NULL, utf8, (int)strlen(utf8), ustr, wlen);
    ustr[wlen] = _T('\0');
    //
    return wlen;
}

char* strcon::ustr_ansi(const TCHAR* ustr) {
    // unicode to ansi
    int alen = WideCharToMultiByte(CP_ACP, NULL, ustr, (int)wcslen(ustr), NULL, 0, NULL, NULL);
    //
    char* ansi = new char[alen + 1];
    WideCharToMultiByte(CP_ACP, NULL, ustr, (int)wcslen(ustr), ansi, alen, NULL, NULL);
    ansi[alen] = '\0';
    //
    return ansi;
}

char* strcon::ustr_ansi(char* ansi, const TCHAR* ustr) {
    // unicode to ansi
    int alen = WideCharToMultiByte(CP_ACP, NULL, ustr, (int)wcslen(ustr), NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, NULL, ustr, (int)wcslen(ustr), ansi, alen, NULL, NULL);
    ansi[alen] = '\0';
    //
    return ansi;
}

TCHAR* strcon::ansi_ustr(const char* ansi) {
    // ansi to unicode
    int wlen = MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), NULL, 0);
    //
    TCHAR* ustr = new TCHAR[wlen + 1];
    MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), ustr, wlen);
    ustr[wlen] = _T('\0');
    //
    return ustr;
}

TCHAR* strcon::ansi_ustr(TCHAR* ustr, const char* ansi) {
    // ansi to unicode
    int wlen = MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), NULL, 0);
    MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), ustr, wlen);
    ustr[wlen] = _T('\0');
    //
    return ustr;
}

//

/* 此函数实现在phaystack指定长度phaystack_len内搜索串pneedle，成功返回起始位置，否则返回NULL */
char *sdstr::strnstr(const char *phaystack, const char *pneedle, const int phaystack_len) {
	const unsigned char *haystack, *needle;
	unsigned b;
	const unsigned char *rneedle;
//
	haystack = (const unsigned char *) phaystack;
//
	if ((b = *(needle = (const unsigned char *) pneedle))) {
		int _count = 0;
		unsigned c;
		haystack--; /* possible ANSI violation */
		{
			unsigned a;
			do {
				_count++;
				if (!(a = *++haystack) || _count > phaystack_len)
					goto retnull;
			} while (a != b);
			/* 两个串的字符有相等的时候跳出，否则直到转到返回NULL */
		}
//
		if (!(c = *++needle)) goto foundneedle;
		++needle;
		goto jin;
//
		for (;_count <= phaystack_len;) {
			{
				unsigned a;
				if (0)
jin:{ /* JIN: 在needle是一个字符串时，匹配它的第二个字符 成功GOTO：crest*/
				_count++;
				if ((a = *++haystack) == c)
					goto crest;
				} else {
					a = *++haystack;
					_count++;
				}
				do {
					for (; a != b && _count <= phaystack_len; a = *++haystack, _count++) {
						if (!a || _count > phaystack_len) 
							goto retnull;
						if ((a = *++haystack) == b) 
							break;
						if (!a || _count > phaystack_len)
							goto retnull;
					}
					_count++;
				} while ((a = *++haystack) != c && _count <= phaystack_len);
			}
crest: /* 保留上一次找到的源串字符起始位置 rhaystack = haystack-- + 1 */
			{
				unsigned a;
				{
					const unsigned char *rhaystack;
					/* rneedle 保存 needle的起始位置 */
					if (*(rhaystack = haystack-- + 1) == (a = *(rneedle = needle))) {
						do {
							if (!a) /* needle 的串匹配完成，直接返回匹配成功的起始位置 */
								goto foundneedle;
							if (*++rhaystack != (a = *++needle)) {
								_count++;
								break;
							}
							if (!a)
								goto foundneedle;
							_count++;
						} while (*++rhaystack == (a = *++needle) && _count <= phaystack_len);
						_count++;
					}
					needle = rneedle; /* took the register-poor aproach */
				}
				if (_count > phaystack_len) {
					goto retnull;
				}
				if (!a) break;
			}
		}
	}
foundneedle:
	return (char *) haystack;
retnull:
	return 0;
}

/* strrstr : 查找指定字符串, 返回最后一次出现的地址 */  
char *sdstr::strrstr(const char *s1, const char *s2) {  
    int len2;  
    char *ps1;  
//
    if (!(len2 = strlen(s2))) {  
        return (char *)s1;  
    }  
//    
    ps1 = (char *)s1 + strlen(s1) - 1;  
    ps1 = ps1 - len2 + 1;  
//
    while (ps1 >= s1) {  
        if ((*ps1 == *s2) && (strncmp(ps1, s2, len2) == 0)) {  
            return (char *)ps1;  
        }  
        ps1--;  
    }  
//
    return NULL;  
} 

char *sdstr::name_value(char **name, char *str, const char delim) {
    char *toksp;
    static char *tokep;
    //
    if (!str) {
        if (!tokep) return NULL;
        toksp = tokep + 1;
	} else {
		// for (; ' ' == *str; ++str);
		toksp = str;
	}
    //
    tokep = s_strchr(toksp, delim);
    if (tokep) *tokep = '\0';
    //
    if (name) *name = toksp;
    char *value = strchr(toksp, '=');
    if (value) {
        *value = '\0';
        ++value;
    } else value = toksp;
    //
    return value;
}

/**
 * strchr - Find the first occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
char *sdstr::s_strchr(const char *s, int c) {
	unsigned int p = 0x00;
	//
	for (; (p & 0x01) || (*s != (char)c); ++s) {
		if('"' == *s) ++p;
		else if ('\0' == *s) 
			return NULL;
	}
	//
	return (char *)s;
}

char *sdstr::no_quote(char *d, char *s) {
	char *p = d;
	if('"' == *s) {
		for (; '"' != *(++s); ) *p++ = *s;
		*p = '\0';
	} else {
		for (; '\0' != *s; ++s) *p++ = *s;
		*p = '\0';
	}
	//
	return d;
}

const char *sdstr::strlchr(const char *str) {
	const char *char_ptr;
	const unsigned long int *longword_ptr;
	unsigned long int longword, himagic, lomagic;
//
	for (char_ptr = str; ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0; ++char_ptr) {
		if (*char_ptr == '\0')
			return (char_ptr - 1);
	}
//
	longword_ptr = (unsigned long int *) char_ptr;
	himagic = 0x80808080L;
	lomagic = 0x01010101L;
	if (sizeof (longword) > 4) {
		himagic = ((himagic << 16) << 16) | himagic;
		lomagic = ((lomagic << 16) << 16) | lomagic;
	}
	if (sizeof (longword) > 8) abort ();
//
	for (;;) {
		longword = *longword_ptr++;
		if (((longword - lomagic) & ~longword & himagic) != 0)
		{
			const char *cp = (const char *) (longword_ptr - 1);
			//
			if (cp[0] == 0)
				return (cp - 1);
			if (cp[1] == 0)
				return cp;
			if (cp[2] == 0)
				return (cp + 1);
			if (cp[3] == 0)
				return (cp + 2);
			if (sizeof (longword) > 4)
			{
				if (cp[4] == 0)
					return (cp + 3);
				if (cp[5] == 0)
					return (cp + 4);
				if (cp[6] == 0)
					return (cp + 5);
				if (cp[7] == 0)
					return (cp + 6);
			}
		}
	}
}

const TCHAR *sdstr::wcslchr(const TCHAR *str) {
	const TCHAR *char_ptr;
	const unsigned long int *longword_ptr;
	unsigned long int longword, himagic, lomagic;
//
	for (char_ptr = str; ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0; ++char_ptr) {
		if (*char_ptr == '\0')
			return (char_ptr - 1);
	}
//
	longword_ptr = (unsigned long int *) char_ptr;
	himagic = 0x80008000L;
	lomagic = 0x01000100L;
	if (sizeof (longword) > 4) {
		himagic = ((himagic << 16) << 16) | himagic;
		lomagic = ((lomagic << 16) << 16) | lomagic;
	}
	if (sizeof (longword) > 8) abort ();
//
	for (;;) {
		longword = *longword_ptr++;
		if (((longword - lomagic) & ~longword & himagic) != 0)
		{
			const TCHAR *cp = (const TCHAR *) (longword_ptr - 1);
			//
			if (cp[0] == 0)
				return (cp - 1);
			if (cp[1] == 0)
				return cp;

			if (sizeof (longword) > 4)
			{
				if (cp[2] == 0)
					return (cp + 1);
				if (cp[3] == 0)
					return (cp + 2);
			}
		}
	}
}


const TCHAR *sdstr::wcsend(const TCHAR *str) {
	const TCHAR *char_ptr;
	const unsigned long int *longword_ptr;
	unsigned long int longword, himagic, lomagic;
//
	for (char_ptr = str; ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0; ++char_ptr) {
		if (*char_ptr == '\0')
			return (char_ptr);
	}
//
	longword_ptr = (unsigned long int *) char_ptr;
	himagic = 0x80008000L;
	lomagic = 0x01000100L;
	if (sizeof (longword) > 4) {
		himagic = ((himagic << 16) << 16) | himagic;
		lomagic = ((lomagic << 16) << 16) | lomagic;
	}
	if (sizeof (longword) > 8) abort ();
//
	for (;;) {
		longword = *longword_ptr++;
		if (((longword - lomagic) & ~longword & himagic) != 0)
		{
			const TCHAR *cp = (const TCHAR *) (longword_ptr - 1);
			//
			if (cp[0] == 0)
				return (cp);
			if (cp[1] == 0)
				return (cp + 1);

			if (sizeof (longword) > 4)
			{
				if (cp[2] == 0)
					return (cp + 2);
				if (cp[3] == 0)
					return (cp + 3);
			}
		}
	}
}