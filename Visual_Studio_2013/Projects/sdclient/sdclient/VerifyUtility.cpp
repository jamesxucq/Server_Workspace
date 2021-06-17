#include "StdAfx.h"
#include "TRANSWorker.h"
// #include "TRANSObject.h"
#include "DebugPrinter.h"

#include "../third party/RiverFS/RiverFS.h"
#include "VerifyUtility.h"
#include "VerifyTasksBzl.h"
#include "HandleTasksBzl.h"

//
DWORD NVerifyUtility::FileSha1Request(HANDLE hFileValid, CTRANSWorker *pVerifyWorker, struct TaskNode *task_node, DWORD dwCacheVerify) {
    // 生成工作记录,初始会话
    int proto_value = PROTO_FAULT;
    do {
        proto_value = pVerifyWorker->GetFileVerifySha1(hFileValid, task_node, dwCacheVerify);
    } while(PROTO_PROCESSING == proto_value);
    //
    DWORD veri_value = VERIFY_VALUE_FINISH;
    switch(proto_value) {
    case PROTO_FAULT:
        veri_value = VERIFY_VALUE_EXCEPTION;
        break;
    case PROTO_OKAY:
        veri_value = VERIFY_VALUE_FINISH;
        break;
    case PROTO_NETWORK:
        veri_value = VERIFY_VALUE_NETWORK;
        break;
    case PROTO_NO_CONTENT:
        veri_value = VERIFY_VALUE_NCONTE;
        break;
	case PROTO_NOT_FOUND:
        veri_value = VERIFY_VALUE_NFOUND;
        break;
    }
_LOG_DEBUG(_T("file sha1 request:%08x"), veri_value);
    return veri_value;
}

DWORD NVerifyUtility::FileChksRequest(HANDLE hFileValid, CTRANSWorker *pVerifyWorker, struct TaskNode *task_node, DWORD dwCacheVerify) {
    // 生成工作记录,初始会话
    int proto_value = PROTO_FAULT;
    do {
        proto_value = pVerifyWorker->GetFileVerifyChks(hFileValid, task_node, dwCacheVerify);
    } while(PROTO_PROCESSING == proto_value);
    //
    DWORD veri_value = VERIFY_VALUE_FINISH;
    switch(proto_value) {
    case PROTO_FAULT:
        veri_value = VERIFY_VALUE_EXCEPTION;
        break;
    case PROTO_OKAY:
        veri_value = VERIFY_VALUE_FINISH;
        break;
    case PROTO_NETWORK:
        veri_value = VERIFY_VALUE_NETWORK;
// _LOG_DEBUG(_T("file chks network!"));
        break;
    case PROTO_NO_CONTENT:
        veri_value = VERIFY_VALUE_NCONTE;
        break;
    case PROTO_NOT_FOUND:
        veri_value = VERIFY_VALUE_NFOUND;
        break;
    }
// _LOG_DEBUG(_T("file chks server:%08x"), veri_value);
    return veri_value;
}

DWORD NVerifyUtility::ChunkChksRequest(HANDLE hFileValid, CTRANSWorker *pVerifyWorker, struct TaskNode *task_node, DWORD dwCacheVerify) {
    // 生成工作记录,初始会话
    DWORD veri_value = VERIFY_VALUE_FINISH;
    switch(pVerifyWorker->GetChunkVerifyChks(hFileValid, task_node, dwCacheVerify)) {
    case PROTO_FAULT:
        veri_value = VERIFY_VALUE_EXCEPTION;
        break;
    case PROTO_OKAY:
        veri_value = VERIFY_VALUE_FINISH;
        break;
    case PROTO_NETWORK:
        veri_value = VERIFY_VALUE_NETWORK;
        break;
    case PROTO_NOT_FOUND:
        veri_value = VERIFY_VALUE_NFOUND;
        break;
    }
_LOG_DEBUG(_T("chunk chks request:%08x"), veri_value);
    return veri_value;
}

