#include "StdAfx.h"
#include "FileUtility.h"
// #include "FilesVec.h"
#include "FileHandler.h"

#include "ValidLayer.h"
#include "./utility/ulog.h"

CValidLayer::CValidLayer(void):
m_pWinThread(NULL),
m_bProcessStarted(FALSE),
previousValid(NULL)
{
}

CValidLayer::~CValidLayer(void) {
}

void CValidLayer::Initialize(void) {
	InitializeCriticalSection(&m_csValidLink);

	InterlockedExchange((LONG*)&m_bProcessStarted, TRUE);
	m_pWinThread = AfxBeginThread(&CValidLayer::ValidActionThread, (LPVOID)this);
	if(!m_pWinThread) {
		printf("thread create failed: %01d\n", __LINE__); 	
	}
	if(!m_objValidPipe.IsOk())
		LOG_DEBUG(_T("vaid pipe create fail!"));
}

void CValidLayer::Finalize(void) {
	InterlockedExchange((LONG*)&m_bProcessStarted, FALSE);
	m_objValidPipe.write(NULL, 0);
	while(WAIT_OBJECT_0 != WaitForSingleObject(m_pWinThread->m_hThread, 3000)) //wait five seconds
		LOG_DEBUG(_T("wait valid layer thread exit!"));

	DeleteCriticalSection(&m_csValidLink);
}

void CValidLayer::InsValidate(struct Valid *pValid) {
// logger(_T("c:\\valid.log"), _T("write:%s type:%x\r\n"), pValid->szFileName1, pValid->dwValidType);
	m_objValidPipe.write(pValid, sizeof(struct Valid));
}

void CValidLayer::DeleteCached() {
	EnterCriticalSection(&m_csValidLink);
	NValidVec::VoidValid(previousValid);
	LeaveCriticalSection(&m_csValidLink);
}

static void AdditionHandle(struct Valid *pValid, BOOL bIsFirst) {
	static vector<struct Valid *> vLockedVec;
	struct Valid *pLockedValid;
	///////////////////////////////////////////
	vector<struct Valid *>::iterator iter;
	if(vLockedVec.size()) { 
		for(iter = vLockedVec.begin(); iter != vLockedVec.end(); ) {
			pLockedValid = *iter;
			if(!NFileUtility::IsFileLocked(pLockedValid->szFileName1)) {
				HandleFileAdded(pLockedValid);
				iter = vLockedVec.erase(iter);
				NValidVec::DelValid(pLockedValid);
			} else iter++;
		}
	}
	///////////////////////////////////////////
	if(bIsFirst) {
		if(NFileUtility::IsFileLocked(pValid->szFileName1))
			vLockedVec.push_back(NValidVec::NewValid(pValid));
		else HandleFileAdded(pValid);
	}
	///////////////////////////////////////////
}

static int ValidActionRouter(struct Valid *previousValid, struct Valid *pValid) {
	static int iNotifyCount;
	int iSwapValue = 0;
	///////////////////////////////////////////
	switch(pValid->dwValidType) {
	case VALID_ADDITION:
		if(VALID_DELETE == previousValid->dwValidType) {
			AdditionHandle(pValid, TRUE);
			pValid->dwValidType = VALID_NONE;
		} else iNotifyCount = 0;
		break;
	case VALID_MODIFY:
		if(_tcscmp(previousValid->szFileName1, pValid->szFileName1)) {
			HandleModified(pValid->szFileName1);
		} else if(VALID_ADDITION == previousValid->dwValidType) {
			iSwapValue = 1;
			if(++iNotifyCount) AdditionHandle(previousValid, iNotifyCount==0x01);
// logger(_T("c:\\valid.log"), _T("valid:%s type:%x modify_time:%d\r\n"), pValid->szFileName1, pValid->dwValidType, iNotifyCount);
		}		
		break;
	case VALID_DELETE|VALID_DIRECTORY:
	case VALID_DELETE:
		HandleRemoved(pValid->szFileName1);
		break;
	case VALID_MOVE|VALID_DIRECTORY:
	case VALID_MOVE:
		HandleMoved(pValid->szFileName1, pValid->szFileName2);
		break;
	case VALID_ADDITION|VALID_DIRECTORY:
		if(VALID_DIRECTORY & pValid->dwValidType) HandleDirAdded(pValid);
		break;
	}
	///////////////////////////////////////////
	return iSwapValue;
}

