#pragma once

#define CHECK_NORMAL_POWEROFF		_T("~\\NormalPoweroff")

namespace SystemCall {
	DWORD CheckNormalPoweroff(TCHAR *szFlagFilePath = CHECK_NORMAL_POWEROFF);
	////////////////////////////////////////////////////////////
	DWORD CheckApplicationAutorun(TCHAR *szValueName);
	DWORD AddApplicationAutorun(TCHAR *szValueName, TCHAR *szApplicaPath);
	DWORD AddApplicationAutorunExt(TCHAR *szValueName);
	DWORD DelApplicationAutorun(TCHAR *szValueName);
	////////////////////////////////////////////////////////////
	DWORD LaunchProcess(TCHAR *szProgramPath); //just for test
	DWORD OpenLink(TCHAR *szURL);
	////////////////////////////////////////////////////////////
	DWORD ExecuteApplication(TCHAR *szApplicaPath);
	DWORD EndApplication(TCHAR *szProcessName);
	DWORD ExecuteApplicationExt(TCHAR *szApplicaPath, TCHAR *szProcessName, bool fFailIfExists);
	////////////////////////////////////////////////////////////
	VOID RefreshExplorer();
	DWORD RebuildIcons();
	VOID RefreshExplorerIcons(LPCWSTR szIconDirectory);
	////////////////////////////////////////////////////////////

#define _WIN_NONE_		0x0000
#define _WIN_95_		0x0400
#define _WIN_95_OSR2_	0x0409
#define _WIN_98_		0x0410
#define _WIN_98_SE_		0x0410
#define _WIN_ME_		0x0490
#define _WIN_2K_		0x0500
#define _WIN_XP_		0x0501
#define _WIN_2003_		0x0502
#define _WIN_VISTA_		0x0600
#define _WIN_SEVEN_		0x0601
	DWORD GetWindowsVersion();
};

///////////////////////////////////////////////////////////////////////////////
/*
This table has been assembled from Usenet postings, personal
observations, and reading other people's code.  Please feel
free to add to it or correct it.


dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
95             1              4               0             950
95 SP1         1              4               0        >950 && <=1080
95 OSR2        1              4             <10           >1080
98             1              4              10            1998
98 SP1         1              4              10       >1998 && <2183
98 SE          1              4              10          >=2183
ME             1              4              90            3000

NT 3.51        2              3              51            1057
NT 4           2              4               0            1381
2000           2              5               0            2195
XP             2              5               1            2600
2003           2              5               2            3790
Vista          2              6               0

CE 1.0         3              1               0
CE 2.0         3              2               0
CE 2.1         3              2               1
CE 3.0         3              3               0
*/

///////////////////////////////////////////////////////////////////////////////