DWORD NVerifyUtility::FileSha1River(unsigned char *szSha1Chks, CTRANSWorker *pVerifyWorker, TCHAR *szFileName) {
    DWORD chks_value = 0x00;
    do {
        chks_value = RiverFS::FileSha1Relay(szSha1Chks, szFileName);
// _LOG_DEBUG(_T("sha1_value:%d"), sha1_value);
        pVerifyWorker->KeepAlive(); // keep alive
    } while(0x02 == chks_value);
_LOG_DEBUG(_T("file sha chks_value:%d"), chks_value);
    return chks_value;
}

DWORD NVerifyUtility::FileChksRiver(HANDLE hRiveChks, CTRANSWorker *pVerifyWorker, TCHAR *szFileName) {
    DWORD chks_value = 0x00;
    do {
        chks_value = RiverFS::FileChksRelay(hRiveChks, szFileName, 0x00);
// _LOG_DEBUG(_T("chks_value:%d"), chks_value);
        pVerifyWorker->KeepAlive(); // keep alive
    } while(0x02 == chks_value);
_LOG_DEBUG(_T("file chks chks_value:%d"), chks_value);
    return chks_value;
}

//
static unsigned char NULL_CHKS[MAX_CHKS_LEN];
DWORD NVerifyUtility::VerifyTasksNode(struct TaskNode *task_node, unsigned char *szSha1Chks, HANDLE hFileValid, DWORD dwReqOk) {
    // disable by james 20140115
// _LOG_DEBUG(_T("file verify node! offset:%llu build_length:%u control_code:%X"), task_node->offset, task_node->build_length, task_node->control_code);
    DWORD local_success = ERR_SUCCESS, serv_success = ERR_SUCCESS;
    // 获取本地效验码
	if(!memcmp(NULL_CHKS, szSha1Chks, SHA1_DIGEST_LEN))
		local_success = ERR_EXCEPTION;
    // 获取服务器端效验码
    struct whole_chks tWholeChks;
    DWORD dwReadSize = 0x00;
    if(!ReadFile(hFileValid, &tWholeChks, sizeof(struct whole_chks), &dwReadSize, NULL) || !dwReadSize) {
        if((ERROR_HANDLE_EOF==GetLastError()) || !dwReadSize) serv_success = ERR_EXCEPTION; // 服务器没有相关资源
        else serv_success = ERR_FAULT; // 读取效验码出错
// _LOG_DEBUG(_T("GetLastError():%d"), GetLastError());
    }
    // disable by james 20140115
_LOG_DEBUG(_T("szSha1Chks:%X serv_success:%X dwReqOk:%d"), szSha1Chks, serv_success, dwReqOk);
ndp::ChksMD5(_T("local_sha1_chks"), szSha1Chks);
// ndp::ChksMD5(_T("tWholeChks.sha1_chks"), tWholeChks.sha1_chks);
    // 读取效验码正确,比对效验码
    DWORD veri_value = ERR_FAULT;
    if(!local_success && !serv_success) { // 读取服务器成功,本地效验码成功
        if(memcmp(szSha1Chks, tWholeChks.sha1_chks, SHA1_DIGEST_LEN)) {
            veri_value = ERR_EXCEPTION;
            memcpy(task_node->correct_chks, szSha1Chks, SHA1_DIGEST_LEN);
            SET_VERIFY_EXCEPTION(task_node->success_flags);
        } else {
            veri_value = ERR_SUCCESS;
            memcpy(task_node->correct_chks, szSha1Chks, SHA1_DIGEST_LEN);
// _LOG_DEBUG(_T("szSha1Chks:%X serv_success:%X"), szSha1Chks, serv_success);
// ndp::ChksMD5(_T("local_sha1_chks"), szSha1Chks);
// ndp::ChksMD5(_T("tWholeChks.sha1_chks"), tWholeChks.sha1_chks);
// _LOG_DEBUG(_T("1 verify tasks node!"));
_LOG_DEBUG(_T("set task break!"));
            SET_TASK_BREAK(task_node->success_flags);
        }
    } else if(!local_success && serv_success) { // 读取服务器失败,本地效验码成功
        veri_value = ERR_EXCEPTION;
        memcpy(task_node->correct_chks, szSha1Chks, SHA1_DIGEST_LEN);
        SET_VERIFY_EXCEPTION(task_node->success_flags);
		SET_INTERNAL_EXP(task_node->excepti);
    } else if(local_success && !serv_success) { // 读取服务器成功,本地效验码失败
// _LOG_DEBUG(_T("szSha1Chks:%X serv_success:%X"), szSha1Chks, serv_success);
// _LOG_DEBUG(_T("2 verify tasks node!"));
_LOG_DEBUG(_T("set task break!"));
        veri_value = ERR_SUCCESS;
        SET_TASK_BREAK(task_node->success_flags);
    } else if(local_success && serv_success) { // file size is 0, 20140107
// _LOG_DEBUG(_T("szSha1Chks:%X serv_success:%X"), szSha1Chks, serv_success);
// _LOG_DEBUG(_T("3 verify tasks node!"));
		if(dwReqOk) {
_LOG_DEBUG(_T("set task break!"));
			veri_value = ERR_SUCCESS;
			SET_TASK_BREAK(task_node->success_flags);
		} else {
_LOG_DEBUG(_T("set task break!, respon"));
	        veri_value = ERR_EXCEPTION;
			SET_VERIFY_EXCEPTION(task_node->success_flags);
			SET_INTERNAL_EXP(task_node->excepti);
		}
    }
    //
ndp::ChksMD5(_T("correct_chks"), task_node->correct_chks);
    return veri_value;
}

