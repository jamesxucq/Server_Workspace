
#include "stdafx.h"
#include "ErrPrint.h"

VOID NErrPrint::Print(HWND hWnd, ec eCode) {
	TCHAR ErrMessage[512];

	if (!hWnd) _stprintf_s(ErrMessage, _T("(NULL)THE ERROR CODE IS: 0X%0X"), eCode);
	else  _stprintf_s(ErrMessage, _T("THE ERROR CODE IS: 0X%0X"), eCode);
	MessageBox(hWnd, ErrMessage, _T("ERROR MESSAGE!"), MB_OK|MB_ICONERROR);
}

VOID NErrPrint::AfxPrint(ec eCode) {
	TCHAR ErrMessage[512];

	_stprintf_s(ErrMessage, _T("THE ERROR CODE IS: 0X%0X"), eCode);
	AfxMessageBox(ErrMessage, MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
}