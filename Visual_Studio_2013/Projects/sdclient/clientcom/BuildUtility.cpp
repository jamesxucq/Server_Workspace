#include "stdafx.h"

#include "common_macro.h"
#include "client_macro.h"

#include "FileUtility.h"
#include "TRANSCache.h"

#include "BuildUtility.h"

#include <vector>
using std::vector;

int BuildFileContent(HANDLE hFileLocal, struct FileBuilder *builder, vector<struct local_match *> *local_vector) {
    unsigned char buffer[BLOCK_SIZE * 0x02];
    vector <struct local_match *>::iterator iter;
    struct file_matcher *matcher;
    unsigned __int64 qwFileOffset;
//
// _LOG_DEBUG(_T("build utility builder->builder_cache is:%s"), builder->builder_cache);
    HANDLE hFileBackup = NTRANSCache::OpenBuilder(builder->builder_cache);
    if(INVALID_HANDLE_VALUE == hFileBackup) return -1;
//
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD wlen, slen;
    for(iter = local_vector->begin(); local_vector->end() != iter; ++iter) {
        matcher = (*iter)->matcher;
        if(CHECK_SUMS & matcher->match_type) {
            qwFileOffset = matcher->inde_len*BLOCK_SIZE + builder->offset;
// _LOG_DEBUG(_T("build chks offset:%llu, BLOCK_SIZE:%d"), qwFileOffset, BLOCK_SIZE);
            OverLapped.Offset = qwFileOffset&0xffffffff;
            OverLapped.OffsetHigh = qwFileOffset >> 32;
            if(!ReadFile(hFileLocal, buffer, BLOCK_SIZE, &slen, &OverLapped) || !slen) {
                // if(ERROR_HANDLE_EOF != GetLastError())
                _LOG_WARN(_T("build read error!"));
                return -1;
            }
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hFileBackup, buffer, BLOCK_SIZE, &wlen, &OverLapped)) {
                _LOG_WARN(_T("build write error!"));
                return -1;
            }
            if(wlen != BLOCK_SIZE) {
                _LOG_WARN(_T("build write error!"));
                return -1;
            }
        } else if(CONEN_MODI & matcher->match_type) {
// _LOG_DEBUG(_T("build modi offset:%u, inde_len:%u"), matcher->offset, matcher->inde_len);
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hFileBackup, (*iter)->buffer, matcher->inde_len, &wlen, &OverLapped)) {
                _LOG_WARN(_T("build write error!"));
                return -1;
            }
            if(wlen != matcher->inde_len) {
                _LOG_WARN(_T("build write error!"));
                return -1;
            }
		}
    }
    NTRANSCache::CloseBuilder(hFileBackup);
//
    return 0;
}


int BuildChunkContent(HANDLE hFileLocal, struct FileBuilder *builder, vector<struct local_match *> *local_vector) {
    unsigned char buffer[BLOCK_SIZE * 0x02];
    vector <struct local_match *>::iterator iter;
    struct file_matcher *matcher;
    unsigned __int64 qwFileOffset;
//
    HANDLE hChunkBackup = NTRANSCache::OpenBuilder(builder->builder_cache);
    if(INVALID_HANDLE_VALUE == hChunkBackup) return -1;
//
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD wlen, slen;
    for(iter = local_vector->begin(); local_vector->end() != iter; ++iter) {
        matcher = (*iter)->matcher;
        if(CHECK_SUMS & matcher->match_type) {
            qwFileOffset = matcher->inde_len*BLOCK_SIZE + builder->offset;
            OverLapped.Offset = qwFileOffset&0xffffffff;
            OverLapped.OffsetHigh = qwFileOffset >> 32;
            if(!ReadFile(hFileLocal, buffer, BLOCK_SIZE, &slen, &OverLapped) || !slen) {
                // if(ERROR_HANDLE_EOF != GetLastError())
                _LOG_WARN(_T("build read error!"));
                return -1;
            }
			//
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hChunkBackup, buffer, BLOCK_SIZE, &wlen, &OverLapped)) {
                _LOG_WARN(_T("build write error!"));
                return -1;
            }
            if(wlen != BLOCK_SIZE) return -1;
        } else if(CONEN_MODI & matcher->match_type) {
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hChunkBackup, (*iter)->buffer, matcher->inde_len, &wlen, &OverLapped)) {
                _LOG_WARN(_T("build write error!"));
                return -1;
            }
            if(wlen != matcher->inde_len) {
                _LOG_WARN(_T("build write error!"));
                return -1;
            }
        }
    }
    NTRANSCache::CloseBuilder(hChunkBackup);
//
    return 0;
}

DWORD NBuildUtility::ContentBuilder(struct FileBuilder *builder, vector<struct local_match *> *local_vector) { 
    if (!builder) return BUILD_FAULT;
    if (local_vector->empty()) return BUILD_SUCCESS;
//
    HANDLE hFileLocal = CreateFile( builder->file_name,
                                    GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE, // | FILE_SHARE_DELETE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileLocal ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return BUILD_NOT_FOUND;
		} else { return BUILD_VIOLATION; }
    }
    objBuilderVector.AppendResource(hFileLocal);
_LOG_DEBUG(_T("build utility file is:%s"), builder->file_name);
    //
	DWORD dwBuildValue = BUILD_SUCCESS;
    if (RANGE_TYPE_FILE == builder->build_type) {
        // lock on the opened file
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
        OverLapped.Offset = builder->offset&0xffffffff;
        OverLapped.OffsetHigh = builder->offset >> 32;
        if(!LockFileEx(hFileLocal,
                       LOCKFILE_EXCLUSIVE_LOCK,
                       0,
                       0xffffffff,
                       0x00000000,
                       &OverLapped)) {
            _LOG_WARN(_T("build file lock file fail!"));
		    objBuilderVector.EraseResource(hFileLocal);
			CloseHandle(hFileLocal);
            return BUILD_VIOLATION;
        }
        if(BuildFileContent(hFileLocal, builder, local_vector)) {
            _LOG_WARN(_T("build file content fail!"));
            dwBuildValue = BUILD_FAULT;
        }
        // unlock on the opened file
        // OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        // OverLapped.Offset = task_node->offset;
        UnlockFileEx(hFileLocal, 0, 0xffffffff, 0x00000000, &OverLapped);
    } else if (RANGE_TYPE_CHUNK == builder->build_type) {
        // lock on the opened file
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
        OverLapped.Offset = builder->offset&0xffffffff;
        OverLapped.OffsetHigh = builder->offset >> 32;
        if(!LockFileEx(hFileLocal,
                       LOCKFILE_EXCLUSIVE_LOCK,
                       0,
                       CHUNK_SIZE,
                       0x00000000,
                       &OverLapped)) {
            _LOG_WARN(_T("build chunk lock file fail!"));
		    objBuilderVector.EraseResource(hFileLocal);
			CloseHandle(hFileLocal);
            return BUILD_VIOLATION;
        }
        if(BuildChunkContent(hFileLocal, builder, local_vector)) {
            _LOG_WARN(_T("build chunk content fail!"));
            dwBuildValue = BUILD_FAULT;
        }
        // unlock on the opened file
        // OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        // OverLapped.Offset = task_node->offset;
        UnlockFileEx(hFileLocal, 0, CHUNK_SIZE, 0x00000000, &OverLapped);
    }
    //
    objBuilderVector.EraseResource(hFileLocal);
    CloseHandle(hFileLocal);
//
    return dwBuildValue;
}

void NBuildUtility::CloseBuilderHandle() {
    objBuilderVector.CloseResource();
}