//
DWORD NVerifyUtility::VerifyTasksNode(struct TaskNode *task_node, HANDLE hFileLocal, HANDLE hFileValid, DWORD dwReqOk) {
    // disable by james 20140115
// _LOG_DEBUG(_T("handle verify node! dwReqOk:%d"), dwReqOk);
// _LOG_DEBUG(_T("offset:%llu build_length:%u control_code:%X"), task_node->offset, task_node->build_length, task_node->control_code);
    //
    DWORD local_success = ERR_SUCCESS, serv_success = ERR_SUCCESS;
    // 获取本地效验码
    struct riv_chks tRiveChks;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD dwReadSize;
    unsigned __int64 qwVerifyOffset;
    qwVerifyOffset = (task_node->offset >> 22) * sizeof(struct riv_chks);
    OverLapped.Offset = qwVerifyOffset & 0xffffffff;
    OverLapped.OffsetHigh = qwVerifyOffset >> 32;
	memset(&tRiveChks, '\0', sizeof(struct riv_chks));
    if(!ReadFile(hFileLocal, &tRiveChks, sizeof(struct riv_chks), &dwReadSize, &OverLapped) || !dwReadSize) {
        if((ERROR_HANDLE_EOF==GetLastError()) || !dwReadSize) local_success = ERR_EXCEPTION; // 服务器没有相关资源
        else local_success = ERR_FAULT; // 读取效验码出错
// _LOG_DEBUG(_T("qwVerifyOffset:%llu GetLastError():%d"), qwVerifyOffset, GetLastError());
    }
    // 获取服务器端效验码
    struct simple_chks tSimpleChks;
    qwVerifyOffset = (task_node->offset >> 22) * sizeof(struct simple_chks);
    OverLapped.Offset = qwVerifyOffset & 0xffffffff;
    OverLapped.OffsetHigh = qwVerifyOffset >> 32;
	memset(&tSimpleChks, '\0', sizeof(struct simple_chks));
    if(!ReadFile(hFileValid, &tSimpleChks, sizeof(struct simple_chks), &dwReadSize, &OverLapped) || !dwReadSize) {
        if((ERROR_HANDLE_EOF==GetLastError()) || !dwReadSize) serv_success = ERR_EXCEPTION; // 服务器没有相关资源
        else serv_success = ERR_FAULT; // 读取效验码出错
// _LOG_DEBUG(_T("sizeof(struct simple_chks):%d dwReadSize:%d GetLastError():%d"), sizeof(struct simple_chks), dwReadSize, GetLastError());
    }
    // disable by james 20140115
_LOG_DEBUG(_T("----------- local_success:%X serv_success:%X chunk:%d dwReqOk:%d"), local_success, serv_success, task_node->offset>>22, dwReqOk);
// if(memcmp(tRiveChks.md5_chks, tSimpleChks.md5_chks, MD5_DIGEST_LEN)) {
// _LOG_DEBUG(_T("----------- task_node->offset:%llX tSimpleChks.offset:%llX chunk:%d"), task_node->offset, tSimpleChks.offset, task_node->offset>>22);
_LOG_DEBUG(_T("----------- task_node->node_inde:%d"), task_node->node_inde);
// ndp::ChksMD5_ex(_T("----------- local_md5sum"), tRiveChks.md5_chks, fn);
// ndp::ChksMD5_ex(_T("----------- tSimpleChks.md5_chks"), tSimpleChks.md5_chks, fn);
// }
	// 读取效验码正确,比对效验码
    DWORD veri_value = ERR_FAULT;
    if(!local_success && !serv_success) { // 读取服务器成功,本地效验码成功
        if(memcmp(tRiveChks.md5_chks, tSimpleChks.md5_chks, MD5_DIGEST_LEN) && (task_node->offset==tSimpleChks.offset)) {
            veri_value = ERR_EXCEPTION;
            if((STYPE_GET|STYPE_PSYNC) & task_node->control_code)
                memcpy(task_node->correct_chks, tSimpleChks.md5_chks, MD5_DIGEST_LEN);
            else if((STYPE_PUT|STYPE_GSYNC) & task_node->control_code)
                memcpy(task_node->correct_chks, tRiveChks.md5_chks, MD5_DIGEST_LEN);
_LOG_DEBUG(_T("set task excep!"));
_LOG_DEBUG(_T("i success_flags:%08x"), task_node->success_flags);
				SET_VERIFY_EXCEPTION(task_node->success_flags);
_LOG_DEBUG(_T("f success_flags:%08x"), task_node->success_flags);
        } else {
            veri_value = ERR_SUCCESS;
            memcpy(task_node->correct_chks, tRiveChks.md5_chks, MD5_DIGEST_LEN);
_LOG_DEBUG(_T("local_success:%X serv_success:%X chunk:%d"), local_success, serv_success, task_node->offset>>22);
// ndp::ChksMD5(_T("local_md5sum"), tRiveChks.md5_chks);
// ndp::ChksMD5(_T("tSimpleChks.md5_chks"), tSimpleChks.md5_chks);
// _LOG_DEBUG(_T("4 verify tasks node! control_code:%08x"), task_node->control_code);
            // 插入正确的效验植 // 20131119
			if((STYPE_GET|STYPE_PSYNC) & task_node->control_code) {
_LOG_DEBUG(_T("verify chunk update lastwrite."));
                RiverFS::ChunkLawiUpdate(task_node->szFileName1, task_node->offset >> 22, task_node->build_length, &task_node->last_write);
			}
_LOG_DEBUG(_T("set task break!"));
            SET_TASK_BREAK(task_node->success_flags);
        }
    } else if(!local_success && serv_success) { // 读取服务器失败,本地效验码成功
        if((STYPE_PUT|STYPE_GSYNC) & task_node->control_code) { // 本地新,文件变长
            veri_value = ERR_EXCEPTION;
            memcpy(task_node->correct_chks, tRiveChks.md5_chks, MD5_DIGEST_LEN);
_LOG_DEBUG(_T("set task excep!"));
            SET_VERIFY_EXCEPTION(task_node->success_flags);
			SET_INTERNAL_EXP(task_node->excepti);
        } else if((STYPE_GET|STYPE_PSYNC) & task_node->control_code) { // 服务器新,文件缩短
            veri_value = ERR_SUCCESS;
_LOG_DEBUG(_T("set task done!"));
            SET_VERIFY_DONE(task_node->success_flags);
        }
    } else if(local_success && !serv_success) { // 读取服务器成功,本地效验码失败
        if((STYPE_GET|STYPE_PSYNC) & task_node->control_code) { // 服务器新,文件变长
            veri_value = ERR_EXCEPTION;
            memcpy(task_node->correct_chks, tSimpleChks.md5_chks, MD5_DIGEST_LEN);
_LOG_DEBUG(_T("set task excep!"));
            SET_VERIFY_EXCEPTION(task_node->success_flags);
			SET_INTERNAL_EXP(task_node->excepti);
        } else if((STYPE_PUT|STYPE_GSYNC) & task_node->control_code) { // 本地新,文件缩短
            veri_value = ERR_SUCCESS;
_LOG_DEBUG(_T("set task done!"));
            SET_VERIFY_DONE(task_node->success_flags);
        }
    } else if(local_success && serv_success) { // file size is 0, 20140107
		if(dwReqOk) {
// _LOG_DEBUG(_T("local_success:%X serv_success:%X chunk:%d"), local_success, serv_success, task_node->offset>>22);
			if((STYPE_GET|STYPE_PSYNC) & task_node->control_code) {
_LOG_DEBUG(_T("set task excep! respon"));
	            veri_value = ERR_EXCEPTION;
		        SET_VERIFY_EXCEPTION(task_node->success_flags);
				SET_INTERNAL_EXP(task_node->excepti);
			} else if((STYPE_PUT|STYPE_GSYNC) & task_node->control_code) {
_LOG_DEBUG(_T("set task break!"));
		        veri_value = ERR_SUCCESS;
			    SET_TASK_BREAK(task_node->success_flags);
			}
		} else {
_LOG_DEBUG(_T("set task excep! respon"));
            veri_value = ERR_EXCEPTION;
            SET_VERIFY_EXCEPTION(task_node->success_flags);
			SET_INTERNAL_EXP(task_node->excepti);
		}
    }
    // disable by james 20140115
_LOG_DEBUG(_T("veri_value:%X"), veri_value);
ndp::ChksMD5(_T("correct_chks"), task_node->correct_chks);
    //
    return veri_value;
}

