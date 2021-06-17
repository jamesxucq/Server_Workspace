#include "StdAfx.h"

#include "iattb_type.h"
#include "third_party.h"

#include "ShellUtility.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default

//
DWORD FileStatusTab[] = { FILE_STATUS_INVALID, FILE_STATUS_ADDED, 
						FILE_STATUS_DELETE, FILE_STATUS_INVALID, FILE_STATUS_MODIFIED};

//
VOID ShellUtility::RefreshExplorer() {
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST|SHCNF_FLUSHNOWAIT, NULL, NULL);
}

//
//
static WNDENUMPROC lpExploWindoEnum;
static WNDENUMPROC lpExploCacheEnum;

//
#define	COMPU_EXPLO_0502	_T("我的电脑")
#define COMPU_EXPLO_0600	_T("计算机")
#define COMPU_EXPLO_0602	_T("这台电脑")
#define COMPU_EXPLO_1000	_T("此电脑")

static BOOL CALLBACK ExploWindowsChildEnumCB(HWND hwnd, LPARAM driveStr) {
    TCHAR s[MAX_PATH];
	//
	GetClassName (hwnd, s, MAX_PATH);
    if (NULL == _tcscmp(s, _T("ComboBoxEx32"))) {
		SendMessage (hwnd, WM_GETTEXT, MAX_PATH, (LPARAM) s);
		if (NULL == _tcsncmp(s, (TCHAR *) driveStr, 0x03)) {
			SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
			return FALSE;
		}
	}
    //
    return TRUE;
}

static BOOL CALLBACK ExploWindowsChildEnumTW(HWND hwnd, LPARAM driveStr) {
    TCHAR s[MAX_PATH];
	//
	GetClassName (hwnd, s, MAX_PATH);
    if (NULL == _tcscmp(s, _T("ToolbarWindow32"))) {
		SendMessage (hwnd, WM_GETTEXT, MAX_PATH, (LPARAM) s);
		if (NULL == _tcsncmp(s+0x04, (TCHAR *) driveStr, 0x03)) {
			SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)(s+0x04), NULL);
			return FALSE;
		}
	}
    //
    return TRUE;
}

static BOOL CALLBACK ExploWindowsEnumXP(HWND hwnd, LPARAM driveNo) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	driveStr[0] = (TCHAR)driveNo;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_0502)) {
			SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
			return TRUE;
		}
        EnumChildWindows(hwnd, ExploWindowsChildEnumCB, (LPARAM) driveStr);
	}
	//
	return TRUE;
}

static BOOL CALLBACK ExploWindowsEnumVS(HWND hwnd, LPARAM driveNo) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	driveStr[0] = (TCHAR)driveNo;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_0600)) {
			SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
			return TRUE;
		}
        EnumChildWindows(hwnd, ExploWindowsChildEnumCB, (LPARAM) driveStr);
	}
	//
	return TRUE;
}

static BOOL CALLBACK ExploWindowsEnum7(HWND hwnd, LPARAM driveNo) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	driveStr[0] = (TCHAR)driveNo;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_0600)) {
			SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
			return TRUE;
		}
        EnumChildWindows(hwnd, ExploWindowsChildEnumTW, (LPARAM) driveStr);
	}
	//
	return TRUE;
}

static BOOL CALLBACK ExploWindowsEnum8(HWND hwnd, LPARAM driveNo) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	driveStr[0] = (TCHAR)driveNo;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_0602)) {
			SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
			return TRUE;
		}
        EnumChildWindows(hwnd, ExploWindowsChildEnumTW, (LPARAM) driveStr);
	}
	//
	return TRUE;
}

static BOOL CALLBACK ExploWindowsEnum10(HWND hwnd, LPARAM driveNo) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	driveStr[0] = (TCHAR)driveNo;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_1000)) {
			SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
			return TRUE;
		}
        EnumChildWindows(hwnd, ExploWindowsChildEnumTW, (LPARAM) driveStr);
	}
	//
	return TRUE;
}

