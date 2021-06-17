// file_time.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "windows.h"


#define RIVER_DIRECTORY_DEFAULT				_T(".\\river_folder.sdo")
#define FILES_RIVER_DEFAULT				_T(".\\files_river.sdo")
#define INVA_INDE_VALU		((ULONG)-1)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SHA1_DIGEST_LEN					20

struct FileEntry {
	ULONG sibling, parent;
	union {
		ULONG isonym_node;
		ULONG recycled;
	};
	//
	TCHAR szFileName[MAX_PATH];
    unsigned __int64 qwFileSize;
	unsigned char szFileChks[SHA1_DIGEST_LEN];
	FILETIME ftCreatTime;
	FILETIME ftLastWrite;
	ULONG list_hinde;
	DWORD chkslen; /* chunk length */
};

static TCHAR *ChksSha1(unsigned char *sha1sum) {
    static TCHAR chksum_str[64];
    for (int inde=0; inde<SHA1_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, 41, _T("%02x"), sha1sum[inde]);
    chksum_str[40] = _T('\0');
//
	return chksum_str;
}

namespace EntryUtility {
	ULONG ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde);
}

ULONG EntryUtility::ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde) {
    DWORD slen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileInde * sizeof(struct FileEntry);
    if(!ReadFile(hFilesRiver, pFileEntry, sizeof(struct FileEntry), &slen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lFileInde;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EntryPrint(struct FileEntry *pFileEntry, DWORD lFileIndex) ;
int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hFilesRiver;
	/////////////////////////////////////////
	hFilesRiver = CreateFile( FILES_RIVER_DEFAULT, 
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFilesRiver == INVALID_HANDLE_VALUE ) {
		printf("create file failed: %d\n", GetLastError() );
		printf(" File: %s Line: %d\n", _T(__FILE__), __LINE__);
		return -1;
	}
	/////////////////////////////////////////
	struct FileEntry tFileEntry;
	DWORD lFileIndex;
	for(lFileIndex = 0; ; lFileIndex++) {
		if(INVA_INDE_VALU == EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lFileIndex))
			break;
		EntryPrint(&tFileEntry, lFileIndex);
	}
	/////////////////////////////////////////
	if(hFilesRiver != INVALID_HANDLE_VALUE) {
		CloseHandle( hFilesRiver );
		hFilesRiver = INVALID_HANDLE_VALUE;
	}
	return 0;
}

static TCHAR *ftim_unis(TCHAR *timestr, FILETIME *filetime) {  // Wed, 15 Nov 1995 04:58:08 GMT
    SYSTEMTIME st;
    if(!FileTimeToSystemTime(filetime, &st)) return NULL;
    struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
    _tcsftime(timestr, 32, _T("%a, %d %b %Y %H:%M:%S GMT"), &gm);
    return timestr;
}

static TCHAR *slawi(TCHAR *timestr, FILETIME *last_write) {
    ftim_unis(timestr, last_write);
	return timestr;
}

void EntryPrint(struct FileEntry *pFileEntry, DWORD lFileIndex) {
	static TCHAR timestr1[64];
	static TCHAR timestr2[64];
	wprintf(_T("lFileIndex:%d qwFileSize:%I64u Creation:%s LastWrite:%s szFileChks:%s name:%s\n"), 
		lFileIndex,
		pFileEntry->qwFileSize,
		slawi(timestr1, &pFileEntry->ftCreatTime),
		slawi(timestr2, &pFileEntry->ftLastWrite),
		ChksSha1(pFileEntry->szFileChks),
		pFileEntry->szFileName);
}