//
DWORD NVerifyUtility::Sha1ValidTransmit(struct TaskNode *task_node, HANDLE hFileValid, DWORD dwReqOk) {
    DWORD corre_success = ERR_SUCCESS, serv_success = ERR_SUCCESS;
    // 获取本地效验码
	if(!memcmp(NULL_CHKS, task_node->correct_chks, SHA1_DIGEST_LEN))
		corre_success = ERR_EXCEPTION;
    // 获取服务器端效验码	
	struct whole_chks tWholeChks;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD dwReadSize;
    if(!ReadFile(hFileValid, &tWholeChks, sizeof(struct whole_chks), &dwReadSize, &OverLapped) || !dwReadSize) {
        if((ERROR_HANDLE_EOF==GetLastError()) || !dwReadSize) serv_success = ERR_EXCEPTION; // 服务器没有相关资源
        else serv_success = ERR_FAULT; // 读取效验码出错
    }
    //
_LOG_DEBUG(_T("corre_success:%X serv_success:%X dwReqOk:%08x"), corre_success, serv_success, dwReqOk);
// if(memcmp(task_node->correct_chks, tWholeChks.sha1_chks, SHA1_DIGEST_LEN)) {
ndp::ChksMD5(_T("correct_chks"), task_node->correct_chks);
// ndp::ChksMD5(_T("tWholeChks.sha1_chks"), tWholeChks.sha1_chks);
// }
	// 读取效验码正确,比对效验码
    DWORD veri_value = ERR_FAULT;
	if(!corre_success && !serv_success) { // 读取服务器成功,正确效验码成功
		if(memcmp(task_node->correct_chks, tWholeChks.sha1_chks, SHA1_DIGEST_LEN))
			veri_value = ERR_EXCEPTION;
		else veri_value = ERR_SUCCESS;
	} else if(!corre_success && serv_success) { // 读取服务器失败,正确效验码成功
		veri_value = ERR_EXCEPTION;
    } else if(corre_success && !serv_success) { // 读取服务器成功,正确效验码失败
		veri_value = ERR_EXCEPTION;
    } else if(corre_success && serv_success) { // file size is 0, 20140107
		if(dwReqOk) veri_value = ERR_SUCCESS;
		else veri_value = ERR_EXCEPTION;
	}
	//
_LOG_DEBUG(_T("veri_value:%X"), veri_value);
    return veri_value;
}

