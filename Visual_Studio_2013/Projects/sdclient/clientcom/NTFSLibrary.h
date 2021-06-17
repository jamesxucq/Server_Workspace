#pragma once

#include "libresiz\libresiz.h"
#pragma comment(lib, "..\\third party\\libresiz\\libresiz.lib")

namespace NTFSLibrary
{
	DWORD NTFSResize(TCHAR *szFileName, DWORD dwNewDisksize);
	DWORD ValidateReduce(DWORD *dwLastDisksize, TCHAR *szFileName, DWORD dwReducedDisksize);
};
