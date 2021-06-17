#pragma once

#include <vector>
using std::vector;

#define CHECK_NORMAL_POWEROFF		_T("~\\NormalPoweroff")

//
namespace SystemCall {
	DWORD CheckNormalPoweroff(TCHAR *szFlagFilePath = CHECK_NORMAL_POWEROFF);
	//
	DWORD CheckApplicaAuto(TCHAR *szValueName);
	DWORD AddiApplicaAuto(TCHAR *szValueName, TCHAR *szApplicaPath);
	DWORD AddiApplicaAutoEx(TCHAR *szValueName);
	DWORD DeliApplicaAuto(TCHAR *szValueName);
	//
	DWORD GetDeviceNumber(TCHAR *szParamName);
	DWORD LaunchProcess(TCHAR *szProgramPath); // just for test
	DWORD OpenLink(TCHAR *szURL);
	//
	DWORD ExecuteApplica(TCHAR *szApplicaPath);
	DWORD EndApplica(TCHAR *szProcessName);
	DWORD ExecuteApplicaEx(TCHAR *szApplicaPath, TCHAR *szProcessName, bool fFailIfExist);
	//
#define PROCESS_INVALID			0x00000000
#define PROCESS_INTEL_I386		0x00000001
#define PROCESS_INTEL_IA64		0x00000002
#define PROCESS_AMD64			0x00000004
	DWORD ProcessorArchitecture();
};

//
namespace WindVers {
	//
#define __WIN_NONE__		0x00000000
#define __WIN_95__			0x00010400
#define __WIN_95_SP1__		0x00010400
#define __WIN_95_OSR2__		0x00010409
#define __WIN_98__			0x00010410
#define __WIN_98_SP1__		0x00010410
#define __WIN_98_SE__		0x00010410
#define __WIN_ME__			0x00010490

#define __WIN_NT3_51__		0x00020351
#define __WIN_NT4__			0x00020400
#define __WIN_2K__			0x00020500
#define __WIN_XP__			0x00020501
#define __WIN_XP_64__		0x00020502
#define __WIN_2003__		0x00020502
#define __WIN_Home_Server__	0x00020502
#define __WIN_2003_R2__		0x00020502
#define __WIN_VISTA__		0x00020600
#define __WIN_2008__		0x00020600
#define __WIN_2008_R2__		0x00020601
#define __WIN_7__			0x00020601
#define __WIN_2012__		0x00020602
#define __WIN_8__			0x00020602
#define __WIN_2012_R2__		0x00020603
#define __WIN_8_1__			0x00020603
#define __WIN_2016__		0x0002A000
#define __WIN_10__			0x0002A000
	DWORD GetWindowsVersion();
	//
#define _WIN_NONE_			0x00000000
#define _WIN_NT4_			0x00040000
#define _WIN_2K_			0x00050000
#define _WIN_XP_			0x00050001
#define _WIN_XP_64_			0x00050002
#define _WIN_VISTA_			0x00060000
#define _WIN_7_				0x00060001
#define _WIN_8_				0x00060002
#define _WIN_8_1_			0x00060003
#define _WIN_10_			0x000A0000
	DWORD GetWinVer();
	DWORD GetWinVerEx();
	//
#define _SHELL_NONE_		0x00000000
#define _SHELL_NT4_			0x00040000
#define _SHELL_2K_			0x00050000
#define _SHELL_XP_			0x00060000
#define _SHELL_VISTA_		0x00060000
#define _SHELL_7_			0x00060001
#define _SHELL_8_			0x00060003
#define _SHELL_10_			0x000A0000
	DWORD GetShellVersion(LPCTSTR lpszDllName);
	DWORD GetShellVersionEx();
};

//
/*
This table has been assembled from Usenet postings, personal
observations, and reading other people's code.  Please feel
free to add to it or correct it.

Version		   dwPlatFormID   dwMajorVersion  dwMinorVersion    dwBuildNumber		wProductType
95             1              4               0					950
95 SP1         1              4               0					>950 && <=1080
95 OSR2        1              4             <10					>1080
98             1              4              10					1998
98 SP1         1              4              10					>1998 && <2183
98 SE          1              4              10					>=2183
ME             1              4              90					3000

NT 3.51        2              3              51					1057
NT 4           2              4               0					1381
2000           2              5               0					2195
XP             2              5               1					2600
XP 64-Bit	   2			  5				  2										(OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
2003           2              5               2					3790				GetSystemMetrics(SM_SERVERR2) == 0
Home Server	   2			  5				  2										OSVERSIONINFOEX.wSuiteMask & VER_SUITE_WH_SERVER
2003 R2        2              5               2										GetSystemMetrics(SM_SERVERR2) != 0
Vista          2              6               0										VER_NT_WORKSTATION
2008           2              6               0										VER_NT_SERVER
2008 R2        2              6               1										VER_NT_SERVER
Win7           2              6               1										VER_NT_WORKSTATION
2012           2              6               2										VER_NT_SERVER
Win8           2              6               2										VER_NT_WORKSTATION
2012 R2        2              6               3*									VER_NT_SERVER
Win8.1         2              6               3*									VER_NT_WORKSTATION
2016           2              10              0*									VER_NT_SERVER
Win10          2              10              0*									VER_NT_WORKSTATION

CE 1.0         3              1               0
CE 2.0         3              2               0
CE 2.1         3              2               1
CE 3.0         3              3               0
*/

/*
Shell32.dll	Version	Distribution Platform
4.0		Windows 95 and Microsoft Windows NT 4.0
4.71	Microsoft Internet Explorer 4.0. See note 1.
4.72	Internet Explorer 4.01 and Windows 98. See note 1.
5.0		Windows 2000 and Windows Millennium Edition (Windows Me). See note 2.
6.0		Windows XP
6.0.1	Windows Vista
6.1		Windows 7
*/

//