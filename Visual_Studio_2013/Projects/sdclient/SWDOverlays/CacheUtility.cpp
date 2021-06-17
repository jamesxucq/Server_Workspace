#include "StdAfx.h"

#include "clientcom/ShellInterface.h"
#include "CacheUtility.h"
#include "./utility/ulog.h"

void NCacheUtility::ClearPathsVec(vector<struct path_attrib *> *paths_vec) {
	vector<struct path_attrib *>::iterator iter;
	//
	for(iter=paths_vec->begin(); paths_vec->end()!=iter; ++iter) // 
		free(*iter);
	//
	paths_vec->clear();
}

struct path_attrib *NCacheUtility::PathAttrib(const TCHAR *path, DWORD statcode) {
	struct path_attrib *path_attri;
	path_attri = (struct path_attrib *)malloc(sizeof(struct path_attrib));
	if(!path_attri) {
// logger(_T("d:\\icon.log"), _T("path_attri is null!"));	
	}
	_tcscpy_s(path_attri->path, MAX_PATH, path);
	path_attri->shell_status = statcode;
	//
	return path_attri;
}

void NCacheUtility::ClearDireCache(unordered_map<wstring, struct dire_attrib *> *dires_cache) {
	unordered_map<wstring, struct dire_attrib *>::iterator dite;
	//
	for(dite=dires_cache->begin(); dires_cache->end()!=dite; ++dite)
		free(dite->second);
	//
	dires_cache->clear();
}

struct dire_attrib *NCacheUtility::DireAttrib() {
	struct dire_attrib *dire_attri;
	dire_attri = (struct dire_attrib *)malloc(sizeof(struct dire_attrib));
	if(!dire_attri) {
// logger(_T("d:\\icon.log"), _T("dire_attri is null!"));	
	}
	memset(dire_attri, 0, sizeof(struct dire_attrib));
	//
	return dire_attri;
}

const TCHAR *NCacheUtility::wcsend(const TCHAR *str) {
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