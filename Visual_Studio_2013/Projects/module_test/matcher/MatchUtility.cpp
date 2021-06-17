#include "stdafx.h"

#include "tiny_map64.h"
#include "MatchUtility.h"

//
static struct file_matcher *AddChksMatch(vector<struct local_match *> *local_vector) {
	struct local_match *alloc_local = NULL;
	struct file_matcher *matcher = NULL;
	//
	alloc_local = (struct local_match *)malloc(sizeof(struct local_match));
	if(!alloc_local) return NULL;
	memset(alloc_local, '\0', sizeof(struct local_match));
	//
	matcher = (struct file_matcher *)malloc(sizeof(struct file_matcher));
	if(!matcher) {
		free(alloc_local);
		return NULL;
	}
	memset(matcher, '\0', sizeof(struct file_matcher));
	//
	matcher->match_type = CHECK_SUMS;
	alloc_local->matcher = matcher;
	local_vector->push_back(alloc_local);
	//
	return matcher;
}

static struct file_matcher *AddVariaMatch(vector<struct local_match *> *local_vector) {
	struct local_match *alloc_local = NULL;
	struct file_matcher *matcher = NULL;
	//
	alloc_local = (struct local_match *)malloc(sizeof(struct local_match));
	if(!alloc_local) return NULL;
	memset(alloc_local, '\0', sizeof(struct local_match));
	//
	matcher = (struct file_matcher *)malloc(sizeof(struct file_matcher));
	if(!matcher) {
		free(alloc_local);
		return NULL;
	}
	memset(matcher, '\0', sizeof(struct file_matcher));
	//
	matcher->match_type = CONEN_VARI;
	alloc_local->matcher = matcher;
	local_vector->push_back(alloc_local);
	//
	return matcher;
}

static struct local_match *AddModifyData(vector<struct local_match *> *local_vector, int length) {
	struct local_match *alloc_local = NULL;
	struct file_matcher *matcher = NULL;
	//
	alloc_local = (struct local_match *)malloc(sizeof(struct local_match));
	if(!alloc_local) return NULL;
	memset(alloc_local, '\0', sizeof(struct local_match));
	//
	matcher = (struct file_matcher *)malloc(sizeof(struct file_matcher));
	if(!matcher) {
		free(alloc_local);
		return NULL;
	}
	memset(matcher, '\0', sizeof(struct file_matcher));
	//
	unsigned char *buffer = (unsigned char *)malloc(length);
	if(!buffer) {
		free(alloc_local);
		free(matcher);
		return NULL;
	}
	memset(buffer, '\0', length);
	//
	matcher->match_type = CONEN_MODI;
	alloc_local->matcher = matcher;
	alloc_local->buffer = buffer;
	local_vector->push_back(alloc_local);
	//
	return alloc_local;
}

static VOID DeleMatchVec(vector<struct local_match *> *local_vector) {
	vector <struct local_match *>::iterator iter;
	//
	for(iter = local_vector->begin(); local_vector->end() != iter; ++iter) {
		if(*iter) {
			if((*iter)->matcher)
				free((*iter)->matcher);
			if((*iter)->buffer) free((*iter)->buffer);
			free(*iter);
		}
	}
	local_vector->clear();
}

static struct local_match *AppendLocalMatch(vector<struct local_match *> &vlocal_match, struct file_matcher *matcher) {
	struct file_matcher *new_mcher = (struct file_matcher *)malloc(sizeof(struct file_matcher));
	if(!new_mcher) return NULL;
	memcpy(new_mcher, matcher, sizeof(struct file_matcher));
	//
	struct local_match *alloc_match = (struct local_match *)malloc(sizeof(struct local_match));
	if(!alloc_match) {
		free(new_mcher);
		return NULL;
	}
	vlocal_match.push_back(alloc_match);
	//
	alloc_match->matcher = new_mcher;
	alloc_match->buffer = NULL;
	//
	return alloc_match;
}


