#include "StdAfx.h"
#include "ErrorPrint.h"

DWORD NErrorPrint::AfxPrintError()
{
	AfxMessageBox(_T("������Ч!\n����."), MB_ICONERROR|MB_OK);
	return 0;
}