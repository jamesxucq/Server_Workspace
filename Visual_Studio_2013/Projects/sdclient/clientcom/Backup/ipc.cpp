#include "stdafx.h"
#include "ipc.h"

#define DONE_WRITE	0x00000001
#define CLEAR_WRITE 0xfffffff0

#define DONE_FILLED 0x00010000
#define CLEAR_FILLED 0xfff0ffff


osIPC::Server::Server(void) {
	// Set default params
	m_hMapFile = INVALID_HANDLE_VALUE;
	m_hSignal = INVALID_HANDLE_VALUE;
	m_hAvail = INVALID_HANDLE_VALUE;
	m_hMutex = INVALID_HANDLE_VALUE;
	m_pBuf = NULL;
};

osIPC::Server::Server(wchar_t *connectAddr) {
	// Set default params
	m_hMapFile = INVALID_HANDLE_VALUE;
	m_hSignal = INVALID_HANDLE_VALUE;
	m_hAvail = INVALID_HANDLE_VALUE;
	m_hMutex = INVALID_HANDLE_VALUE;
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
	wchar_t szAddress[IPC_MAX_ADDR], szEvtAvail[IPC_MAX_ADDR], szEvtFilled[IPC_MAX_ADDR], szMutexName[IPC_MAX_ADDR], szMemName[IPC_MAX_ADDR];

	// Determine the name of the memory
	_tcscpy_s(szAddress, IPC_MAX_ADDR, connectAddr);
	_stprintf_s(szEvtAvail, IPC_MAX_ADDR, _T("%s_evt_avail"), szAddress);
	_stprintf_s(szEvtFilled, IPC_MAX_ADDR, _T("%s_evt_filled"), szAddress);
	_stprintf_s(szMutexName, IPC_MAX_ADDR, _T("%s_mutex"), szAddress);
	_stprintf_s(szMemName, IPC_MAX_ADDR, _T("%s_mem"), szAddress);

	// Create the events
	m_hSignal = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)szEvtFilled);
	if (NULL == m_hSignal || INVALID_HANDLE_VALUE == m_hSignal) {
		printf("server_create: failed: %01d\n", __LINE__); 
		return false; 
	}
	m_hAvail = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)szEvtAvail);
	if (NULL == m_hAvail || INVALID_HANDLE_VALUE == m_hAvail) { 
		printf("server_create: failed: %01d\n", __LINE__); 
		return false; 
	}

	// Cretate the Mutex
	m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
	if (NULL == m_hMutex || INVALID_HANDLE_VALUE == m_hMutex) { 
		printf("server_create: failed: %01d\n", __LINE__);
		return false; 
	}

	// Create the file mapping
	m_hMapFile = CreateFileMapping(	INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(MemBuff),
		(LPCWSTR)szMemName);
	if (NULL == m_hMapFile || INVALID_HANDLE_VALUE == m_hMapFile)  { 
		printf("server_create: failed: %01d\n", __LINE__); 
		return false; 
	}

	// Map to the file
	m_pBuf = (MemBuff*)MapViewOfFile( m_hMapFile,				// handle to map object
		FILE_MAP_ALL_ACCESS,	// read/write permission
		0,
		0,
		sizeof(MemBuff)); 
	if (NULL == m_pBuf) { 
		printf("server_create: failed: %01d\n", __LINE__); 
		return false; 
	}

	// Clear the buffer
	ZeroMemory(m_pBuf, sizeof(MemBuff));

	// Initialize the pointers
	m_pBuf->m_Available = 0;
	m_pBuf->m_Filled = 0;

	// Add all the blocks into the available list
	int N = 0;
	for (; N < IPC_BLOCK_COUNT-1; N++) {
		// Add this block into the available list
		m_pBuf->m_Blocks[N].Next = N+1;
	}
	m_pBuf->m_Blocks[N].Next = 0;

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
	if (m_hMutex) {
		CloseHandle(m_hMutex);
		m_hMutex = INVALID_HANDLE_VALUE;
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

osIPC::Block* osIPC::Server::getBlock(DWORD dwTimeout) {
	// Grab another block to read from
	// Enter a continous loop (this is to make sure the operation is atomic)
	for (;;) {
		// Check if there is room to expand the read start cursor
		if (m_pBuf->m_Available==m_pBuf->m_Filled && !(m_pBuf->doneWrite&DONE_WRITE)) {
			// printf("server no room, Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Available, m_pBuf->m_Filled, m_pBuf->doneWrite);
			// No room is available, wait for room to become available
			if (WaitForSingleObject(m_hSignal, dwTimeout) == WAIT_OBJECT_0)
				continue;

			// Timeout
			return NULL;
		}
		// printf("server Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Available, m_pBuf->m_Filled, m_pBuf->doneWrite);
		return m_pBuf->m_Blocks + m_pBuf->m_Filled;
	}
};

void osIPC::Server::retBlock(Block* pBlock) {
	WaitForSingleObject(m_hMutex, INFINITE);
	// Signal availability of more data but only if a thread is waiting
	if (m_pBuf->m_Available==m_pBuf->m_Filled && m_pBuf->doneWrite&DONE_FILLED) {
		m_pBuf->doneWrite &= CLEAR_FILLED;
		SetEvent(m_hAvail);	
		// printf("server set event, Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Available, m_pBuf->m_Filled, m_pBuf->doneWrite);
	}	

	// Make sure the operation is atomic
	m_pBuf->m_Filled = pBlock->Next;

	if (m_pBuf->m_Available == m_pBuf->m_Filled) m_pBuf->doneWrite &= CLEAR_WRITE;
	ReleaseMutex(m_hMutex);
};

DWORD osIPC::Server::read(void *pBuff, DWORD buffSize, DWORD dwTimeout) {
	// Grab a block
	Block *pBlock = getBlock(dwTimeout);
	if (!pBlock) return 0;

	// Copy the data
	DWORD dwAmount = min(pBlock->Amount, buffSize);
	memcpy(pBuff, pBlock->Data, dwAmount);

	// Return the block
	retBlock(pBlock);

	// Success
	return dwAmount;
};

osIPC::Client::Client(void) {
	// Set default params
	m_hMapFile = INVALID_HANDLE_VALUE;
	m_hSignal = INVALID_HANDLE_VALUE;
	m_hAvail = INVALID_HANDLE_VALUE;
	m_hMutex = INVALID_HANDLE_VALUE;
	m_pBuf = NULL;
};

osIPC::Client::Client(wchar_t *connectAddr) {
	// Set default params
	m_hMapFile = INVALID_HANDLE_VALUE;
	m_hSignal = INVALID_HANDLE_VALUE;
	m_hAvail = INVALID_HANDLE_VALUE;
	m_hMutex = INVALID_HANDLE_VALUE;
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
	wchar_t szAddress[MAX_PATH], szEvtAvail[MAX_PATH], szEvtFilled[IPC_MAX_ADDR], szMutexName[IPC_MAX_ADDR], szMemName[MAX_PATH];

	// Determine the name of the memory
	_tcscpy_s(szAddress, MAX_PATH, connectAddr);
	_stprintf_s(szEvtAvail, MAX_PATH, _T("%s_evt_avail"), szAddress);
	_stprintf_s(szEvtFilled, IPC_MAX_ADDR, _T("%s_evt_filled"), szAddress);
	_stprintf_s(szMutexName, IPC_MAX_ADDR, _T("%s_mutex"), szAddress);
	_stprintf_s(szMemName, MAX_PATH, _T("%s_mem"), szAddress);

	// Create the events
	m_hSignal = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)szEvtFilled);
	if (NULL == m_hSignal || INVALID_HANDLE_VALUE == m_hSignal) { 
		printf("server_create: failed: %01d\n", __LINE__); 
		return false; 
	}
	m_hAvail = CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)szEvtAvail);
	if (NULL == m_hAvail || INVALID_HANDLE_VALUE == m_hAvail) { 
		printf("client_create: failed: %01d\n", __LINE__); 
		return false; 
	}

	// Cretate the Mutex
	m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
	if (NULL == m_hMutex || INVALID_HANDLE_VALUE == m_hMutex) { 
		printf("server_create: failed: %01d\n", __LINE__); 
		return false; 
	}

	// Open the shared file
	m_hMapFile = OpenFileMapping( FILE_MAP_ALL_ACCESS,	// read/write access
		FALSE,					// do not inherit the name
		(LPCWSTR)szMemName);	// name of mapping object
	if (NULL == m_hMapFile || INVALID_HANDLE_VALUE == m_hMapFile)  { 
		printf("client_create: failed: %01d\n", __LINE__); 
		return false; 
	}

	// Map to the file
	m_pBuf = (MemBuff*)MapViewOfFile( m_hMapFile,				// handle to map object
		FILE_MAP_ALL_ACCESS,	// read/write permission
		0,
		0,
		sizeof(MemBuff)); 
	if (NULL == m_pBuf) { 
		printf("server_create: failed: %01d\n", __LINE__); 
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
	if (m_hMutex) {
		CloseHandle(m_hMutex);
		m_hMutex = INVALID_HANDLE_VALUE;
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

osIPC::Block* osIPC::Client::getBlock(DWORD dwTimeout) {
	// Grab another block to write too
	// Enter a continous loop (this is to make sure the operation is atomic)
	for (;;) {
		// Check if there is room to expand the write start cursor
		if (m_pBuf->m_Filled==m_pBuf->m_Available && m_pBuf->doneWrite&DONE_FILLED) {
			// printf("client no room, Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Available, m_pBuf->m_Filled, m_pBuf->doneWrite);

			// No room is available, wait for room to become available
			if (WaitForSingleObject(m_hAvail, dwTimeout) == WAIT_OBJECT_0)
				continue;

			// Timeout
			return NULL;
		}
		// printf("client Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Available, m_pBuf->m_Filled, m_pBuf->doneWrite);
		return m_pBuf->m_Blocks + m_pBuf->m_Available;
	}
};

void osIPC::Client::postBlock(Block *pBlock) {
	WaitForSingleObject(m_hMutex, INFINITE);
	// Signal availability of more data but only if threads are waiting
	if (m_pBuf->m_Filled==m_pBuf->m_Available && !(m_pBuf->doneWrite&DONE_WRITE)) {
		m_pBuf->doneWrite |= DONE_WRITE;
		SetEvent(m_hSignal);
		// printf("client set event, Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Available, m_pBuf->m_Filled, m_pBuf->doneWrite);
	}

	// Make sure the operation is atomic
	m_pBuf->m_Available = pBlock->Next;

	if (m_pBuf->m_Filled == m_pBuf->m_Available) m_pBuf->doneWrite |= DONE_FILLED;
	ReleaseMutex (m_hMutex);
};

DWORD osIPC::Client::write(void *pBuff, DWORD amount, DWORD dwTimeout) {
	// Grab a block
	Block *pBlock = getBlock(dwTimeout);
	if (!pBlock) return 0;

	// Copy the data
	DWORD dwAmount = min(amount, IPC_BLOCK_SIZE);
	memcpy(pBlock->Data, pBuff, dwAmount);
	pBlock->Amount = dwAmount;

	// Post the block
	postBlock(pBlock);

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