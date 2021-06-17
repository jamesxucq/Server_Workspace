#include "StdAfx.h"

#include "StringUtility.h"
#include "NTFSLibrary.h"

#include "third_party.h"


DWORD NTFSLibrary::NTFSResize(TCHAR *szFileName, DWORD dwNewDisksize)
{
	char file_name[MAX_PATH];

	nstriconv::unicode_ansi(file_name, szFileName);
	int resiz_result = ntfsresiz(file_name, dwNewDisksize<<10);
	LOG_DEBUG(_T("ntfs resize status:%d"), resiz_result);

	return resiz_result;
}

DWORD NTFSLibrary::ValidateReduce(TCHAR *szFileName, int iReducedDisksize)
{
	char file_name[MAX_PATH];
	int used_size;

	nstriconv::unicode_ansi(file_name, szFileName);

	int valid_result = ntfsinfo(&used_size, file_name);
	LOG_DEBUG(_T("ntfs inform status:%d"), valid_result);
	if(valid_result) return valid_result;

	int dwNewDisksize = iReducedDisksize << 10;
	LOG_DEBUG(_T("used_size:%d new disk size:%d"), used_size, dwNewDisksize);
	if(used_size > dwNewDisksize) return 1;

	return 0;
}