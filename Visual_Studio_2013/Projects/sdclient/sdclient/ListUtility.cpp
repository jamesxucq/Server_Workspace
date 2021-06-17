#include "StdAfx.h"

#include "FastUtility.h"
#include "DebugPrinter.h"
#include "ListUtility.h"

//
static STATUS_METHOD HandleSlowFolder(TreeDB *lattdb, struct attent *atte) {
    struct attent vbuf;
    STATUS_METHOD statm = ES_UNUSE;
    LONGLONG ftcomp = 0x00;
    //
    DWORD key_len = _tcslen(atte->file_name) << 1;
    if(0 < lattdb->get((const char *)atte->file_name, key_len, (char *)&vbuf, sizeof(struct attent))) {
// _LOG_DEBUG(_T("file1:%s file2:%s"), atte->file_name, vbuf.file_name);
		if(!ATTRB_IS_DIRECTORY(vbuf.action_type)) statm = ES_DELETE; // is file
		else statm = ES_UNUSE;
		lattdb->remove((const char *)atte->file_name, key_len);
    } else statm = ES_GET;
// _LOG_DEBUG(_T("ftcomp:%lld statm:%u"), ftcomp, statm);
    //
    return statm;
}

static STATUS_METHOD HandleSlowFile(TreeDB *lattdb, struct attent *atte) {
    struct attent vbuf;
    STATUS_METHOD statm = ES_UNUSE;
    LONGLONG ftcomp = 0x00;
    //
    DWORD key_len = _tcslen(atte->file_name) << 1;
    if(0 < lattdb->get((const char *)atte->file_name, key_len, (char *)&vbuf, sizeof(struct attent))) {
// _LOG_DEBUG(_T("file1:%s file2:%s"), atte->file_name, vbuf.file_name);
// TCHAR time_str[32];
// _LOG_DEBUG(_T("clent:%s|%llu"), timcon::ftim_unis(time_str, &vbuf.last_write), vbuf.last_write);
// _LOG_DEBUG(_T("server:%s|%llu"), timcon::ftim_unis(time_str, &atte->last_write), atte->last_write);
		if(ATTRB_IS_DIRECTORY(vbuf.action_type)) statm = ES_DELETE;
		else {
			ftcomp = timcon::ftim_comp(&vbuf.last_write, &atte->last_write);
			if(!ftcomp) {
				if(atte->file_size == vbuf.file_size) statm = ES_UNUSE;
				else statm = ES_PSYNC;
			} else if(0x00 > ftcomp) {
				atte->file_size = vbuf.file_size;
				memcpy(&atte->last_write, &vbuf.last_write, sizeof(FILETIME));
				statm = ES_GSYNC;
			} else if(0x00 < ftcomp) statm = ES_PSYNC;
		}
		lattdb->remove((const char *)atte->file_name, key_len);
	} else statm = ES_GET;
// _LOG_DEBUG(_T("ftcomp:%lld statm:%u"), ftcomp, statm);
    //
    return statm;
}