#define SWAP_VALID(PTR1, PTR2) { \
struct Valid *PTR3 = PTR2; \
	PTR2 = PTR1; \
	PTR1 = PTR3; \
}

UINT CValidLayer::ValidActionThread(LPVOID lpParam) {
	LONG lProcessContinue;
	struct Valid *pReadValid;
	///////////////////////////////////////////////
	CValidLayer *lpObject = (CValidLayer *)lpParam;
	if(!lpObject) {
		LOG_DEBUG(_T("create valid thread fail!"));
		return -1;
	}
	CTinyPipe *pValidPipe = &lpObject->m_objValidPipe;
	CRITICAL_SECTION *csValidLink = &lpObject->m_csValidLink;
	///////////////////////////////////////////////
	pReadValid = (struct Valid *)malloc(sizeof(struct Valid));
	lpObject->previousValid = (struct Valid *)malloc(sizeof(struct Valid));
	NValidVec::VoidValid(lpObject->previousValid);
	///////////////////////////////////////////////
	// Enter a loop reading data
	InterlockedExchange(&lProcessContinue, lpObject->m_bProcessStarted);
	while(lProcessContinue) { // 循环等待事件发生或超时. 检查 重复修改 删除为移动 增加文件锁定
		//////////////////////////////////////////////////////////////////////////////////////
		while(sizeof(struct Valid) == pValidPipe->read(pReadValid, sizeof(struct Valid))) {
// logger(_T("c:\\valid.log"), _T("read:%s type:%x\r\n"), pReadValid->szFileName1, pReadValid->dwValidType);
			EnterCriticalSection(csValidLink);
			if(!ValidActionRouter(lpObject->previousValid, pReadValid)) 
				SWAP_VALID(pReadValid, lpObject->previousValid)
			LeaveCriticalSection(csValidLink);
		}
		//////////////////////////////////////////////////////////////////////////////////////
		InterlockedExchange(&lProcessContinue, lpObject->m_bProcessStarted);
	}
	///////////////////////////////////////////////
	NValidVec::DelValid(lpObject->previousValid);
	NValidVec::DelValid(pReadValid);
	///////////////////////////////////////////////
	return 0;
}

struct Valid *NValidVec::FillValid(const TCHAR *szFileName, DWORD dwValidType) {
	static struct Valid tValid;
	_tcscpy_s(tValid.szFileName1, szFileName);
	tValid.szFileName2[0] = _T('\0');
	tValid.dwValidType = dwValidType;
	return &tValid;
}

struct Valid *NValidVec::FillValid(const TCHAR *szFileName1, const TCHAR *szFileName2, DWORD dwValidType) {
	static struct Valid tValid;
	_tcscpy_s(tValid.szFileName1, szFileName1);
	_tcscpy_s(tValid.szFileName2, szFileName2);
	tValid.dwValidType = dwValidType;
	return &tValid;
}

void NValidVec::VoidValid(struct Valid *pValid) {
	if(pValid) {
		pValid->szFileName1[0] = _T('\0');
		pValid->szFileName2[0] = _T('\0');
		pValid->dwValidType = VALID_NONE;
	}
}

struct Valid *NValidVec::NewValid(struct Valid *pValid) {
	struct Valid *pNewValid = (struct Valid *)malloc(sizeof(struct Valid));
	if(pNewValid) memcpy(pNewValid, pValid, sizeof(struct Valid));
	return pNewValid;
}

void NValidVec::DelValid(struct Valid *pValid) {
	if(pValid) free(pValid);
}