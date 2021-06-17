#pragma once

#include <windows.h>
#include <assert.h>


class CFileMon
{
public:
	CFileMon(void);
	~CFileMon(void);
public:
	enum Filter
	{
		FILTER_FILE_NAME        = 0x00000001, // add/remove/rename
		FILTER_DIR_NAME         = 0x00000002, // add/remove/rename
		FILTER_ATTR_NAME        = 0x00000004,
		FILTER_SIZE_NAME        = 0x00000008,
		FILTER_LAST_WRITE_NAME  = 0x00000010, // timestamp
		FILTER_LAST_ACCESS_NAME = 0x00000020, // timestamp
		FILTER_CREATION_NAME    = 0x00000040, // timestamp
		FILTER_SECURITY_NAME    = 0x00000100
	};
	enum ACTION
	{
		ACTION_ERRSTOP          = -1,
		ACTION_ADDED            = 0x00000001,
		ACTION_REMOVED          = 0x00000002,
		ACTION_MODIFIED         = 0x00000003,
		ACTION_RENAMED_OLD      = 0x00000004,
		ACTION_RENAMED_NEW      = 0x00000005
	};

    typedef void (__stdcall *LPDEALFUNCTION)( ACTION act, LPCWSTR filename, LPVOID lParam );
public:
    // LPCTSTR dir: ended-with "\\"
    bool Run( LPCTSTR dir, bool bWatchSubtree, DWORD dwNotifyFilter, LPDEALFUNCTION dealfun, LPVOID lParam );
    void Close( DWORD dwMilliseconds=INFINITE );
private:
    HANDLE m_hDir;
    DWORD m_dwNotifyFilter;
    bool m_bWatchSubtree;
    HANDLE m_hThread;
    volatile bool m_bRequestStop;
    LPDEALFUNCTION m_DealFun;
    LPVOID m_DealFunParam;
private:
    static DWORD WINAPI Routine( LPVOID lParam );
};

extern CFileMon OFileMonD;