//
BOOL ShellUtility::RefreshExplorerWindows(const TCHAR *szDriveLetter) {
	if ('A' <= szDriveLetter[0]) {
		EnumWindows(lpExploWindoEnum, (LPARAM) szDriveLetter[0]);
	}
	//
	return TRUE;
}

//
struct EnumCache {
	char driveDetter;
	unordered_map<wstring, DWORD> *mOpenedCache;
};

static BOOL CALLBACK WindowsCacheChildEnumCB(HWND hwnd, LPARAM enumParam) {
	TCHAR driveStr[10] = _T(" :\\");
    TCHAR s[MAX_PATH];
	struct EnumCache *enumCache = (struct EnumCache *)enumParam;
	driveStr[0] = enumCache->driveDetter;
	//
	GetClassName (hwnd, s, MAX_PATH);
    if (NULL == _tcscmp(s, _T("ComboBoxEx32"))) {
		SendMessage (hwnd, WM_GETTEXT, MAX_PATH, (LPARAM) s);
		if (NULL == _tcsncmp(s, (TCHAR *) driveStr, 0x03)) {
			(*enumCache->mOpenedCache)[s] = 0xFFFFFFFF;
			return FALSE;
		}
	}
    //
    return TRUE;
}

static BOOL CALLBACK WindowsCacheChildEnumTW(HWND hwnd, LPARAM enumParam) {
	TCHAR driveStr[10] = _T(" :\\");
    TCHAR s[MAX_PATH];
	struct EnumCache *enumCache = (struct EnumCache *)enumParam;
	driveStr[0] = enumCache->driveDetter;
	//
	GetClassName (hwnd, s, MAX_PATH);
    if (NULL == _tcscmp(s, _T("ToolbarWindow32"))) {
		SendMessage (hwnd, WM_GETTEXT, MAX_PATH, (LPARAM) s);
		if (NULL == _tcsncmp(s+0x04, (TCHAR *) driveStr, 0x03)) {
			(*enumCache->mOpenedCache)[(s+4)] = 0xFFFFFFFF;
			return FALSE;
		}
	}
    //
    return TRUE;
}

static BOOL CALLBACK WindowsCacheEnumXP(HWND hwnd, LPARAM enumParam) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	struct EnumCache *enumCache = (struct EnumCache *)enumParam;
	driveStr[0] = enumCache->driveDetter;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_0502)) {
// _LOG_DEBUG(_T("callback found:%s"), s);
			(*enumCache->mOpenedCache)[s] = 0xFFFFFFFF;
			return TRUE;
		}
		EnumChildWindows(hwnd, WindowsCacheChildEnumCB, (LPARAM) enumParam);
	}
	//
	return TRUE;
}

static BOOL CALLBACK WindowsCacheEnumVS(HWND hwnd, LPARAM enumParam) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	struct EnumCache *enumCache = (struct EnumCache *)enumParam;
	driveStr[0] = enumCache->driveDetter;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_0600)) {
			// SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
// _LOG_DEBUG(_T("callback found:%s"), s);
			(*enumCache->mOpenedCache)[s] = 0xFFFFFFFF;
			return TRUE;
		}
		EnumChildWindows(hwnd, WindowsCacheChildEnumCB, (LPARAM) enumParam);
	}
	//
	return TRUE;
}

static BOOL CALLBACK WindowsCacheEnum7(HWND hwnd, LPARAM enumParam) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	struct EnumCache *enumCache = (struct EnumCache *)enumParam;
	driveStr[0] = enumCache->driveDetter;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_0600)) {
			// SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
// _LOG_DEBUG(_T("callback found:%s"), s);
			(*enumCache->mOpenedCache)[s] = 0xFFFFFFFF;
			return TRUE;
		}
		EnumChildWindows(hwnd, WindowsCacheChildEnumTW, (LPARAM) enumParam);
	}
	//
	return TRUE;
}

