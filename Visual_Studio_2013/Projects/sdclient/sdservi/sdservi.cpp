// sdservi.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include "ulog.h"

void __stdcall DebugDetail(HWND hWnd, TCHAR *lpText, TCHAR *lpCaption, UINT uType) {
	MessageBox(hWnd, lpText, lpCaption, uType);
	// logger(_T("c:\\install.log"), _T("Caption:%s Text:%s\r\n"), lpCaption, lpText);
}

