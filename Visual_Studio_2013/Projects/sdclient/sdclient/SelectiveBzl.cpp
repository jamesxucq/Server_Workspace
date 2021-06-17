#include "StdAfx.h"

//#include "StringUtility.h"
#include "LocalBzl.h"
#include "ShellLinkBzl.h"
#include "TRANSWorker.h"
#include "ListUtility.h"
#include "DebugPrinter.h"
#include "ExecuteUtility.h"

#include "SelectiveBzl.h"

CSelectiveObject::CSelectiveObject(void):
    m_dwDowndThreadStatus(0)
{
}

CSelectiveObject::~CSelectiveObject(void) {
}

class CSelectiveObject objSelectObject;

//
/*
void NSelectiveBzl::InitDiresTreeTest()
{
    DiresVec *pDiresVec = &objSelectObject.m_vDiresVec;
//
    TCHAR *str;
    str = _tcsdup(_T("\\xxxx\\kou\\"));
    pDiresVec->push_back(str);
    str = _tcsdup(_T("\\xxxx\\uuu\\"));
    pDiresVec->push_back(str);
    str = _tcsdup(_T("\\jjj\\fff\\"));
    pDiresVec->push_back(str);
    str = _tcsdup(_T("\\jjj\\"));
    pDiresVec->push_back(str);
    str = _tcsdup(_T("\\fffff\\kkk\\ppp\\"));
    pDiresVec->push_back(str);
    str = _tcsdup(_T("\\fffff\\kkk\\ddd\\"));
    pDiresVec->push_back(str);
    str = _tcsdup(_T("\\fffff\\kkk\\sss\\aaa\\bbb\\ccc\\ddd\\"));
    pDiresVec->push_back(str);
}
*/

VOID NSelectiveBzl::ClearDiresAttrib(const TCHAR *szDriveLetter, FileFobdVec *pFobdVec) {
    FileFobdVec::iterator fite;
    for(fite=pFobdVec->begin(); pFobdVec->end()!=fite; ) { // ++fite
        if((*fite)->dwAttrib&FILE_STATUS_DIRECTORY && (*fite)->dwAttrib&FILE_STATUS_FORBID && !_tcscmp((*fite)->szOwnerType, _T("client"))) {
            NDireUtility::SetDirectoryAttrib(szDriveLetter, (*fite)->szFilePath, false);
            NFileFobd::DeleFobd(*fite);
            fite = pFobdVec->erase(fite);
        } else ++fite;
    }
}


DWORD NSelectiveBzl::MakeLocalChanged(FileFobdVec *pFobdVec, DWORD dwAdded, const TCHAR *szDriveLetter) {
    NShellLinkBzl::ClearDisplayDirectory();
    NShellLinkBzl::DisplayDenyVector(pFobdVec, szDriveLetter);
    NFileFobd::SaveFileFobd();
_LOG_DEBUG(_T("----- make local changed!"));
	if(dwAdded) { // 写Anchor标志,慢同步
		NAnchorBzl::ClearAnchorEnviro(FALSE);
	    // 修改选择同步结束,触发同步事件
_LOG_DEBUG(_T("----- make local changed! set event"));
		NExecTransBzl::SetWaitingEvent();	
	}
    //
    return 0x00;
}

static DWORD FindUnchk(DiresVec *pUnchkVec, TCHAR *szFileName) {
	DiresVec::iterator dite;
	DWORD dwFound = 0x00;
	//
// _LOG_DEBUG(_T("----- FindUnchk szFileName:%s"), szFileName);
	for(dite=pUnchkVec->begin(); pUnchkVec->end()!=dite; ++dite) { // ++dite
		if(!_tcsncmp(szFileName, (*dite), _tcslen(*dite))) {
			dwFound = 0x01;
// _LOG_DEBUG(_T("----- szFileName:%s unchk:%s"), szFileName, (*dite));
			break;		
		}
	}
	//
// _LOG_DEBUG(_T("----- dwFound:%d"), dwFound);
	return dwFound;
}

