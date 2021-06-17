#include "StdAfx.h"
#include "CacheLinker.h"


CCacheLinker::CCacheLinker(void)
	: m_hPipe(INVALID_HANDLE_VALUE)
{
}

CCacheLinker::~CCacheLinker(void)
{
	ClosePipe();
}

bool CCacheLinker::EnsurePipeOpen(wchar_t *lpszPipeName, DWORD fdwOpenMode)
{
	if(m_hPipe != INVALID_HANDLE_VALUE) return true;

	m_hPipe = CreateFile(
		lpszPipeName,				// pipe name
		fdwOpenMode,					// write access
		0,								// no sharing
		NULL,							// default security attributes
		OPEN_EXISTING,					// opens existing pipe
		FILE_FLAG_OVERLAPPED,			// default attributes
		NULL);							// no template file

	if (m_hPipe == INVALID_HANDLE_VALUE && GetLastError() == ERROR_PIPE_BUSY)
	{
		// TSVNCache is running but is busy connecting a different client.
		// Do not give up immediately but wait for a few milliseconds until
		// the server has created the next pipe instance
		if (WaitNamedPipe(lpszPipeName, 50))
		{
			m_hPipe = CreateFile(
				lpszPipeName,				// pipe name
				fdwOpenMode,					// read and write access
				0,								// no sharing
				NULL,							// default security attributes
				OPEN_EXISTING,					// opens existing pipe
				FILE_FLAG_OVERLAPPED,			// default attributes
				NULL);							// no template file
		}
	}

	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		// The pipe connected; change to message-read mode.
		DWORD dwMode;

		dwMode = PIPE_READMODE_MESSAGE;
		if(!SetNamedPipeHandleState(
			m_hPipe,    // pipe handle
			&dwMode,  // new pipe mode
			NULL,     // don't set maximum bytes
			NULL))    // don't set maximum time
		{
			//TRACE("SetNamedPipeHandleState failed");
			//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
			CloseHandle(m_hPipe);
			m_hPipe = INVALID_HANDLE_VALUE;
			return false;
		}
	} else {
		//TRACE( _T("open pipe failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);	
		return false;
	}

	return true;
}

