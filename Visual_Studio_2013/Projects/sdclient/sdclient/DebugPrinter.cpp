#include "StdAfx.h"
#include "DebugPrinter.h"


VOID ndp::FilesVector(FilesVec *pFilesVec) {  // for debug 20110222
    FilesVec::iterator iter;
    for(iter = pFilesVec->begin(); pFilesVec->end()!=iter; ++iter) {
_LOG_DEBUG(_T("---- name:%s type:%08x size:%I64u"), (*iter)->file_name, (*iter)->action_type, (*iter)->file_size);
    }
}

VOID ndp::TasksCache(struct TasksCache *pTasksCache) {  // for debug 20110216
    struct taskatt *tmp_atte;
    TasksVec::iterator iter;

_LOG_DEBUG(_T("****************"));
    if(!pTasksCache) return ;
    if (!pTasksCache->ltskatt.empty()) {
        for(iter = pTasksCache->ltskatt.begin(); pTasksCache->ltskatt.end() != iter; ++iter) {
            tmp_atte = *iter; /* ++iter;
_LOG_DEBUG(_T("local:%s new:%s"), tmp_atte->file_name, (*iter)->file_name); */
_LOG_DEBUG(_T("local:%s size:%I64u action:%08x"), tmp_atte->file_name, tmp_atte->file_size, tmp_atte->action_type);
        }
    }
    if (!pTasksCache->stskatt.empty()) {
        for(iter = pTasksCache->stskatt.begin(); pTasksCache->stskatt.end() != iter; ++iter) {
            tmp_atte = *iter; /* ++iter;
_LOG_DEBUG(_T("server:%s new:%s"), tmp_atte->file_name, (*iter)->file_name); */
_LOG_DEBUG(_T("server:%s size:%I64u action:%08x"), tmp_atte->file_name, tmp_atte->file_size, tmp_atte->action_type);
        }
    }
_LOG_DEBUG(_T("****************"));
}

VOID ndp::Matcher(struct file_matcher *matcher) {
 _LOG_DEBUG(_T("----------------"));
 _LOG_DEBUG(_T("match_type:%d"), matcher->match_type);
 _LOG_DEBUG(_T("offset:%d"), matcher->offset);
 _LOG_DEBUG(_T("inde_len:%d"), matcher->inde_len);
}

VOID ndp::TaskNode(struct TaskNode *task_node) {
 _LOG_DEBUG(_T("----------------"));
 _LOG_DEBUG(_T("node_inde:%d"), task_node->node_inde);
 _LOG_DEBUG(_T("array_id:%d"), task_node->array_id);
 _LOG_DEBUG(_T("success_flags:%08x"), task_node->success_flags);
 _LOG_DEBUG(_T("control_code:%08x"), task_node->control_code);
 _LOG_DEBUG(_T("builder_cache:%s"), task_node->builder_cache);
 _LOG_DEBUG(_T("FileName1:%s"), task_node->szFileName1);
 _LOG_DEBUG(_T("FileName2:%s"), task_node->szFileName2);
 _LOG_DEBUG(_T("offset:%I64u"), task_node->offset);
 _LOG_DEBUG(_T("build_length:%d"), task_node->build_length);
    TCHAR timestr[64];
    timcon::ftim_unis(timestr, &task_node->last_write);
 _LOG_DEBUG(_T("LastWrite:%s"), timestr);
 _LOG_DEBUG(_T("dwReserved:%d"), task_node->reserved);
}

VOID ndp::TaskNodeI(struct TaskNode *task_node) {
    // _LOG_DEBUG(_T("----------------"));
    // _LOG_DEBUG(_T("node_inde:%d"), task_node->node_inde);
 _LOG_DEBUG(_T("array_id:%d"), task_node->array_id);
    // _LOG_DEBUG(_T("success_flags:%08x"), task_node->success_flags);
    // _LOG_DEBUG(_T("control_code:%08x"), task_node->control_code);
}

VOID ndp::ChksMD5(TCHAR *label, unsigned char *md5sum) {
    TCHAR chksum_str[64];
    for (int inde=0; inde<MD5_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, MD5_TEXT_LENGTH, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
 _LOG_DEBUG(_T("%s:%s"), label, chksum_str);
}

VOID ndp::ChksMD5_ex(TCHAR *label, unsigned char *md5sum, TCHAR *szFileName) {
    TCHAR chksum_str[64];
    for (int inde=0; inde<MD5_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, MD5_TEXT_LENGTH, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
 _LOG_DEBUG(_T("%s %s:%s"), szFileName, label, chksum_str);
}

VOID ndp::ChksSha1(TCHAR *label, unsigned char *sha1sum) {
    TCHAR chksum_str[64];
    for (int inde=0; inde<SHA1_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, 41, _T("%02x"), sha1sum[inde]);
    chksum_str[40] = _T('\0');
 _LOG_DEBUG(_T("%s:%s"), label, chksum_str);
}

//
VOID ndp::LastTaskNode() {
    struct TaskNode task_node;
    DWORD node_inde;
    int read_value;
    objTaskList.SetHandNumbe(0);
    for(; ; ) {
        node_inde = objTaskList.GetHandNodeAdd();
        read_value = objTaskList.ReadNodeEx(&task_node, node_inde);
        if(!read_value) {
 _LOG_DEBUG(_T("the last task node inform! read task node finish!"));
            break;
        } else if(0 > read_value) {
 _LOG_DEBUG(_T("the last task node inform! read task node error!"));
            break;
        }
    }
    TaskNode(&task_node);
}

//
VOID ndp::ListVector(ListVec *pListVec) {
    ListVec::iterator iter;

    for(iter = pListVec->begin(); pListVec->end() != iter; ++iter) {
_LOG_DEBUG(_T("node_inde:%d control_code:%08x offset:%I64u"), (*iter)->node_inde, (*iter)->control_code, (*iter)->offset);
_LOG_DEBUG(_T("builder_cache:%s"), (*iter)->builder_cache);
        switch(LNODE_LTYPE_BYTE((*iter)->control_code)) {
        case LIST_RECU_FILES:
        case LIST_LIST_DIREC:
        case LIST_RECU_DIRES:
 _LOG_DEBUG(_T("directory:%s"), (*iter)->directory);
            break;
        case LIST_ANCH_FILES:
 _LOG_DEBUG(_T("llast_anchor:%d"), (*iter)->llast_anchor);
            break;
        }
    }
}

VOID ndp::ActionPrinter(LkAiVec *pLkAiVec) {
    LkAiVec::iterator iter;
    struct LockActio *lock_acti;
    //
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
        lock_acti = *iter;
_LOG_DEBUG(_T("---- dwActioType:%08x coupleID:%d dwExcepFound:%d szFileName:%s"), lock_acti->dwActioType, lock_acti->coupleID, lock_acti->dwExcepFound, lock_acti->szFileName);
    }
}

VOID ndp::DiresVector(DiresVec *pListVec) {
    DiresVec::iterator iter;
    for(iter=pListVec->begin(); pListVec->end()!=iter; ++iter) {
_LOG_DEBUG(_T("directory:%s"), (*iter));
    }
}