//
static int FileTailMatch(vector<struct local_match *> &vlocal_match, hash_table *htab, struct tiny_map64 *buffer, unsigned __int64 file_oset, unsigned int malen, int last_toke, int cure_toke) {
	struct local_match *alloc_local;
	struct file_matcher *matcher;
	//
	unsigned int tail_size = POW2N_MOD(malen, BLOCK_SIZE);
	// _LOG_DEBUG(_T("tail match tail_size:%d"), tail_size);
	if(tail_size) {
		int end_toke = malen - tail_size;
		sea_env senv;
		DWORD found_block = 0x01;
		int find_inde = -1;
		while (end_toke >= cure_toke) {
			if (found_block) {
				senv.mode = INITI_ADLER_CHECK; // bFine :>=0 found <0  not found
				find_inde = NBdHTab::HashSearch(htab, (BYTE *)map_ptr64(buffer, file_oset + cure_toke, tail_size), &senv, tail_size);
			} else {
				senv.mode = ROLLING_ADLER_CHECK; // bFine :>=0 found <0  not found
				find_inde = NBdHTab::HashSearch(htab, (BYTE *)map_ptr64(buffer, file_oset + cure_toke, tail_size), &senv, tail_size);
			}
			if (0x00 <= find_inde) { // found the same block.
				// _LOG_DEBUG(_T("match one block.")); // disable by james 20130410
				if ((cure_toke - last_toke)) {
					unsigned int molen = (unsigned int)(cure_toke - last_toke);
					alloc_local = AddModifyData(&vlocal_match, molen);
					if (!alloc_local) return -1;
					//
					// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
					alloc_local->matcher->offset = last_toke;
					alloc_local->matcher->inde_len = molen;
					memcpy(alloc_local->buffer, map_ptr64(buffer, file_oset + last_toke, molen), molen);
				}
				//
				matcher = AddVariaMatch(&vlocal_match);
				if (!matcher) return -1;
				//
				// _LOG_DEBUG(_T("matcher ose:%u inde:%u"), cure_toke, (unsigned int)(find_inde / BLOCK_SIZE));
				matcher->offset = cure_toke;
				matcher->inde_len = (unsigned int)(find_inde / BLOCK_SIZE) << 16 | tail_size;
				//
				found_block = 0x01;
				cure_toke += tail_size;
				last_toke = cure_toke;
			} else { // not found the same block.
				if(end_toke == cure_toke) {
					unsigned int molen = (unsigned int)(cure_toke - last_toke) + tail_size;
					alloc_local = AddModifyData(&vlocal_match, molen);
					if (!alloc_local) return -1;
					//
					// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
					alloc_local->matcher->offset = last_toke;
					alloc_local->matcher->inde_len = molen;
					memcpy(alloc_local->buffer, map_ptr64(buffer, file_oset + last_toke, molen), molen);
					//
					cure_toke++;
					last_toke = cure_toke;
				} else {// _LOG_DEBUG(_T("not found the same block."));
					found_block = 0x00;
					cure_toke++;
				}
			} //
		}
	} else { //
		unsigned int molen = (unsigned int)(cure_toke - last_toke) + BLOCK_SIZE;
		alloc_local = AddModifyData(&vlocal_match, molen);
		if (!alloc_local) return -1;
		//
		// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
		alloc_local->matcher->offset = last_toke;
		alloc_local->matcher->inde_len = molen;
		memcpy(alloc_local->buffer, map_ptr64(buffer, file_oset + last_toke, molen), molen);
	}
	// BinaryLogger::LogBinaryCode(alloc_local->buffer, molen);
	return 0;
}

