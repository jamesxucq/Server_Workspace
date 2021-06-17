#pragma once

#define NO_ROOT(FILE_PATH)				(FILE_PATH) + 2
#define PATH_ROOT(FILE_PATH, ROOT)		FILE_PATH[0] = ROOT[0]; FILE_PATH[1] = _T(':');
#define ROOT_LENGTH						2
#define DIGEST_LENGTH					16

namespace CommonUtility {
	/////////////////////////////////////////////////////////
	TCHAR *get_name(TCHAR *name, const TCHAR *location, const TCHAR *subdir);
	TCHAR *full_name(TCHAR *file_name, const TCHAR *path, const TCHAR *file);
	TCHAR *append_path(TCHAR *desti, const TCHAR *source);
	TCHAR *del_last_path(TCHAR *desti, const TCHAR *source);
	char* unicode_ansi(char* szAnsi, const TCHAR* szUnicode);
	/////////////////////////////////////////////////////////
	bool IsDirectory(FILETIME *ftCreationTime, const TCHAR *szFolderPath);
	BOOL FileExists(TCHAR *szFileName);
	DWORD FileSize(const TCHAR *szFileName);
	DWORD FileSizeTimeExt(DWORD *nFileSize, FILETIME *lastwrite, TCHAR *szFileName);
	void file_csum(unsigned char *sum, const TCHAR *file_name, off_t offset);
	/////////////////////////////////////////////////////////
};
