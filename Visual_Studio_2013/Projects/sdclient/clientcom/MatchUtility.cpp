#include "stdafx.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default

#include "third_party.h"
#include "StringUtility.h"
#include "FileUtility.h"
#include "BuildHTable.h"
#include "Checksum.h"

#include "MatchUtility.h"

//
static int FileChksMatcher(vector<struct local_match *> &vlocal_match, hash_table *htab, struct tiny_map64 *buffer, unsigned __int64 qwFileLength, unsigned __int64 foset) {
    struct local_match *alloc_local;
    struct file_matcher *matcher;
    int last_toke, cure_toke, end_toke;
    end_toke = int(qwFileLength-foset) - BLOCK_SIZE;
	last_toke = cure_toke = 0x00;
// _LOG_DEBUG(_T("foset:%llu end_toke:%u"), foset, end_toke);
	sea_env senv;
	senv.mode = INITI_ADLER_CHECK;
	int find_inde = -1;
	while (end_toke >= cure_toke) {
// _LOG_DEBUG(_T("foset + cure_toke:%llu"), foset + cure_toke);
		// bFine :>=0 found <0  not found
		find_inde = NBdHTab::HashSearch(htab, (BYTE *)map_ptr64(buffer, foset + cure_toke, BLOCK_SIZE), &senv);
// if (0x00 <= find_inde) _LOG_DEBUG(_T("offset:%llu cure_toke:%u"), foset + cure_toke, cure_toke);
		if (0x00 <= find_inde) { // found the same block.
// _LOG_DEBUG(_T("match one block.")); // disable by james 20130410
			if ((cure_toke - last_toke)) {
				unsigned int molen = (unsigned int)(cure_toke - last_toke);
				alloc_local = MUtil::AddModifyData(&vlocal_match, molen);
				if (!alloc_local) return -1;
				//
// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
				alloc_local->matcher->offset = last_toke;
				alloc_local->matcher->inde_len = molen;
				memcpy(alloc_local->buffer, map_ptr64(buffer, foset + last_toke, molen), molen);
			}
			//
			matcher = MUtil::AddChksMatch(&vlocal_match);
			if (!matcher) return -1;
// _LOG_DEBUG(_T("matcher ose:%u inde:%u"), cure_toke, (unsigned int)(find_inde / BLOCK_SIZE));
			matcher->offset = cure_toke;
			matcher->inde_len = (unsigned int)(find_inde / BLOCK_SIZE);
			//
			cure_toke += BLOCK_SIZE;
			last_toke = cure_toke;
			senv.mode = INITI_ADLER_CHECK;
		} else { // not found the same block.
			if((BLOCK_SIZE-0x01) == (cure_toke - last_toke)) {
				_LOG_DEBUG(_T("not found the same block."));
				alloc_local = MUtil::AddModifyData(&vlocal_match, BLOCK_SIZE);
				if (!alloc_local) return -1;
				alloc_local->matcher->offset = last_toke;
				alloc_local->matcher->inde_len = BLOCK_SIZE;
				memcpy(alloc_local->buffer, map_ptr64(buffer, foset + last_toke, BLOCK_SIZE), BLOCK_SIZE);
				//
				cure_toke++;
				last_toke = cure_toke;
			} else {
// _LOG_DEBUG(_T("not found the same block."));
				cure_toke++;
				senv.mode = ROLLING_ADLER_CHECK;
			}
		}
	} // while
	// match the tail
	if((end_toke+BLOCK_SIZE) > cure_toke) {
// _LOG_DEBUG(_T("the last not match data."));
        unsigned int molen = (end_toke+BLOCK_SIZE) - last_toke;
        alloc_local = MUtil::AddModifyData(&vlocal_match, molen);
        if (!alloc_local) return -1;
// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
        alloc_local->matcher->offset = last_toke;
        alloc_local->matcher->inde_len = molen;
        memcpy(alloc_local->buffer, map_ptr64(buffer, foset + last_toke, molen), molen);
	}
// _LOG_DEBUG(_T("end_toke:%u cure_toke:%u"), end_toke, cure_toke);
	//
// _LOG_DEBUG(_T("local cure_toke:%u"), cure_toke);
    return 0;
}

