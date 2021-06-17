#include "StdAfx.h"
#include "ErrorPrint.h"

DWORD NErrorPrint::AfxPrintError()
{
	AfxMessageBox(_T("数据无效!\n请检查."), MB_ICONERROR|MB_OK);
	return 0;
}