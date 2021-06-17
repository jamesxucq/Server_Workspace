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

#include "third_party.h"
#include "FileUtility.h"
#include "StringUtility.h"

#include "Checksum.h"


void get_md5(char *buf, int len, unsigned char *sum) {
	MD5_CTX md;

	MD5Init(&md);
	MD5Update(&md, (unsigned char *)buf, len);
	MD5Final(&md);

	memcpy(sum, md.digest, DIGEST_LENGTH);
}

//static int sumresidue;
static MD5_CTX md;

inline void sum_init(int seed)
{MD5Init(&md);}

/**
* Feed data into an MD4 accumulator, md.  The results may be
* retrieved using sum_end().  md is used for different purposes at
* different points during execution.
*
* @todo Perhaps get rid of md and just pass in the address each time.
* Very slightly clearer and slower.
**/
inline void sum_update(char *p, int len)
{MD5Update(&md, (unsigned char *)p, len);}

inline int sum_end(unsigned char *sum) {
	MD5Final(&md); 
	memcpy(sum, md.digest, DIGEST_LENGTH);
	return DIGEST_LENGTH;
}

void file_csum(unsigned char *sum, const TCHAR *file_name, off_t offset) {
	HANDLE hFile;
	struct tiny_map *buf;
	off_t i, dwFileSize;
	MD5_CTX md;
	int remainder;

	memset(sum, '\0', DIGEST_LENGTH);
	hFile = CreateFile( file_name, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return;
	} 

	DWORD dwFileOffset = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwFileOffset && NO_ERROR!=GetLastError()) 
		return;
	dwFileSize = GetFileSize(hFile, NULL);

	buf = (struct tiny_map *) map_file(hFile, dwFileSize, MAX_MAP_SIZE, CSUM_CHUNK);
	MD5Init(&md);
	for (i = offset; i + CSUM_CHUNK <= dwFileSize; i += CSUM_CHUNK) {
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, CSUM_CHUNK), CSUM_CHUNK);
	}
	remainder = (int) (dwFileSize - i);
	if (remainder > 0)
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, remainder), remainder);
	MD5Final(&md);
	memcpy(sum, md.digest, DIGEST_LENGTH);

	unmap_file(buf);
	if(hFile != INVALID_HANDLE_VALUE) {
		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
	}
}

void file_csum(unsigned char *sum, const TCHAR *file_name) {
	HANDLE hFile;
	struct tiny_map *buf;
	off_t i, dwFileSize;
	MD5_CTX md;
	int remainder;

	memset(sum, '\0', DIGEST_LENGTH);
	hFile = CreateFile( file_name, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return;
	} 

	DWORD dwFileOffset = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwFileOffset && NO_ERROR!=GetLastError()) 
		return;
	dwFileSize = GetFileSize(hFile, NULL);

	buf = (struct tiny_map *) map_file(hFile, dwFileSize, MAX_MAP_SIZE, CSUM_CHUNK);
	MD5Init(&md);
	for (i = 0; i + CSUM_CHUNK <= dwFileSize; i += CSUM_CHUNK) {
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, CSUM_CHUNK), CSUM_CHUNK);
	}
	remainder = (int) (dwFileSize - i);
	if (remainder > 0)
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, remainder), remainder);
	MD5Final(&md);
	memcpy(sum, md.digest, DIGEST_LENGTH);

	unmap_file(buf);
	if(hFile != INVALID_HANDLE_VALUE) {
		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
	}
}

char *get_md5sum_text(char *buffer, int length, char *sum_text) {
	MD5_CTX md;

	MD5Init(&md);
	MD5Update(&md, (unsigned char *)buffer, length);
	MD5Final(&md);

	for (int index=0; index<DIGEST_LENGTH; index++)
		sprintf_s (sum_text+index*2, MD5_TEXT_LENGTH, "%02x", md.digest[index]);
	sum_text[32] = '\0';

	return sum_text;
}