DWORD MUtil::MatchFileChks(vector<struct local_match *> &vlocal_match, hash_table *htab, TCHAR *szFileLocal, unsigned __int64 foset) {
// _LOG_DEBUG(_T("match file sums. new file is:%s"), szFileLocal); // disable by james 20130410
    HANDLE hFileLocal = CreateFile( szFileLocal,
                                    GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileLocal ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return MATCH_NOT_FOUND;
		} else { return MATCH_VIOLATION; }
    }
    //
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileLength = GetFileSize(hFileLocal, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileLength && NO_ERROR!=GetLastError()) {
        CloseHandle( hFileLocal );
        return MATCH_FAULT;
    }
    qwFileLength += (((unsigned __int64)dwFileSizeHigh) << 32);
	if(!qwFileLength || (foset>=qwFileLength)) {
        CloseHandle( hFileLocal );	
		return MATCH_SUCCESS;
	}
    //
    struct tiny_map64 *buffer = (struct tiny_map64 *)map_file64(hFileLocal, qwFileLength, MAX_MAP_SIZE, BLOCK_SIZE);
    if(!buffer) {
        CloseHandle( hFileLocal );
        return MATCH_FAULT;
    }
// _LOG_DEBUG(_T("buffer:%08x"), buffer);
	if(FileChksMatcher(vlocal_match, htab, buffer, qwFileLength, foset)) {
        CloseHandle( hFileLocal );
		return MATCH_FAULT;
	}
// _LOG_DEBUG(_T("end match file sums.")); // disable by james 20130410
    // get_md5((char *) buffer, FileSize, FileChksMD5);
    unmap_file64(buffer);
    CloseHandle( hFileLocal );
    return MATCH_SUCCESS;
}

//
static int ChunkChksMatcher(vector<struct local_match *> &vlocal_match, hash_table *htab, struct tiny_map64 *buffer, unsigned __int64 coset) {
    struct local_match *alloc_local;
    struct file_matcher *matcher;
    unsigned int last_toke, cure_toke, end_toke;
    end_toke = CHUNK_SIZE - BLOCK_SIZE;
    last_toke = cure_toke = 0x00;
	//
// _LOG_DEBUG(_T("coset:%llu"), coset);
    sea_env senv;
	senv.mode = INITI_ADLER_CHECK;
    int find_inde = -1;
    while (end_toke >= cure_toke) {
		// bFine :>=0 found <0  not found
        find_inde = NBdHTab::HashSearch(htab, (BYTE *)map_ptr64(buffer, coset + cure_toke, BLOCK_SIZE), &senv);
// if (0x00 <= find_inde) _LOG_DEBUG(_T("offset:%llu"), coset + cure_toke);
// _LOG_DEBUG(_T("found_block:%u find_inde:%d last_toke:%u cure_toke:%u"), found_block, find_inde, last_tok, cure_toke);
        if (0x00 <= find_inde) { // found the same block.
// _LOG_DEBUG(_T("found the same block."));
            if ((cure_toke - last_toke)) {
                unsigned int molen = (unsigned int)(cure_toke - last_toke);
                alloc_local = MUtil::AddModifyData(&vlocal_match, molen);
                if (!alloc_local) return -1;
                //
// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
                alloc_local->matcher->offset = last_toke;
                alloc_local->matcher->inde_len = molen;
                memcpy(alloc_local->buffer, map_ptr64(buffer, coset + last_toke, molen), molen);
            }
            //
            matcher = MUtil::AddChksMatch(&vlocal_match);
            if (!matcher) return -1;
// _LOG_DEBUG(_T("matcher ose:%u inde:%u"), cure_toke, (unsigned int)(find_inde / BLOCK_SIZE));
            matcher->offset = cure_toke;
            matcher->inde_len = (unsigned int)(find_inde / BLOCK_SIZE);
            //
            cure_toke += BLOCK_SIZE;
            last_toke = cure_toke;
			senv.mode = INITI_ADLER_CHECK;
        } else { // not found the same block.
            if((BLOCK_SIZE-0x01) == (cure_toke - last_toke)) {
                alloc_local = MUtil::AddModifyData(&vlocal_match, BLOCK_SIZE);
                if (!alloc_local) return -1;
                alloc_local->matcher->offset = last_toke;
                alloc_local->matcher->inde_len = BLOCK_SIZE;
                memcpy(alloc_local->buffer, map_ptr64(buffer, coset + last_toke, BLOCK_SIZE), BLOCK_SIZE);
                //
                cure_toke++;
                last_toke = cure_toke;
            } else { // _LOG_DEBUG(_T("not found the same block."));
                cure_toke++;
				senv.mode = ROLLING_ADLER_CHECK;
            }
        }
        //
    }
	//
	if(CHUNK_SIZE > cure_toke) {
// _LOG_DEBUG(_T("the last not match data.\n"));
		unsigned int molen = CHUNK_SIZE - last_toke;
		alloc_local = MUtil::AddModifyData(&vlocal_match, molen);
		if (!alloc_local) return -1;
// _LOG_DEBUG(_T("local ose:%u molen:%d"), last_toke, molen);
		alloc_local->matcher->offset = last_toke;
		alloc_local->matcher->inde_len = molen;
		memcpy(alloc_local->buffer, map_ptr64(buffer, coset + last_toke, molen), molen);
	}
    //
    return 0;
}

