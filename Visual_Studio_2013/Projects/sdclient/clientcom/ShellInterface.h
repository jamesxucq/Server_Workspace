#pragma once

#define IPC_COMMAND_PIPE	_T("\\\\.\\pipe\\sdclient")

// use in shell ext
// A structure passed as a request from the shell (or other client) to the external cache
struct ShellRequest {
	WCHAR path[MAX_PATH];
	DWORD attrib;
};

struct CacheResponse {
	WCHAR path[MAX_PATH];
	DWORD fstat;
};

// Use in fstat namepipe
struct CacheCommand {
	WCHAR path[MAX_PATH];		// < path to do the file_status for
	DWORD fstat;				// < the fstat to execute
};

// 0xffff0000 is cache ctrl code
#define	CACHE_CONTROL_ERASE		0x00010000
#define	CACHE_CONTROL_SETATTRI	0x00020000
#define	CACHE_CONTROL_CLEAR		0x01000000
#define	CACHE_CONTROL_SETVALUE	0x02000000
#define	CACHE_CONTROL_QUIT		0x04000000
// #define	CACHE_CONTROL_WATCH_ROOT	0x00100000

//
// 0x0000ffff is cache data code
//
#define	FILE_STATUS_INVALID		0x00000000
// 下段显示优先中
// 以下有向上的图标继承性
#define	FILE_STATUS_SYNCING		0x00000001
// 下段显示优先最小
#define	FILE_STATUS_ADDED		0x00000002
#define	FILE_STATUS_DELETE		0x00000004
#define	FILE_STATUS_MODIFIED	0x00000008
// 下段显示优先最大
#define	FILE_STATUS_CONFLICT	0x00000010 // 优先0
// 以下有向下的图标继承性
#define	FILE_STATUS_READONLY	0x00000020
#define	FILE_STATUS_FORBID		0x00000040
#define	FILE_STATUS_LOCKED		0x00000080
//
#define	FILE_STATUS_NORMAL		0x00000100
// 目录文件属性
#define	FILE_STATUS_FILE		0x00001000
#define	FILE_STATUS_DIRECTORY	0x00002000
// 虚拟属性
#define	WATCH_DIRECTORY			0x00004000
// #define	FILE_STATUS_ERASE		0x00008000
//