static int FileChksMatcher(vector<struct local_match *> &vlocal_match, hash_table *htab, struct tiny_map64 *buffer, unsigned __int64 qwFileLength, unsigned __int64 file_oset) {
	struct local_match *alloc_local;
	struct file_matcher *matcher;
	int last_toke, cure_toke, end_toke;
	end_toke = int(qwFileLength - file_oset);
	last_toke = cure_toke = 0x00;
	// _LOG_DEBUG(_T("file_oset:%llu end_toke:%u"), file_oset, end_toke);
	sea_env senv;
	DWORD found_block = 0x01;
	int find_inde = -1;
	while ((end_toke - BLOCK_SIZE) >= cure_toke) {
		// _LOG_DEBUG(_T("file_oset + cure_toke:%llu"), file_oset + cure_toke);
		if (found_block) {
			senv.mode = INITI_ADLER_CHECK; // bFine :>=0 found <0  not found
			find_inde = NBdHTab::HashSearch(htab, (BYTE *)map_ptr64(buffer, file_oset + cure_toke, BLOCK_SIZE), &senv);
		} else {
			senv.mode = ROLLING_ADLER_CHECK; // bFine :>=0 found <0  not found
			find_inde = NBdHTab::HashSearch(htab, (BYTE *)map_ptr64(buffer, file_oset + cure_toke, BLOCK_SIZE), &senv);
		}
		// if (0x00 <= find_inde)_LOG_DEBUG(_T("offset:%llu cure_toke:%u"), file_oset + cure_toke, cure_toke);
		if (0x00 <= find_inde) { // found the same block.
			// _LOG_DEBUG(_T("match one block.")); // disable by james 20130410
			if ((cure_toke - last_toke)) {
				unsigned int molen = (unsigned int)(cure_toke - last_toke);
				alloc_local = AddModifyData(&vlocal_match, molen);
				if (!alloc_local) return -1;
				//
				// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
				alloc_local->matcher->offset = last_toke;
				alloc_local->matcher->inde_len = molen;
				memcpy(alloc_local->buffer, map_ptr64(buffer, file_oset + last_toke, molen), molen);
			}
			//
			matcher = AddChksMatch(&vlocal_match);
			if (!matcher) return -1;
			// _LOG_DEBUG(_T("matcher ose:%u inde:%u"), cure_toke, (unsigned int)(find_inde / BLOCK_SIZE));
			matcher->offset = cure_toke;
			matcher->inde_len = (unsigned int)(find_inde / BLOCK_SIZE);
			//
			found_block = 0x01;
			cure_toke += BLOCK_SIZE;
			last_toke = cure_toke;
		} else { // not found the same block.
			if(BLOCK_SIZE == (cure_toke - last_toke)) {
				// _LOG_DEBUG(_T("not found the same block."));
				alloc_local = AddModifyData(&vlocal_match, BLOCK_SIZE);
				if (!alloc_local) return -1;
				alloc_local->matcher->offset = last_toke;
				alloc_local->matcher->inde_len = BLOCK_SIZE;
				memcpy(alloc_local->buffer, map_ptr64(buffer, file_oset + last_toke, BLOCK_SIZE), BLOCK_SIZE);
				//
				cure_toke++;
				last_toke = cure_toke;
			} else {// _LOG_DEBUG(_T("not found the same block."));
				found_block = 0x00;
				cure_toke++;
			}
		}
	}
	// match the tail
	// _LOG_DEBUG(_T("the last not match data."));
	if(FileTailMatch(vlocal_match, htab, buffer, file_oset, end_toke, last_toke, cure_toke))
		return -1;
	// _LOG_DEBUG(_T("end_toke:%u cure_toke:%u"), end_toke, cure_toke);
	//
	// _LOG_DEBUG(_T("local cure_toke:%u"), cure_toke);
	return 0;
}

