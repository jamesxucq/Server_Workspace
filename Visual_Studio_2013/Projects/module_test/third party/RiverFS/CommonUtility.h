#pragma once

#define NO_ROOT(FILE_PATH)				(FILE_PATH) + 2
#define PATH_ROOT(FILE_PATH, ROOT)		FILE_PATH[0] = ROOT[0]; FILE_PATH[1] = _T(':');
#define MKZERO(TEXT)					TEXT[0] = _T('\0')
#define ROOT_LENGTH						2
#define DIGEST_LENGTH					16

namespace CommonUtility {
	/////////////////////////////////////////////////////////
	TCHAR *get_name(TCHAR *name, const TCHAR *location, const TCHAR *subdir);
	TCHAR *full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file);
	TCHAR *append_path(TCHAR *to, const TCHAR *from);
	TCHAR *del_last_path(TCHAR *to, const TCHAR *from);
	char* unicode_ansi(char* ansi, const TCHAR* unicode);
	/////////////////////////////////////////////////////////
	BOOL FileExists(TCHAR *szFileName);
	DWORD FileSizeTimeExt(DWORD *dwFileSize, FILETIME *lastwrite, TCHAR *szFileName);
	VOID file_csum(unsigned char *sum, const TCHAR *file_name);
	/////////////////////////////////////////////////////////
};
