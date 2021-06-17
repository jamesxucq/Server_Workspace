#include "stdafx.h"
/*
* Routines to support checksumming of bytes.
*
* Copyright (C) 1996 Andrew Tridgell
* Copyright (C) 1996 Paul Mackerras
* Copyright (C) 2004-2009 Wayne Davison
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, visit the http://fsf.org website.
*/

// #include "FileUtility.h"
// #include "StringUtility.h"
#include "windows.h"

#include "tiny_map64.h"
#include "Checksum.h"


void chksm::get_md5(unsigned char *buf, int len, unsigned char *sum)
{
    MD5_CTX md;

    MD5Init(&md);
    MD5Update(&md, buf, len);
    MD5Final(&md);
    memcpy(sum, md.digest, MD5_DIGEST_LEN);
}

unsigned char *chksm::file_chks(unsigned char *md5_chks, const TCHAR *file_name, unsigned __int64 offset)
{
    HANDLE hFile;
    struct tiny_map64 *buf;
    unsigned __int64 inde, qwFileSize;
    DWORD dwFileSizeHigh;
    MD5_CTX md;
    int remainder;
//
    memset(md5_chks, '\0', MD5_DIGEST_LEN);
    hFile = CreateFile( file_name,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
    if( INVALID_HANDLE_VALUE == hFile ) {
// _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return NULL;
    }
//
    qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    buf = (struct tiny_map64 *) map_file64(hFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = offset; inde + CHKS_CHUNK <= qwFileSize; inde += CHKS_CHUNK) {
        MD5Update(&md, map_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int) (qwFileSize - inde);
    if (remainder > 0)
        MD5Update(&md, map_ptr64(buf, inde, remainder), remainder);
    MD5Final(&md);
	memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
//
    unmap_file64(buf);
    if(INVALID_HANDLE_VALUE != hFile) {
        CloseHandle( hFile );
        hFile = INVALID_HANDLE_VALUE;
    }
//
    return md5_chks;
}

unsigned char *chksm::file_chks(unsigned char *md5_chks, const TCHAR *file_name)
{
    HANDLE hFile;
    struct tiny_map64 *buf;
    unsigned __int64 inde, qwFileSize;
    DWORD dwFileSizeHigh;
    MD5_CTX md;
    int remainder;
//
    memset(md5_chks, '\0', MD5_DIGEST_LEN);
    hFile = CreateFile( file_name,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
    if( INVALID_HANDLE_VALUE == hFile ) {
// _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return NULL;
    }
//
    qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    buf = (struct tiny_map64 *) map_file64(hFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = 0; inde + CHKS_CHUNK <= qwFileSize; inde += CHKS_CHUNK) {
        MD5Update(&md, map_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int) (qwFileSize - inde);
    if (remainder > 0)
        MD5Update(&md, map_ptr64(buf, inde, remainder), remainder);
    MD5Final(&md);
	memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
//
    unmap_file64(buf);
    if(INVALID_HANDLE_VALUE != hFile) {
        CloseHandle( hFile );
        hFile = INVALID_HANDLE_VALUE;
    }
//
    return md5_chks;
}

char *chksm::get_md5sum_text(char *chksum_str, char *buffer, int length)
{
    MD5_CTX md;
    unsigned char digest[MD5_DIGEST_LEN];
//
    MD5Init(&md);
    MD5Update(&md, (unsigned char *)buffer, length);
    MD5Final(&md);
	memcpy(digest, md.digest, MD5_DIGEST_LEN);
//
    for (int inde=0; inde<MD5_DIGEST_LEN; inde++)
        sprintf(chksum_str+inde*2, "%02x", digest[inde]);
    chksum_str[32] = '\0';
//
    return chksum_str;
}
