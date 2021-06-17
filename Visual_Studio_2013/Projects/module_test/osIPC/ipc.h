#pragma once

#ifndef IPC_H
#define IPC_H

// Definitions
#define IPC_BLOCK_COUNT			128
// #define IPC_BLOCK_SIZE		4096
#define IPC_BLOCK_SIZE			1024

// #define IPC_MAX_ADDR			256
#define IPC_MAX_ADDR			32

// ---------------------------------------
// -- Inter-Process Communication class --
// ---------------------------------------------------------------
// Provides intercommunication between processes and there threads
// ---------------------------------------------------------------
class osIPC {
	// Block that represents a piece of data to transmit between the
	// client and server
	struct Block {
		// Variables
		DWORD				Amount;						// Amount of data help in this block
		BYTE				Data[IPC_BLOCK_SIZE];		// Data contained in this block
		volatile LONG		Next;						// Next block in the single linked list
	};

	// Shared memory buffer that contains everything required to transmit
	// data between the client and server
	struct MemBuff {
		volatile LONG		m_Available;				// List of available blocks
		volatile LONG		m_Filled;					// List of blocks that have been filled with data
		volatile LONG		doneWrite;					// Flag used to signal that this block has been written
		Block				m_Blocks[IPC_BLOCK_COUNT];	// Array of buffers that are used in the communication
	};

public:
	// Server class
	class Server {
	public:
		// Construct / Destruct
		Server();
		Server(wchar_t *connectAddr);
		~Server();
	private:
		// Internal variables
		HANDLE					m_hMapFile;		// Handle to the mapped memory file
		HANDLE					m_hSignal;		// Event used to signal when data exists
		HANDLE					m_hAvail;		// Event used to signal when some blocks become available
		HANDLE					m_hAtomMod;
		MemBuff					*m_pBuf;		// Buffer that points to the shared memory

	public:
		// Exposed functions
		DWORD					read(void *pBuff, DWORD buffSize, DWORD timeout = INFINITE);
		// Block functions
		Block*					getBlock(DWORD dwTimeout = INFINITE);
		void					retBlock(Block* pBlock);
		// Create and destroy functions
		bool					create(wchar_t *connectAddr);
		void					close(void);
	};

	// Client class
	class Client {
	public:
		// Construct / Destruct
		Client(void);
		Client(wchar_t *connectAddr);
		~Client();
	private:
		// Internal variables
		HANDLE					m_hMapFile;		// Handle to the mapped memory file
		HANDLE					m_hSignal;		// Event used to signal when data exists
		HANDLE					m_hAvail;		// Event used to signal when some blocks become available
		HANDLE					m_hAtomMod;
		MemBuff					*m_pBuf;		// Buffer that points to the shared memory
	public:
		// Exposed functions
		DWORD					write(void *pBuff, DWORD amount, DWORD dwTimeout = INFINITE);	// Writes to the buffer
		bool					waitAvailable(DWORD dwTimeout = INFINITE);						// Waits until some blocks become available

		Block*					getBlock(DWORD dwTimeout = INFINITE);							// Gets a block
		void					postBlock(Block *pBlock);										// Posts a block to be processed				
		bool					create(wchar_t *connectAddr);
		void					close(void);
		// Functions
		BOOL					IsOk(void) { return m_pBuf? true: false; };
	};
};

#endif