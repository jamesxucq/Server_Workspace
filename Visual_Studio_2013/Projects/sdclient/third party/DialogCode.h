#pragma once

namespace DialogCode {
	VOID WaitCursor ( VOID );
	VOID NormalCursor ( VOID );
	VOID ArrowWaitCursor ( VOID );
	VOID HandCursor ();

	//
	CString BrowseDirectories(HWND hParentHWnd, const CString &csDlgTitle);
	BOOL BrowseDirectories (HWND hWnd, TCHAR *lpszTitle, TCHAR *szDireName);
	// BOOL BrowseFiles (HWND hwndDlg, TCHAR *stringId, TCHAR *lpszFileName, BOOL keepHistory, BOOL saveMode, TCHAR *browseFilter);
	BOOL OpenDirectorise(HWND hParentHWnd, const  TCHAR *szDireName);
};
