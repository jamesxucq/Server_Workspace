#pragma once
#include "DropArray.h"

#include "OleIdl.h"
//#define	DROPEFFECT_NONE	( 0 )
//#define	DROPEFFECT_COPY	( 1 )
//#define	DROPEFFECT_MOVE	( 2 )
//#define	DROPEFFECT_LINK	( 4 )

namespace ClipboardViewer {
	static BOOL s_bRegisterWindow;
	static HWND s_hWnd;
//////////////////////////////////////////////////
	static BOOL RegisterWindowClass(LPCTSTR szClassName);
	BOOL CreateViewerWindow();
//////////////////////////////////////////////////
	static HWND s_hNextViewer;
//////////////////////////////////////////////////
	static void OnDrawClipboard(HWND hWnd);
	static void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	static int OnCreate(HWND hWnd);
	static void OnDestroy(HWND hWnd);
//////////////////////////////////////////////////
	void DestroyWindow();
};