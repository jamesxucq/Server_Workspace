#include "stdafx.h"

#include "common_macro.h"
#include "client_macro.h"

#include "FileUtility.h"
#include "TRANSCache.h"
#include "BuilderVector.h"

#include "BuildUtility.h"


int BuildFileContent(HANDLE hFileLocal, struct FileBuilder *builder, vector<struct local_match *> *local_vector) {
	char buffer[BLOCK_SIZE];
	vector <struct local_match *>::iterator iter;
	struct file_matcher *inform;

	// LOG_DEBUG(_T("build utility builder->build_cache is:%s"), builder->build_cache);
	HANDLE hFileBackup = NTRANSCache::OpenBuild(builder->build_cache);
	if(hFileBackup == INVALID_HANDLE_VALUE) return -1;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	DWORD wlen, slen;
	for(iter = local_vector->begin(); iter != local_vector->end(); iter++) {
		inform = (*iter)->inform;
		if(CHECK_SUM & inform->match_type) {
			OverLapped.Offset = inform->index_length*BLOCK_SIZE + builder->offset;
			if(!ReadFile(hFileLocal, buffer, BLOCK_SIZE, &slen, &OverLapped)) {
				if(ERROR_HANDLE_EOF != GetLastError()) { LOG_WARN(_T("build read error!")); return -1; }
			}
			OverLapped.Offset = inform->offset;
			if(!WriteFile(hFileBackup, buffer, BLOCK_SIZE, &wlen, &OverLapped))
				{ LOG_WARN(_T("build write error!")); return -1; }
			if(wlen != BLOCK_SIZE) { LOG_WARN(_T("build write error!")); return -1; }
		} else if(MODIFY_CONTENT & inform->match_type) {
			OverLapped.Offset = inform->offset;
			if(!WriteFile(hFileBackup, (*iter)->buffer, inform->index_length, &wlen, &OverLapped))
				{ LOG_WARN(_T("build write error!")); return -1; }
			if(wlen != inform->index_length) { LOG_WARN(_T("build write error!")); return -1; }
		}
	}
	NTRANSCache::CloseBuild(hFileBackup);

	return 0;
}


int BuildChunkContent(HANDLE hFileLocal, struct FileBuilder *builder, vector<struct local_match *> *local_vector) {
	char buffer[BLOCK_SIZE];
	vector <struct local_match *>::iterator iter;
	struct file_matcher *inform;

	HANDLE hChunkBackup = NTRANSCache::OpenBuild(builder->build_cache);	
	if(hChunkBackup == INVALID_HANDLE_VALUE) return -1;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	DWORD wlen, slen;
	for(iter = local_vector->begin(); iter != local_vector->end(); iter++) {
		inform = (*iter)->inform;
		if(CHECK_SUM & inform->match_type){
			OverLapped.Offset = inform->index_length*BLOCK_SIZE + builder->offset;
			if(!ReadFile(hFileLocal, buffer, BLOCK_SIZE, &slen, &OverLapped)) {
				if(ERROR_HANDLE_EOF != GetLastError()) { LOG_WARN(_T("build read error!")); return -1; }
			}
			OverLapped.Offset = inform->offset;
			if(!WriteFile(hChunkBackup, buffer, BLOCK_SIZE, &wlen, &OverLapped))
				{ LOG_WARN(_T("build write error!")); return -1; }
			if(wlen != BLOCK_SIZE) return -1;
		} else if(MODIFY_CONTENT & inform->match_type){
			OverLapped.Offset = inform->offset;
			if(!WriteFile(hChunkBackup, (*iter)->buffer, inform->index_length, &wlen, &OverLapped))
				{ LOG_WARN(_T("build write error!")); return -1; }
			if(wlen != inform->index_length) { LOG_WARN(_T("build write error!")); return -1; }
		}
	}
	NTRANSCache::CloseBuild(hChunkBackup);

	return 0;
}


DWORD NBuildUtility::ContentBuilder(struct FileBuilder *builder, vector<struct local_match *> *local_vector) {
	if (!builder || local_vector->empty()) return -1;

	///////////////////////////////////////////////////////////////////////////////////
	HANDLE hFileLocal = CreateFile( builder->file_name, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFileLocal == INVALID_HANDLE_VALUE ) {
		LOG_WARN( _T("create file failed.\n") );
		return -1;
	}
	objBuilderVector.AppendBuilder(hFileLocal);
	// LOG_DEBUG(_T("build utility file is:%s"), builder->file_name);
	///////////////////////////////////////////////////////////////////////////////////
	if (RANGE_TYPE_FILE == builder->build_type) {
		// lock on the opened file
		OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
		OverLapped.Offset = builder->offset;
		if(!LockFileEx(hFileLocal, 
			LOCKFILE_EXCLUSIVE_LOCK, 
			0, 
			0xffffffff, 
			0x00000000, 
			&OverLapped)) {
				LOG_WARN(_T("build file lock file fail!!!"));
				return -1;
		}
		if(BuildFileContent(hFileLocal, builder, local_vector)) {
			LOG_WARN(_T("build file content fail!!!"));
			return -1;
		}
		// unlock on the opened file
		// OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
		// OverLapped.Offset = task_node->offset;
		UnlockFileEx(hFileLocal, 0, 0xffffffff, 0x00000000, &OverLapped);
	} else if (RANGE_TYPE_CHUNK == builder->build_type) {
		// lock on the opened file
		OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
		OverLapped.Offset = builder->offset;
		if(!LockFileEx(hFileLocal, 
			LOCKFILE_EXCLUSIVE_LOCK, 
			0, 
			CHUNK_SIZE, 
			0x00000000, 
			&OverLapped)) {
				LOG_WARN(_T("build chunk lock file fail!!!"));
				return -1;
		}
		if(BuildChunkContent(hFileLocal, builder, local_vector)) {
			LOG_WARN(_T("build chunk content fail!!!"));
			return -1;
		}
		// unlock on the opened file
		// OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
		// OverLapped.Offset = task_node->offset;
		UnlockFileEx(hFileLocal, 0, CHUNK_SIZE, 0x00000000, &OverLapped);
	}
	///////////////////////////////////////////////////////////////////////////////////
	objBuilderVector.EraseBuilder(hFileLocal);
	CloseHandle(hFileLocal);

	return 0;
}

void NBuildUtility::CloseBuildHandle() {
	objBuilderVector.CloseBuildHandle();
}