static DWORD MatchFileChks(vector<struct local_match *> &vlocal_match, hash_table *htab, TCHAR *szFileLocal, unsigned __int64 file_oset) {
	// _LOG_DEBUG(_T("match file sums. new file is:%s"), szFileLocal); // disable by james 20130410
	HANDLE hFileLocal = CreateFile( szFileLocal,
		GENERIC_READ,
		FILE_SHARE_READ, // | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileLocal ) {
		// _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return MATCH_NOT_FOUND;
		} else return MATCH_VIOLATION;
	}
	//
	DWORD dwFileSizeHigh;
	unsigned __int64 qwFileLength = GetFileSize(hFileLocal, &dwFileSizeHigh);
	if(INVALID_FILE_SIZE == qwFileLength) {
		CloseHandle( hFileLocal );
		return MATCH_FAULT;
	}
	qwFileLength += (((unsigned __int64)dwFileSizeHigh) << 32);
	//
	struct tiny_map64 *buffer = (struct tiny_map64 *)map_file64(hFileLocal, qwFileLength, MAX_MAP_SIZE, BLOCK_SIZE);
	if(!buffer) {
		CloseHandle( hFileLocal );
		return MATCH_FAULT;
	}
	// _LOG_DEBUG(_T("buffer:%08x"), buffer);
	if(FileChksMatcher(vlocal_match, htab, buffer, qwFileLength, file_oset))
		return MATCH_FAULT;
	// _LOG_DEBUG(_T("end match file sums.")); // disable by james 20130410
	// get_md5((char *) buffer, FileSize, FileChksMD5);
	unmap_file64(buffer);
	CloseHandle( hFileLocal );
	//
	return MATCH_SUCCESS;
}

//
static int ChunkChksMatcher(vector<struct local_match *> &vlocal_match, hash_table *htab, struct tiny_map64 *buffer, unsigned __int64 chunk_oset) {
	struct local_match *alloc_local;
	struct file_matcher *matcher;
	unsigned int last_toke, cure_toke, end_toke;
	end_toke = CHUNK_SIZE - BLOCK_SIZE;
	last_toke = cure_toke = 0x00;
	//
	// _LOG_DEBUG(_T("chunk_oset:%llu"), chunk_oset);
	sea_env senv;
	senv.mode = INITI_ADLER_CHECK;
	// DWORD found_block = 0x01;
	int find_inde = -1;
	while (end_toke >= cure_toke) {
		if (INITI_ADLER_CHECK == senv.mode) {
			// bFine :>=0 found <0  not found
			find_inde = NBdHTab::HashSearch(htab, (BYTE *)map_ptr64(buffer, chunk_oset + cure_toke, BLOCK_SIZE), &senv);
		} else if (ROLLING_ADLER_CHECK == senv.mode) {
			// bFine :>=0 found <0  not found
			find_inde = NBdHTab::HashSearch(htab, (BYTE *)map_ptr64(buffer, chunk_oset + cure_toke, BLOCK_SIZE), &senv);
		}
		//        if (0x00 <= find_inde)_LOG_DEBUG(_T("offset:%llu"), chunk_oset + cure_toke);
		// _LOG_DEBUG(_T("found_block:%u find_inde:%d last_toke:%u cure_toke:%u"), found_block, find_inde, last_tok, cure_toke);
		if (0x00 <= find_inde) { // found the same block.
			// _LOG_DEBUG(_T("found the same block."));
			if ((cure_toke - last_toke)) {
				unsigned int molen = (unsigned int)(cure_toke - last_toke);
				alloc_local = AddModifyData(&vlocal_match, molen);
				if (!alloc_local) return -1;
				//
				// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
				alloc_local->matcher->offset = last_toke;
				alloc_local->matcher->inde_len = molen;
				memcpy(alloc_local->buffer, map_ptr64(buffer, chunk_oset + last_toke, molen), molen);
			}
			//
			matcher = AddChksMatch(&vlocal_match);
			if (!matcher) return -1;
			// _LOG_DEBUG(_T("matcher ose:%u inde:%u"), cure_toke, (unsigned int)(find_inde / BLOCK_SIZE));
			matcher->offset = cure_toke;
			matcher->inde_len = (unsigned int)(find_inde / BLOCK_SIZE);
			//
			cure_toke += BLOCK_SIZE;
			last_toke = cure_toke;
			senv.mode = INITI_ADLER_CHECK;
		} else { // not found the same block.
			if(BLOCK_SIZE == (cure_toke - last_toke)) {
				alloc_local = AddModifyData(&vlocal_match, BLOCK_SIZE);
				if (!alloc_local) return -1;
				alloc_local->matcher->offset = last_toke;
				alloc_local->matcher->inde_len = BLOCK_SIZE;
				memcpy(alloc_local->buffer, map_ptr64(buffer, chunk_oset + last_toke, BLOCK_SIZE), BLOCK_SIZE);
				//
				cure_toke++;
				last_toke = cure_toke;
			} else {// _LOG_DEBUG(_T("not found the same block."));
				cure_toke++;
				senv.mode = ROLLING_ADLER_CHECK;
			}
		}
wprintf(_T("end_toke:%08X cure_toke:%08X\n"), end_toke, cure_toke);
	}
	//
	if(CHUNK_SIZE > cure_toke) {
		unsigned int molen = CHUNK_SIZE - last_toke;
		alloc_local = AddModifyData(&vlocal_match, molen);
		if (!alloc_local) return -1;
wprintf(_T("the last not match data.\n"));
			// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
		alloc_local->matcher->offset = last_toke;
		alloc_local->matcher->inde_len = molen;
		memcpy(alloc_local->buffer, map_ptr64(buffer, chunk_oset + last_toke, molen), molen);
	}
	//
wprintf(_T("chunk chks match.\n"));
	return 0;
}