DWORD MUtil::MatchChunkChks(vector<struct local_match *> &vlocal_match, hash_table *htab, TCHAR *szFileLocal, unsigned __int64 coset) {
// _LOG_DEBUG(_T("match chunk sums.")); // disable by james 20130410
    HANDLE hFileLocal = CreateFile( szFileLocal,
                                    GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileLocal ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return MATCH_NOT_FOUND;
		} else { return MATCH_VIOLATION; }
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileLength = GetFileSize(hFileLocal, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileLength && NO_ERROR!=GetLastError()) {
        CloseHandle( hFileLocal );
        return MATCH_FAULT;
    }
    qwFileLength += (((unsigned __int64)dwFileSizeHigh) << 32);
	if((coset + CHUNK_SIZE) > qwFileLength) {
	    CloseHandle( hFileLocal );
        return MATCH_FAULT;	
	}
    //
    struct tiny_map64 *buffer = (struct tiny_map64 *)map_file64(hFileLocal, qwFileLength, MAX_MAP_SIZE, BLOCK_SIZE);
    if(!buffer) {
        CloseHandle( hFileLocal );
        return MATCH_FAULT;
    }
    //
	if(ChunkChksMatcher(vlocal_match, htab, buffer, coset)) {
	    CloseHandle( hFileLocal );
		return MATCH_FAULT;
	}
    // get_md5((char *) buffer, FileSize, FileChksMD5);
    unmap_file64(buffer);
    CloseHandle( hFileLocal );
    return MATCH_SUCCESS;
}

DWORD MUtil::BlockMatchFile(struct SeionCtrl *sctrl, struct InteValue *ival, TCHAR *szFileLocal) {
    vector<struct local_match *> vlocal_match;
    hash_table *htab = NULL;
// _LOG_DEBUG(_T("match block map local dat file section.")); // disable by james 20130410
    if( INVALID_HANDLE_VALUE == sctrl->file_handle ) {
        _LOG_WARN( _T("create file failed.") );
        return MATCH_FAULT;
    }
    //
// _LOG_DEBUG(_T("match block create build data section.")); // disable by james 20130410
    if(!(htab = CreateHTable(MATCH_BACKET_SIZE))) {
        _LOG_WARN( _T("create hash table failed.") );
        return MATCH_FAULT;
    }
    if (NBdHTab::BuildHTable(htab, sctrl->file_handle, ival->content_length, 0x01)) {
        NBdHTab::DeleBHTab(htab);
        return MATCH_FAULT;
    }
// _LOG_DEBUG(_T("match sums chunk.")); // disable by james 20130410
	DWORD matchValue = MatchFileChks(vlocal_match, htab, szFileLocal, ival->offset);
    if (matchValue) {
        DeleMatchVec(&vlocal_match);
        NBdHTab::DeleBHTab(htab);
        return matchValue;
    }
    //
// _LOG_DEBUG(_T("match block create data file section.")); // disable by james 20130410
    if (htab) NBdHTab::DeleBHTab(htab);
    //
    DWORD flush_value = MATCH_SUCCESS;
    if(FlushMatchFile(sctrl->file_handle, &vlocal_match, 0x01)) flush_value = MATCH_FAULT;
    DeleMatchVec(&vlocal_match);
    //
// _LOG_DEBUG(_T("flush_value:%d"), flush_value);
    return flush_value;
}