DWORD NVerifyUtility::FileValidTransmit(struct TaskNode *task_node, HANDLE hFileValid, DWORD dwReqOk) {
    DWORD corre_success = ERR_SUCCESS, serv_success = ERR_SUCCESS;
    // 获取本地效验码
	if(!memcmp(NULL_CHKS, task_node->correct_chks, MD5_DIGEST_LEN))
		corre_success = ERR_EXCEPTION;
    // 获取服务器端效验码	
    struct simple_chks tSimpleChks;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD dwReadSize;
    unsigned __int64 qwValidOffset = (task_node->offset>>22) * sizeof(struct simple_chks);
    OverLapped.Offset = qwValidOffset & 0xffffffff;
    OverLapped.OffsetHigh = qwValidOffset >> 32;
	memset(&tSimpleChks, '\0', sizeof(struct simple_chks));
    if(!ReadFile(hFileValid, &tSimpleChks, sizeof(struct simple_chks), &dwReadSize, &OverLapped) || !dwReadSize) {
        if((ERROR_HANDLE_EOF==GetLastError()) || !dwReadSize) serv_success = ERR_EXCEPTION; // 服务器没有相关资源
        else serv_success = ERR_FAULT; // 读取效验码出错
    }
_LOG_DEBUG(_T("task_node->offset::%llX tSimpleChks.offset::%llX chunk:%d"), task_node->offset, tSimpleChks.offset, task_node->offset>>22);
    if(task_node->offset != tSimpleChks.offset) return ERR_EXCEPTION;
_LOG_DEBUG(_T("corre_success:%X serv_success:%X chunk:%d dwReqOk:%08x"), corre_success, serv_success, task_node->offset>>22, dwReqOk);
// if(memcmp(task_node->correct_chks, tSimpleChks.md5_chks, MD5_DIGEST_LEN)) {
//	ndp::ChksMD5(_T("task_node->correct_chks"), task_node->correct_chks);
//	ndp::ChksMD5(_T("tSimpleChks.md5_chks"), tSimpleChks.md5_chks); // disable by james 20140410
// }
	// 读取效验码正确,比对效验码
    DWORD veri_value = ERR_FAULT;
	if(!corre_success && !serv_success) { // 读取服务器成功,正确效验码成功
		if(memcmp(task_node->correct_chks, tSimpleChks.md5_chks, MD5_DIGEST_LEN))
			veri_value = ERR_EXCEPTION;
		else veri_value = ERR_SUCCESS;
	} else if(!corre_success && serv_success) { // 读取服务器失败,正确效验码成功
		veri_value = ERR_EXCEPTION;
    } else if(corre_success && !serv_success) { // 读取服务器成功,正确效验码失败
		veri_value = ERR_EXCEPTION;
    } else if(corre_success && serv_success) { // file size is 0, 20140107
		if(dwReqOk) veri_value = ERR_SUCCESS;
		else veri_value = ERR_EXCEPTION;
	}
//
_LOG_DEBUG(_T("veri_value:%X"), veri_value);
    return veri_value;
}