static DWORD MatchChunkChks(vector<struct local_match *> &vlocal_match, hash_table *htab, TCHAR *szFileLocal, unsigned __int64 chunk_oset) {
	// _LOG_DEBUG(_T("match chunk sums.")); // disable by james 20130410
	HANDLE hFileLocal = CreateFile( szFileLocal,
		GENERIC_READ,
		FILE_SHARE_READ, // | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileLocal ) {
		// _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return MATCH_NOT_FOUND;
		} else return MATCH_VIOLATION;
	}
	DWORD dwFileSizeHigh;
	unsigned __int64 qwFileLength = GetFileSize(hFileLocal, &dwFileSizeHigh);
	if(INVALID_FILE_SIZE == qwFileLength) {
		CloseHandle( hFileLocal );
		return MATCH_FAULT;
	}
	qwFileLength += (((unsigned __int64)dwFileSizeHigh) << 32);
	if(chunk_oset + CHUNK_SIZE > qwFileLength)
		return MATCH_FAULT;
	//
	struct tiny_map64 *buffer = (struct tiny_map64 *)map_file64(hFileLocal, qwFileLength, MAX_MAP_SIZE, BLOCK_SIZE);
	if(!buffer) {
		CloseHandle( hFileLocal );
		return MATCH_FAULT;
	}
	//
	if(ChunkChksMatcher(vlocal_match, htab, buffer, chunk_oset))
		return MATCH_FAULT;
	// get_md5((char *) buffer, FileSize, FileChksMD5);
	unmap_file64(buffer);
	CloseHandle( hFileLocal );
	//
	return MATCH_SUCCESS;
}