void PrinSlowVec(HANDLE hsatt, TreeDB *lattdb) {  // for test
    char *kbuf;
    const char *vbuf;
    size_t key_len, vsiz;
    DB::Cursor* cursor = lattdb->cursor();
    cursor->jump();
    while (kbuf = cursor->get(&key_len, &vbuf, &vsiz, true)) {
        struct attent *p = (struct attent*)vbuf;
_LOG_DEBUG(_T(":%X :%llu :%s"), p->action_type, p->last_write, p->file_name);
    }
    delete cursor;
 _LOG_DEBUG(_T("------------"));
    //
    struct attent atte;
    DWORD dwResult = SetFilePointer(hsatt, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ;
    while(!NFilesVec::ReadNode(&atte, hsatt)) {
_LOG_DEBUG(_T(":%X :%llu :%s"), atte.action_type, atte.last_write, atte.file_name);
    }
}

DWORD ListUtility::OptimizeSlowVec(struct TasksCache *slowvec, HANDLE hsatt, TreeDB *lattdb) {
// PrinSlowVec(hsatt, lattdb); // for test
    // 慢同步以服务器为准
    unsigned __int64 qwFileSize = NFileUtility::FileSizeHandle(hsatt);
    DWORD dwResult = SetFilePointer(hsatt, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!qwFileSize && !lattdb->count()) return ((DWORD)-1);
    //
    char *kbuf;
    const char *vbuf;
    size_t key_len, vsiz;
    if(!qwFileSize) {
_LOG_DEBUG(_T("server space is zero."));
        // traverse records
        DB::Cursor* cursor = lattdb->cursor();
        cursor->jump();
        while (kbuf = cursor->get(&key_len, &vbuf, &vsiz, true)) {
_LOG_DEBUG(_T("optimize:%s type:%08X"), ((struct attent*)vbuf)->file_name, ((struct attent*)vbuf)->action_type);
            NTasksVec::AppNewNode(&(slowvec)->ltskatt, (struct attent*)vbuf, 
				ATTRB_IS_DIRECTORY(((struct attent*)vbuf)->action_type)|DELE);
            delete kbuf;
        }
        delete cursor;
        return 0x00;
    }
    //
    struct attent atte;
    if(!lattdb->count()) {
_LOG_DEBUG(_T("local space is zero."));
        while(!NFilesVec::ReadNode(&atte, hsatt)) {
            // (slowvec)->ltskatt.push_back(*iter);
_LOG_DEBUG(_T("optimize:%s type:%08X"), atte.file_name, atte.action_type);
            NTasksVec::AppNewNode(&(slowvec)->ltskatt, &atte);
        }
        return 0x00;
    }
    //
    STATUS_METHOD statm = ES_UNUSE;
// _LOG_DEBUG(_T("local server space not zero."));
    while(!NFilesVec::ReadNode(&atte, hsatt)) {
		if(ATTRB_IS_DIRECTORY(atte.action_type)) {
			statm = HandleSlowFolder(lattdb, &atte);
			switch (statm) {
			case ES_PUT:
				// (slowvec)->stskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->stskatt, &atte);
				break;
			case ES_GET:
				// (slowvec)->ltskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->ltskatt, &atte);
				break;
			case ES_DELETE:
				atte.action_type = DELE;
				// (slowvec)->ltskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->ltskatt, &atte);
				//
				atte.action_type = RECURSIVE | DIRECTORY;
				// (slowvec)->ltskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->ltskatt, &atte);
				break;
			case ES_UNUSE:
				// NFilesVec::DeleNode(atte);
				break;
			}
		} else { //
			statm = HandleSlowFile(lattdb, &atte);
			switch (statm) {
			case ES_GSYNC:
				atte.action_type = MODIFY;
				// (slowvec)->stskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->stskatt, &atte);
				break;
			case ES_PSYNC:
				atte.action_type = MODIFY;
				// (slowvec)->ltskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->ltskatt, &atte);
				break;
			case ES_PUT:
				// (slowvec)->stskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->stskatt, &atte);
				break;
			case ES_GET:
				// (slowvec)->ltskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->ltskatt, &atte);
				break;
			case ES_DELETE:
				atte.action_type = DELE | DIRECTORY;
				// (slowvec)->ltskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->ltskatt, &atte);
				//
				atte.action_type = RECURSIVE;
				// (slowvec)->ltskatt.push_back(atte);
				NTasksVec::AppNewNode(&(slowvec)->ltskatt, &atte);
				break;
			case ES_UNUSE:
				// NFilesVec::DeleNode(atte);
				break;
			}
		}
    }
    // // ES_DELETE:
    // traverse records
    DB::Cursor* cursor = lattdb->cursor();
    cursor->jump();
    while (kbuf = cursor->get(&key_len, &vbuf, &vsiz, true)) {
        NTasksVec::AppNewNode(&(slowvec)->ltskatt, (struct attent*)vbuf, 
			ATTRB_IS_DIRECTORY(((struct attent*)vbuf)->action_type)|DELE);
        delete kbuf;
    }
    delete cursor;
    //
    return 0x00;
}

//
static DWORD HandleFastLocalVec(struct TasksCache *fastvec, FilesVec *sattent, FilesVec *lattent) {
    FilesVec::iterator iter;
	FastKHmap mServKHmap;
	struct attent *pNode;
	//
	NFastUtili::BuildFastKHmap(&mServKHmap, sattent);
    for(iter = lattent->begin(); lattent->end() != iter; ) {
		pNode = (*iter);
		if(ATTRB_IS_DIRECTORY(pNode->action_type)) {
			NEWN_DELN(&(fastvec->stskatt), pNode)
            iter = lattent->erase(iter);
		} else { //
			if(NFastUtili::FindFromHmap(&mServKHmap, pNode)){
				if(EXIST & pNode->action_type) ++iter;
				++iter;
			} else {
				switch (FILE_REAL_STAT(pNode->action_type)) {
				case ADDI:
				case MODIFY:
				case DELE:
					NEWN_DELN(&(fastvec->stskatt), pNode)
					iter = lattent->erase(iter);
					break;
				case EXIST:
					NEWN_DELN(&(fastvec->stskatt), pNode)
					iter = lattent->erase(iter);
					// COPY MOVE
					NEWN_DELN(&(fastvec->stskatt), (*iter))
					iter = lattent->erase(iter);
					break;
				default:
					++iter;
					break;
				}
			}
		}
    }
    //
	NFastUtili::DeleFastKHmap(&mServKHmap);
    return 0x00;
}


static DWORD HandleFastServVec(struct TasksCache *fastvec, FilesVec *lattent, FilesVec *sattent) {
    FilesVec::iterator iter;
	FastKHmap mLocaKHmap;
	struct attent *pNode;
    //
	NFastUtili::BuildFastKHmap(&mLocaKHmap, lattent);
    for(iter = sattent->begin(); sattent->end() != iter; ) {
		pNode = (*iter);
		if(ATTRB_IS_DIRECTORY(pNode->action_type)) {
			NEWN_DELN(&(fastvec->ltskatt), pNode)
			iter = sattent->erase(iter);
		} else {
			if(NFastUtili::FindFromHmap(&mLocaKHmap, pNode)) {
				if(EXIST & pNode->action_type) ++iter;
				++iter;
			} else {
			    switch (FILE_REAL_STAT(pNode->action_type)) {
				case ADDI:
	            case MODIFY:
		        case DELE:
					NEWN_DELN(&(fastvec->ltskatt), pNode)
				    iter = sattent->erase(iter);
					break;
	            case EXIST:
					NEWN_DELN(&(fastvec->ltskatt), pNode)
				    iter = sattent->erase(iter);
					// COPY MOVE
					NEWN_DELN(&(fastvec->ltskatt), (*iter))
				    iter = sattent->erase(iter);
					break;
	            default:
		            ++iter;
			        break;
				}
			}
		}
    }
    //
	NFastUtili::DeleFastKHmap(&mLocaKHmap);
    return 0x00;
}

//
static DWORD HandleFastConflictVec(struct TasksCache *fastvec, FilesVec *sattent, FilesVec *lattent) {
	FastKHmap mServKHmap;
	struct FastKNode *pFoundKNode;
    FilesVec::iterator loca_iter;
    struct attent *loca_anod, *aexis;
    struct attent *loca_cnod, *serv_cnod;
    //
	NFastUtili::BuildFastKHmap(&mServKHmap, sattent);
    for(loca_iter = lattent->begin(); lattent->end() != loca_iter; ++loca_iter) {
        //
        loca_cnod = *loca_iter; // 得到本地修改文件
		if(EXIST & loca_cnod->action_type) loca_anod = *(++loca_iter);
		// 得到服务器端和本地修改相同的文件
		pFoundKNode = NFastUtili::FindFromHmap(&mServKHmap, loca_cnod);
        //
		serv_cnod = NFastUtili::FindNextValue(pFoundKNode);
        NFilesVec::DeleFromVec(sattent, serv_cnod); // 把文件从vector中删除
        while(serv_cnod) {
            //
            if(ADDI&loca_cnod->action_type && ADDI&serv_cnod->action_type) {
				// _LOG_DEBUG(_T("last_write:%llu"), atte->last_write);
_LOG_DEBUG(_T("loca_cnod:%llu serv_cnod:%llu"), loca_cnod->last_write, serv_cnod->last_write);
				if(timcon::ftim_comp(&loca_cnod->last_write, &serv_cnod->last_write) > 0) {
					NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod, MODIFY);
				} else NEWT_DELN(&(fastvec->ltskatt), serv_cnod, MODIFY)
            } else if(ADDI&loca_cnod->action_type && MODIFY&serv_cnod->action_type) { // modify by james 20120508
				NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
            } else if(ADDI&loca_cnod->action_type && DELE&serv_cnod->action_type) { // modify by james 20120508
                NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
            } else if(ADDI&loca_cnod->action_type && EXIST&serv_cnod->action_type) { // modify by james 20120508
                aexis = NFilesVec::FindActioNode(sattent, serv_cnod);
				NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
                NFilesVec::DeleFromVec(sattent, aexis);
				NEWN_DELN(&(fastvec->ltskatt), aexis)
//
                NTasksVec::AppNewNode(&(fastvec)->stskatt, aexis, MODIFY);
            } else if(ADDI&loca_cnod->action_type && COPY&serv_cnod->action_type) {
                aexis = NFilesVec::FindExistNode(sattent, serv_cnod);
                NFilesVec::DeleFromVec(sattent, aexis);
				NEWN_DELN(&(fastvec->ltskatt), aexis)
				NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
//
                NTasksVec::AppNewNode(&(fastvec)->stskatt, loca_cnod, MODIFY);
            } else if(ADDI&loca_cnod->action_type && MOVE&serv_cnod->action_type) {
                aexis = NFilesVec::FindExistNode(sattent, serv_cnod);
                NFilesVec::DeleFromVec(sattent, aexis);
                NEWN_DELN(&(fastvec->ltskatt), aexis)
                NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
//
                NTasksVec::AppNewNode(&(fastvec)->stskatt, loca_cnod, MODIFY);
            } else if(MODIFY&loca_cnod->action_type && ADDI&serv_cnod->action_type) { // modify by james 20151026
                NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod);
            } else if(MODIFY&loca_cnod->action_type && MODIFY&serv_cnod->action_type) {
                if(timcon::ftim_comp(&loca_cnod->last_write, &serv_cnod->last_write) > 0)
                    NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod);
                else NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
            } else if(MODIFY&loca_cnod->action_type && DELE&serv_cnod->action_type) {
				if(timcon::ftim_comp(&loca_cnod->last_write, &serv_cnod->last_write) > 0)
                    NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod, ADDI);
				else NEWN_DELN(&(fastvec->ltskatt), serv_cnod);
            } else if(MODIFY&loca_cnod->action_type && EXIST&serv_cnod->action_type) {
                aexis = NFilesVec::FindActioNode(sattent, serv_cnod);
                NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
                NFilesVec::DeleFromVec(sattent, aexis);
                NEWN_DELN(&(fastvec->ltskatt), aexis)
//
				NTasksVec::AppNewNode(&(fastvec)->stskatt, aexis, MODIFY);
            } else if(MODIFY&loca_cnod->action_type && COPY&serv_cnod->action_type) { // modify by james 20120508
                aexis = NFilesVec::FindExistNode(sattent, serv_cnod);
//
                NFilesVec::DeleFromVec(sattent, aexis);
                NFilesVec::DeleNode(aexis);
                NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod);
            } else if(MODIFY&loca_cnod->action_type && MOVE&serv_cnod->action_type) { // modify by james 20120508
                aexis = NFilesVec::FindExistNode(sattent, serv_cnod);
//
                NFilesVec::DeleFromVec(sattent, aexis);
                NFilesVec::DeleNode(aexis);
                NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod);
            } else if(DELE&loca_cnod->action_type && ADDI&serv_cnod->action_type) { // modify by james 20120508
                NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
            } else if(DELE&loca_cnod->action_type && MODIFY&serv_cnod->action_type) {
				if(timcon::ftim_comp(&loca_cnod->last_write, &serv_cnod->last_write) > 0)
                    NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod);
				else NEWT_DELN(&(fastvec->ltskatt), serv_cnod, ADDI)
            } else if(DELE&loca_cnod->action_type && DELE&serv_cnod->action_type) {
                NFilesVec::DeleNode(serv_cnod);
            } else if(DELE&loca_cnod->action_type && EXIST&serv_cnod->action_type) {
                if(timcon::ftim_comp(&loca_cnod->last_write, &serv_cnod->last_write) > 0) {
                    aexis = NFilesVec::FindActioNode(sattent, serv_cnod);
                    NFilesVec::DeleFromVec(sattent, aexis);
					NEWT_DELN(&(fastvec->stskatt), aexis, DELE)
                } else {
                    aexis = NFilesVec::FindActioNode(sattent, serv_cnod);
                    NFilesVec::DeleFromVec(sattent, aexis);
					NEWT_DELN(&(fastvec->ltskatt), aexis, ADDI)
                }
                NFilesVec::DeleNode(serv_cnod);
            } else if(DELE&loca_cnod->action_type && COPY&serv_cnod->action_type) { // modify by james 20120508
                aexis = NFilesVec::FindExistNode(sattent, serv_cnod);
                NFilesVec::DeleFromVec(sattent, aexis);
                NFilesVec::DeleNode(aexis);
//
                NEWT_DELN(&(fastvec->ltskatt), serv_cnod, ADDI)
            } else if(DELE&loca_cnod->action_type && MOVE&serv_cnod->action_type) { // modify by james 20120508
                aexis = NFilesVec::FindExistNode(sattent, serv_cnod);
                NFilesVec::DeleFromVec(sattent, aexis);
                NFilesVec::DeleNode(aexis);
//
                NEWT_DELN(&(fastvec->ltskatt), serv_cnod, ADDI)
            }
            // else if(EXIST&loca_cnod->action_type && ADDI&serv_cnod->action_type) { // modify by james 20120508
            // }
            else if(EXIST&loca_cnod->action_type && MODIFY&serv_cnod->action_type) {
                NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod);
				NTasksVec::AppNewNode(&(fastvec->stskatt), loca_anod);