DWORD MUtil::BlockMatchChunk(struct SeionCtrl *sctrl, struct InteValue *ival, TCHAR *szFileLocal) {
    vector<struct local_match *> vlocal_match;
    hash_table *htab = NULL;
// _LOG_DEBUG(_T("match block map local dat file section.")); // disable by james 20130410
    if( INVALID_HANDLE_VALUE == sctrl->file_handle ) {
        _LOG_WARN( _T("create file failed.") );
        return MATCH_FAULT;
    }
    //
// _LOG_DEBUG(_T("match block create build data section.")); // disable by james 20130410
    if(!(htab = CreateHTable(MATCH_BACKET_SIZE))) {
        _LOG_WARN( _T("create hash table failed.") );
        return MATCH_FAULT;
    }
    if (NBdHTab::BuildHTable(htab, sctrl->file_handle, ival->content_length, 0x00)) {
        NBdHTab::DeleBHTab(htab);
        return MATCH_FAULT;
    }
// _LOG_DEBUG(_T("match sums chunk.")); // disable by james 20130410
	DWORD matchValue = MatchChunkChks(vlocal_match, htab, szFileLocal, ival->offset);
    if (matchValue) {
        DeleMatchVec(&vlocal_match);
        NBdHTab::DeleBHTab(htab);
        return matchValue;
    }
    //
// _LOG_DEBUG(_T("match block create data file section.")); // disable by james 20130410
    if (htab) NBdHTab::DeleBHTab(htab);
    //
    DWORD flush_value = MATCH_SUCCESS;
    if(FlushMatchFile(sctrl->file_handle, &vlocal_match, 0x00)) flush_value = MATCH_FAULT;
    DeleMatchVec(&vlocal_match);
    //
    return flush_value;
}

VOID MUtil::CloseBlockMatch(struct SeionCtrl *sctrl) {
    CloseHandle(sctrl->file_handle);
    sctrl->file_handle = INVALID_HANDLE_VALUE;
}

//
DWORD MUtil::FlushMatchFile(HANDLE hFileFlush, vector<struct local_match *> *local_vector, int file_chunk) {
    vector<struct local_match *>::iterator iter;
    DWORD dwWritenSize;
    //
    DWORD dwResult = SetFilePointer(hFileFlush, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    dwResult = SetEndOfFile(hFileFlush);
    if(FALSE==dwResult && NO_ERROR!=GetLastError()) {
        _LOG_WARN(_T("set end of file false, last error is:%d"), GetLastError());
        return ((DWORD)-1);
    }
    //
// _LOG_DEBUG(_T("------ POST ------ file_chunk:%d"), file_chunk);
// DWORD inde = 0x00; // for test
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
// _LOG_DEBUG(_T("fchk:%d inde:%d type:%d offset:%d inde_len:%d"), file_chunk, inde++, matcher->match_type, matcher->offset, matcher->inde_len);
    }
    //
_LOG_DEBUG(_T("flush match to file."));
    FlushFileBuffers(hFileFlush);
    //
    return 0;
}

