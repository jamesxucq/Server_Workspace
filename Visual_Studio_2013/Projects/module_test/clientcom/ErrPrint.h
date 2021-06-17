#pragma once
#include "ErrorCode.h"

namespace NErrPrint
{
	void Print(HWND hWnd, ec eCode);
	void AfxPrint(ec eCode);
};

#define PRINT_ERRCODE(HWND, ERR_CODE)	NErrPrint::Print(HWND, ERR_CODE)
#define AFX_PRINT_ERRCODE(ERR_CODE)	NErrPrint::AfxPrint(ERR_CODE)