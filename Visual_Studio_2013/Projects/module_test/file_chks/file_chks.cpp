// file_chks.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "windows.h"


#define FILES_CHKS_DEFAULT				_T(".\\files_chks.sdo")
#define INVA_INDE_VALU ((ULONG)-1)
#define MD5_DIGEST_LEN					16
#define MD5_TEXT_LENGTH					33
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ChksEntry {
    ULONG sibling, recycled;
    FILETIME ftLastWrite;
    unsigned char szFileChks[MD5_DIGEST_LEN];
};

namespace ChksUtility {
	ULONG ReadNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListInde);
};

ULONG ChksUtility::ReadNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListInde) {
    DWORD slen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListInde * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, pChksEntry, sizeof(struct ChksEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lListInde;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EntryPrint(struct ChksEntry *pChksEntry, DWORD lListIndex) ;
int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hChksList;
	/////////////////////////////////////////
	hChksList = CreateFile( FILES_CHKS_DEFAULT, 
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hChksList == INVALID_HANDLE_VALUE ) {
		printf("create file failed: %d\n", GetLastError() );
		printf(" File: %s Line: %d\n", _T(__FILE__), __LINE__);
		return -1;
	}
	/////////////////////////////////////////
	if(0x02 != argc) {
		printf("inva input arg!\n");
		return -1;
	}
	DWORD lListIndex = _ttoi(argv[1]);
	/////////////////////////////////////////
	struct ChksEntry tChksEntry;
	tChksEntry.sibling = lListIndex;
	DWORD lIndex;
	for(lIndex = 0; ; lIndex++) {
		if(INVA_INDE_VALU == ChksUtility::ReadNode(hChksList, &tChksEntry, tChksEntry.sibling))
			break;
		EntryPrint(&tChksEntry, lIndex);
	}
	/////////////////////////////////////////
	if(hChksList != INVALID_HANDLE_VALUE) {
		CloseHandle( hChksList );
		hChksList = INVALID_HANDLE_VALUE;
	}
	return 0;
}

TCHAR *ChksMD5(unsigned char *md5sum) {
    static TCHAR chksum_str[64];
    for (int inde=0; inde<MD5_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, MD5_TEXT_LENGTH, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
	//
	return chksum_str;
}

static TCHAR *ftim_unis(TCHAR *timestr, FILETIME *filetime) {  // Wed, 15 Nov 1995 04:58:08 GMT
    SYSTEMTIME st;
    if(!FileTimeToSystemTime(filetime, &st)) return NULL;
    struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
    _tcsftime(timestr, 32, _T("%a, %d %b %Y %H:%M:%S GMT"), &gm);
    return timestr;
}

static TCHAR *slawi(FILETIME *last_write) {
    static TCHAR timestr[64];
    ftim_unis(timestr, last_write);
	return timestr;
}

void EntryPrint(struct ChksEntry *pChksEntry, DWORD lListIndex) {
	wprintf(_T("lListIndex:%d sibling:%d recycled:%d LastWrite:%s szFileChks:%s\n"), 
		lListIndex,
		pChksEntry->sibling,
		pChksEntry->recycled,
		slawi(&pChksEntry->ftLastWrite),
		ChksMD5(pChksEntry->szFileChks));
}