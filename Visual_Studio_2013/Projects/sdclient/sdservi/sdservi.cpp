// sdservi.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include "ulog.h"

void __stdcall DebugDetail(HWND hWnd, TCHAR *lpText, TCHAR *lpCaption, UINT uType) {
	MessageBox(hWnd, lpText, lpCaption, uType);
	// logger(_T("c:\\install.log"), _T("Caption:%s Text:%s\r\n"), lpCaption, lpText);
}