static DWORD CompaUnchk(FileFobdVec *pFobdVec, DiresVec *pUnchkVec) {
    FileFobdVec::iterator fite;
	struct FileFobd *atte;
	DWORD dwAdded = 0x00;
	//
    for(fite=pFobdVec->begin(); pFobdVec->end()!=fite; ++fite) { // ++fite
		atte = (*fite);
        if(atte->dwAttrib&FILE_STATUS_DIRECTORY && atte->dwAttrib&FILE_STATUS_FORBID && !_tcscmp(atte->szOwnerType, _T("client"))) {
// _LOG_DEBUG(_T("----- szFilePath:%s"), atte->szFilePath);
			if(!FindUnchk(pUnchkVec, atte->szFilePath)) {
				dwAdded = 0x01;
				break;
			}
        }
		//
    }
	//
// _LOG_DEBUG(_T("----- dwAdded:%d"), dwAdded);
	return dwAdded;
}

DWORD NSelectiveBzl::FinishDirectoriesTView(CAdvaTView *tvDiresTView) {
    struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
    FileFobdVec *pFobdVec = NFileFobd::GetFileFobdPoint();
    DiresVec vUnchkedVec;
    NTVUtility::GetAllDiresUnchecked(&vUnchkedVec, tvDiresTView);
	DWORD dwAdded = CompaUnchk(pFobdVec, &vUnchkedVec);
    ClearDiresAttrib(pClientConfig->szDriveLetter, pFobdVec);
// _LOG_DEBUG(_T("xxxxxxxxxxxxxxxxxx"));
    struct FileFobd *atte;
    DiresVec::iterator dite;
    for(dite=vUnchkedVec.begin(); vUnchkedVec.end()!=dite; ++dite) {
        atte = NFileFobd::AddNewFobd(pFobdVec);
        if(!atte) return ((DWORD)-1);
        _tcscpy_s(atte->szFilePath, *dite);
        _tcscpy_s(atte->szOwnerType, _T("client"));
        atte->dwAttrib = FILE_STATUS_DIRECTORY|FILE_STATUS_FORBID;
        //
        NDireUtility::SetDirectoryAttrib(pClientConfig->szDriveLetter, *dite, true);
    }
    MakeLocalChanged(pFobdVec, dwAdded, pClientConfig->szDriveLetter);
    //
    return 0x00;
}

//
static DWORD FinishDownd(LIST_ARGUMENT *pListArgu) {
    DWORD finish_value = LIST_VALUE_FINISH;
    //
    CTRANSWorker *pListWorker = ListUtility::CreateWorker(&finish_value, pListArgu);
    if(pListWorker) {
        ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_FINISH);
    }
    //
 _LOG_DEBUG(_T("finish_value:%08x"), finish_value);
    return finish_value;
}

// PROCESS_STATUS_RUNNING PROCESS_STATUS_WAITING
#define SET_PROCESS_STATUS(PROCESS_STATUS) \
	NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS); \
	NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS); \
 
