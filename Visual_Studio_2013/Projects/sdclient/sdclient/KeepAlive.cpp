#include "StdAfx.h"
#include "KeepAlive.h"

CKeepAlive::CKeepAlive(void) {
	
}

CKeepAlive::~CKeepAlive(void) {
}

CKeepAlive objKeepAlive;

//
DWORD CKeepAlive::Continue(DWORD dwThreadInde) {
	if(KEEP_ALIVE_INTE < (GetTickCount() - ulTimeStamp[dwThreadInde])) {
		ulTimeStamp[dwThreadInde] = GetTickCount(); 
		return 0x01;
	}
	return 0x00;
}