//
                _tcscpy_s(serv_cnod->file_name, MAX_PATH, loca_anod->file_name);
				NEWT_DELN(&(fastvec->ltskatt), serv_cnod, MODIFY)
            } else if(EXIST&loca_cnod->action_type && DELE&serv_cnod->action_type) {
				NTasksVec::AppNewNode(&(fastvec->ltskatt), loca_anod, DELE);
//
                NFilesVec::DeleNode(serv_cnod);
            } else if(EXIST&loca_cnod->action_type && EXIST&serv_cnod->action_type) {
                if(timcon::ftim_comp(&loca_cnod->last_write, &serv_cnod->last_write) > 0) {
                    aexis = NFilesVec::FindActioNode(sattent, serv_cnod);
                    NFilesVec::DeleFromVec(sattent, aexis);
					NEWT_DELN(&(fastvec->stskatt), aexis, EXIST)
//
                    NTasksVec::AppNewNode(&(fastvec->stskatt), loca_anod);
//
                    NFilesVec::DeleNode(serv_cnod);
                } else {
					NTasksVec::AppNewNode(&(fastvec->ltskatt), loca_anod, EXIST);
//
                    aexis = NFilesVec::FindActioNode(sattent, serv_cnod);
                    NFilesVec::DeleFromVec(sattent, aexis);
					NEWN_DELN(&(fastvec->ltskatt), aexis)
//
                    NFilesVec::DeleNode(serv_cnod);
                }
            } else if(EXIST&loca_cnod->action_type && COPY&serv_cnod->action_type) { // modify by james 20120508
                NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod);
                NTasksVec::AppNewNode(&(fastvec->stskatt), loca_anod);
