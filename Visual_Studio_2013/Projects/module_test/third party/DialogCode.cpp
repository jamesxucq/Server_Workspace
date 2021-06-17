#include "StdAfx.h"
#include "DialogCode.h"


static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	switch(uMsg) {
	case BFFM_INITIALIZED: 
		{
			/* WParam is TRUE since we are passing a path.
			It would be FALSE if we were passing a pidl. */
			SendMessage (hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)pData);
			break;
		}

	case BFFM_SELCHANGED: 
		{
			TCHAR szDir[MAX_PATH];

			/* Set the status window to the currently selected path. */
			if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir)) 
			{
				SendMessage (hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
			}
			break;
		}

	default:
		break;
	}

	return 0;
}


BOOL DialogCode::BrowseDirectories (HWND hwndDlg, TCHAR *lpszTitle, TCHAR *dirName)
{
	BROWSEINFOW bi;
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	BOOL bOK  = FALSE;

	CoInitialize (NULL);

	if (SUCCEEDED (SHGetMalloc (&pMalloc))) 
	{
		ZeroMemory (&bi, sizeof(bi));
		bi.hwndOwner = hwndDlg;
		bi.pszDisplayName = 0;
		bi.lpszTitle = lpszTitle;
		bi.pidlRoot = 0;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = (LPARAM)dirName;

		pidl = SHBrowseForFolderW (&bi);
		if (pidl != NULL) 
		{
			if (SHGetPathFromIDList(pidl, dirName)) 
			{
				bOK = TRUE;
			}

			pMalloc->Free (pidl);
			pMalloc->Release();
		}
	}

	CoUninitialize();

	return bOK;
}

CString DialogCode::BrowseDirectories(HWND ParentHWnd, const CString &strDlgTitle)    
{  
	TCHAR   szDir[MAX_PATH]; 
	BROWSEINFO   brinfo;  
	ITEMIDLIST   *pidl;  
	LPMALLOC pMalloc;
	BOOL bOK  = FALSE;

	CoInitialize (NULL);

	if (SUCCEEDED (SHGetMalloc (&pMalloc))) 
	{
		brinfo.hwndOwner = ParentHWnd;  
		brinfo.pidlRoot = NULL;  
		brinfo.pszDisplayName = szDir;  
		brinfo.lpszTitle = strDlgTitle;  
		brinfo.ulFlags = BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT;
		brinfo.lpfn = NULL;  
		brinfo.lParam = NULL;  
		brinfo.iImage = NULL; 

		pidl = SHBrowseForFolder (&brinfo);
		if (pidl != NULL) 
		{
			if (SHGetPathFromIDList(pidl, szDir)) 
			{
				bOK = TRUE;
			}

			pMalloc->Free (pidl);
			pMalloc->Release();
		}
	}

	CoUninitialize();

	return   bOK? CString(szDir): _T("");    
}

VOID DialogCode::WaitCursor ()
{
	static HCURSOR hcWait;
	//if (hcWait == NULL) hcWait = LoadCursor (NULL, IDC_WAIT);
	SetCursor (hcWait);
	//hCursor = hcWait;
}

VOID DialogCode::NormalCursor ()
{
	static HCURSOR hcArrow;
	//if (hcArrow == NULL) hcArrow = LoadCursor (NULL, IDC_ARROW);
	SetCursor (hcArrow);
	//hCursor = NULL;
}

VOID DialogCode::ArrowWaitCursor ()
{
	static HCURSOR hcArrowWait;
	//if (hcArrowWait == NULL) hcArrowWait = LoadCursor (NULL, IDC_APPRUNING);
	SetCursor (hcArrowWait);
	//hCursor = hcArrowWait;
}

VOID DialogCode::HandCursor ()
{
	static HCURSOR hcHand;
	//if (hcHand == NULL) hcHand = LoadCursor (NULL, IDC_HAND);
	SetCursor (hcHand);
	//hCursor = hcHand;
}


/*
BOOL BrowseFilesInDir (HWND hwndDlg, TCHAR *stringId, TCHAR *initialDir, TCHAR *lpszFileName, BOOL keepHistory, BOOL saveMode, TCHAR *browseFilter, const TCHAR *initialFileName, const TCHAR *defaultExtension)
{
OPENFILENAMEW ofn;
TCHAR file[MAX_PATH] = { 0 };
TCHAR wInitialDir[MAX_PATH] = { 0 };
TCHAR filter[1024];
BOOL status = FALSE;

CoInitialize (NULL);

ZeroMemory (&ofn, sizeof (ofn));
*lpszFileName = 0;

if (initialDir)
{
swprintf_s (wInitialDir, sizeof (wInitialDir) / 2, L"%hs", initialDir);
ofn.lpstrInitialDir			= wInitialDir;
}

if (initialFileName)
_tcscpy_s (file, array_capacity (file), initialFileName);

ofn.lStructSize				= sizeof (ofn);
ofn.hwndOwner				= hwndDlg;

wsprintfW (filter, "%ls (*.*)%c*.*%c%ls (*.tc)%c*.tc%c%c",
"ALL_FILES", 0, 0, L"TC_VOLUMES", 0, 0, 0);
ofn.lpstrFilter				= browseFilter ? browseFilter : filter;
ofn.nFilterIndex			= 1;
ofn.lpstrFile				= file;
ofn.nMaxFile				= sizeof (file) / sizeof (file[0]);
ofn.lpstrTitle				= stringId;
ofn.lpstrDefExt				= defaultExtension;
ofn.Flags					= OFN_HIDEREADONLY
| OFN_PATHMUSTEXIST
| (keepHistory ? 0 : OFN_DONTADDTORECENT)
| (saveMode ? OFN_OVERWRITEPROMPT : 0);

if (!keepHistory)
CleanLastVisitedMRU ();

SystemFileSelectorCallerThreadId = GetCurrentThreadId();
SystemFileSelectorCallPending = TRUE;

if (!saveMode)
{
if (!GetOpenFileNameW (&ofn))
goto ret;
}
else
{
if (!GetSaveFileNameW (&ofn))
goto ret;
}

SystemFileSelectorCallPending = FALSE;

WideCharToMultiByte (CP_ACP, 0, file, -1, lpszFileName, MAX_PATH, NULL, NULL);

if (!keepHistory)
CleanLastVisitedMRU ();

status = TRUE;

ret:
SystemFileSelectorCallPending = FALSE;
ResetCurrentDirectory();
CoUninitialize();

return status;
}


BOOL DialogCode::BrowseFiles (HWND hwndDlg, TCHAR *stringId, TCHAR *lpszFileName, BOOL keepHistory, BOOL saveMode, TCHAR *browseFilter)
{
return BrowseFilesInDir (hwndDlg, stringId, NULL, lpszFileName, keepHistory, saveMode, browseFilter);
}

*/

BOOL DialogCode::OpenDirectorise(HWND ParentHWnd, const  TCHAR *dirName)
{
	if((int)ShellExecute(ParentHWnd, _T("open"), dirName, NULL, NULL, SW_SHOWNORMAL) <= 32)
		return FALSE;

	return TRUE;
}