static BOOL CALLBACK WindowsCacheEnum8(HWND hwnd, LPARAM enumParam) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	struct EnumCache *enumCache = (struct EnumCache *)enumParam;
	driveStr[0] = enumCache->driveDetter;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_0602)) {
			// SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
// _LOG_DEBUG(_T("callback found:%s"), s);
			(*enumCache->mOpenedCache)[s] = 0xFFFFFFFF;
			return TRUE;
		}
		EnumChildWindows(hwnd, WindowsCacheChildEnumTW, (LPARAM) enumParam);
	}
	//
	return TRUE;
}

static BOOL CALLBACK WindowsCacheEnum10(HWND hwnd, LPARAM enumParam) {
	TCHAR driveStr[10] = _T(" :\\");
	TCHAR s[MAX_PATH];
	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	struct EnumCache *enumCache = (struct EnumCache *)enumParam;
	driveStr[0] = enumCache->driveDetter;
	//
	GetClassName (hwnd, s, sizeof s);
	if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
		GetWindowText (hwnd, s, sizeof s);
		if (NULL == _tcsncmp(s, driveStr, 0x03) || NULL == _tcscmp(s, COMPU_EXPLO_1000)) {
			// SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)s, NULL);
// _LOG_DEBUG(_T("callback found:%s"), s);
			(*enumCache->mOpenedCache)[s] = 0xFFFFFFFF;
			return TRUE;
		}
		EnumChildWindows(hwnd, WindowsCacheChildEnumTW, (LPARAM) enumParam);
	}
	//
	return TRUE;
}

static BOOL RebuildWindowsCache(unordered_map<wstring, DWORD> *mExploCache, const TCHAR *szDriveLetter) {
	struct EnumCache enumCache;
	//
	mExploCache->clear();
	enumCache.driveDetter = szDriveLetter[0];
	enumCache.mOpenedCache = mExploCache;
	//
// _LOG_DEBUG(_T("rebuild windows cache, drive letter:%s"), szDriveLetter);
	if ('A' <= enumCache.driveDetter) {
		EnumWindows (lpExploCacheEnum, (LPARAM)&enumCache);
	}
	//
	return TRUE;
}

static void CompareRefreshWindows(unordered_map<wstring, DWORD> *mExploCache, const TCHAR *szIconDirectory) {
	unordered_map<wstring, DWORD>::iterator dite;
	dite = mExploCache->find(szIconDirectory);
	if(mExploCache->end() != dite) {
// _LOG_DEBUG(_T("compare refresh windows, found:%s"), szIconDirectory);
		SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)szIconDirectory, NULL);
	}
}

#define EXPLO_WINDOWS_INTERV		32000 // 32s
VOID ShellUtility::RefreshExplorerIcons(const TCHAR *szIconDirectory) {
	static DWORD ulTimestamp;
	static unordered_map<wstring, DWORD> mExploCache;
// _LOG_DEBUG(_T("refresh explorer, directory:%s"), szIconDirectory);
	if(EXPLO_WINDOWS_INTERV < (GetTickCount() - ulTimestamp)) {
// _LOG_DEBUG(_T("refresh GetTickCount:%08x ulTimestamp:%08x"), GetTickCount(), ulTimestamp);
		RebuildWindowsCache(&mExploCache, szIconDirectory);
		ulTimestamp = GetTickCount();
	}
	CompareRefreshWindows(&mExploCache, szIconDirectory);
}

