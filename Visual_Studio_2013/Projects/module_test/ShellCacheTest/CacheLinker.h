#pragma once

#include "CacheInterface.h"

#include <vector>
using std::vector;

class CCacheLinker
{
public:
	CCacheLinker(void);
	~CCacheLinker(void);
protected:
	HANDLE m_hPipe;
public:
	bool EnsurePipeOpen(wchar_t *lpszPipeName, DWORD fdwOpenMode);
	void ClosePipe();
public:
	inline HANDLE GetPipe() { return m_hPipe; } //just for test
	DWORD PipeSend(char *szMessage, size_t len);
};


class CCmdCacheLinker : public CCacheLinker
{
public:
	CCmdCacheLinker(void);
	~CCmdCacheLinker(void);
public:
	DWORD Create();
	DWORD Destroy();
public:
	DWORD SendFileStatus(const wchar_t *path, DWORD file_status);
	DWORD SendCommand(DWORD Command);
	DWORD SendCommand(DWORD Command, const wchar_t *path);
public:
	inline bool CacheClear(DWORD dwStatus) {
		return SendCommand(CACHE_CTRL_CLEAR|dwStatus) > 0 ? true: false;
	}
	inline bool CacheErase(const wchar_t *path, DWORD dwStatus) {
		return SendCommand(CACHE_CTRL_ERASE|dwStatus, path) > 0 ? true: false;
	}
	inline bool CacheInsert(const wchar_t *path, DWORD dwStatus) {
		return SendCommand(CACHE_CTRL_INSERT|dwStatus, path) > 0 ? true: false;
	}
	inline bool QuitCacheServer() {
		return SendCommand(CACHE_CTRL_QUIT) > 0 ? true: false;
	}
public:
	inline bool SetCacheWatchingDirectory(const wchar_t *dirpath) {
		return SendCommand(CACHE_CTRL_SETVALUE|FILE_STATUS_WATCHEDDIR, dirpath) > 0 ? true: false;
	}
public:
	inline bool CacheDirInsert(const wchar_t *dirpath, DWORD dwStatus) {
		return SendCommand(CACHE_CTRL_INSERT|FILE_STATUS_DIRECTORY|dwStatus, dirpath) > 0 ? true: false;
	}
	inline bool CacheFileInsert(const wchar_t *filepath, DWORD dwStatus) {
		return SendCommand(CACHE_CTRL_INSERT|dwStatus, filepath) > 0 ? true: false;
	}
};
extern CCmdCacheLinker OCmdCacheLinker;


class CShellCacheLinker : public CCacheLinker
{
public:
	CShellCacheLinker(void);
	~CShellCacheLinker(void);
private:
	DWORD m_iPipeOpened;
public:
	bool EnsurePipeOpen(wchar_t *lpszPipeName, DWORD fdwOpenMode);
	void ClosePipe();
public:
	DWORD PipeSendRecv(BYTE *pRecvData, BYTE *szMessage, size_t len);
	DWORD ShellRequestStatus(DWORD *dwFileStatus, LPCWSTR pwszPath, DWORD dwAttrib);
	DWORD ShellRequestStatusExt(DWORD *dwFileStatus, LPCWSTR pwszPath, DWORD dwAttrib);
};
extern CShellCacheLinker OShellCacheLinker;