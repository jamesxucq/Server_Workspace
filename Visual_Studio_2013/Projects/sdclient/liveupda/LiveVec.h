#pragma once

#include "clientcom/lupdatecom.h"

#include <vector>
using std::vector;

struct LiveData {
	DWORD command;
	DWORD pid;
	TCHAR szNoA[MAX_PATH];
	TCHAR szNoB[MAX_PATH];
	char szReqURI[URI_LENGTH];
};

typedef vector <struct LiveData *> LiveVec;

namespace NLiveVec {
	void DeleLiveVec(LiveVec *pLiveVec);
	struct LiveData *AddNewLiveData(LiveVec *pLiveVec);
	DWORD MaxLivePid(LiveVec *pLiveVec);
	struct LiveData *GetLivePid(LiveVec *pLiveVec, DWORD pid);
};

// file or directory operation
#define LIVE_INVALID_CMD			0x00000000
	// must download data
#define LIVE_REPLACE_CMD			0x00000001 
	// file not download
#define LIVE_COPY_CMD				0x00000010
#define LIVE_DELETE_CMD				0x00000020
#define LIVE_EXECUTE_CMD			0x00000040
#define LIVE_UNILIB_CMD				0x00000080
#define LIVE_INSLIB_CMD				0x00000100
		// directory
#define LIVE_ADDDIR_CMD				0x00001000
#define LIVE_COPYDIR_CMD			0x00002000
#define LIVE_DELDIR_CMD				0x00004000
#define LIVE_ERADIR_CMD				0x00008000
// not directory
#define LIVE_CREKEY_CMD				0x00100000
#define LIVE_SETREG_CMD				0x00200000
#define LIVE_DELKEY_CMD				0x00400000
#define LIVE_DELREG_CMD				0x00800000
#define LIVE_ENDRIV_CMD				0x01000000
#define LIVE_DSDRIV_CMD				0x02000000
#define LIVE_KILL_CMD				0x04000000
#define LIVE_EDELKEY_CMD			0x08000000

#define MUST_DOWND_OPE(COMMAND)		0x0000000F & COMMAND
#define CONV_PATH_OPE(COMMAND)		0x000FFFFF & COMMAND

DWORD GetLiveCmd(char *szCommand);