#pragma once

#include "./utility/ipc.h"
#include "ShellInterface.h"

#include <vector>
using std::vector;

class CShellLinker {
public:
	CShellLinker(void);
	~CShellLinker(void);
protected:
	osIPC::Client client;
	bool creat_ok;
public:
	void LinkOpen(TCHAR *szLinkName);
	void CloseLink();
public:
	DWORD LinkSend(char *szMessage, size_t length);
};


class CCommandLinker : public CShellLinker {
public:
	CCommandLinker(void);
	~CCommandLinker(void);
public:
	DWORD Initialize();
	DWORD Finalize();
public:
	DWORD SendCommand(DWORD Command);
	DWORD SendCommand(DWORD Command, const TCHAR *path);
public:
	inline DWORD CacheClear(DWORD dwStatus) {
		return SendCommand(CACHE_CONTROL_CLEAR|dwStatus) > 0 ? 0: -1;
	}
	inline DWORD CacheErase(const TCHAR *path, DWORD dwStatus) {
		return SendCommand(CACHE_CONTROL_ERASE|dwStatus, path) > 0 ? 0: -1;
	}
	inline DWORD CacheInsert(const TCHAR *path, DWORD dwStatus) {
		return SendCommand(CACHE_CONTROL_INSERT|dwStatus, path) > 0 ? 0: -1;
	}
	inline DWORD QuitCacheServer() {
		return SendCommand(CACHE_CONTROL_QUIT) > 0 ? 0: -1;
	}
public:
	inline DWORD SetCacheWatchingDirectory(const TCHAR *dirpath) {
		return SendCommand(CACHE_CONTROL_SETVALUE|WATCHED_DIRECTORY, dirpath) > 0 ? 0: -1;
	}
public:
	inline DWORD CacheDirectoryInsert(const TCHAR *dirpath, DWORD dwStatus) {
		return SendCommand(CACHE_CONTROL_INSERT|FILE_STATUS_DIRECTORY|dwStatus, dirpath) > 0 ? 0: -1;
	}
	inline DWORD CacheFileInsert(const TCHAR *filepath, DWORD dwStatus) {
		return SendCommand(CACHE_CONTROL_INSERT|dwStatus, filepath) > 0 ? 0: -1;
	}
};
extern CCommandLinker objCommandLinker;
