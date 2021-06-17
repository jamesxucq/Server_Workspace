// osIPC_link.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "ipc.h"

// Static variables
TCHAR *server_addr = _T("osIPCx");
long stats_server_rcv_count = 0;
long stats_client_rcv_count = 0;

#define osipc_client

#ifdef osipc_client

// Client thread
int _tmain(int argc, _TCHAR* argv[])
{
	// Create the IPC client
	osIPC::Client client(server_addr);

	// Declare vars
	DWORD testData[3000];

	// Continuously write data
	DWORD N;
	for (N = 0; N < 32; N++)
	{
//
// testData[0] = N;
// printf("+ client:%d\n", testData[0]);
// if(!(N%9)) Sleep(1500);
//
		// Write some data
		if (client.write(testData, 3000) == 0)
			client.waitAvailable();
		else
			stats_client_rcv_count++;
	}

	// Success
	return 0;
};

#else
// Server thread
int _tmain(int argc, _TCHAR* argv[])
{
	// Create the IPC server
	osIPC::Server server(server_addr);

	// Get the address
	// server_addr = server.getAddress();

	// Start the thread
	// HANDLE hThread = CreateThread(NULL, 0, test_client, NULL, 0, NULL);
	// if (!hThread) return false;

	// Declare vars
	DWORD timerOutput = GetTickCount();
	unsigned __int64 N = 0;
	unsigned __int64 lN = 0;
	unsigned __int64 received = 0;
	DWORD buff[3000];

	// Enter a loop reading data
	for (; N < 1500000 ;N ++)
	{
		// Read the data
		DWORD readAmount = server.read(&buff, 3000);
//
// printf("+ server:%d\n", buff[0]);
//
		if (readAmount > 0)
		{
			received += readAmount;
			stats_server_rcv_count++;
		}

		// Check the timer
		DWORD curTime = GetTickCount();
		if (curTime - timerOutput > 1000) {
			timerOutput = curTime;

			// Print the data
printf("Server:%01d\tClient%01d\tRate: %01u\tAmount: %01u\n", stats_server_rcv_count, stats_client_rcv_count, (DWORD)(N - lN), (DWORD)received);
			lN = N;
			received = 0;
		}
	}

	// Success
	return 0;
};
#endif
