#pragma once


//#define SDCACHE_PROGRAM_NAME				_T("sdcache.exe")
#define SDCACHE_SHELL_PIPE					_T("\\\\.\\pipe\\sdcache_shell")
#define SDCACHE_COMMAND_PIPE				_T("\\\\.\\pipe\\sdcache_command")
//#define SDCACHE_WINDOW_NAME				_T("sdcache_window")
#define SDCACHE_MUTEX_NAME					_T("sdcache_mutex")


//use in shell ext
//A structure passed as a request from the shell (or other client) to the external cache
struct SDShellRequest
{
//	DWORD flags;
	WCHAR path[MAX_PATH];
	DWORD dwAttrib;
};

struct SDCacheResponse
{
	WCHAR path[MAX_PATH];
	DWORD file_status;
};


//Use in file_status namepipe
struct SDCacheCommand
{
	WCHAR path[MAX_PATH];		///< path to do the file_status for
	DWORD file_status;				///< the file_status to execute
};

//0xffff0000 is cache ctrl code
#define		CACHE_CTRL_QUIT							0x00010000
#define		CACHE_CTRL_ERASE						0x00020000
#define		CACHE_CTRL_INSERT						0x00040000
#define		CACHE_CTRL_CLEAR						0x00080000
#define		CACHE_CTRL_SETVALUE						0x00100000
//#define		CACHE_CTRL_WATCH_ROOT					0x00100000

///////////////////////////////////////////////////////////////////////////
//0x0000ffff is Cache data code
///////////////////////////////////////////////////////////////////////////
#define		FILE_STATUS_UNKNOWN						0x00000000
//////////////////////////////////////////////////////////////下段显示优先中
	//以下有向上的图标继承性
#define		FILE_STATUS_SYNCING						0x00000001
//////////////////////////////////////////////////////////////下段显示优先最小
#define		FILE_STATUS_DELETE						0x00000002
#define		FILE_STATUS_MODIFIED					0x00000004
#define		FILE_STATUS_ADDED						0x00000008
//////////////////////////////////////////////////////////////下段显示优先最大
#define		FILE_STATUS_CONFLICT					0x00000010 //优先0
	//以下有向下的图标继承性
#define		FILE_STATUS_LOCKED						0x00000020
#define		FILE_STATUS_READONLY					0x00000040
#define		FILE_STATUS_FORBID						0x00000080
//////////////////////////////////////////////////////////////
#define		FILE_STATUS_NORMAL						0x00001000
#define		FILE_STATUS_DIRECTORY					0x00002000
//////////////////////////////////////////////////////////////虚拟属性
#define		FILE_STATUS_WATCHEDDIR					0x00004000
//#define		FILE_STATUS_ERASE						0x00008000

///////////////////////////////////////////////////////////////////////////