//
static DWORD FlushMatchFile(HANDLE hFileFlush, vector<struct local_match *> *local_vector) {
	vector<struct local_match *>::iterator iter;
	DWORD dwWritenSize;
	//
	DWORD dwResult = SetFilePointer(hFileFlush, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
		return ((DWORD)-1);
	dwResult = SetEndOfFile(hFileFlush);
	if(FALSE==dwResult && NO_ERROR!=GetLastError()) {
		wprintf(_T("set end of file false, last error is:%d"), GetLastError());
		return ((DWORD)-1);
	}
	//
	// _LOG_DEBUG(_T("---------------------------------------------"));
	wprintf(_T("------ POST ------\n"));
	DWORD inde = 0x00;
	struct file_matcher *matcher;
	for(iter=local_vector->begin(); local_vector->end()!=iter; ++iter) {
		matcher = (*iter)->matcher;
		if(!WriteFile(hFileFlush, matcher, sizeof (struct file_matcher), &dwWritenSize, NULL))
			return ((DWORD)-1);
		if (CONEN_MODI & matcher->match_type) {
			if(!WriteFile(hFileFlush, (*iter)->buffer, matcher->inde_len, &dwWritenSize, NULL))
				return ((DWORD)-1);
			// BinaryLogger::LogBinaryCode((*iter)->buffer, matcher->inde_len);
		}
		wprintf(_T("inde:%d type:%d offset:%d inde_len:%d\n"), inde++, matcher->match_type, matcher->offset, matcher->inde_len);
	}
	//
	wprintf(_T("flush match to file.\n"));
	FlushFileBuffers(hFileFlush);
	//
	return 0;
}

//
DWORD MUtil::BlockMatchFile(HANDLE hFileFlush, struct complex_head *phead_complex, unsigned __int64 offset, TCHAR *szFileLocal) {
	vector<struct local_match *> vlocal_match;
	hash_table *htab = NULL;
	// _LOG_DEBUG(_T("match block map local dat file section.")); // disable by james 20130410
	if( !phead_complex ) {
		// _LOG_WARN( _T("create file failed.") );
		return MATCH_FAULT;
	}
	//
	// _LOG_DEBUG(_T("match block create build data section.")); // disable by james 20130410
	if(!(htab = CreateHTable(MATCH_BACKET_SIZE))) {
		// _LOG_WARN( _T("create hash table failed.") );
		return MATCH_FAULT;
	}
	if (NBdHTab::BuildHTable(htab, phead_complex->chks, phead_complex->tatol)) {
		NBdHTab::DeleBHTab(htab);
		return MATCH_FAULT;
	}
	// _LOG_DEBUG(_T("match sums chunk.")); // disable by james 20130410
	DWORD matchValue = MatchFileChks(vlocal_match, htab, szFileLocal, offset);
	if (matchValue) {
		DeleMatchVec(&vlocal_match);
		NBdHTab::DeleBHTab(htab);
		return matchValue;
	}
	//
	// _LOG_DEBUG(_T("match block create data file section.")); // disable by james 20130410
	// if (htab) NBdHTab::DeleBHTab(htab);
	//
	DWORD flush_value = MATCH_SUCCESS;
	if(FlushMatchFile(hFileFlush, &vlocal_match)) flush_value = MATCH_FAULT;
	DeleMatchVec(&vlocal_match);
	//
	return flush_value;
}

DWORD MUtil::BlockMatchChunk(HANDLE hFileFlush, struct complex_head *phead_complex, unsigned __int64 offset, TCHAR *szFileLocal) {
	vector<struct local_match *> vlocal_match;
	hash_table *htab = NULL;
	// _LOG_DEBUG(_T("match block map local dat file section.")); // disable by james 20130410
	if( !phead_complex ) {
		// _LOG_WARN( _T("create file failed.") );
		return MATCH_FAULT;
	}
	//
	// _LOG_DEBUG(_T("match block create build data section.")); // disable by james 20130410
	if(!(htab = CreateHTable(MATCH_BACKET_SIZE))) {
		// _LOG_WARN( _T("create hash table failed.") );
		return MATCH_FAULT;
	}
	if (NBdHTab::BuildHTable(htab, phead_complex->chks, phead_complex->tatol)) {
		NBdHTab::DeleBHTab(htab);
		return MATCH_FAULT;
	}
	// _LOG_DEBUG(_T("match sums chunk.")); // disable by james 20130410
	DWORD matchValue = MatchChunkChks(vlocal_match, htab, szFileLocal, offset);
	if (matchValue) {
		DeleMatchVec(&vlocal_match);
		NBdHTab::DeleBHTab(htab);
		return matchValue;
	}
	//
	// _LOG_DEBUG(_T("match block create data file section.")); // disable by james 20130410
	// if (htab) NBdHTab::DeleBHTab(htab);
	//
	DWORD flush_value = MATCH_SUCCESS;
	if(FlushMatchFile(hFileFlush, &vlocal_match)) flush_value = MATCH_FAULT;
	DeleMatchVec(&vlocal_match);
	//
	return flush_value;
}