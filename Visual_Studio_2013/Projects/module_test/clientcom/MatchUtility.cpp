#include "stdafx.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::tr1::unordered_map;      // namespace where class lives by default

#include "third_party.h"
#include "StringUtility.h"
#include "FileUtility.h"
#include "BuildHashTable.h"
#include "Checksum.h"

#include "MatchUtility.h"


#define MATCH_BACKET_SIZE		2048

/**
* Scan through a origin file, looking for sections that match
* psums_complex from the generator, and transmit either literal or token
* data.
**/
int MatchFileCsums(vector<struct local_match *> &vlocal_match, hashtable *htable, TCHAR *szFileLocal, DWORD start_offset) {
	HANDLE hFileLocal;
	DWORD dwFileLength, dwFileHigh;
	DWORD last_posi, current_posi, end_posi;
	struct tiny_map *buffer;
	int found_index, last_found;

	if(!htable || !szFileLocal) return -1;
	LOG_DEBUG(_T("match file sums. new file is:%s"), szFileLocal);

	hFileLocal = CreateFile( szFileLocal, 
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFileLocal == INVALID_HANDLE_VALUE ) {
		LOG_TRACE( _T("create file failed.\n") );
		return -1;
	} 
	dwFileLength = GetFileSize(hFileLocal, &dwFileHigh);
	if(INVALID_FILE_SIZE == dwFileLength) return -1;

	buffer = (struct tiny_map *)map_file(hFileLocal, dwFileLength, MAX_MAP_SIZE, BLOCK_SIZE);
	if(!buffer) return -1;  
	// LOG_DEBUG(_T("buffer:%x"), buffer);
	end_posi = dwFileLength - start_offset;

	struct local_match *alloc_local; struct file_matcher *alloc_inform;
	last_posi = current_posi = 0;
	last_found = 1;
	while (current_posi <= end_posi) {
		if (last_found) found_index = HashTableSearch((BYTE *)map_ptr(buffer, current_posi + start_offset, BLOCK_SIZE), htable, FIRST_HASH_CHECK); // bFine :>=0 found <0  not found
		else found_index = HashTableSearch((BYTE *)map_ptr(buffer, current_posi + start_offset, BLOCK_SIZE), htable, ROLLING_HASH_CHECK); // bFine :>=0 found <0  not found

		if (0 <= found_index) { // found the same block.
			LOG_DEBUG(_T("match one block."));
			////////////////////////////////////////////////////////////////////////
			if (!last_found) {
				int mlen = current_posi - last_posi;
				alloc_local = AddModifyData(&vlocal_match, mlen);
				if (!alloc_local) return -1;

				alloc_local->inform->offset = last_posi;
				alloc_local->inform->index_length = mlen;
				memcpy(alloc_local->buffer, map_ptr(buffer, last_posi, mlen), mlen);
			}
			////////////////////////////////////////////////////////////////////////
			alloc_inform = AddCsumMatch(&vlocal_match);
			if (!alloc_inform) return -1;

			alloc_inform->offset = current_posi;
			alloc_inform->index_length = found_index / BLOCK_SIZE;
			////////////////////////////////////////////////////////////////////////
			last_found = 1;
			current_posi += BLOCK_SIZE;
			last_posi = current_posi;
		} else { // not found the same block.
			// LOG_DEBUG(_T("not found the same block."));
			last_found = 0;
			current_posi++;
		}

		if(!last_found && (end_posi==current_posi)) {
			LOG_DEBUG(_T("the last not match data."));
			int mlen = current_posi - last_posi;
			alloc_local = AddModifyData(&vlocal_match, mlen);
			if (!alloc_local) return -1;

			alloc_local->inform->offset = last_posi;
			alloc_local->inform->index_length = mlen;
			memcpy(alloc_local->buffer, map_ptr(buffer, last_posi, mlen), mlen);	
			// BinaryLogger::LogBinaryCode(alloc_local->buffer, mlen);
		}
	}

	LOG_DEBUG(_T("end match file sums."));
	//get_md5((char *) buffer, FileSize, FileCsumMD5);
	unmap_file(buffer);
	CloseHandle( hFileLocal );

	return 0;
}

