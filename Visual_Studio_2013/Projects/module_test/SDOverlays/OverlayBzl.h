#pragma once

#include "CacheObject.h"

namespace NOverlayBzl {
	/////////////////////////////////////////////////////
	DWORD Initialize();
	DWORD Finalize();
	/////////////////////////////////////////////////////
	inline TCHAR *GetWatchingDirectory() {
		return CACHE_OBJECT(m_wsWatchingDirectory);
	}
	/////////////////////////////////////////////////////
	static VOID GetAnswerToResponse(struct CacheResponse* pReply, const struct ShellRequest* pRequest);
	VOID HandleRequestBzl(DWORD *dwFileStatus, LPCWSTR pwszPath, DWORD dwAttrib);
	/////////////////////////////////////////////////////
	static bool HandleCacheErase(const TCHAR *key, unsigned int statcode);
	VOID HandleCommandBzl(struct CacheCommand *pCommand);
	/////////////////////////////////////////////////////
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#define FILE_STATUS_CODE(FILE_STATUS)				(FILE_STATUS) & 0x0000FFFF
#define FILE_CONTROL_CODE(FILE_STATUS)					(FILE_STATUS) & 0xFFFF0000