DWORD NSelectiveBzl::PerfromDirectoriesDownd() {
    struct LocalConfig *pLocalConfig = NConfigBzl::GetLocalConfig();
    if(!pLocalConfig) return DOWND_VALUE_FAILED;
    //
    // 检查同步进程状态
    if(!NExecTransBzl::GetTransmitProcessStatus())
        return DOWND_PROECSS_BUSY;
    SET_PROCESS_STATUS(PROCESS_STATUS_RUNNING)
    //
    // 检查连网状态
    if (NExecTransBzl::ValidConnectStatus(pLocalConfig, pLocalConfig->tClientConfig.szAccountLinked)) {
        SET_PROCESS_STATUS(PROCESS_STATUS_WAITING)
        return DOWND_VALUE_DISCONN;
    }
    //
    // 下载目录数据
    // reinit session and validate
    UINT cached_anchor;
    struct ComandSeion *pCmdSeion = NCmdSeionBzl::Initialize(&pLocalConfig->tClientConfig);
    if(!pCmdSeion) {
        SET_PROCESS_STATUS(PROCESS_STATUS_WAITING)
        return DOWND_VALUE_FAILED;
    }
    //
    struct ReplyArgu tReplyArgu;
    CONVERT_AUTHSOCKS_ARGU(&tReplyArgu, pCmdSeion->ldata, &pLocalConfig->tNetworkConfig);
    if(NComandBzl::InitComand(&tReplyArgu)) {
        NComandBzl::FinishComand();
        SET_PROCESS_STATUS(PROCESS_STATUS_WAITING)
        return DOWND_VALUE_FAILED;
    }
    //
    DWORD cache_locked = WORKER_STATUS_FAULT;
    for(int retry_times=0; REGIST_RETRY_TIMES>retry_times; retry_times++) {
        cache_locked = NComandBzl::QueryCached(&cached_anchor, pCmdSeion);
        if(WORKER_STATUS_NOTAUTH != cache_locked)
            break;
        Sleep(REGIST_RETRY_DELAY);
        NExecTransBzl::RegisterRetry(pLocalConfig);
    }
    //
    // uninit session and validate
    NComandBzl::FinishComand();
    NCmdSeionBzl::Finalize();
// _LOG_ANSI("sin_addr:%s", pCmdSeion->ldata->tServAddress.sin_addr);
// _LOG_DEBUG(_T("sin_port:%d"), pCmdSeion->ldata->tServAddress.sin_port);
    //
    if(WORKER_STATUS_FAULT == cache_locked) {
        SET_PROCESS_STATUS(PROCESS_STATUS_WAITING)
        return DOWND_VALUE_QUERYERR;
    } else if(WORKER_STATUS_LOCKED == cache_locked) {
        SET_PROCESS_STATUS(PROCESS_STATUS_WAITING)
        return DOWND_VALUE_SERVBUSY;
    }
// _LOG_DEBUG(_T("ddddddddddddddddddd"));
    //
    struct LIST_ARGUMENT tListArgu;
    DWORD list_value = DOWND_VALUE_FAILED;
    DiresVec *pListVec = NSelectiveBzl::GetDiresVec();
    //
    // Initialize Winsock
    NTRANSSockets::InitialWinsock();
    //
    CONVERT_SESSION_ARGU(&(tListArgu.tSeionArgu), &pLocalConfig->tClientConfig);
    CONVERT_SOCKETS_ARGU(&(tListArgu.tSockeArgu), &pLocalConfig->tNetworkConfig);
    //
    DWORD dwExceptionRetry = 0x00;
    while((RETRY_PRODUCER_TIMES>dwExceptionRetry++) && (DOWND_VALUE_SUCCESS!=list_value)) {
        SET_SOCKETS_THREADS(&(tListArgu.tSockeArgu), ONE_SOCKETS_THREAD); // get head
        //
        if(LIST_VALUE_FINISH == NBuildTasksBzl::RecursiveListEnviro(&tListArgu)) {
            SET_SOCKETS_THREADS(&(tListArgu.tSockeArgu), FIVE_SOCKETS_THREAD); // get list
            if(!NBuildTasksBzl::RecursiveListVector(pListVec, &tListArgu, pLocalConfig->tClientConfig.szLocation))
                list_value = DOWND_VALUE_SUCCESS;
// ndp::DiresVector(pListVec);
        } else list_value = DOWND_VALUE_FAILED; // 生成同步任务失败
    }
    //
    SET_SOCKETS_THREADS(&(tListArgu.tSockeArgu), ONE_SOCKETS_THREAD); // final
    FinishDownd(&tListArgu);
    // Finish Winsock
    NTRANSSockets::FinishWinsock();
    SET_PROCESS_STATUS(PROCESS_STATUS_WAITING)
    //
 _LOG_DEBUG(_T("list_value:%08x"), list_value);
    return list_value;
}

static DWORD GetDiINodeChecked(FileFobdVec *pFobdVec, TCHAR *path) {
	FileFobdVec::iterator iter;
	for(iter = pFobdVec->begin(); pFobdVec->end() != iter; ++iter) {
        if(!_tcsncmp((*iter)->szFilePath, path, _tcslen((*iter)->szFilePath)))
			return 0x02; // sub directory all unchecked.
		if(!_tcsncmp(path, (*iter)->szFilePath, _tcslen(path)))
			return 0x01; // parent directory part unchecked.
	}
	return 0x00; // checked
}