DWORD NVerifyUtility::ChunkValidTransmit(struct TaskNode *task_node, HANDLE hFileValid) {
    struct simple_chks tSimpleChks;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD dwReadSize;
	memset(&tSimpleChks, '\0', sizeof(struct simple_chks));
    if(!ReadFile(hFileValid, &tSimpleChks, sizeof(struct simple_chks), &dwReadSize, &OverLapped) || !dwReadSize) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return ((DWORD)-1);
    }
_LOG_DEBUG(_T("task_node->offset::%llX tSimpleChks.offset::%llX chunk:%d"), task_node->offset, tSimpleChks.offset, task_node->offset>>22);
if(memcmp(task_node->correct_chks, tSimpleChks.md5_chks, MD5_DIGEST_LEN)) {
 ndp::ChksMD5(_T("task_node->correct_chks"), task_node->correct_chks);
 ndp::ChksMD5(_T("tSimpleChks.md5_chks"), tSimpleChks.md5_chks); // disable by james 20140410
}
    return memcmp(task_node->correct_chks, tSimpleChks.md5_chks, MD5_DIGEST_LEN);
}

DWORD NVerifyUtility::ValidTransmitDownd(struct TaskNode *task_node) {
    DWORD valid_value = VERIFY_VALUE_FINISH;
    unsigned char build_md5sum[MD5_DIGEST_LEN];
    HANDLE	hBuilderCache = INVALID_HANDLE_VALUE;
//
    if((STYPE_GET|STYPE_PSYNC) & task_node->control_code) {
        if(!ONLY_TRANSMIT_OKAY(task_node->success_flags) && ONLY_VERIFY_OKAY(task_node->success_flags)) {
            //
            hBuilderCache = NTRANSCache::OpenBuilder(task_node->builder_cache);
            if( INVALID_HANDLE_VALUE == hBuilderCache ) {
_LOG_WARN( _T("validate open file failed. file name is: %s"), task_node->builder_cache);
                return VERIFY_VALUE_FAILED;
            }
            if(FileChks::ChunkChks(build_md5sum, hBuilderCache, task_node->build_length))
                return VERIFY_VALUE_FAILED;
            NTRANSCache::CloseBuilder(hBuilderCache);
//
            if(memcmp(build_md5sum, task_node->correct_chks, MD5_DIGEST_LEN)) {
_LOG_DEBUG(_T("validate download exception! node_inde:%d"), task_node->node_inde);  // disable by james 20140410
// _LOG_DEBUG(_T("build cache:%s build length:%d"), task_node->builder_cache, task_node->build_length);
// _LOG_DEBUG(_T("szFileName1:%s"), task_node->szFileName1);
// ndp::ChksMD5(_T("build_md5sum"), build_md5sum);
// ndp::ChksMD5(_T("correct_chks"), task_node->correct_chks);
                valid_value = VERIFY_VALUE_EXCEPTION;
_LOG_DEBUG(_T("set transmit verify excep, szFileName1:%s node_inde:%d"), task_node->szFileName1, task_node->node_inde);
                SET_TRANSMIT_VERIFY_EXCEPTION(task_node->success_flags);
				SET_DOWND_EXP(task_node->excepti);
            } else {
_LOG_DEBUG(_T("validate download Ok! node_inde:%d"), task_node->node_inde);  // disable by james 20140410
                valid_value = VERIFY_VALUE_FINISH;
                SET_VERIFY_DONE(task_node->success_flags);
            }
            objTaskList.WriteNodeEx(task_node, task_node->node_inde);
        }
    }
//
    return valid_value;
}

