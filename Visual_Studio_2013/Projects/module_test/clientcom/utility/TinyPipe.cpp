#include "StdAfx.h"
#include "TinyPipe.h"

#define DONE_WRITE	0x00000001
#define CLEAR_WRITE 0xfffffff0

#define DONE_FILLED 0x00010000
#define CLEAR_FILLED 0xfff0ffff

CTinyPipe::CTinyPipe(void)
{
	// Create the events
	m_hSignal = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hSignal == NULL || m_hSignal == INVALID_HANDLE_VALUE) {
		printf("server_create: failed: %01d\n", __LINE__); 
		return; 
	}
	m_hAvail = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hAvail == NULL || m_hAvail == INVALID_HANDLE_VALUE) { 
		printf("server_create: failed: %01d\n", __LINE__); 
		return; 
	}

	InitializeCriticalSection(&m_csPipeLink);

	// malloc buffer memory
	m_pBuf = (MemBuff*)malloc(sizeof(MemBuff));
	if (m_pBuf == NULL) { 
		printf("server_create: failed: %01d\n", __LINE__); 
		return; 
	}

	// Clear the buffer
	ZeroMemory(m_pBuf, sizeof(MemBuff));

	// Initialize the pointers
	m_pBuf->m_Available = 0;
	m_pBuf->m_Filled = 0;

	// Add all the blocks into the available list
	int N = 0;
	for (; N < PIPE_BLOCK_COUNT-1; N++) {
		// Add this block into the available list
		m_pBuf->m_Blocks[N].Next = N+1;
	}
	m_pBuf->m_Blocks[N].Next = 0;

	// Release memory
}

CTinyPipe::~CTinyPipe(void)
{
	// Unmap the memory
	if (m_pBuf) {
		free(m_pBuf);
		m_pBuf = NULL;
	}

	DeleteCriticalSection(&m_csPipeLink);

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
}

CTinyPipe::Block* CTinyPipe::getReady(DWORD dwTimeout) {
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

void CTinyPipe::retBlock(Block* pBlock) {
	EnterCriticalSection(&m_csPipeLink);
	// Signal availability of more data but only if a thread is waiting
	if (m_pBuf->m_Available==m_pBuf->m_Filled && m_pBuf->doneWrite&DONE_FILLED) {
		m_pBuf->doneWrite &= CLEAR_FILLED;
		SetEvent(m_hAvail);	
		// printf("server set event, Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Available, m_pBuf->m_Filled, m_pBuf->doneWrite);
	}	

	// Make sure the operation is atomic
	m_pBuf->m_Filled = pBlock->Next;

	if (m_pBuf->m_Available == m_pBuf->m_Filled) m_pBuf->doneWrite &= CLEAR_WRITE;
	LeaveCriticalSection(&m_csPipeLink);
};

DWORD CTinyPipe::read(void *pBuff, DWORD buffSize, DWORD dwTimeout) {
	// Grab a block
	Block *pBlock = getReady(dwTimeout);
	if (!pBlock) return 0;

	// Copy the data
	DWORD dwAmount = min(pBlock->Amount, buffSize);
	memcpy(pBuff, pBlock->Data, dwAmount);

	// Return the block
	retBlock(pBlock);

	// Success
	return dwAmount;
};


CTinyPipe::Block* CTinyPipe::getAvail(DWORD dwTimeout) {
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

void CTinyPipe::postBlock(Block *pBlock) {
	EnterCriticalSection(&m_csPipeLink);
	// Signal availability of more data but only if threads are waiting
	if (m_pBuf->m_Filled==m_pBuf->m_Available && !(m_pBuf->doneWrite&DONE_WRITE)) {
		m_pBuf->doneWrite |= DONE_WRITE;
		SetEvent(m_hSignal);
		// printf("client set event, Available:%d m_Filled:%d doneWrite:%x\n", m_pBuf->m_Available, m_pBuf->m_Filled, m_pBuf->doneWrite);
	}

	// Make sure the operation is atomic
	m_pBuf->m_Available = pBlock->Next;

	if (m_pBuf->m_Filled == m_pBuf->m_Available) m_pBuf->doneWrite |= DONE_FILLED;
	LeaveCriticalSection(&m_csPipeLink);
};

DWORD CTinyPipe::write(void *pBuff, DWORD amount, DWORD dwTimeout) {
	// Grab a block
	Block *pBlock = getAvail(dwTimeout);
	if (!pBlock) return 0;

	// Copy the data
	DWORD dwAmount = min(amount, PIPE_BLOCK_SIZE);
	memcpy(pBlock->Data, pBuff, dwAmount);
	pBlock->Amount = dwAmount;

	// Post the block
	postBlock(pBlock);

	// Success
	return dwAmount;
};

bool CTinyPipe::waitAvailable(DWORD dwTimeout) {
	// Wait on the available event
	if (WaitForSingleObject(m_hAvail, dwTimeout) != WAIT_OBJECT_0)
		return false;

	// Success
	return true;
};