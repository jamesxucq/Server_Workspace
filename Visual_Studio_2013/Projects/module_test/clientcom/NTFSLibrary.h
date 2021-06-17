#pragma once

#include "libresiz\libresiz.h"
#pragma comment(lib, "..\\third party\\libresiz\\libresiz.lib")

namespace NTFSLibrary
{
	DWORD NTFSResize(TCHAR *szFileName, DWORD dwNewDisksize);
	DWORD ValidateReduce(TCHAR *szFileName, int iReducedDisksize);
};