CTRANSWorker *NVerifyUtility::CreateWorker(DWORD *cvalue, struct VERIFY_ARGUMENT *pVerifyArgu) {
    CTRANSWorker *pVerifyWorker = NTRANSWorker::Factory(&pVerifyArgu->tSeionArgu, &pVerifyArgu->tSockeArgu);
    if (!pVerifyWorker) {
        NTRANSWorker::DestroyObject(pVerifyWorker);
        *cvalue = VERIFY_VALUE_NETWORK;
        return NULL;
    }
    // 生成工作记录,初始会话
    int proto_value = PROTO_FAULT;
    if(proto_value = pVerifyWorker->Initial()) {
        *cvalue = PROTO_NETWORK == proto_value? VERIFY_VALUE_NETWORK: VERIFY_VALUE_FAILED;
        NTRANSWorker::DestroyObject(pVerifyWorker);
        pVerifyWorker = NULL;
    }
    return pVerifyWorker;
}

void NVerifyUtility::DropWorker(CTRANSWorker *pVerifyWorker) {
    if(pVerifyWorker)
        NTRANSWorker::DestroyObject(pVerifyWorker);
}

void NVerifyUtility::DestroyWorker(CTRANSWorker *pVerifyWorker) {
    if(pVerifyWorker) {
        pVerifyWorker->Final(FINAL_STATUS_END);
        NTRANSWorker::DestroyObject(pVerifyWorker);
    }
}