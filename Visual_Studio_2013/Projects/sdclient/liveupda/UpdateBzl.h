#pragma once

namespace NUpdateBzl {
	static DWORD SingleInstance(LPCTSTR szAppName);
// system
#define EXECUTE_FAULT						-1
#define EXECUTE_SUCCESS						0x00000000
// exception
#define EXECUTE_ONE_INSTANCE				0x00000001
#define EXECUTE_CONFIGURATION				0x00000002
#define EXECUTE_LIVE_UPDATE					0x00000040
#define EXECUTE_ACTIVE_UPDATE				0x00000080
	DWORD ApplicaExecuteType();
	//
	static DWORD ExecuteApplica(TCHAR *szLocation, TCHAR *szExecuteName);
	DWORD ExecuteActiveUpdate();
};
