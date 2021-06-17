// chkchks.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "md5.h"
#include "tiny_map64.h"
#include "windows.h"

//
#define MD5_TEXT_LENGTH					33

#define MD5_DIGEST_LEN 16
#define BLOCK_SIZE						0x2000  // 8K
#define CHUNK_SIZE						((int)1 << 22) // 4M

#define MAX_MAP_SIZE 0x80000 // 512K
#define CHKS_CHUNK	4096
//
int ChunkChksExt(unsigned char *md5_chks, HANDLE hChunkFile, unsigned __int64 qwFileSize, unsigned __int64 offset) {
    struct tiny_map64 *buffer;
    unsigned __int64 inde, offset_end;
    MD5_CTX md;
    int remainder;
    //
    if(offset > qwFileSize) return -1;
// _LOG_DEBUG(_T("chunk check sum file size:%d offset:%d"), qwFileSize, offset); // disable by james 20140410
    memset(md5_chks, '\0', MD5_DIGEST_LEN);
    buffer = (struct tiny_map64 *) map_file64(hChunkFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
    offset_end = offset + CHUNK_SIZE;
    if (offset_end > qwFileSize) offset_end = qwFileSize;
    //
    MD5Init(&md);
    for (inde = offset; inde + CHKS_CHUNK <= offset_end; inde += CHKS_CHUNK) {
        MD5Update(&md, map_ptr64(buffer, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int) (offset_end - inde);
    if (remainder > 0)
        MD5Update(&md, map_ptr64(buffer, inde, remainder), remainder);
    MD5Final(&md);
    memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
    //
    unmap_file64(buffer);
    return 0;
}

//
VOID ChksMD5(TCHAR *label, unsigned char *md5sum) {
    TCHAR chksum_str[64];
    for (int inde=0; inde<MD5_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, MD5_TEXT_LENGTH, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
	wprintf(_T("%s:%s\n"), label, chksum_str);
}

//
void check_md5(TCHAR *szFileName){
	HANDLE hChksFile;
	hChksFile = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hChksFile == INVALID_HANDLE_VALUE ) {
wprintf( _T("create file failed.\n") );
wprintf(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		// return -1;
	} 
//
	unsigned __int64 qwFileSize;
	DWORD dwFileSizeHigh;
    qwFileSize = GetFileSize(hChksFile, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
	//
	unsigned char md5_chks[MD5_DIGEST_LEN];
	int iChksValue = 0;
	unsigned __int64 inde;
	for(inde=0x00; !iChksValue; inde+=CHUNK_SIZE) {
		iChksValue = ChunkChksExt(md5_chks, hChksFile, qwFileSize, inde);
		if(!iChksValue) ChksMD5(_T("MD5"), md5_chks);
	}
//
	CloseHandle( hChksFile );
}

#define ORG_FILE _T("D:\\9 Aretha Franklin.dat")
#define LOCAL_FILE _T("E:\\9 Aretha Franklin.dat")
#define SERVER_FILE _T("F:\\9 Aretha Franklin.dat")
int _tmain(int argc, _TCHAR* argv[])
{
	wprintf( _T("file name:%s\n"),ORG_FILE);
	check_md5(ORG_FILE);
	wprintf( _T("\n"));
	wprintf( _T("file name:%s\n"),LOCAL_FILE);
	check_md5(LOCAL_FILE);
	wprintf( _T("\n"));
	wprintf( _T("file name:%s\n"),SERVER_FILE);
	check_md5(SERVER_FILE);
//
	return 0;
}

