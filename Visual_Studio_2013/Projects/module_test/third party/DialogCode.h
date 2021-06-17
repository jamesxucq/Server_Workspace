#pragma once

namespace DialogCode {
	VOID WaitCursor ( VOID );
	VOID NormalCursor ( VOID );
	VOID ArrowWaitCursor ( VOID );
	VOID HandCursor ();

	/////////////////////////////////////////////////////////////////////
	CString BrowseDirectories(HWND ParentHWnd, const CString &strDlgTitle);
	BOOL BrowseDirectories (HWND hWnd, TCHAR *lpszTitle, TCHAR *dirName);
	//BOOL BrowseFiles (HWND hwndDlg, TCHAR *stringId, TCHAR *lpszFileName, BOOL keepHistory, BOOL saveMode, TCHAR *browseFilter);
	BOOL OpenDirectorise(HWND ParentHWnd, const  TCHAR *dirName);
};
