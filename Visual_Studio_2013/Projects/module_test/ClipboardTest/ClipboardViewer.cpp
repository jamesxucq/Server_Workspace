#include "StdAfx.h"
#include "ClipboardViewer.h"


static HINSTANCE GetInstanceHandle() {
	return (HINSTANCE)GetModuleHandle(NULL);
}

// HWND ClipboardViewer::s_hNextViewer = NULL;
static LRESULT CALLBACK ViewerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CHANGECBCHAIN:
		ClipboardViewer::OnChangeCbChain((HWND)wParam, (HWND)lParam);
		break;
	case WM_DRAWCLIPBOARD:
		ClipboardViewer::OnDrawClipboard(hWnd);
		break;
	case WM_CREATE:
		ClipboardViewer::OnCreate(hWnd);
		break;
	case WM_DESTROY:
		ClipboardViewer::OnDestroy(hWnd);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL ClipboardViewer::RegisterWindowClass(LPCTSTR szClassName) {
	WNDCLASS wc = {0};

	wc.style = 0;
	wc.hInstance		= GetInstanceHandle();
	wc.lpszClassName	= szClassName;
	wc.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	wc.lpfnWndProc		= ViewerWndProc;

	ATOM ant = RegisterClass( &wc );
	if( ant == NULL ) {
		TRACE(_T("ClipboardViewer::RegisterWindowClass - RegisterClass failed: %d\n"), GetLastError());
	}
	return (BOOL)(ant!= NULL);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//ClipboardViewer static member vars:
// HWND ClipboardViewer::s_hWnd = NULL;
// BOOL ClipboardViewer::s_bRegisterWindow = FALSE;
//
BOOL ClipboardViewer::CreateViewerWindow() {
	TCHAR szClassName[] = _T("Clipboard_Viewer");
	if( !s_bRegisterWindow ) s_bRegisterWindow = RegisterWindowClass(szClassName);
	if( !s_hWnd ) s_hWnd = CreateWindowEx(0, szClassName, _T("ViewerWnd"),0,0,0,0,0, NULL, 0, GetInstanceHandle(), NULL);
	if( s_hWnd == NULL ) {
		TRACE(_T("Unable to create Viewer window! GetLastError(): %d\n"), GetLastError());
		TRACE(_T("File: %s Line: %d\n"), _T(__FILE__), __LINE__);
	}

	return (BOOL)(s_hWnd != NULL);
}

void ClipboardViewer::OnDrawClipboard(HWND hWnd) {
	// 如果观看链中在当前程序下面存在下一个程序的话  
	// 就传递一个WM_DRAWCLIPBOARD 消息给它  
	if(s_hNextViewer) SendMessage(s_hNextViewer, WM_DRAWCLIPBOARD, 0, 0);

	/////////////////////////////////////////////////////////////////
	UINT uDropEffect = RegisterClipboardFormat(_T("Preferred DropEffect"));
	DWORD dwEffect, *dw;
	HGLOBAL hMemory;
	wchar_t szFileName[MAX_PATH];

	if (IsClipboardFormatAvailable(CF_HDROP)) {
		OpenClipboard(hWnd);
		if(hMemory = GetClipboardData(CF_HDROP)) {
			dw = (DWORD*)(GetClipboardData( uDropEffect));
			if(dw == NULL) 
				dwEffect = DROPEFFECT_COPY;
			else dwEffect = *dw;

			UINT cFiles = ::DragQueryFile((HDROP)hMemory, -1, NULL, 0); //得到文件数量  
			for (UINT index = 0; index < cFiles; index++) {
				//获得路径及文件名
				DragQueryFile((HDROP)hMemory, index, szFileName, sizeof(szFileName));
				objDropArray.InsDropPath(szFileName, dwEffect);
			}
		}
		CloseClipboard();
//		if(dwEffect & DROPEFFECT_MOVE) {
//			MessageBox(NULL,szFileName,_T("Move Files"),MB_OK);
//		} else	if(dwEffect & DROPEFFECT_COPY) {
//			MessageBox(NULL,szFileName,_T("Copy Files"),MB_OK);
//		}
	}
}

void ClipboardViewer::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter) {
	if (s_hNextViewer == hWndRemove) s_hNextViewer = hWndAfter;  
	else if (s_hNextViewer) ::SendMessage(s_hNextViewer, WM_CHANGECBCHAIN, (WPARAM)hWndRemove, (LPARAM)hWndAfter); 
}

int ClipboardViewer::OnCreate(HWND hWnd) {
	s_hNextViewer = ::SetClipboardViewer(hWnd);
	return 0;
}

void ClipboardViewer::OnDestroy(HWND hWnd) {
	ChangeClipboardChain(hWnd, s_hNextViewer);
}

void ClipboardViewer::DestroyWindow() {
	if( s_hWnd ) DestroyWindow( s_hWnd );
	s_hWnd = NULL;
}