int MatchChunkCsums(vector<struct local_match *> &vlocal_match, hashtable *htable, TCHAR *szFileLocal, DWORD chunk_offset) {
	HANDLE hFileLocal;
	DWORD dwFileLength, dwFileHigh;
	DWORD last_posi, current_posi, end_posi;
	struct tiny_map *buffer;
	int found_index, last_found;

	if(!htable || !szFileLocal) return -1;
	LOG_DEBUG(_T("match chunk sums."));

	hFileLocal = CreateFile( szFileLocal, 
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFileLocal == INVALID_HANDLE_VALUE ) {
		LOG_TRACE( _T("create file failed.\n") );
		return -1;
	} 
	dwFileLength = GetFileSize(hFileLocal, &dwFileHigh);
	if(INVALID_FILE_SIZE == dwFileLength) return -1;

	buffer = (struct tiny_map *)map_file(hFileLocal, dwFileLength, MAX_MAP_SIZE, BLOCK_SIZE);
	if(!buffer) return -1;  
	if(chunk_offset + CHUNK_SIZE > dwFileLength) end_posi = dwFileLength - chunk_offset;
	else end_posi = CHUNK_SIZE;

	struct local_match *alloc_local; struct file_matcher *alloc_inform;
	last_posi = current_posi = 0;
	last_found = 1;
	while (current_posi <= end_posi) {
		if (last_found) found_index = HashTableSearch((BYTE *)map_ptr(buffer, current_posi + chunk_offset, BLOCK_SIZE), htable, FIRST_HASH_CHECK); // bFine :>=0 found <0  not found
		else found_index = HashTableSearch((BYTE *)map_ptr(buffer, current_posi + chunk_offset, BLOCK_SIZE), htable, ROLLING_HASH_CHECK); // bFine :>=0 found <0  not found

		if (0 <= found_index) { // found the same block.
			// LOG_DEBUG(_T("found the same block."));
			////////////////////////////////////////////////////////////////////////
			if (!last_found) {
				int mlen = current_posi - last_posi;
				alloc_local = AddModifyData(&vlocal_match, mlen);
				if (!alloc_local) return -1;

				alloc_local->inform->offset = last_posi;
				alloc_local->inform->index_length = mlen;
				memcpy(alloc_local->buffer, map_ptr(buffer, last_posi, BLOCK_SIZE), mlen);
			}
			////////////////////////////////////////////////////////////////////////
			alloc_inform = AddCsumMatch(&vlocal_match);
			if (!alloc_inform) return -1;

			alloc_inform->offset = current_posi;
			alloc_inform->index_length = found_index / BLOCK_SIZE;
			////////////////////////////////////////////////////////////////////////
			last_found = 1;
			current_posi += BLOCK_SIZE;
			last_posi = current_posi;
		} else { // not found the same block.
			// LOG_DEBUG(_T("not found the same block."));
			last_found = 0;
			current_posi++;
		}

		if(!last_found && (end_posi==current_posi)) {
			LOG_DEBUG(_T("the last not match data."));
			int mlen = current_posi - last_posi;
			alloc_local = AddModifyData(&vlocal_match, mlen);
			if (!alloc_local) return -1;

			alloc_local->inform->offset = last_posi;
			alloc_local->inform->index_length = mlen;
			memcpy(alloc_local->buffer, map_ptr(buffer, last_posi, mlen), mlen);		
		}
	}
	//get_md5((char *) buffer, FileSize, FileCsumMD5);
	unmap_file(buffer);
	CloseHandle( hFileLocal );

	return 0;
}

DWORD BlockMatchFileRange(struct SessionCtrl *psctrl, struct InternalValue *psvalue, TCHAR *szFileLocal) {
	vector<struct local_match *> vlocal_match;
	hashtable *htable = NULL;

	/////////////////////////////////////////////////////////////////////////
	if(!psctrl || !psvalue || !szFileLocal) return -1;
	LOG_DEBUG(_T("match block map local dat file section."));
	if( INVALID_HANDLE_VALUE == psctrl->file_stream ) {
		LOG_TRACE( _T("create file failed.\n") );
		LOG_TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} 

	/////////////////////////////////////////////////////////////////////////
	LOG_DEBUG(_T("match block create build data section."));
	htable = CreateHashTable(MATCH_BACKET_SIZE);
	if (BuildHashTable(htable, psctrl->file_stream, psvalue->content_length)) return -1;
	LOG_DEBUG(_T("match sums chunk."));
	if (MatchFileCsums(vlocal_match, htable, szFileLocal, psvalue->offset))
		return -1;

	/////////////////////////////////////////////////////////////////////////
	LOG_DEBUG(_T("match block create data file section."));
	if (htable) hash_destroy(htable, free);

	/////////////////////////////////////////////////////////////////////////
	int match_status = 0;
	if(FlushMatch2File(psctrl, psvalue, &vlocal_match)) match_status = -1;
	DeleteMatchVec(&vlocal_match);

	return match_status;
}

