#include "StdAfx.h"

#include "./utility/ulog.h"
#include "StringUtility.h"
#include "NTFSLibrary.h"

#include "third_party.h"


DWORD NTFSLibrary::NTFSResize(TCHAR *szFileName, DWORD dwNewDisksize) {
	char file_name[MAX_PATH];

	strcon::ustr_ansi(file_name, szFileName);
	// logger(_T("c:\\setting.log"), _T("ntfs resize file name: %s new disksize: %d\r\n"), szFileName, dwNewDisksize);
	int resiz_result = ntfsresiz(file_name, dwNewDisksize<<10);
	// logger(_T("c:\\setting.log"), _T("ntfs resize status: %d\r\n"), resiz_result);

	return resiz_result;
}

DWORD NTFSLibrary::ValidateReduce(DWORD *dwLastDisksize, TCHAR *szFileName, DWORD dwReducedDisksize) {
	char file_name[MAX_PATH];
	int used_size;

	strcon::ustr_ansi(file_name, szFileName);
	int valid_value = ntfsinfo(&used_size, file_name);
	// logger(_T("c:\\setting.log"), _T("ntfs inform status: %d\r\n"), valid_value);
	if(valid_value) return valid_value;

	*dwLastDisksize = used_size;
	int dwNewDisksize = dwReducedDisksize << 10;
	// logger(_T("c:\\setting.log"), _T("used_size:%d new disk size: %d\r\n"), used_size, dwNewDisksize);
	if(used_size > dwNewDisksize) return 0x01;

	return 0x00;
}