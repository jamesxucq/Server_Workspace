#include "stdafx.h"

#include "ipc.h"
#include "ulog.h"

//
#define INVA_LINK_VALU		((ULONG)-1)

//
osIPC::Server::Server(void) {
	// Set default params
	m_hMapFile = INVALID_HANDLE_VALUE;
	m_hSignal = INVALID_HANDLE_VALUE;
	m_hAvail = INVALID_HANDLE_VALUE;
	m_hAtomAvail = INVALID_HANDLE_VALUE;
	m_hAtomFilled = INVALID_HANDLE_VALUE;
	m_pBuf = NULL;
};

osIPC::Server::Server(wchar_t *connectAddr) {
	// Set default params
	m_hMapFile = INVALID_HANDLE_VALUE;
	m_hSignal = INVALID_HANDLE_VALUE;
	m_hAvail = INVALID_HANDLE_VALUE;
	m_hAtomAvail = INVALID_HANDLE_VALUE;
	m_hAtomFilled = INVALID_HANDLE_VALUE;
	m_pBuf = NULL;

	// create the server
	create(connectAddr);
};

osIPC::Server::~Server(void) {
	// Close the server
	close();
};

bool osIPC::Server::create(wchar_t *connectAddr) {
	// Address of this server
	wchar_t szAddress[IPC_MAX_ADDR], szEvtAvail[IPC_MAX_ADDR], szEvtFilled[IPC_MAX_ADDR], 
		szAtomAvail[IPC_MAX_ADDR], szAtomFilled[IPC_MAX_ADDR], szShareMem[IPC_MAX_ADDR];

	// Determine the name of the memory
	_tcscpy_s(szAddress, IPC_MAX_ADDR, connectAddr);
	_stprintf_s(szEvtAvail, IPC_MAX_ADDR, _T("%s_evt_avail"), szAddress);
	_stprintf_s(szEvtFilled, IPC_MAX_ADDR, _T("%s_evt_filled"), szAddress);
	_stprintf_s(szAtomAvail, IPC_MAX_ADDR, _T("%s_mtex_avail"), szAddress);
	_stprintf_s(szAtomFilled, IPC_MAX_ADDR, _T("%s_mtex_filled"), szAddress);
	_stprintf_s(szShareMem, IPC_MAX_ADDR, _T("%s_share_mem"), szAddress);

	// Create the events
	m_hSignal = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)szEvtFilled);
	if (NULL == m_hSignal || INVALID_HANDLE_VALUE == m_hSignal) {
logger(_T("d:\\osIPC.log"), _T("server_create: failed: %01d\n"), __LINE__);
		return false; 
	}
	m_hAvail = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)szEvtAvail);
	if (NULL == m_hAvail || INVALID_HANDLE_VALUE == m_hAvail) { 
logger(_T("d:\\osIPC.log"), _T("server_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Cretate the Mutex
	m_hAtomAvail = CreateMutex(NULL, FALSE, szAtomAvail);
	if (NULL == m_hAtomAvail || INVALID_HANDLE_VALUE == m_hAtomAvail) { 
logger(_T("d:\\osIPC.log"), _T("server_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Cretate the Mutex
	m_hAtomFilled = CreateMutex(NULL, FALSE, szAtomFilled);
	if (NULL == m_hAtomFilled || INVALID_HANDLE_VALUE == m_hAtomFilled) { 
logger(_T("d:\\osIPC.log"), _T("server_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Create the file mapping
	m_hMapFile = CreateFileMapping(	INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(MemBuff),
		(LPCWSTR)szShareMem);
	if (NULL == m_hMapFile || INVALID_HANDLE_VALUE == m_hMapFile)  { 
logger(_T("d:\\osIPC.log"), _T("server_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Map to the file
	m_pBuf = (MemBuff*)MapViewOfFile( m_hMapFile,	// handle to map object
		FILE_MAP_ALL_ACCESS,	// read/write permission
		0,
		0,
		sizeof(MemBuff)); 
	if (NULL == m_pBuf) { 
logger(_T("d:\\osIPC.log"), _T("server_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Clear the buffer Initialize the pointers
	ZeroMemory(m_pBuf, sizeof(MemBuff));
	m_pBuf->m_Avail = 0;
	m_pBuf->m_Filled = INVA_LINK_VALU;
	m_pBuf->m_FillTail = INVA_LINK_VALU;

	// Add all the blocks into the available list
	int N = 0;
	for (; N < IPC_BLOCK_COUNT-1; N++) {
		// Add this block into the available list
		m_pBuf->m_Blocks[N].Next = N+1;
	}
	m_pBuf->m_Blocks[N].Next = INVA_LINK_VALU;

	// Release memory
	return true;
};

void osIPC::Server::close(void) {
	// Unmap the memory
	if (m_pBuf) {
		UnmapViewOfFile(m_pBuf);
		m_pBuf = NULL;
	}

	// Close the mutex
	if (m_hAtomFilled) {
		CloseHandle(m_hAtomFilled);
		m_hAtomFilled = INVALID_HANDLE_VALUE;
	}

	// Close the mutex
	if (m_hAtomAvail) {
		CloseHandle(m_hAtomAvail);
		m_hAtomAvail = INVALID_HANDLE_VALUE;
	}

	// Close the event
	if (m_hAvail) {
		CloseHandle(m_hAvail);
		m_hAvail = INVALID_HANDLE_VALUE;
	}

	// Close the event
	if (m_hSignal) {
		CloseHandle(m_hSignal);
		m_hSignal = INVALID_HANDLE_VALUE;
	}

	// Close the file handle
	if (m_hMapFile) {
		CloseHandle(m_hMapFile);
		m_hMapFile = INVALID_HANDLE_VALUE;
	}
};

ULONG osIPC::Server::getBlock(DWORD dwTimeout) {
	ULONG ulFilled;
	// Grab another block to read from
	// Enter a continous loop (this is to make sure the operation is atomic)
	for (;;) {
		WaitForSingleObject(m_hAtomFilled, INFINITE);
		ulFilled = m_pBuf->m_Filled;
// printf("----- sgb ulFilled:%d\n", ulFilled);
		ReleaseMutex(m_hAtomFilled);
		// Check if there is room to expand the read start cursor
		if (INVA_LINK_VALU == ulFilled) {
printf("server no room, Available:%d m_Filled:%d\n", m_pBuf->m_Avail, m_pBuf->m_Filled);
			// No room is available, wait for room to become available
			if (WaitForSingleObject(m_hSignal, dwTimeout) == WAIT_OBJECT_0)
				continue;

			// Timeout
			return INVA_LINK_VALU;
		}
		WaitForSingleObject(m_hAtomFilled, INFINITE);
		m_pBuf->m_Filled = m_pBuf->m_Blocks[ulFilled].Next;
		if(ulFilled = m_pBuf->m_FillTail) m_pBuf->m_FillTail = INVA_LINK_VALU;
// printf("----- sgb m_pBuf->m_Blocks[ulFilled].Next:%d\n", m_pBuf->m_Blocks[ulFilled].Next);
		ReleaseMutex(m_hAtomFilled);
	// printf("server Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Avail, m_pBuf->m_Filled, m_pBuf->doneWrite);
		return ulFilled;
	}
};

void osIPC::Server::retBlock(ULONG ulFilled) {
	ULONG ulAvail;
	//
	WaitForSingleObject(m_hAtomAvail, INFINITE);
	ulAvail = m_pBuf->m_Avail;
	// Signal availability of more data but only if a thread is waiting
	m_pBuf->m_Blocks[ulFilled].Next = ulAvail;
	m_pBuf->m_Avail = ulFilled;
// printf("----- srb ulAvail:%d ulFilled:%d\n", ulAvail, ulFilled);
	ReleaseMutex(m_hAtomAvail);
	//
	if (INVA_LINK_VALU == ulAvail) SetEvent(m_hAvail);
};

DWORD osIPC::Server::read(void *pBuff, DWORD buffSize, DWORD dwTimeout) {
	// Grab a block
	ULONG ulFilled = getBlock(dwTimeout);
	if (INVA_LINK_VALU == ulFilled) return 0;
// printf("----- ulFilled:%d\n", ulFilled);

	// Copy the data
	DWORD dwAmount = min(m_pBuf->m_Blocks[ulFilled].Amount, buffSize);
	memcpy(pBuff, m_pBuf->m_Blocks[ulFilled].Data, dwAmount);

	// Return the block
	retBlock(ulFilled);

	// Success
	return dwAmount;
};

osIPC::Client::Client(void) {
	// Set default params
	m_hMapFile = INVALID_HANDLE_VALUE;
	m_hSignal = INVALID_HANDLE_VALUE;
	m_hAvail = INVALID_HANDLE_VALUE;
	m_hAtomAvail = INVALID_HANDLE_VALUE;
	m_hAtomFilled = INVALID_HANDLE_VALUE;
	m_pBuf = NULL;
};

osIPC::Client::Client(wchar_t *connectAddr) {
	// Set default params
	m_hMapFile = INVALID_HANDLE_VALUE;
	m_hSignal = INVALID_HANDLE_VALUE;
	m_hAvail = INVALID_HANDLE_VALUE;
	m_hAtomAvail = INVALID_HANDLE_VALUE;
	m_hAtomFilled = INVALID_HANDLE_VALUE;
	m_pBuf = NULL;

	// create the server
	create(connectAddr);
};

osIPC::Client::~Client(void) {
	// Close the server
	close();
};

bool osIPC::Client::create(wchar_t *connectAddr) {
	// Address of this server
	wchar_t szAddress[MAX_PATH], szEvtAvail[MAX_PATH], szEvtFilled[IPC_MAX_ADDR], 
		szAtomAvail[IPC_MAX_ADDR], szAtomFilled[IPC_MAX_ADDR], szShareMem[MAX_PATH];

	// Determine the name of the memory
	_tcscpy_s(szAddress, MAX_PATH, connectAddr);
	_stprintf_s(szEvtAvail, MAX_PATH, _T("%s_evt_avail"), szAddress);
	_stprintf_s(szEvtFilled, IPC_MAX_ADDR, _T("%s_evt_filled"), szAddress);
	_stprintf_s(szAtomAvail, IPC_MAX_ADDR, _T("%s_mtex_avail"), szAddress);
	_stprintf_s(szAtomFilled, IPC_MAX_ADDR, _T("%s_mtex_filled"), szAddress);
	_stprintf_s(szShareMem, MAX_PATH, _T("%s_share_mem"), szAddress);

	// Create the events
	m_hSignal = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)szEvtFilled);
	if (NULL == m_hSignal || INVALID_HANDLE_VALUE == m_hSignal) { 
logger(_T("d:\\osIPC.log"), _T("client_create: failed: %01d\n"), __LINE__);
		return false; 
	}
	m_hAvail = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)szEvtAvail);
	if (NULL == m_hAvail || INVALID_HANDLE_VALUE == m_hAvail) { 
logger(_T("d:\\osIPC.log"), _T("client_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Cretate the Mutex
	m_hAtomAvail = CreateMutex(NULL, FALSE, szAtomAvail);
	if (NULL == m_hAtomAvail || INVALID_HANDLE_VALUE == m_hAtomAvail) { 
logger(_T("d:\\osIPC.log"), _T("client_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Cretate the Mutex
	m_hAtomFilled = CreateMutex(NULL, FALSE, szAtomFilled);
	if (NULL == m_hAtomFilled || INVALID_HANDLE_VALUE == m_hAtomFilled) { 
logger(_T("d:\\osIPC.log"), _T("client_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Open the shared file
	m_hMapFile = OpenFileMapping( FILE_MAP_ALL_ACCESS,	// read/write access
		FALSE,					// do not inherit the name
		(LPCWSTR)szShareMem);	// name of mapping object
	if (NULL == m_hMapFile || INVALID_HANDLE_VALUE == m_hMapFile)  { 
logger(_T("d:\\osIPC.log"), _T("client_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Map to the file
	m_pBuf = (MemBuff*)MapViewOfFile( m_hMapFile,				// handle to map object
		FILE_MAP_ALL_ACCESS,	// read/write permission
		0,
		0,
		sizeof(MemBuff)); 
	if (NULL == m_pBuf) { 
logger(_T("d:\\osIPC.log"), _T("client_create: failed: %01d\n"), __LINE__);
		return false; 
	}

	// Release memory
	return true;
};

void osIPC::Client::close(void) {
	// Unmap the memory
	if (m_pBuf) {
		UnmapViewOfFile(m_pBuf);
		m_pBuf = NULL;
	}

	// Close the mutex
	if (m_hAtomFilled) {
		CloseHandle(m_hAtomFilled);
		m_hAtomFilled = INVALID_HANDLE_VALUE;
	}

	// Close the mutex
	if (m_hAtomAvail) {
		CloseHandle(m_hAtomAvail);
		m_hAtomAvail = INVALID_HANDLE_VALUE;
	}

	// Close the event
	if (m_hAvail) {
		CloseHandle(m_hAvail);
		m_hAvail = INVALID_HANDLE_VALUE;
	}

	// Close the event
	if (m_hSignal) {
		CloseHandle(m_hSignal);
		m_hSignal = INVALID_HANDLE_VALUE;
	}

	// Close the file handle
	if (m_hMapFile) {
		CloseHandle(m_hMapFile);
		m_hMapFile = INVALID_HANDLE_VALUE;
	}
};

ULONG osIPC::Client::getBlock(DWORD dwTimeout) {
	ULONG ulAvail;
	// Grab another block to write too
	// Enter a continous loop (this is to make sure the operation is atomic)
	for (;;) {
		WaitForSingleObject(m_hAtomAvail, INFINITE);
		ulAvail = m_pBuf->m_Avail;
// printf("----- cgb ulAvail:%d\n", ulAvail);
		ReleaseMutex(m_hAtomAvail);
		// Check if there is room to expand the write start cursor
		if (INVA_LINK_VALU == ulAvail) {
printf("client no room, Available:%d m_Filled:%d\n", m_pBuf->m_Avail, m_pBuf->m_Filled);

			// No room is available, wait for room to become available
			if (WaitForSingleObject(m_hAvail, dwTimeout) == WAIT_OBJECT_0)
				continue;

			// Timeout
			return INVA_LINK_VALU;
		}
		WaitForSingleObject(m_hAtomAvail, INFINITE);
		m_pBuf->m_Avail = m_pBuf->m_Blocks[ulAvail].Next;
// printf("----- cgb m_pBuf->m_Blocks[ulAvail].Next:%d\n", m_pBuf->m_Blocks[ulAvail].Next);
		ReleaseMutex(m_hAtomAvail);
printf("client Available:%d m_Filled:%d\n", m_pBuf->m_Avail, m_pBuf->m_Filled);
		return ulAvail;
	}
};

void osIPC::Client::postBlock(ULONG ulAvail) {
	ULONG ulFilled;
	//
	WaitForSingleObject(m_hAtomFilled, INFINITE);
	ulFilled = m_pBuf->m_Filled;
	//
	if(INVA_LINK_VALU != m_pBuf->m_FillTail)
		m_pBuf->m_Blocks[m_pBuf->m_FillTail].Next = ulAvail;
// printf("----- m_pBuf->m_FillTail:%d\n", m_pBuf->m_FillTail);
	m_pBuf->m_FillTail = ulAvail;
	//
	if(INVA_LINK_VALU == m_pBuf->m_Filled) m_pBuf->m_Filled = ulAvail;
// printf("----- crb ulAvail:%d ulFilled:%d m_pBuf->m_FillTail:%d\n", ulAvail, ulFilled, m_pBuf->m_FillTail);
	ReleaseMutex (m_hAtomFilled);
	// Signal availability of more data but only if threads are waiting
	if (INVA_LINK_VALU == ulFilled) SetEvent(m_hSignal);
};

DWORD osIPC::Client::write(void *pBuff, DWORD amount, DWORD dwTimeout) {
	// Grab a block
	ULONG ulAvail = getBlock(dwTimeout);
	if (INVA_LINK_VALU == ulAvail) return 0;
// printf("----- ulAvail:%d\n", ulAvail);

	// Copy the data
	DWORD dwAmount = min(amount, IPC_BLOCK_SIZE);
	memcpy(m_pBuf->m_Blocks[ulAvail].Data, pBuff, dwAmount);
	m_pBuf->m_Blocks[ulAvail].Amount = dwAmount;
	m_pBuf->m_Blocks[ulAvail].Next = INVA_LINK_VALU;

	// Post the block
	postBlock(ulAvail);

	// Success
	return dwAmount;
};

bool osIPC::Client::waitAvailable(DWORD dwTimeout) {
	// Wait on the available event
	if (WaitForSingleObject(m_hAvail, dwTimeout) != WAIT_OBJECT_0)
		return false;

	// Success
	return true;
};