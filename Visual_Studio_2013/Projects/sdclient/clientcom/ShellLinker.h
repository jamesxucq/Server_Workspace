#pragma once

#include "ShellInterface.h"

#include <vector>
using std::vector;

class CShellLinker {
public:
	CShellLinker(void);
	~CShellLinker(void);
protected:
	HANDLE hPipeLink;
	BOOL fConnected;
public:
	VOID EnsureLinkOpen(TCHAR *szLinkName);
	VOID CloseLink();
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
	VOID FastSend(DWORD Command, const TCHAR *path);
public:
	inline DWORD CacheClear(DWORD status) {
		return SendCommand(CACHE_CONTROL_CLEAR|status) > 0 ? 0: -1;
	}
	inline DWORD CacheErase(const TCHAR *path, DWORD status) {
		return SendCommand(CACHE_CONTROL_ERASE|status, path) > 0 ? 0: -1;
	}
	inline DWORD CacheSeta(const TCHAR *path, DWORD status) {
		return SendCommand(CACHE_CONTROL_SETATTRI|status, path) > 0 ? 0: -1;
	}
	inline DWORD QuitCache() {
		return SendCommand(CACHE_CONTROL_QUIT) > 0 ? 0: -1;
	}
public:
	inline DWORD SetWatchDire(const TCHAR *dirpath) {
		return SendCommand(CACHE_CONTROL_SETVALUE|WATCH_DIRECTORY, dirpath) > 0 ? 0: -1;
	}
public:
	inline DWORD CacheDireSet(const TCHAR *dirpath, DWORD status) {
		return SendCommand(CACHE_CONTROL_SETATTRI|FILE_STATUS_DIRECTORY|status, dirpath) > 0 ? 0: -1;
	}
};

extern CCommandLinker objCommandLinker;

//