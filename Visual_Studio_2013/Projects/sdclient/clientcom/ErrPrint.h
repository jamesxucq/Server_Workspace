#pragma once
#include "ErrorCode.h"

namespace NErrPrint {
	VOID Print(HWND hWnd, ec eCode);
	VOID AfxPrint(ec eCode);
};

#define PRINT_ERRCODE(HWND, ERR_CODE)	NErrPrint::Print(HWND, ERR_CODE)
#define AFX_PRINT_ERRCODE(ERR_CODE)	NErrPrint::AfxPrint(ERR_CODE)