VOID ShellUtility::InitiExploWindo() {
	DWORD dwWindowsVersion = WindVers::GetWinVerEx();
	if(_WIN_10_ == dwWindowsVersion) {
		lpExploWindoEnum = ExploWindowsEnum10;
		lpExploCacheEnum = WindowsCacheEnum10;
	} else if(_WIN_7_ == dwWindowsVersion) {
		lpExploWindoEnum = ExploWindowsEnum7;
		lpExploCacheEnum = WindowsCacheEnum7;
	} else if(_WIN_8_==dwWindowsVersion || _WIN_8_1_==dwWindowsVersion) {
		lpExploWindoEnum = ExploWindowsEnum8;
		lpExploCacheEnum = WindowsCacheEnum8;
	} else if(_WIN_VISTA_ == dwWindowsVersion) {
		lpExploWindoEnum = ExploWindowsEnumVS;
		lpExploCacheEnum = WindowsCacheEnumVS;
	} else if(_WIN_XP_64_ >= dwWindowsVersion) {
		lpExploWindoEnum = ExploWindowsEnumXP;
		lpExploCacheEnum = WindowsCacheEnumXP;
	}
}

//
DWORD ShellUtility::RebuildIcons() {
	const int BUFFER_SIZE = 1024;
	TCHAR *buf = NULL;
	HKEY hRegKey = 0;
	DWORD dwRegValue;
	DWORD dwRegValueTemp;
	DWORD dwSize;
	DWORD_PTR dwResult;
	LONG lRegResult;
	std::wstring sRegValueName;
	std::wstring sDefaultIconSize;
	int DefaultIconSize;
	DWORD retValue = ((DWORD)-1);
	//
	lRegResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Control Panel\\Desktop\\WindowMetrics"),
		0, KEY_READ | KEY_WRITE, &hRegKey);
	if (lRegResult != ERROR_SUCCESS)
		goto Cleanup;
	//
	buf = new TCHAR[BUFFER_SIZE];
	if(NULL == buf)
		goto Cleanup;
	//
	// we're going to change the Shell Icon Size value
	sRegValueName = _T("Shell Icon Size");
	//
	// Read registry value
	dwSize = BUFFER_SIZE;
	lRegResult = RegQueryValueEx(hRegKey, sRegValueName.c_str(), NULL, NULL, 
		(LPBYTE) buf, &dwSize);
	if (lRegResult != ERROR_FILE_NOT_FOUND) {
		// If registry key doesn't exist create it using system current setting
		DefaultIconSize = ::GetSystemMetrics(SM_CXICON);
		if (0 == DefaultIconSize)
			DefaultIconSize = 32;
		_sntprintf_s(buf, BUFFER_SIZE, BUFFER_SIZE, _T("%d"), DefaultIconSize); 
	}
	else if (lRegResult != ERROR_SUCCESS)
		goto Cleanup;
	//
	// Change registry value
	dwRegValue = _ttoi(buf);
	dwRegValueTemp = dwRegValue-1;
	//
	dwSize = _sntprintf_s(buf, BUFFER_SIZE, BUFFER_SIZE, _T("%d"), dwRegValueTemp) + sizeof(TCHAR); 
	lRegResult = RegSetValueEx(hRegKey, sRegValueName.c_str(), 0, REG_SZ, 
		(LPBYTE) buf, dwSize); 
	if (lRegResult != ERROR_SUCCESS)
		goto Cleanup;
	//
	// Update all windows
	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, 
		0, SMTO_ABORTIFHUNG, 5000, &dwResult);
	//
	// Reset registry value
	dwSize = _sntprintf_s(buf, BUFFER_SIZE, BUFFER_SIZE, _T("%d"), dwRegValue) + sizeof(TCHAR); 
	lRegResult = RegSetValueEx(hRegKey, sRegValueName.c_str(), 0, REG_SZ, 
		(LPBYTE) buf, dwSize); 
	if(lRegResult != ERROR_SUCCESS)
		goto Cleanup;
	//
	// Update all windows
	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, 
		0, SMTO_ABORTIFHUNG, 5000, &dwResult);
	//
	retValue = 0x00;
	//
Cleanup:
	if (hRegKey != 0) {
		RegCloseKey(hRegKey);
	}
	if (NULL != buf) {
		delete [] buf;
	}
	//
	return retValue;
}
