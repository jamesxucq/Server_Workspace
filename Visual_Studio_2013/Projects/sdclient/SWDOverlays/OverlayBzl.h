#pragma once

#include "CacheObject.h"

namespace NOverlayBzl {
	DWORD Initialize();
	DWORD Finalize();
	//
	static VOID GetAnswerToResponse(struct CacheResponse* pReply, const struct ShellRequest* pRequest);
	DWORD HandleRequestBzl(LPCWSTR szPath, DWORD dwAttrib);
	//;
	VOID HandleCommandBzl(struct CacheCommand *pCommand);
};

//
#define FILE_STATUS_BYTE(FILE_STATUS)		(FILE_STATUS) & 0x0000FFFF
#define FILE_CONTROL_BYTE(FILE_STATUS)		(FILE_STATUS) & 0xFFFF0000
#define DIRE_BYTE(FILE_STATUS)				((FILE_STATUS) & 0x0000F000)