static VOID BuildTViewHmap(DiresHmap *pTViewHmap, DiresVec *pListVec, FileFobdVec *pFobdVec) {
	struct DiINode *pRootINode, *pParentINode, *pDiINode;
	pRootINode = NDiresVec::NewDiINode(_T("\\"), NULL); // root
	pRootINode->selected = GetDiINodeChecked(pFobdVec, _T("\\"));
	pTViewHmap->insert(DiresHmap::value_type(_T("\\"), pRootINode));
//
	DiresHmap::iterator hiter;
	TCHAR path[MAX_PATH], dire[MAX_PATH];
	TCHAR  *dire_path, *prev_toke, *next_toke;
    DiresVec::iterator dite;
	for(dite=pListVec->begin(); pListVec->end()!=dite; ++dite) {
		dire_path = *dite;
		pParentINode = pRootINode;
// _LOG_DEBUG(_T("dire_path:%s"), dire_path);
		prev_toke = dire_path+0x01;
		next_toke = _tcschr(prev_toke, _T('\\'));
		//
		while(next_toke++) {
			_tcsncpy(path, dire_path, (next_toke-dire_path));
			path[next_toke-dire_path] = _T('\0');
			hiter = pTViewHmap->find(path);
			if(pTViewHmap->end() == hiter) {
				_tcsncpy(dire, prev_toke, (next_toke-prev_toke));
				dire[next_toke-prev_toke] = _T('\0');
				pDiINode = NDiresVec::NewDiINode(dire, pParentINode->child);
// _LOG_DEBUG(_T("pParentINode->child:%08X new pDiINode:%08X"), pParentINode->child, pDiINode);
				pDiINode->selected = GetDiINodeChecked(pFobdVec, path);
				//
				pParentINode->child = pDiINode;
				pTViewHmap->insert(DiresHmap::value_type(path, pDiINode));
// _LOG_DEBUG(_T("path:%s pDiINode->szINodeName:%s"), path, pDiINode->szINodeName);
				pParentINode = pDiINode;
			} else pParentINode = (struct DiINode *)hiter->second;
// _LOG_DEBUG(_T("new pParentINode:%08X"), pParentINode);
			prev_toke = next_toke;
			next_toke = _tcschr(next_toke, _T('\\'));
		}
	}
//
}

DWORD NSelectiveBzl::PrepaDisplayTView() {
	DiresVec *pListVec = &objSelectObject.m_vDiresVec;
	DiresHmap *pTViewHmap = &objSelectObject.m_mTViewHmap;
//
    FileFobdVec vFobdVec;
	NFileFobd::GetFileFobdVec(&vFobdVec);
//
	BuildTViewHmap(pTViewHmap, pListVec, &vFobdVec);
// _LOG_DEBUG(_T("build hmap advanced finish."));
	return 0x00;
}

VOID NSelectiveBzl::FinalizeEnviro(CAdvaTView *tvDiresTView) {
	if(tvDiresTView) {
	    tvDiresTView->SetRedraw(FALSE);
		tvDiresTView->DeleteAllItems();
	}
	//
	DiresHmap *pTViewHmap = &objSelectObject.m_mTViewHmap;
	NDiresVec::DeleDiresHmap(pTViewHmap);
	//
	DiresVec *pListVec = &objSelectObject.m_vDiresVec;
	NDiresVec::DeleDiresVec(pListVec);
}

//
#define NORMAL_DIRECTORY			0x0001
#define ADVANCED_DIRECTORY			0x0003
static int CheckDirectoryNormal(TCHAR *szDirectory) {
	TCHAR *pValue = szDirectory;
	pValue = _tcschr(++pValue, _T('\\'));
    return pValue? ADVANCED_DIRECTORY : NORMAL_DIRECTORY;
}

int NSelectiveBzl::GetSelectiveDlgType() {
    FileFobdVec vFobdVec;
    FileFobdVec::iterator fite;
    //
    NFileFobd::GetFileFobdVec(&vFobdVec);
	struct FileFobd *atte;
    for(fite=vFobdVec.begin(); vFobdVec.end()!=fite; ++fite) {
		atte = (*fite);
// _LOG_DEBUG(_T("szFilePath:%s"), (*fite)->szFilePath);
		if(atte->dwAttrib&FILE_STATUS_FORBID && !_tcscmp(atte->szOwnerType, _T("client"))) {
			if(ADVANCED_DIRECTORY == CheckDirectoryNormal(atte->szFilePath)) {
				return ADVANCED_SELECTIVE;
			}
		}
    }
    //
    return NORMAL_SELECTIVE;
}