void CCacheLinker::ClosePipe()
{
	if(m_hPipe != INVALID_HANDLE_VALUE) {
		DisconnectNamedPipe(m_hPipe); 
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
}


DWORD CCacheLinker::PipeSend(char *szMessage, size_t len)
{
	DWORD	  cbWritten;

	BOOL fSuccess = WriteFile( 
		m_hPipe,	// handle to pipe 
		szMessage,			// buffer to write from 
		len,	// number of bytes to write 
		&cbWritten,		// number of bytes written 
		NULL);			// not overlapped I/O 
	if (! fSuccess || len != cbWritten) {
		ClosePipe();
		return 0;
	}

	return cbWritten;
}

CCmdCacheLinker::CCmdCacheLinker(void)
{
}

CCmdCacheLinker::~CCmdCacheLinker(void)
{
}
CCmdCacheLinker OCmdCacheLinker;

DWORD CCmdCacheLinker::Create()
{
	if(!EnsurePipeOpen(SDCACHE_COMMAND_PIPE, GENERIC_WRITE)) 
		return -1;

	return 0;
}

DWORD CCmdCacheLinker::Destroy()
{
	//OCmdCacheLinker.QuitCacheServer();//just for test
	ClosePipe();
	
	return 0;
}

DWORD CCmdCacheLinker::SendFileStatus(const wchar_t *path, DWORD file_status)
{
	struct SDCacheCommand stCommand;
	DWORD	  cbWritten;
	
	memset(&stCommand, '\0', sizeof(struct SDCacheCommand));
	_tcscpy_s(stCommand.path, MAX_PATH, path);
	stCommand.file_status = file_status;
	BOOL fSuccess = WriteFile( 
		m_hPipe,	// handle to pipe 
		&stCommand,			// buffer to write from 
		sizeof(struct SDCacheCommand),	// number of bytes to write 
		&cbWritten,		// number of bytes written 
		NULL);			// not overlapped I/O 
	if (! fSuccess || sizeof(struct SDCacheCommand) != cbWritten) {
		ClosePipe();
		return 0;
	}

	return cbWritten;
}

DWORD CCmdCacheLinker::SendCommand(DWORD Command)
{
	struct SDCacheCommand stCommand;
	DWORD	  cbWritten;
	
	memset(&stCommand, '\0', sizeof(struct SDCacheCommand));
	stCommand.file_status = Command;
	BOOL fSuccess = WriteFile( 
		m_hPipe,	// handle to pipe 
		&stCommand,			// buffer to write from 
		sizeof(struct SDCacheCommand),	// number of bytes to write 
		&cbWritten,		// number of bytes written 
		NULL);			// not overlapped I/O 
	if (! fSuccess || sizeof(struct SDCacheCommand) != cbWritten) {
		ClosePipe();
		return 0;
	}

	return cbWritten;
}

DWORD CCmdCacheLinker::SendCommand(DWORD Command, const wchar_t *path)
{
	struct SDCacheCommand stCommand;
	DWORD	  cbWritten;
	
	memset(&stCommand, '\0', sizeof(struct SDCacheCommand));
	_tcscpy_s(stCommand.path, MAX_PATH, path);
	stCommand.file_status = Command;
	BOOL fSuccess = WriteFile( 
		m_hPipe,	// handle to pipe 
		&stCommand,			// buffer to write from 
		sizeof(struct SDCacheCommand),	// number of bytes to write 
		&cbWritten,		// number of bytes written 
		NULL);			// not overlapped I/O 
	if (! fSuccess || sizeof(struct SDCacheCommand) != cbWritten) {
		ClosePipe();
		return 0;
	}

	return cbWritten;
}


CShellCacheLinker::CShellCacheLinker(void):
m_iPipeOpened(0)
{
}

CShellCacheLinker::~CShellCacheLinker(void)
{
}
CShellCacheLinker OShellCacheLinker;

bool CShellCacheLinker::EnsurePipeOpen(wchar_t *lpszPipeName, DWORD fdwOpenMode)
{
	if(m_iPipeOpened++ && INVALID_HANDLE_VALUE!=m_hPipe) return true;
	return CCacheLinker::EnsurePipeOpen(lpszPipeName, fdwOpenMode);
}

void CShellCacheLinker::ClosePipe()
{
	m_iPipeOpened--;
	if(!m_iPipeOpened) CCacheLinker::ClosePipe();
}

DWORD CShellCacheLinker::PipeSendRecv(BYTE *pRecvData, BYTE *szMessage, size_t len)
{
    DWORD cbBytesRead, cbWritten;

	BOOL fSuccess = WriteFile( 
		m_hPipe,	// handle to pipe 
		szMessage,			// buffer to write from 
		len,	// number of bytes to write 
		&cbWritten,		// number of bytes written 
		NULL);			// not overlapped I/O 
	if (! fSuccess || len != cbWritten) {
		ClosePipe();
		return 0;
	}

    fSuccess = ReadFile(
        m_hPipe,        // handle to pipe
        &pRecvData,    // buffer to receive data
        len, // size of buffer
        &cbBytesRead, // number of bytes read
        NULL);        // not overlapped I/O
    if (! fSuccess || cbBytesRead == 0) {
		ClosePipe();
		return 0;
	}

	return cbBytesRead;
}

DWORD CShellCacheLinker::ShellRequestStatus(DWORD *dwFileStatus, LPCWSTR pwszPath, DWORD dwAttrib)
{
	struct SDShellRequest stRequest;
	struct SDCacheResponse stResponse;
    DWORD cbBytesRead, cbWritten;

	memset(&stRequest, '\0', sizeof(struct SDShellRequest));
	memset(&stResponse, '\0', sizeof(struct SDCacheResponse));
	_tcscpy_s(stRequest.path, _tcslen(pwszPath)+1, pwszPath);
	stRequest.dwAttrib = dwAttrib;
    DWORD requestLength = sizeof(struct SDShellRequest);

	BOOL fSuccess = WriteFile( 
		m_hPipe,	// handle to pipe 
		&stRequest,			// buffer to write from 
		requestLength,	// number of bytes to write 
		&cbWritten,		// number of bytes written 
		NULL);			// not overlapped I/O 
	if (! fSuccess || requestLength != cbWritten) {
		ClosePipe();
		return 0;
	}

    fSuccess = ReadFile(
        m_hPipe,        // handle to pipe
        &stResponse,    // buffer to receive data
		sizeof(struct SDCacheResponse), // size of buffer
        &cbBytesRead, // number of bytes read
        NULL);        // not overlapped I/O
    if (! fSuccess || cbBytesRead == 0) {
		ClosePipe();
		return 0;
	}

	//if(_tcscmp(stResponse.path, pwszPath)) return 0; //just for test
	*dwFileStatus = stResponse.file_status;

	return cbBytesRead;
}

DWORD CShellCacheLinker::ShellRequestStatusExt(DWORD *dwFileStatus, LPCWSTR pwszPath, DWORD dwAttrib)
{
	static wchar_t sRequestPath[MAX_PATH]; // fast cache
	static DWORD file_status; // fast cache
	DWORD iRequestReturn = true;

	if(_tcscmp(sRequestPath, pwszPath)) {
		iRequestReturn = ShellRequestStatus(&file_status, pwszPath, dwAttrib);	
		if(iRequestReturn) _tcscpy_s(sRequestPath, pwszPath);
	}
	*dwFileStatus = file_status;

	return iRequestReturn;
}