struct file_matcher *MUtil::AddChksMatch(vector<struct local_match *> *local_vector) {
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

struct local_match *MUtil::AddModifyData(vector<struct local_match *> *local_vector, int length) {
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

VOID MUtil::DeleMatchVec(vector<struct local_match *> *local_vector) {
    vector <struct local_match *>::iterator iter;
    //
DWORD inde = 0x00; // for test
    for(iter = local_vector->begin(); local_vector->end() != iter; ++iter) {
        if(*iter) {
// _LOG_DEBUG(_T("fchk:%d inde:%d type:%d offset:%d inde_len:%d"), fchunk, inde++, (*iter)->matcher->match_type, (*iter)->matcher->offset, (*iter)->matcher->inde_len);
            if((*iter)->matcher)
                free((*iter)->matcher);
            if((*iter)->buffer) free((*iter)->buffer);
            free(*iter);
        }
    }
    local_vector->clear();
}

struct local_match *MUtil::AppendLocalMatch(vector<struct local_match *> &vlocal_match, struct file_matcher *matcher) {
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
DWORD MUtil::FlushZeroFile(HANDLE hFileZero, struct tiny_map64 *buffer, unsigned __int64 offset, unsigned int posit, unsigned int molen) {
    struct file_matcher matcher;
    matcher.match_type = CONEN_MODI;
    matcher.offset = posit;
    matcher.inde_len = molen;
    //
    DWORD dwWritenSize;
    if(!WriteFile(hFileZero, &matcher, sizeof (struct file_matcher), &dwWritenSize, NULL))
        return ((DWORD)-1);
    if (CONEN_MODI & matcher.match_type) {
        if(!WriteFile(hFileZero, map_ptr64(buffer, offset + posit, molen), molen, &dwWritenSize, NULL))
            return ((DWORD)-1);
        // BinaryLogger::LogBinaryCode((*iter)->buffer, matcher->inde_len);
    }
    return 0x00;
}

DWORD MUtil::MatchZeroFile(struct SeionCtrl *sctrl, struct InteValue *ival, TCHAR *szFileLocal) {
    DWORD dwResult = SetFilePointer(sctrl->file_handle, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return MATCH_FAULT;
    dwResult = SetEndOfFile(sctrl->file_handle);
    if(FALSE==dwResult && NO_ERROR!=GetLastError()) {
        _LOG_WARN(_T("set end of file false, last error is:%d"), GetLastError());
        return MATCH_FAULT;
    }
//
    HANDLE hFileLocal = CreateFile( szFileLocal,
                                    GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileLocal ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return MATCH_NOT_FOUND;
		} else { return MATCH_VIOLATION; }
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileLength = GetFileSize(hFileLocal, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileLength && NO_ERROR!=GetLastError()) {
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
    //
    unsigned int vleng = (unsigned int)(qwFileLength - ival->offset);
    unsigned int block_tatol = vleng / BLOCK_SIZE;
    unsigned int posit = 0, inde;
    for(inde = 0; inde < block_tatol; inde++) {
        FlushZeroFile(sctrl->file_handle, buffer, ival->offset, posit, BLOCK_SIZE);
        posit += BLOCK_SIZE;
    }
    if (POW2N_MOD(vleng, BLOCK_SIZE))
        FlushZeroFile(sctrl->file_handle, buffer, ival->offset, posit, POW2N_MOD(vleng, BLOCK_SIZE));
    //
// _LOG_DEBUG(_T("flush match to file. nContentLength:%d"), ival->content_length);
    unmap_file64(buffer);
    CloseHandle( hFileLocal );
    FlushFileBuffers(sctrl->file_handle);
    return 0x00;
}

DWORD MUtil::MatchZeroChunk(struct SeionCtrl *sctrl, struct InteValue *ival, TCHAR *szFileLocal) {
    DWORD dwResult = SetFilePointer(sctrl->file_handle, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return MATCH_FAULT;
    dwResult = SetEndOfFile(sctrl->file_handle);
    if(FALSE==dwResult && NO_ERROR!=GetLastError()) {
        _LOG_WARN(_T("set end of file false, last error is:%d"), GetLastError());
        return MATCH_FAULT;
    }
//
    HANDLE hFileLocal = CreateFile( szFileLocal,
                                    GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileLocal ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return MATCH_NOT_FOUND;
		} else { return MATCH_VIOLATION; }
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileLength = GetFileSize(hFileLocal, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileLength && NO_ERROR!=GetLastError()) {
        CloseHandle( hFileLocal );
        return MATCH_FAULT;
    }
    qwFileLength += (((unsigned __int64)dwFileSizeHigh) << 32);
    if(qwFileLength < (ival->offset + CHUNK_SIZE)) {
        CloseHandle( hFileLocal );
        return MATCH_FAULT;
    }
//
    struct tiny_map64 *buffer = (struct tiny_map64 *)map_file64(hFileLocal, qwFileLength, MAX_MAP_SIZE, BLOCK_SIZE);
    if(!buffer) {
        CloseHandle( hFileLocal );
        return MATCH_FAULT;
    }
    //
    unsigned int block_tatol = CHUNK_SIZE / BLOCK_SIZE;
    unsigned int posit = 0, inde;
    for(inde = 0; inde < block_tatol; inde++) {
        FlushZeroFile(sctrl->file_handle, buffer, ival->offset, posit, BLOCK_SIZE);
        posit += BLOCK_SIZE;
    }
    //
// _LOG_DEBUG(_T("flush match to file. nContentLength:%d"), ival->content_length);
    unmap_file64(buffer);
    CloseHandle( hFileLocal );
    FlushFileBuffers(sctrl->file_handle);
    return 0x00;
}