DWORD BlockMatchChunkRange(struct SessionCtrl *psctrl, struct InternalValue *psvalue, TCHAR *szFileLocal) {
	vector<struct local_match *> vlocal_match;
	hashtable *htable = NULL;

	/////////////////////////////////////////////////////////////////////////
	if(!psctrl || !psvalue || !szFileLocal) return -1;
	LOG_DEBUG(_T("match block map local dat file section."));
	if( INVALID_HANDLE_VALUE == psctrl->file_stream ) {
		LOG_TRACE( _T("create file failed.\n") );
		LOG_TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} 

	/////////////////////////////////////////////////////////////////////////
	LOG_DEBUG(_T("match block create build data section."));
	htable = CreateHashTable(MATCH_BACKET_SIZE);
	if (BuildHashTable(htable, psctrl->file_stream, psvalue->content_length)) return -1;
	LOG_DEBUG(_T("match sums chunk."));
	if (MatchChunkCsums(vlocal_match, htable, szFileLocal, psvalue->offset))
		return -1;

	/////////////////////////////////////////////////////////////////////////
	LOG_DEBUG(_T("match block create data file section."));
	if (htable) hash_destroy(htable, free);

	/////////////////////////////////////////////////////////////////////////
	int match_status = 0;
	if(FlushMatch2File(psctrl, psvalue, &vlocal_match)) match_status = -1;
	DeleteMatchVec(&vlocal_match);

	return match_status;
}

#define MATCH_INFORM_PRINT(INFORM) { \
	LOG_DEBUG(_T("*********************************************")); \
	LOG_DEBUG(_T("match_type:%d"), INFORM->match_type); \
	LOG_DEBUG(_T("offset:%d"), INFORM->offset); \
	LOG_DEBUG(_T("index_length:%d"), INFORM->index_length); \
	LOG_DEBUG(_T("*********************************************")); \
}

