#include "StdAfx.h"

#include "OverlayBzl.h"
#include "ShellThread.h"

#define BUFFER_SIZE		4096 // 4K bytes
static bool bProcessStarted = true;

DWORD WINAPI CommandWaitThread(LPVOID lpvParam) {
	HANDLE hPipeLink = (HANDLE)lpvParam;
	struct CacheCommand tCommand;
	DWORD rlen;
	//
	// Enter a loop reading data
	LONG lProcessContinue;
	InterlockedExchange(&lProcessContinue, bProcessStarted);
	while (lProcessContinue) {
		// Read client requests from the pipe
		BOOL bResult = ReadFile(hPipeLink, &tCommand, sizeof(struct CacheCommand), &rlen, NULL);
		if (!bResult || !rlen) break;
		NOverlayBzl::HandleCommandBzl(&tCommand);
// logger(_T("d:\\cmd read.log"), _T("command:%08x %s\r\n"), tCommand.fstat, tCommand.path);
		InterlockedExchange(&lProcessContinue, bProcessStarted);
	}
	// Success
	FlushFileBuffers(hPipeLink); 
	DisconnectNamedPipe(hPipeLink); 
	CloseHandle(hPipeLink);
	//
	return 0x00;
}

DWORD WINAPI ConnectThread(LPVOID lpvParam) {
	HANDLE hPipeLink = INVALID_HANDLE_VALUE;
	//
	LONG lProcessContinue;
	InterlockedExchange(&lProcessContinue, bProcessStarted);
	while (lProcessContinue) {
		// Create the named pipe.
		hPipeLink = CreateNamedPipe(IPC_COMMAND_PIPE, PIPE_ACCESS_INBOUND, 
			PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, BUFFER_SIZE, 
			NMPWAIT_USE_DEFAULT_WAIT, NULL);
		if (INVALID_HANDLE_VALUE == hPipeLink) {
// logger(_T("d:\\shell.log"), _T("Unable to create named pipe %s w/err 0x%08lx\r\n"), IPC_COMMAND_PIPE, GetLastError());
			InterlockedExchange(&lProcessContinue, bProcessStarted);
			continue;
		}
		// Wait for the client to connect.
		BOOL bConnected = ConnectNamedPipe(hPipeLink, NULL) ? 
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (!bConnected) {
// logger(_T("d:\\shell.log"), _T("Error occurred while connecting to the client: 0x%08lx\r\n"), GetLastError()); 
			CloseHandle(hPipeLink);
			InterlockedExchange(&lProcessContinue, bProcessStarted);
			continue;
		}
// logger(_T("d:\\shell.log"), _T("The named pipe, %s, is created.\r\n"), IPC_COMMAND_PIPE);
		DWORD dwThreadId;
		HANDLE hCommandThread = CreateThread(NULL, 0, CommandWaitThread,
			(LPVOID)hPipeLink, 0, &dwThreadId); 
		if (NULL == hCommandThread) {
			// DebugOutputLastError();
			return ((DWORD)-1);
		}
		CloseHandle(hCommandThread);
	} 
//
	return 0x00;
}

DWORD ShellThread::OpenThread() {
	DWORD dwThreadId;
	// Create a thread which waits for incoming pipe connections 
	HANDLE hPipeThread = CreateThread(NULL, 0, ConnectThread,
		(LPVOID)NULL, 0, &dwThreadId);
	if (NULL == hPipeThread) CloseHandle(hPipeThread); 
	//
	return 0x00;
}

VOID ShellThread::CloseThread() {
	InterlockedExchange((LONG*)&bProcessStarted, false);
}