//
                aexis = NFilesVec::FindExistNode(sattent, serv_cnod);
                NFilesVec::DeleFromVec(sattent, aexis);
				NEWN_DELN(&(fastvec->ltskatt), aexis)
				NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
            } else if(EXIST&loca_cnod->action_type && MOVE&serv_cnod->action_type) { // modify by james 20120508
                NTasksVec::AppNewNode(&(fastvec->stskatt), loca_cnod);
                NTasksVec::AppNewNode(&(fastvec->stskatt), loca_anod);
//
                aexis = NFilesVec::FindExistNode(sattent, serv_cnod);
                NFilesVec::DeleFromVec(sattent, aexis);
				NEWN_DELN(&(fastvec->ltskatt), aexis);
				NEWN_DELN(&(fastvec->ltskatt), serv_cnod)
            }
            // else if(NEW&loca_cnod->action_type && ADDI&serv_cnod->action_type) { // modify by james 20120508
            // }
            // else if(NEW&loca_cnod->action_type && MODIFY&serv_cnod->action_type) { // modify by james 20120508
            // }
            // else if(NEW&loca_cnod->action_type && DELE&serv_cnod->action_type) { // modify by james 20120508
            // }
            // else if(NEW&loca_cnod->action_type && EXIST&serv_cnod->action_type) { // modify by james 20120508
            // }
            // else if(NEW&loca_cnod->action_type && NEW&serv_cnod->action_type) { // modify by james 20120508
            // }
            //
            // serv_cnod = NFilesVec::FindFromVecEx(stskatt, NULL);
			serv_cnod = NFastUtili::FindNextValue(NULL);
        }
		//
		NFastUtili::DeleListHmap(&mServKHmap, pFoundKNode);
    }
// _LOG_DEBUG(_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
	NFastUtili::DeleFastKHmap(&mServKHmap);
    return 0x00;
}

//
static DWORD HandleColiServVec(FilesVec *sattent, FileColiHmap *pColiHmap) {
	FilesVec::iterator iter;
	FileColiHmap::iterator aite;
	struct attent *pNode;
//
    for(iter = sattent->begin(); sattent->end() != iter; ) {
		pNode = (*iter);
		if(ATTRB_IS_DIRECTORY(pNode->action_type)) ++iter;
		else { //
// _LOG_DEBUG(_T("### 1"));
			aite = pColiHmap->find(pNode->file_name);
			if((pColiHmap->end()!=aite) && !COFT_HAND_DONE((aite->second)->dwExcepValue)) {
// _LOG_DEBUG(_T("### 2"));
				if(COFT_ATTR_DONE((aite->second)->dwExcepValue)) { // excep
// _LOG_DEBUG(_T("### 3"));
					switch (FILE_REAL_STAT(pNode->action_type)) {
					case ADDI:
					case MODIFY:
					case DELE:
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = sattent->erase(iter);
						break;
					case EXIST:
						iter = sattent->erase(iter);
						// COPY MOVE
						NFilesVec::AppNewNode(sattent, (*iter), MODIFY);
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = sattent->erase(iter);
						break;
					default:
						++iter;
						break;
					}					
				} else { // success
// _LOG_DEBUG(_T("### 4"));
					switch (FILE_REAL_STAT(pNode->action_type)) {
					case ADDI:
					case MODIFY:
						NFilesVec::DeleNode(pNode);
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = sattent->erase(iter);
						break;
					case DELE:
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = sattent->erase(iter);
						break;
					case EXIST:
						iter = sattent->erase(iter);
						// COPY MOVE
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = sattent->erase(iter);
						break;
					default:
						++iter;
						break;
					}		
				}
			} else ++iter;
		}
    }
//
    return 0x00;
}

static DWORD HandleColiLocalVec(FilesVec *lattent, FileColiHmap *pColiHmap, FilesVec *sattent) {
	FilesVec::iterator iter;
	FileColiHmap::iterator aite;
	struct attent *pNode;
//
    for(iter = lattent->begin(); lattent->end() != iter; ) {
		pNode = (*iter);
		if(ATTRB_IS_DIRECTORY(pNode->action_type)) ++iter;
		else { //
// _LOG_DEBUG(_T("### 5"));
			aite = pColiHmap->find(pNode->file_name);
			if((pColiHmap->end()!=aite) && !COFT_HAND_DONE((aite->second)->dwExcepValue)) {
// _LOG_DEBUG(_T("### 6"));
				if(COFT_ATTR_DONE((aite->second)->dwExcepValue)) { // excep
// _LOG_DEBUG(_T("### 7"));
					switch (FILE_REAL_STAT(pNode->action_type)) {
					case ADDI:
					case MODIFY:
						SETF_REAL_STAT(pNode->action_type, MODIFY);
						pNode->file_size = (aite->second)->qwFileSize;
						pNode->last_write = (aite->second)->ftLastWrite;
						sattent->push_back(pNode);
//
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = lattent->erase(iter);
						break;
					case DELE:
						SETF_REAL_STAT(pNode->action_type, ADDI);
						pNode->file_size = (aite->second)->qwFileSize;
						pNode->last_write = (aite->second)->ftLastWrite;
						sattent->push_back(pNode);
//
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = lattent->erase(iter);
						break;
					case EXIST:
						SETF_REAL_STAT(pNode->action_type, ADDI);
						pNode->file_size = (aite->second)->qwFileSize;
						pNode->last_write = (aite->second)->ftLastWrite;
						sattent->push_back(pNode);
						iter = lattent->erase(iter);
						// COPY MOVE
						SETF_REAL_STAT((*iter)->action_type, ADDI);
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = lattent->erase(iter);
						break;
					default:
						++iter;
						break;
					}
				} else {
// _LOG_DEBUG(_T("### 8"));
					switch (FILE_REAL_STAT(pNode->action_type)) {
					case ADDI:
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = lattent->erase(iter);
						break;
					case MODIFY:
						SETF_REAL_STAT(pNode->action_type, ADDI);
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = lattent->erase(iter);
						break;
					case DELE:
						NFilesVec::DeleNode(pNode);
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = lattent->erase(iter);
						break;
					case EXIST:
						NFilesVec::DeleNode(pNode);
						iter = lattent->erase(iter);
						// COPY MOVE
						SETF_REAL_STAT((*iter)->action_type, ADDI);
						SECF_HAND_SUCCE((aite->second)->dwExcepValue);
						iter = lattent->erase(iter);
						break;
					default:
						++iter;
						break;
					}
				}
			} else ++iter;
		}
    }
//
    return 0x00;
}

