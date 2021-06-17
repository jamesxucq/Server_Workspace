#pragma once

// Definitions
#define PIPE_BLOCK_COUNT		128
#define PIPE_BLOCK_SIZE			2048

// ---------------------------------------
// -- Inter-Process Communication class --
// ---------------------------------------------------------------
// Provides intercommunication between processes and there threads
// ---------------------------------------------------------------

class CTinyPipe
{
	// Block that represents a piece of data to transmit between the
	// client and server
	struct Block {
		// Variables
		LONG				Next;						// Next block in the single linked list
		DWORD				Amount;						// Amount of data help in this block
		BYTE				Data[PIPE_BLOCK_SIZE];		// Data contained in this block
	};

	// Shared memory buffer that contains everything required to transmit
	// data between the client and server
	struct MemBuff {
		volatile LONG		m_Available;				// List of available blocks
		volatile LONG		m_Filled;					// List of blocks that have been filled with data
		volatile LONG		doneWrite;					// Flag used to signal that this block has been written
		Block				m_Blocks[PIPE_BLOCK_COUNT];	// Array of buffers that are used in the communication
	};

public:
	CTinyPipe(void);
	~CTinyPipe(void);
private:
	// Internal variables
	HANDLE					m_hSignal;		// Event used to signal when data exists
	HANDLE					m_hAvail;		// Event used to signal when some blocks become available
	CRITICAL_SECTION		m_csPipeLink;
	MemBuff					*m_pBuf;		// Buffer that points to the shared memory
public:
	// Exposed functions
	DWORD					read(void *pBuff, DWORD buffSize, DWORD timeout = INFINITE);
	// Block functions
	Block*					getReady(DWORD dwTimeout = INFINITE);
	void					retBlock(Block* pBlock);
public:
	// Exposed functions
	DWORD					write(void *pBuff, DWORD amount, DWORD dwTimeout = INFINITE);	// Writes to the buffer
	bool					waitAvailable(DWORD dwTimeout = INFINITE);						// Waits until some blocks become available

	Block*					getAvail(DWORD dwTimeout = INFINITE);							// Gets a block
	void					postBlock(Block *pBlock);										// Posts a block to be processed
	// Functions
	BOOL					IsOk(void) { return m_pBuf? true: false; }
};
