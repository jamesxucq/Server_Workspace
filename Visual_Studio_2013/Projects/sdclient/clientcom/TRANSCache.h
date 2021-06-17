#pragma once

namespace NTRANSCache {
	//
	TCHAR *CreatBuilderName(TCHAR *szBuilderName, const TCHAR *szFileName, size_t offset);
	HANDLE OpenBuilder(const TCHAR *szBuilderName);
	VOID CloseBuilder(HANDLE hBuilderCache);
	VOID DestroyBuilder(HANDLE hBuilderCache, const TCHAR *szBuilderName);
};