static DWORD HandleDrieColiVec(FilesVec *lattent, FilesVec *sattent, FileColiVec *pColiVec) {
	FileColiVec::iterator aite;
	FilesVec::iterator iter;
	struct attent *pNode;
//
	DWORD path_size;
	for(iter = lattent->begin(); lattent->end() != iter; ++iter) {
// _LOG_DEBUG(_T("### 9"));
		pNode = (*iter);
		if(ATTRB_IS_DIRECTORY(pNode->action_type) && (DELE&pNode->action_type)) {
			path_size = _tcslen(pNode->file_name);
// _LOG_DEBUG(_T("### A"));
		    for(aite = pColiVec->begin(); pColiVec->end() != aite; ++aite) {
				if(COFT_HAND_DONE((*aite)->dwExcepValue)) 
					continue;
				if(!_tcsncmp((*aite)->szFileName, pNode->file_name, path_size))
					SECF_HAND_SUCCE((*aite)->dwExcepValue);
// _LOG_DEBUG(_T("### B"));
			}
		}
	}
//
	for(iter = sattent->begin(); sattent->end() != iter; ++iter) {
// _LOG_DEBUG(_T("### C"));
		pNode = (*iter);
		if(ATTRB_IS_DIRECTORY(pNode->action_type) && (DELE&pNode->action_type)) {
			path_size = _tcslen(pNode->file_name);
// _LOG_DEBUG(_T("### D"));
		    for(aite = pColiVec->begin(); pColiVec->end() != aite; ++aite) {
				if(COFT_HAND_DONE((*aite)->dwExcepValue)) 
					continue;
				if(!_tcsncmp((*aite)->szFileName, pNode->file_name, path_size))
					SECF_HAND_SUCCE((*aite)->dwExcepValue);
// _LOG_DEBUG(_T("### E"));
			}
		}
	}
	//
	return 0x00;
}

DWORD ListUtility::OptimizeColiVec(FilesVec *sattent, FilesVec *lattent, FileColiVec *pColiVec) {
	FileColiHmap mColiHmap;
    if(pColiVec->empty() || (sattent->empty()&&lattent->empty())) return 0x00;
	HandleDrieColiVec(lattent, sattent, pColiVec);
	//
	NFileColi::BuildColiHmap(&mColiHmap, pColiVec);
	HandleColiLocalVec(lattent, &mColiHmap, sattent);
	HandleColiServVec(sattent, &mColiHmap);
	NFileColi::DeleColiHmap(&mColiHmap);
_LOG_DEBUG(_T("----------------------- coli tasks vector: -----------------------"));
    return 0x00;
}

DWORD ListUtility::OptimizeFastVec(struct TasksCache *fastvec, FilesVec *sattent, FilesVec *lattent) {
    if(sattent->empty() && lattent->empty()) return 0x00;
    if(HandleFastLocalVec(fastvec, sattent, lattent)) return ((DWORD)-1);
_LOG_DEBUG(_T("----------------------- local tasks vector: -----------------------"));
ndp::TasksCache(fastvec);
    if(HandleFastServVec(fastvec, lattent, sattent)) return ((DWORD)-1);
_LOG_DEBUG(_T("----------------------- serv tasks vector: -----------------------"));
ndp::TasksCache(fastvec);
    if(!sattent->empty() && !lattent->empty()) {
_LOG_DEBUG(_T("hand fast conflict."));
		if(HandleFastConflictVec(fastvec, sattent, lattent)) {
_LOG_WARN(_T("hand fast conflict error!"));
            return ((DWORD)-1);
		}
    }
_LOG_DEBUG(_T("----------------------- atte tasks vector: -----------------------"));
ndp::TasksCache(fastvec);
    return 0x00;
}

CTRANSWorker *ListUtility::CreateWorker(DWORD *cvalue, struct LIST_ARGUMENT *pListArgu) {
    CTRANSWorker *pListWorker = NTRANSWorker::Factory(&pListArgu->tSeionArgu, &pListArgu->tSockeArgu);
    if (!pListWorker) {
        NTRANSWorker::DestroyObject(pListWorker);
        *cvalue = LIST_VALUE_NETWORK;
        return NULL;
    }
    // 生成工作记录,初始会话
    int proto_value = PROTO_FAULT;
    if(proto_value = pListWorker->Initial()) {
        NTRANSWorker::DestroyObject(pListWorker);
        *cvalue = PROTO_NETWORK == proto_value? LIST_VALUE_NETWORK: LIST_VALUE_FAILED;
        pListWorker = NULL;
    }
    return pListWorker;
}

void ListUtility::DropWorker(CTRANSWorker *pListWorker) {
    if(pListWorker)
        NTRANSWorker::DestroyObject(pListWorker);
}

void ListUtility::DestroyWorker(CTRANSWorker *pListWorker, DWORD dwFinalType) {
    if(pListWorker) {
        pListWorker->Final(dwFinalType);
        NTRANSWorker::DestroyObject(pListWorker);
    }
}

//
int ListUtility::FindExceptTNode(TNodeVec *pTNodeV) { // 0:finish -1:error n:n
	struct TaskNode tReadNode;
	DWORD dwFileFinish = ERR_SUCCESS;
	int iFindValue = 0;
	//
	objTaskList.SetNodePoint(0);
	while(0 < objTaskList.ReadNode(&tReadNode)) {
		if(FILE_TASK_BEGIN & tReadNode.control_code) 
			dwFileFinish = ERR_SUCCESS;
			//
		if(ONLY_TASK_UNDONE(tReadNode.success_flags))
			dwFileFinish |= ERR_EXCEPTION;
			//
		if((FILE_TASK_END&tReadNode.control_code) && dwFileFinish) {
			if(!NTNodeVec::AppNewNode(pTNodeV, &tReadNode)) {
				iFindValue = -1;
			}	
_LOG_DEBUG(_T("find tasks except.tReadNode node_inde:%d success_flags:%08x"), tReadNode.node_inde, tReadNode.success_flags);
_LOG_DEBUG(_T("tReadNode szFileName1:%s control_code:%08x excepti:%08x"), tReadNode.szFileName1, tReadNode.control_code, tReadNode.excepti);
		}
	}
	//
_LOG_DEBUG(_T("iFindValue:%d"), iFindValue);
	return iFindValue;
}

//
//
VOID ProduceUtility::AddFileMovecNode(struct taskatt *exist, struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_MOVE;
    tWriteNode.success_flags = TRANSMIT_UNDONE|OWRTMRVE_UNDONE;
    //
    _tcscpy_s(tWriteNode.szFileName1, exist->file_name);
    _tcscpy_s(tWriteNode.szFileName2, katt->file_name);
	tWriteNode.file_size = exist->file_size;
    tWriteNode.last_write = exist->last_write;
    tWriteNode.reserved = exist->reserved;
	tWriteNode.isdire = FALSE;
    //
    objTaskList.WriteNode(&tWriteNode);
}

VOID ProduceUtility::AddFileMovesNode(struct taskatt *exist, struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_MOVE;
    tWriteNode.success_flags = TRANSMIT_UNDONE|VERIFY_UNDONE;
    //
    _tcscpy_s(tWriteNode.szFileName1, exist->file_name);
    _tcscpy_s(tWriteNode.szFileName2, katt->file_name);
	tWriteNode.file_size = exist->file_size;
    tWriteNode.last_write = exist->last_write;
    tWriteNode.reserved = exist->reserved;
	tWriteNode.isdire = FALSE;
    //
    objTaskList.WriteNode(&tWriteNode);
}