DWORD FlushMatch2File(struct SessionCtrl *psctrl, struct InternalValue *psvalue, vector<struct local_match *> *local_vector) {
	vector<struct local_match *>::iterator iter;
	DWORD dwWritenSize;

	if (!psctrl || !psvalue || !local_vector) return -1;

	DWORD result = SetFilePointer(psctrl->file_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	result = SetEndOfFile(psctrl->file_stream);
	if(FALSE==result && NO_ERROR!=GetLastError()) {
		LOG_DEBUG(_T("set end of file false, last error is:%d"), GetLastError());
		return -1;
	}

	struct file_matcher *inform;
	for(iter=local_vector->begin(); iter!=local_vector->end(); iter++) {
		inform = (*iter)->inform;
		if(!WriteFile(psctrl->file_stream, inform, sizeof (struct file_matcher), &dwWritenSize, NULL)) 
			return -1;
		if (MODIFY_CONTENT & inform->match_type) {
			if(!WriteFile(psctrl->file_stream, (*iter)->buffer, inform->index_length, &dwWritenSize, NULL)) 
				return -1;
			// BinaryLogger::LogBinaryCode((*iter)->buffer, inform->index_length);
		}
		MATCH_INFORM_PRINT(inform);
	}

	// LOG_DEBUG(_T("flush match to file. nContentLength:%d"), psvalue->content_length);
	psctrl->control_stype = FILE_CACHE;
	FlushFileBuffers(psctrl->file_stream);

	return 0;
}

struct file_matcher *AddCsumMatch(vector<struct local_match *> *local_vector) {
	struct local_match *alloc_local = NULL;
	struct file_matcher *alloc_inform = NULL;

	if (!local_vector) return NULL;

	alloc_local = (struct local_match *)malloc(sizeof(struct local_match));
	if(!alloc_local) return NULL;
	memset(alloc_local, '\0', sizeof(struct local_match));

	alloc_inform = (struct file_matcher *)malloc(sizeof(struct file_matcher));
	if(!alloc_inform) return NULL;
	memset(alloc_inform, '\0', sizeof(struct file_matcher));

	alloc_inform->match_type = CHECK_SUM;
	alloc_local->inform = alloc_inform;
	local_vector->push_back(alloc_local);

	return alloc_inform;
}

struct local_match *AddModifyData(vector<struct local_match *> *local_vector, int length) {
	struct local_match *alloc_local = NULL;
	struct file_matcher *alloc_inform = NULL;

	if (!local_vector) return NULL;

	alloc_local = (struct local_match *)malloc(sizeof(struct local_match));
	if(!alloc_local) return NULL;
	memset(alloc_local, '\0', sizeof(struct local_match));

	alloc_inform = (struct file_matcher *)malloc(sizeof(struct file_matcher));
	if(!alloc_inform) return NULL;
	memset(alloc_inform, '\0', sizeof(struct file_matcher));

	char *buffer = (char *)malloc(length);
	if(!buffer) return NULL;
	memset(buffer, '\0', length);

	alloc_inform->match_type = MODIFY_CONTENT;
	alloc_local->inform = alloc_inform;
	alloc_local->buffer = buffer;
	local_vector->push_back(alloc_local);

	return alloc_local;
}

void DeleteMatchVec(vector<struct local_match *> *local_vector) {
	vector <struct local_match *>::iterator iter;

	if(!local_vector) return;
	for(iter = local_vector->begin(); iter != local_vector->end(); iter++) {
		if(*iter) {
			if((*iter)->inform) 
				free((*iter)->inform);
			if((*iter)->buffer) free((*iter)->buffer);
			free(*iter);
		}
	}
	local_vector->clear();
}


int MatchChunkCsums(vector<struct simple_sum> *pChunkCsumVec, struct SessionCtrl *psctrl, TCHAR *szFileName) {
	unordered_map<string, struct simple_sum *> htLocalChunkCsums;
	struct simple_sums *psums_simple;

	if (!pChunkCsumVec || !psctrl || !szFileName) return -1;

	psums_simple = BuildFileSimpleCsum(szFileName, 0);
	if(!psums_simple) return -1;

	char md5_str[DIGEST_LENGTH+1];
	memset(md5_str, '\0', DIGEST_LENGTH+1);
	for(int index=0; index<psums_simple->count; index++) {
		memcpy(md5_str, ((struct simple_sum)psums_simple->sums[index]).md5_sum, DIGEST_LENGTH);
		htLocalChunkCsums[md5_str] = &(psums_simple->sums[index]);
	}

	unordered_map<string, struct simple_sum *>::iterator iter;
	DWORD slen; 
	struct simple_sum tSimpleCsum;

	DWORD result = SetFilePointer(psctrl->file_stream, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) return -1;
	do{
		if(!ReadFile(psctrl->file_stream, &tSimpleCsum, sizeof(struct simple_sum), &slen, NULL)) {
			if(ERROR_HANDLE_EOF != GetLastError()) return -1;
		}
		if(0 >= slen) break;
		if(sizeof(struct simple_sum) == slen) {
			memcpy(md5_str, tSimpleCsum.md5_sum, DIGEST_LENGTH);
			htLocalChunkCsums.find(md5_str);
			if(!((iter!=htLocalChunkCsums.end()) && ((iter->second)->offset==tSimpleCsum.offset))) {
				pChunkCsumVec->push_back(tSimpleCsum);
			}
		}

	}while(0 < slen);

	DeleteSimpleCsum(psums_simple);
	htLocalChunkCsums.clear();

	return 0;
}

struct local_match *AppendLocalMatch(OUT vector<struct local_match *> &vlocal_match, struct file_matcher *inform) {
	struct file_matcher *matcher = (struct file_matcher *)malloc(sizeof(struct file_matcher));
	if(!matcher) return NULL;
	memcpy(matcher, inform, sizeof(struct file_matcher));

	struct local_match *alloc_match = (struct local_match *)malloc(sizeof(struct local_match));
	if(!alloc_match) return NULL;
	vlocal_match.push_back(alloc_match);

	alloc_match->inform = matcher;
	alloc_match->buffer = NULL;

	return alloc_match;
}