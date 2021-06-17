#pragma once

#include "clientcom/clientcom.h"

//
// #define	VERIFY_VALUE_FAULT	SYNP_VALUE_FAULT		// -1
// #define	VERIFY_VALUE_RUNNING	SYNP_VALUE_RUNNING		// 0x00000000
// #define	VERIFY_VALUE_WAITING	SYNP_VALUE_WAITING		// 0x00000001
#define	VERIFY_VALUE_PAUSED	SYNP_VALUE_PAUSED		// 0x00000002
#define	VERIFY_VALUE_FORCESTOP	SYNP_VALUE_FORCESTOP	// 0x00000004
#define	VERIFY_VALUE_FINISH	SYNP_VALUE_FINISH		// 0x00000008
#define	VERIFY_VALUE_FAILED	SYNP_VALUE_FAILED		// 0x00000010
#define	VERIFY_VALUE_KILLED	SYNP_VALUE_KILLED		// 0x00000020
#define	VERIFY_VALUE_NETWORK	SYNP_VALUE_NETWORK		// 0x00000040
// #define VERIFY_VALUE_LOCKED	SYNP_VALUE_LOCKED		// 0x00000080
#define	VERIFY_VALUE_EXCEPTION	SYNP_VALUE_EXCEPTION	// 0x00010000
//
#define VERIFY_VALUE_NFOUND	SYNP_VALUE_UDEFA		// 0x01000000
#define VERIFY_VALUE_NCONTE	SYNP_VALUE_UDEFB		// 0x02000000

//
#define CONVERT_VERIFY_STATUS(RET_VALUE, VERIFY_STATUS) { \
	switch(VERIFY_STATUS) { \
		case PROCESS_STATUS_FORCESTOP: \
		RET_VALUE = VERIFY_VALUE_FORCESTOP; \
		break; \
		case PROCESS_STATUS_PAUSED: \
		RET_VALUE = VERIFY_VALUE_PAUSED; \
		break; \
		case PROCESS_STATUS_KILLED: \
		RET_VALUE = VERIFY_VALUE_KILLED; \
		break; \
	} \
}

#define FIVE_VERIFY_THREAD		5
namespace VerifyTasksBzl {
	DWORD BeginVerifyThread(struct LocalConfig *pLocalConfig);
	DWORD ValidTransmitFirst(struct LocalConfig *pLocalConfig);
	DWORD ValidTransmitSecond(struct LocalConfig *pLocalConfig);
};

//
namespace DowndVerify {
	int VerifyPrepa(DWORD *dwVerifyType, DWORD *sta_inde, DWORD *end_inde);
	static DWORD Sha1Verifier(HANDLE hFileValid, DWORD node_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
	static DWORD FileVerifier(HANDLE hFileValid, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
	DWORD ListVerifier(DWORD dwVerifyType, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
};

namespace UploadVerify {
	int VerifyPrepa(DWORD *dwVerifyType, DWORD *sta_inde, DWORD *end_inde);
	static DWORD Sha1Verifier(HANDLE hFileValid, DWORD node_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
	static DWORD FileVerifier(HANDLE hFileValid, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
	DWORD ListVerifier(DWORD dwVerifyType, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
};

namespace UploadSecond {
	int VerifyPrepa(DWORD *dwVerifyType, DWORD *sta_inde, DWORD *end_inde);
	static DWORD Sha1Verifier(HANDLE hFileValid, DWORD node_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
	static DWORD ChunkVerifier(HANDLE hFileValid, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
	DWORD ListVerifier(DWORD dwVerifyType, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath);
};

//
struct VERIFY_ARGUMENT {
	struct SessionArgu	tSeionArgu;
	struct SocketsArgu	tSockeArgu;
	struct ClientConfig	*pClientConfig;
};

//
#define TNODE_PREPA_FAULT		-1
#define FINISH_VERIFY			0
#define TNODE_PREPA_OKAY		1