VOID ProduceUtility::AddFileCopycNode(struct taskatt *exist, struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_COPY;
    tWriteNode.success_flags = TRANSMIT_UNDONE|OWRTMRVE_UNDONE; // modify by james 20140117
    //
    _tcscpy_s(tWriteNode.szFileName1, exist->file_name);
    _tcscpy_s(tWriteNode.szFileName2, katt->file_name);
	tWriteNode.file_size = exist->file_size;
    tWriteNode.last_write = exist->last_write;
    tWriteNode.reserved = exist->reserved;
	tWriteNode.isdire = FALSE;
    //
    objTaskList.WriteNode(&tWriteNode);

}

VOID ProduceUtility::AddFileCopysNode(struct taskatt *exist, struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_COPY;
    tWriteNode.success_flags = TRANSMIT_UNDONE|VERIFY_UNDONE; // modify by james 20140117
    //
    _tcscpy_s(tWriteNode.szFileName1, exist->file_name);
    _tcscpy_s(tWriteNode.szFileName2, katt->file_name);
	tWriteNode.file_size = exist->file_size;
    tWriteNode.last_write = exist->last_write;
    tWriteNode.reserved = exist->reserved;
	tWriteNode.isdire = FALSE;
    //
    objTaskList.WriteNode(&tWriteNode);

}

VOID ProduceUtility::AddFileDelecNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_DELETE;
	tWriteNode.success_flags = TRANSMIT_UNDONE|OWRTMRVE_UNDONE;
    //
    _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
	tWriteNode.file_size = katt->file_size;
    tWriteNode.last_write = katt->last_write;
    tWriteNode.reserved = katt->reserved;
	tWriteNode.isdire = FALSE;
    //
    objTaskList.WriteNode(&tWriteNode);
}

VOID ProduceUtility::AddFileDelesNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_DELETE;
	tWriteNode.success_flags = TRANSMIT_UNDONE;
    //
    _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
	tWriteNode.file_size = katt->file_size;
    tWriteNode.last_write = katt->last_write;
    tWriteNode.reserved = katt->reserved;
	tWriteNode.isdire = FALSE;
    //
    objTaskList.WriteNode(&tWriteNode);
}

/*
char success_flag;	// 0:success 1:undone
DWORD control_code;		/* add mod del list recu *
TCHAR szFileName[MAX_PATH];
off_t offset;
DWORD dwFileSize;
FILETIME ftLastWrite;		/* When the item was last modified *
DWORD dwReserved;  /* filehigh *
*/
VOID ProduceUtility::AddFileGetNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    DWORD chunk_tatol;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.success_flags = TRANSMIT_UNDONE|VERIFY_UNDONE|OWRTMRVE_UNDONE;
    //
    if(katt->file_size <= CHUNK_SIZE) {
// _LOG_DEBUG(_T("katt->file_size:%lld <= CHUNK_SIZE:%d"), katt->file_size, CHUNK_SIZE); // disable by james 20130406
        tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
        tWriteNode.array_id = dwArrayID;
        tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_GET|RANGE_TYPE_FILE;
        NTRANSCache::CreatBuilderName(tWriteNode.builder_cache, katt->file_name, 0);
// _LOG_DEBUG( _T("create cache tWriteNode.builder_cache name is %s."), tWriteNode.builder_cache);
        _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
        tWriteNode.offset = 0;
        tWriteNode.build_length = (DWORD)katt->file_size;
		tWriteNode.file_size = katt->file_size;
        tWriteNode.last_write = katt->last_write;
        tWriteNode.reserved = katt->reserved;
		tWriteNode.isdire = FALSE;

        objTaskList.WriteNode(&tWriteNode);
    } else {
// _LOG_DEBUG(_T("katt->file_size:%lld > CHUNK_SIZE:%d"), katt->file_size, CHUNK_SIZE); // disable by james 20130406
        for(chunk_tatol = 0; chunk_tatol*(unsigned __int64)CHUNK_SIZE <= katt->file_size; chunk_tatol++) {
            tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
            tWriteNode.array_id = dwArrayID;
            if (!chunk_tatol) {
                tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|STYPE_GET|RANGE_TYPE_CHUNK;
                tWriteNode.build_length = CHUNK_SIZE;
            } else if((chunk_tatol + 1)*(unsigned __int64)CHUNK_SIZE > katt->file_size) {
                tWriteNode.control_code = dwTaskCode|FILE_TASK_END|STYPE_GET|RANGE_TYPE_FILE;
                tWriteNode.build_length = POW2N_MOD(katt->file_size, CHUNK_SIZE);
            } else {
                tWriteNode.control_code = dwTaskCode|STYPE_GET|RANGE_TYPE_CHUNK;
                tWriteNode.build_length = CHUNK_SIZE;
            }
            NTRANSCache::CreatBuilderName(tWriteNode.builder_cache, katt->file_name, chunk_tatol*(unsigned __int64)CHUNK_SIZE);
// _LOG_DEBUG( _T("create cache tWriteNode.builder_cache name is %s."), tWriteNode.builder_cache);
            _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
            tWriteNode.offset = chunk_tatol*(unsigned __int64)CHUNK_SIZE;
			tWriteNode.file_size = katt->file_size;
            tWriteNode.last_write = katt->last_write;
            tWriteNode.reserved = katt->reserved;
			tWriteNode.isdire = FALSE;
            //
            objTaskList.WriteNode(&tWriteNode);
        }
    }
    //
}

