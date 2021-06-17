#include "StdAfx.h"
#include "BinaryLogger.h"

VOID BinaryLogger::LogBinaryCode(char *binary_code, int length) {
	FILE *code_fp = NULL;

	if(!binary_code || length<0) return;
	_tfopen_s(&code_fp, _T("c:\\binary_code.bin"), _T("ab"));
	if (!code_fp) return;

	int wlen = fwrite(binary_code, 1, length, code_fp);
	if (length != wlen) return;

	fclose(code_fp);
}

VOID BinaryLogger::LogBinaryCodeW(TCHAR *binary_code, int length) {
	FILE *code_fp = NULL;

	if(!binary_code || length<0) return;
	_tfopen_s(&code_fp, _T("c:\\binary_code.bin"), _T("ab"));
	if (!code_fp) return;

	int wlen = fwrite(binary_code, 1, length*sizeof(WCHAR), code_fp);
	if (length != wlen) return;

	fclose(code_fp);
}