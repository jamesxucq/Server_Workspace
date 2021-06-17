#include "StdAfx.h"

#include "OverlayBzl.h"
#include "ShellThread.h"

#include "logger.h"

static bool bProcessStarted = true;
static HANDLE hCommandThread = INVALID_HANDLE_VALUE;

DWORD WINAPI CommandWaitThread(LPVOID lpvParam) {
	LONG lProcessContinue;

	//ATLTRACE("CommandWaitThread started\n");
	bool *bProcessStarted = (bool *)lpvParam;
	// Create the IPC server
	osIPC::Server server(IPC_COMMAND_SHARED);
	// Enter a loop reading data
	struct CacheCommand tCommand;

	InterlockedExchange(&lProcessContinue, *bProcessStarted);
	while (lProcessContinue) {
		// Read client requests from the pipe
		if (server.read(&tCommand, sizeof(struct CacheCommand)) > 0)  
			NOverlayBzl::HandleCommandBzl(&tCommand);

		InterlockedExchange(&lProcessContinue, *bProcessStarted);
	} 

	// Success
	return 0;
}

DWORD ShellThread::EnsureThreadOpen() {
	DWORD dwThreadId; 

	hCommandThread = CreateThread( 
		NULL,              // no security attribute 
		0,                 // default stack size 
		CommandWaitThread, 
		(LPVOID) &bProcessStarted,    // thread parameter 
		0,                 // not suspended 
		&dwThreadId);      // returns thread ID 

	if (hCommandThread == NULL) {
		//DebugOutputLastError();
		return -1;
	}

	return 0;
}

void ShellThread::CloseThread() {
	InterlockedExchange((LONG*)&bProcessStarted, false);
}