VOID ProduceUtility::AddFilePutNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    DWORD chunk_tatol;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.success_flags = TRANSMIT_UNDONE|VERIFY_UNDONE;
    //
    if(katt->file_size <= CHUNK_SIZE) {
// _LOG_DEBUG(_T("katt->file_size:%lld <= CHUNK_SIZE:%d"), katt->file_size, CHUNK_SIZE); // disable by james 20130406
        tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
        tWriteNode.array_id = dwArrayID;
        tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_PUT|RANGE_TYPE_FILE;
        //
        _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
        tWriteNode.offset = 0;
        tWriteNode.build_length = (DWORD)katt->file_size;
		tWriteNode.file_size = katt->file_size;
        tWriteNode.last_write = katt->last_write;
        tWriteNode.reserved = katt->reserved;
		tWriteNode.isdire = FALSE;
        //
        objTaskList.WriteNode(&tWriteNode);
    } else {
// _LOG_DEBUG(_T("katt->file_size:%lld > CHUNK_SIZE:%d"), katt->file_size, CHUNK_SIZE); // disable by james 20130406
        for(chunk_tatol = 0; chunk_tatol*(unsigned __int64)CHUNK_SIZE <= katt->file_size; chunk_tatol++) {
            tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
            tWriteNode.array_id = dwArrayID;
            if (!chunk_tatol) {
                tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|STYPE_PUT|RANGE_TYPE_CHUNK;
                tWriteNode.build_length = CHUNK_SIZE;
            } else if((chunk_tatol + 1)*(unsigned __int64)CHUNK_SIZE > katt->file_size) {
                tWriteNode.control_code = dwTaskCode|FILE_TASK_END|STYPE_PUT|RANGE_TYPE_FILE;
                tWriteNode.build_length = POW2N_MOD(katt->file_size, CHUNK_SIZE);
            } else {
                tWriteNode.control_code = dwTaskCode|STYPE_PUT|RANGE_TYPE_CHUNK;
                tWriteNode.build_length = CHUNK_SIZE;
            }
            _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
            tWriteNode.offset = chunk_tatol*(unsigned __int64)CHUNK_SIZE;
			tWriteNode.file_size = katt->file_size;
            tWriteNode.last_write = katt->last_write;
            tWriteNode.reserved = katt->reserved;
			tWriteNode.isdire = FALSE;
            //
            objTaskList.WriteNode(&tWriteNode);
        }
    }
    //
}

VOID ProduceUtility::FilePutSynchNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    DWORD chunk_tatol;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.success_flags = TRANSMIT_UNDONE|VERIFY_UNDONE|OWRTMRVE_UNDONE;
    //
    if(katt->file_size <= CHUNK_SIZE) {
// _LOG_DEBUG(_T("katt->file_size:%lld <= CHUNK_SIZE:%d"), katt->file_size, CHUNK_SIZE); // disable by james 20130406
        tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
        tWriteNode.array_id = dwArrayID;
        tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_PSYNC|RANGE_TYPE_FILE;
        NTRANSCache::CreatBuilderName(tWriteNode.builder_cache, katt->file_name, 0);
// _LOG_DEBUG( _T("create cache tWriteNode.builder_cache name is %s."), tWriteNode.builder_cache);
        _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
        tWriteNode.offset = 0;
        tWriteNode.build_length = (DWORD)katt->file_size;
		tWriteNode.file_size = katt->file_size;
        tWriteNode.last_write = katt->last_write;
        tWriteNode.reserved = katt->reserved;
		tWriteNode.isdire = FALSE;
        //
        objTaskList.WriteNode(&tWriteNode);
    } else {
// _LOG_DEBUG(_T("katt->file_size:%lld > CHUNK_SIZE:%d"), katt->file_size, CHUNK_SIZE); // disable by james 20130406
        for(chunk_tatol = 0; chunk_tatol*(unsigned __int64)CHUNK_SIZE <= katt->file_size; chunk_tatol++) {
            tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
            tWriteNode.array_id = dwArrayID;
            if (!chunk_tatol) {
                tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|STYPE_PSYNC|RANGE_TYPE_CHUNK;
                tWriteNode.build_length = CHUNK_SIZE;
            } else if((chunk_tatol + 1)*(unsigned __int64)CHUNK_SIZE > katt->file_size) {
                tWriteNode.control_code = dwTaskCode|FILE_TASK_END|STYPE_PSYNC|RANGE_TYPE_FILE;
                tWriteNode.build_length = POW2N_MOD(katt->file_size, CHUNK_SIZE);
            } else {
                tWriteNode.control_code = dwTaskCode|STYPE_PSYNC|RANGE_TYPE_CHUNK;
                tWriteNode.build_length = CHUNK_SIZE;
            }
            NTRANSCache::CreatBuilderName(tWriteNode.builder_cache, katt->file_name, chunk_tatol*(unsigned __int64)CHUNK_SIZE);
// _LOG_DEBUG( _T("create cache tWriteNode.builder_cache name is %s."), tWriteNode.builder_cache);
            _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
            tWriteNode.offset = chunk_tatol*(unsigned __int64)CHUNK_SIZE;
			tWriteNode.file_size = katt->file_size;
            tWriteNode.last_write = katt->last_write;
            tWriteNode.reserved = katt->reserved;
			tWriteNode.isdire = FALSE;
            //
            objTaskList.WriteNode(&tWriteNode);
        }
    }
    //
}

VOID ProduceUtility::FileGetSynchNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    DWORD chunk_tatol;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.success_flags = TRANSMIT_UNDONE|VERIFY_UNDONE;
    //
    if(katt->file_size <= CHUNK_SIZE) {
// _LOG_DEBUG(_T("katt->file_size:%lld <= CHUNK_SIZE:%d"), katt->file_size, CHUNK_SIZE); // disable by james 20130406
        tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
        tWriteNode.array_id = dwArrayID;
        tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_GSYNC|RANGE_TYPE_FILE;
        //
        _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
        tWriteNode.offset = 0;
        tWriteNode.build_length = (DWORD)katt->file_size;
		tWriteNode.file_size = katt->file_size;
        tWriteNode.last_write = katt->last_write;
        tWriteNode.reserved = katt->reserved;
		tWriteNode.isdire = FALSE;
        //
        objTaskList.WriteNode(&tWriteNode);
    } else {
// _LOG_DEBUG(_T("katt->file_size:%lld > CHUNK_SIZE:%d"), katt->file_size, CHUNK_SIZE); // disable by james 20130406
        for(chunk_tatol = 0; chunk_tatol*(unsigned __int64)CHUNK_SIZE <= katt->file_size; chunk_tatol++) {
            tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
            tWriteNode.array_id = dwArrayID;
            if (!chunk_tatol) {
                tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|STYPE_GSYNC|RANGE_TYPE_CHUNK;
                tWriteNode.build_length = CHUNK_SIZE;
            } else if((chunk_tatol + 1)*(unsigned __int64)CHUNK_SIZE > katt->file_size) {
                tWriteNode.control_code = dwTaskCode|FILE_TASK_END|STYPE_GSYNC|RANGE_TYPE_FILE;
                tWriteNode.build_length = POW2N_MOD(katt->file_size, CHUNK_SIZE);
            } else {
                tWriteNode.control_code = dwTaskCode|STYPE_GSYNC|RANGE_TYPE_CHUNK;
                tWriteNode.build_length = CHUNK_SIZE;
            }
            _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
            tWriteNode.offset = chunk_tatol*(unsigned __int64)CHUNK_SIZE;
			tWriteNode.file_size = katt->file_size;
            tWriteNode.last_write = katt->last_write;
            tWriteNode.reserved = katt->reserved;
			tWriteNode.isdire = FALSE;
            //
            objTaskList.WriteNode(&tWriteNode);
        }
    }
    //
}

