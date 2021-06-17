#pragma once

namespace NTRANSCache {
	///////////////////////////////////////////////////////////////////////////////////////////
	TCHAR *CreatBuildName(TCHAR *szBuildName, const TCHAR *szFileName, off_t offset);
	HANDLE OpenBuild(const TCHAR *szBuildName);
	void CloseBuild(HANDLE hBuildCache);
	void DestroyBuild(HANDLE hBuildCache, const TCHAR *szBuildName);
};
