#include "StdAfx.h"
#include "ShellLinker.h"
#include "utility/ulog.h"
// #include "third_party.h"

#define BUFFER_SIZE		4096 // 4K bytes

CShellLinker::CShellLinker(void):
hPipeLink(INVALID_HANDLE_VALUE),
fConnected(FALSE)
{
}

CShellLinker::~CShellLinker(void) {
}

VOID CShellLinker::EnsureLinkOpen(TCHAR *szLinkName) {
//
	for (;;) {
		hPipeLink = CreateFile(szLinkName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		// Break if the pipe handle is valid. 
		if (INVALID_HANDLE_VALUE != hPipeLink) 
			break; 
		// Exit if an error other than ERROR_PIPE_BUSY occurs
		// All pipe instances are busy, so wait for 5 seconds
		if (ERROR_PIPE_BUSY!=GetLastError() || !WaitNamedPipe(szLinkName, 512)) {
logger(_T("d:\\shell.log"), _T("Unable to open named pipe %s w/err 0x%08lx\r\n"), szLinkName, GetLastError());
			return;
		}
	}
// logger(_T("d:\\shell.log"), _T("The named pipe, %s, is connected.\r\n"), szLinkName);
	// Set data to be read from the pipe as a stream of messages
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if(!SetNamedPipeHandleState(hPipeLink, &dwMode, NULL, NULL)){
logger(_T("d:\\shell.log"), _T("SetNamedPipeHandleState failed w/err 0x%08lx\r\n"), GetLastError()); 
		return ;
	}
	//
	fConnected = TRUE;
}

VOID CShellLinker::CloseLink() {
	fConnected = FALSE;
	// Close the pipe.
	CloseHandle(hPipeLink); 
// _LOG_DEBUG(_T("client closed."));
}


DWORD CShellLinker::LinkSend(char *szMessage, size_t length) {
	DWORD wlen = 0x00;
	//
	if(fConnected) {
		// Send one message to the pipe.
		BOOL bResult = WriteFile(hPipeLink, szMessage, length, &wlen, NULL);
		if (!bResult || length!=wlen) {
logger(_T("d:\\shell.log"), _T("WriteFile failed w/err 0x%08lx\r\n"), GetLastError());
			return 0x00;
		}	
	}
	// Write some data
	return wlen;
}

CCommandLinker::CCommandLinker(void) {
}

CCommandLinker::~CCommandLinker(void) {
}
CCommandLinker objCommandLinker;

DWORD CCommandLinker::Initialize() {
	EnsureLinkOpen(IPC_COMMAND_PIPE);
	return 0x00;
}

DWORD CCommandLinker::Finalize() {
	CloseLink();
	return 0x00;
}

DWORD CCommandLinker::SendCommand(DWORD Command) {
	struct CacheCommand tCommand;
//
	memset(&tCommand, '\0', sizeof(struct CacheCommand));
	tCommand.fstat = Command;
// logger(_T("d:\\cmd write.log"), _T("+command:%08x %s\r\n"), tCommand.fstat, tCommand.path);
	// Write some data
	DWORD wlen = 0x00;
	if(fConnected) {
		// Send one message to the pipe.
		BOOL bResult = WriteFile(hPipeLink, &tCommand, sizeof(struct CacheCommand), &wlen, NULL);
		if (!bResult || sizeof(struct CacheCommand)!=wlen) {
logger(_T("d:\\shell.log"), _T("WriteFile failed w/err 0x%08lx\r\n"), GetLastError());
			return 0x00;
		}	
	}
	//
	return wlen;
}

DWORD CCommandLinker::SendCommand(DWORD Command, const TCHAR *path) {
	struct CacheCommand tCommand;
//
	memset(&tCommand, '\0', sizeof(struct CacheCommand));
	_tcscpy_s(tCommand.path, MAX_PATH, path);
	tCommand.fstat = Command;
// logger(_T("d:\\cmd write.log"), _T("-command:%08x %s\r\n"), tCommand.fstat, tCommand.path);
	// Write some data
	DWORD wlen = 0x00;
	if(fConnected) {
		// Send one message to the pipe.
		BOOL bResult = WriteFile(hPipeLink, &tCommand, sizeof(struct CacheCommand), &wlen, NULL);
		if (!bResult || sizeof(struct CacheCommand)!=wlen) {
logger(_T("d:\\shell.log"), _T("WriteFile failed w/err 0x%08lx\r\n"), GetLastError());
			return 0x00;
		}	
	}
	//
	return wlen;
}

//
VOID CCommandLinker::FastSend(DWORD Command, const TCHAR *path) {
	struct CacheCommand tCommand;
	_tcscpy_s(tCommand.path, MAX_PATH, path);
	tCommand.fstat = Command;
	DWORD wlen = 0x00;
	if(fConnected) {
		// Send one message to the pipe.
		BOOL bResult = WriteFile(hPipeLink, &tCommand, sizeof(struct CacheCommand), &wlen, NULL);
		if (!bResult || sizeof(struct CacheCommand)!=wlen) {
logger(_T("d:\\shell.log"), _T("WriteFile failed w/err 0x%08lx\r\n"), GetLastError());
		}	
	}
}