//
VOID ProduceUtility::AddDireDelecNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_DELETE;
	tWriteNode.success_flags = TRANSMIT_UNDONE|OWRTMRVE_UNDONE;
    //
    _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
	tWriteNode.file_size = katt->file_size;
    tWriteNode.last_write = katt->last_write;
    tWriteNode.reserved = katt->reserved;
	tWriteNode.isdire = TRUE;
    //
    objTaskList.WriteNode(&tWriteNode);
}

VOID ProduceUtility::AddDireDelesNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_DELETE;
	tWriteNode.success_flags = TRANSMIT_UNDONE;
    //
    _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
	tWriteNode.file_size = katt->file_size;
    tWriteNode.last_write = katt->last_write;
    tWriteNode.reserved = katt->reserved;
	tWriteNode.isdire = TRUE;
    //
    objTaskList.WriteNode(&tWriteNode);
}

VOID ProduceUtility::AddDireGetNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_GET;
    tWriteNode.success_flags = TRANSMIT_UNDONE|OWRTMRVE_UNDONE;
    //
    _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
	tWriteNode.file_size = katt->file_size;
    tWriteNode.last_write = katt->last_write;
    tWriteNode.reserved = katt->reserved;
	tWriteNode.isdire = TRUE;
    //
    objTaskList.WriteNode(&tWriteNode);
}

VOID ProduceUtility::AddDirePutNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID) {
    struct TaskNode tWriteNode;
    //
    memset(&tWriteNode, '\0', sizeof(struct TaskNode));
    tWriteNode.success_flags = TRANSMIT_UNDONE;
	//
    tWriteNode.node_inde = objTaskList.GetHandNodeAdd();
    tWriteNode.array_id = dwArrayID;
    tWriteNode.control_code = dwTaskCode|FILE_TASK_BEGIN|FILE_TASK_END|STYPE_PUT;
    //
    _tcscpy_s(tWriteNode.szFileName1, katt->file_name);
	tWriteNode.file_size = katt->file_size;
    tWriteNode.last_write = katt->last_write;
    tWriteNode.reserved = katt->reserved;
	tWriteNode.isdire = TRUE;
    //
    objTaskList.WriteNode(&tWriteNode);
}

//
VOID ProduceUtility::BuildServerTlist(TasksVec *stskatt, DWORD *dwArrayID) {
    TasksVec::iterator iter;
    struct taskatt *exist;

	// 后把本地修改同步去服务器
    for(iter=stskatt->begin(); stskatt->end()!=iter; ++iter) {
        ++(*dwArrayID);
		//
		if(ATTRB_IS_DIRECTORY((*iter)->action_type)) {
_LOG_DEBUG(_T("dire file_name:%s action_type:%08X (*dwArrayID):%d"), (*iter)->file_name, (*iter)->action_type, (*dwArrayID));
			switch(FILE_REAL_STAT((*iter)->action_type)) {
			case ADDI:
			case RECURSIVE:
				AddDirePutNode(*iter, LOCATION_SERVER, (*dwArrayID));
				break;
			case DELE:
				AddDireDelesNode(*iter, LOCATION_SERVER, (*dwArrayID));
				break;			
			}
		} else {
_LOG_DEBUG(_T("file file_name:%s action_type:%08X (*dwArrayID):%d"), (*iter)->file_name, (*iter)->action_type, (*dwArrayID));
			switch(FILE_REAL_STAT((*iter)->action_type)) {
			case ADDI:
			case RECURSIVE:
				AddFilePutNode(*iter, LOCATION_SERVER, (*dwArrayID));
				break;
			case MODIFY:
				FileGetSynchNode(*iter, LOCATION_SERVER, (*dwArrayID));
				break;
			case DELE:
				AddFileDelesNode(*iter, LOCATION_SERVER, (*dwArrayID));
				break;
			case EXIST:
				exist = (*iter);
				break;
			case COPY:
				AddFileCopysNode(exist, *iter, LOCATION_SERVER, (*dwArrayID));
				break;
			case MOVE:
				AddFileMovesNode(exist, *iter, LOCATION_SERVER, (*dwArrayID));
				break;
			}
		}
// _LOG_DEBUG(_T("stskatt (*dwArrayID):%d"), (*dwArrayID)); // disable by james 20130406
    }
}

VOID ProduceUtility::BuildClientTlist(TasksVec *ltskatt, DWORD *dwArrayID) {
    TasksVec::iterator iter;
    struct taskatt *exist;

    // 先在本地执行服务器的修改
    for(iter=ltskatt->begin(); ltskatt->end()!=iter; ++iter) {
        ++(*dwArrayID);
		//
		if(ATTRB_IS_DIRECTORY((*iter)->action_type)) {
_LOG_DEBUG(_T("dire file_name:%s action_type:%08X (*dwArrayID):%d"), (*iter)->file_name, (*iter)->action_type, (*dwArrayID));
			switch(FILE_REAL_STAT((*iter)->action_type)) {
			case ADDI:
			case RECURSIVE:
				AddDireGetNode(*iter, LOCATION_CLIENT, (*dwArrayID));
				break;
			case DELE:
				AddDireDelecNode(*iter, LOCATION_CLIENT, (*dwArrayID));
				break;			
			}
		} else {
_LOG_DEBUG(_T("file file_name:%s action_type:%08X (*dwArrayID):%d"), (*iter)->file_name, (*iter)->action_type, (*dwArrayID));
			switch(FILE_REAL_STAT((*iter)->action_type)) {
			case ADDI:
			case RECURSIVE:
				AddFileGetNode(*iter, LOCATION_CLIENT, (*dwArrayID));
				break;
			case MODIFY:
				FilePutSynchNode(*iter, LOCATION_CLIENT, (*dwArrayID));
				break;
			case DELE:
				AddFileDelecNode(*iter, LOCATION_CLIENT, (*dwArrayID));
				break;
			case EXIST:
				exist = (*iter);
				break;
			case COPY:
				AddFileCopycNode(exist, *iter, LOCATION_CLIENT, (*dwArrayID));
				break;
			case MOVE:
				AddFileMovecNode(exist, *iter, LOCATION_CLIENT, (*dwArrayID));
				break;
			}	
		}
// _LOG_DEBUG(_T("ltskatt (*dwArrayID):%d"), (*dwArrayID)); // disable by james 20130406
    }
}

//
DWORD ProduceUtility::CreateTasksList(struct TasksCache *pTasksCache) {
    DWORD dwArrayID = 0x00; // array id must start 0x01
	// 先把本地改动写服务器
_LOG_DEBUG(_T("build server tlist."));
	BuildServerTlist(&pTasksCache->stskatt, &dwArrayID);
	// 先把服务器改动写本地
_LOG_DEBUG(_T("build client tlist."));
	BuildClientTlist(&pTasksCache->ltskatt, &dwArrayID);
    //
    return 0x00;
}

