#include "StdAfx.h"

#include "client_macro.h"
#include "third_party.h"
#include "Session.h"
#include "TRANSSessionBzl.h"
#include "SDTProtocol.h"
#include "ValueLayer.h"
#include "TRANSSockets.h"
#include "AnchorXml.h"
#include "TRANSHandler.h"
#include "FileUtility.h"
#include "MatchUtility.h"
#include "BuildUtility.h"

#include "TRANSWorker.h"

CSDTPUtility::CSDTPUtility(void) :
m_pTRANSSessionBzl(NULL),
m_pTRANSSockets(NULL)
{
}

CSDTPUtility::~CSDTPUtility(void) {
}

DWORD CSDTPUtility::Create(struct SessionArgu *pSessionArgu, struct SocketsArgu *pSocketsArgu) {
	if(!pSessionArgu || !pSocketsArgu) return -1;

	m_pTRANSSessionBzl = NTRANSSessionBzl::Factory(pSessionArgu);
	if (!m_pTRANSSessionBzl) return -1;

	CONVERT_SOCKETS_ARGU(pSocketsArgu, m_pTRANSSessionBzl->GetListInform());
	m_pTRANSSockets = NTRANSSockets::Factory(pSocketsArgu);// Todo: 20110411	
	if (m_pTRANSSockets->Connect(pSocketsArgu->ServAddress.sin_addr, pSocketsArgu->ServAddress.sin_port)) 
		return -1;

	return 0;
}

void CSDTPUtility::Destroy() {
	if(m_pTRANSSockets) {
		m_pTRANSSockets->Close();
		NTRANSSockets::DestroyObject(m_pTRANSSockets);
	}
	m_pTRANSSockets = NULL;

	if(m_pTRANSSessionBzl) NTRANSSessionBzl::DestroyObject(m_pTRANSSessionBzl);
	m_pTRANSSessionBzl = NULL;
}

DWORD CSDTPUtility::Initial() { // -1 error; 0 succ; 1 failed
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	NValueLayer::InitialRequest(&RequestValue, pTRANSSession->list_inform);
	NSDTProtocol::BuildInitialSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->SendReceive(psbuffer->srecv_buffer, psbuffer->srecv_buffer);
	if (0 > psbuffer->srlength) return -1;
	///////////////////////////////////////////////////////////////////////////////
	DWORD dwInitValue = INIT_FAILED;	
	psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
	psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);
	if(STATUS_OK != NSDTProtocol::ParseInitialRecv(&ResponseValue, psbuffer->head_buffer)) dwInitValue = INIT_FAILED;
	else {
		NValueLayer::InitialResponse(&pTRANSSession->inte_value, &ResponseValue);
		dwInitValue = INIT_SUCCESS;
	}
	///////////////////////////////////////////////////////////////////////////////

	return dwInitValue;
}

DWORD CSDTPUtility::GetAnchor(DWORD *last_anchor) { // -1 error; 0 succ; 1 failed
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;

	///////////////////////////////////////////////////////////////////////////////
	struct XmlAnchor tXmlAnchor;
	///////////////////////////////////////////////////////////////////////////////
	if (m_pTRANSSessionBzl->OpenGetAnchorSe(&tXmlAnchor)) return -1;
	NValueLayer::GetAnchorRequest(&RequestValue, pTRANSSession->list_inform);
	NSDTProtocol::BuildGetAnchorSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->SendReceive(psbuffer->srecv_buffer, psbuffer->srecv_buffer);
	if (0 > psbuffer->srlength) return -1;
	///////////////////////////////////////////////////////////////////////////////
	DWORD dwHandleValue = -1;
	psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
	psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);
	if(STATUS_OK != NSDTProtocol::ParseGetAnchorRecv(&ResponseValue, psbuffer->head_buffer)) dwHandleValue = INIT_FAILED;
	else {
		NValueLayer::GetAnchorResponse(&pTRANSSession->inte_value, &ResponseValue);
		NTRANSHandler::HandleGetAnchorRecv(pTRANSSession, psbuffer->body_buffer, psbuffer->blen);	
		dwHandleValue = 0;
	}
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->CloseGetAnchorSe();
	*last_anchor = tXmlAnchor.last_anchor;
	LOG_DEBUG(_T("tXmlAnchor.last_anchor:%d"), tXmlAnchor.last_anchor);

	return dwHandleValue;
}

//FilesVec serv_files
DWORD CSDTPUtility::GetList(void *serv_point, DWORD dwListType, DWORD dwAnchor, TCHAR *szDirectory) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	if (m_pTRANSSessionBzl->OpenGetListSe(dwListType, dwAnchor, szDirectory, serv_point))
		return -1;
	NValueLayer::GetListRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildGetListSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->Send(psbuffer->srecv_buffer, strlen(psbuffer->srecv_buffer));
	if (0 > psbuffer->srlength) { m_pTRANSSessionBzl->CloseGetListSe(); return -1; }
	///////////////////////////////////////////////////////////////////////////////
	DWORD dwHandleValue = -1;
	DWORD dwFirstEntry = 0;
	do{
		memset(psbuffer->srecv_buffer, '\0', sizeof(psbuffer->srecv_buffer));
		psbuffer->srlength = m_pTRANSSockets->Receive(psbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > psbuffer->srlength) break;

		if (!dwFirstEntry) {
			psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
			psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);

			if(STATUS_OK != NSDTProtocol::ParseGetListRecv(&ResponseValue, psbuffer->head_buffer)) break; 
			NValueLayer::GetListResponse(&pTRANSSession->inte_value, &ResponseValue);
			dwHandleValue = NTRANSHandler::HandleGetListRecv(pTRANSSession, psbuffer->body_buffer, psbuffer->blen);

			dwFirstEntry++;
		} else dwHandleValue = NTRANSHandler::HandleGetListRecv(pTRANSSession, psbuffer->srecv_buffer, psbuffer->srlength);
	} while(dwHandleValue > 0);

	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->CloseGetListSe();
	LOG_DEBUG(_T("dwHandleValue:%d"), dwHandleValue);

	return dwHandleValue? -1: 0;
}

#define GETARGUMENT_TSESSION(TSESSION, GETARGUMENT) { \
	_tcscpy_s(TSESSION->inte_value.szFileName, GETARGUMENT.szFileName); \
	TSESSION->inte_value.FileLength = GETARGUMENT.FileLength; \
	memcpy(&TSESSION->inte_value.ftLastWrite, &GETARGUMENT.ftLastWrite, sizeof(FILETIME)); \
	TSESSION->inte_value.nContentLength = GETARGUMENT.nContentLength; \
	_tcscpy_s(TSESSION->list_inform->szSessionID, GETARGUMENT.szSessionID); \
}

DWORD CSDTPUtility::GetFile(struct TaskNode *task_node) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenGetFileSe(task_node);
	NValueLayer::GetFileRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildGetFileSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->Send(psbuffer->srecv_buffer, strlen(psbuffer->srecv_buffer));
	if (0 > psbuffer->srlength) { m_pTRANSSessionBzl->CloseGetFileSe(task_node); return -1; }
	///////////////////////////////////////////////////////////////////////////////
	DWORD dwHandleValue = -1;
	DWORD dwFirstEntry = 0;
	do{
		memset(psbuffer->srecv_buffer, '\0', sizeof(psbuffer->srecv_buffer));
		psbuffer->srlength = m_pTRANSSockets->Receive(psbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > psbuffer->srlength) break;

		if (!dwFirstEntry) {
			psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
			psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);

			if(STATUS_OK != NSDTProtocol::ParseGetFileRecv(&ResponseValue, psbuffer->head_buffer)) break;		
			NValueLayer::GetFileResponse(&pTRANSSession->inte_value, &ResponseValue);
			//			GETARGUMENT_TSESSION(pTRANSSession, tGetRecvArgu)
			dwHandleValue = NTRANSHandler::HandleGetFileRecv(pTRANSSession, psbuffer->body_buffer, psbuffer->blen);

			dwFirstEntry++;
		} else dwHandleValue = NTRANSHandler::HandleGetFileRecv(pTRANSSession, psbuffer->srecv_buffer, psbuffer->srlength);
	} while(dwHandleValue > 0);
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->CloseGetFileSe(task_node);

	return dwHandleValue? -1: 0;
}


DWORD CSDTPUtility::PutFile(struct TaskNode *task_node) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	LOG_DEBUG(_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenPutFileSe(task_node);
	NValueLayer::PutFileRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildPutFileSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	size_t nSendLength = 0;
	DWORD dwHandleValue = -1;
	DWORD dwFirstEntry = 0;
	do{
		if (!dwFirstEntry) {
			dwHandleValue = NTRANSHandler::HandlePutFileSend(pTRANSSession, psbuffer->body_buffer, &psbuffer->blen);
			nSendLength = psbuffer->hlen + psbuffer->blen;
			dwFirstEntry++;
		} else {
			nSendLength = RECV_BUFF_SIZE;
			dwHandleValue = NTRANSHandler::HandlePutFileSend(pTRANSSession, psbuffer->srecv_buffer, &nSendLength);
		}
		if(-1 != dwHandleValue) {
			psbuffer->srlength = m_pTRANSSockets->Send(psbuffer->srecv_buffer, nSendLength);
			if (0 > psbuffer->srlength) dwHandleValue = -1;
		}
	} while(dwHandleValue > 0);
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->ClosePutFileSe(task_node);
	if (dwHandleValue) return -1;
	///////////////////////////////////////////////////////////////////////////////
	memset(psbuffer->srecv_buffer, '\0', sizeof(psbuffer->srecv_buffer));
	psbuffer->srlength = m_pTRANSSockets->Receive(psbuffer->srecv_buffer, RECV_BUFF_SIZE);
	if (0 > psbuffer->srlength) dwHandleValue = -1;
	else {
		psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
		if(STATUS_OK != NSDTProtocol::ParsePutFileRecv(&ResponseValue, psbuffer->head_buffer)) dwHandleValue = -1;
		NValueLayer::PutFileResponse(&pTRANSSession->inte_value, &ResponseValue);
	}

	return dwHandleValue;
}

DWORD CSDTPUtility::MoveLocal(struct TaskNode *exist_task_node, struct TaskNode *new_task_node) {
	ListInform *list_inform = m_pTRANSSessionBzl->GetTRANSSession()->list_inform;
	TCHAR ExistsName[MAX_PATH];
	TCHAR NewName[MAX_PATH];

	nstriutility::full_path(ExistsName, list_inform->szLocation, exist_task_node->szFileName);
	nstriutility::full_path(NewName, list_inform->szLocation, new_task_node->szFileName);
	if(!MoveFileEx(ExistsName, NewName, MOVEFILE_REPLACE_EXISTING)) {
		LOG_WARN(_T("move local file %s error!"), ExistsName);
		return -1;
	} 
	NFileUtility::SetFileLastWrite(NewName, &new_task_node->ftLastWrite);
	// delete by james 20110307
	// return MoveFileEx(ExistsName, NewName, MOVEFILE_REPLACE_EXISTING)? true: false;

	return 0;
}


DWORD CSDTPUtility::MoveServer(struct TaskNode *exist_task_node, struct TaskNode *new_task_node) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenMoveServerSe(exist_task_node, new_task_node);
	NValueLayer::MoveRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildMoveSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->SendReceive(psbuffer->srecv_buffer, psbuffer->srecv_buffer);
	if (0 > psbuffer->srlength) return -1;
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
	if(STATUS_OK != NSDTProtocol::ParseMoveRecv(&ResponseValue, psbuffer->head_buffer)) return -1;
	NValueLayer::MoveResponse(&pTRANSSession->inte_value, &ResponseValue);

	return 0;
}


DWORD CSDTPUtility::DeleteLocal(struct TaskNode *task_node) {
	ListInform *list_inform = m_pTRANSSessionBzl->GetTRANSSession()->list_inform;
	TCHAR szDeleteName[MAX_PATH];

	nstriutility::full_path(szDeleteName, list_inform->szLocation, task_node->szFileName);
	if(!DeleteFile(szDeleteName))
		LOG_WARN(_T("Delete client file %s error!"), szDeleteName);
	//return DeleteFile(DelName)? true: false; // delete by james 20110307

	return 0;
}


DWORD CSDTPUtility::DeleteServer(struct TaskNode *task_node) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenDeleteServerSe(task_node);
	NValueLayer::DeleteRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildDeleteSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->SendReceive(psbuffer->srecv_buffer, psbuffer->srecv_buffer);
	if (0 > psbuffer->srlength) return -1;
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
	if(STATUS_OK != NSDTProtocol::ParseDeleteRecv(&ResponseValue, psbuffer->head_buffer)) return -1;
	NValueLayer::DeleteResponse(&pTRANSSession->inte_value, &ResponseValue);

	return 0;
}

DWORD CSDTPUtility::GetCsum(struct TaskNode *task_node) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenGetCsumSe(task_node);
	NValueLayer::GetCsumRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildGetCsumSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->Send(psbuffer->srecv_buffer, strlen(psbuffer->srecv_buffer));
	if (0 > psbuffer->srlength) return -1;
	///////////////////////////////////////////////////////////////////////////////
	DWORD dwHandleValue = -1;
	DWORD dwFirstEntry = 0;
	do{
		memset(psbuffer->srecv_buffer, '\0', sizeof(psbuffer->srecv_buffer));
		psbuffer->srlength = m_pTRANSSockets->Receive(psbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > psbuffer->srlength) break;

		if (!dwFirstEntry) {
			psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
			psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);
			if(STATUS_OK != NSDTProtocol::ParseGetCsumRecv(&ResponseValue, psbuffer->head_buffer)) break;
			NValueLayer::GetCsumResponse(&pTRANSSession->inte_value, &ResponseValue);
			dwHandleValue = NTRANSHandler::HandleGetCsumRecv(pTRANSSession, psbuffer->body_buffer, psbuffer->blen);

			dwFirstEntry++;
		} else dwHandleValue = NTRANSHandler::HandleGetCsumRecv(pTRANSSession, psbuffer->srecv_buffer, psbuffer->srlength);
	} while(dwHandleValue > 0);
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->CloseGetCsumSe();

	// return dwHandleValue? false: true;
	return 0;
}

DWORD CSDTPUtility::Post(struct TaskNode *task_node) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenPostSe(task_node);
	NValueLayer::PostRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildPostSend(psbuffer, &RequestValue);
	LOG_DEBUG(_T("+++++++++++++++++++++++++++++ psbuffer->hlen=%d"), psbuffer->hlen);
	///////////////////////////////////////////////////////////////////////////////
	size_t nSendLength = 0;
	DWORD dwHandleValue = -1;
	DWORD dwFirstEntry = 0;
	do {
		if (!dwFirstEntry) {
			dwHandleValue = NTRANSHandler::HandlePostSend(pTRANSSession, psbuffer->body_buffer, &psbuffer->blen);
			nSendLength = psbuffer->hlen + psbuffer->blen;
			dwFirstEntry++;
		} else {
			nSendLength = RECV_BUFF_SIZE;
			dwHandleValue = NTRANSHandler::HandlePostSend(pTRANSSession, psbuffer->srecv_buffer, &nSendLength);
		}
		if(-1 != dwHandleValue) {
			LOG_DEBUG(_T("+++++++++++++++++++++++++++++ dwHandleValue=%d, nSendLength=%d"), dwHandleValue, nSendLength);
			psbuffer->srlength = m_pTRANSSockets->Send(psbuffer->srecv_buffer, nSendLength);
			if (0 > psbuffer->srlength) dwHandleValue = -1;
			LOG_DEBUG(_T("+++++++++++++++++++++++++++++ psbuffer->srlength=%d"), psbuffer->srlength);
		}
	} while(dwHandleValue > 0);

	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->ClosePostSe();
	// if(dwHandleValue) return false;
	///////////////////////////////////////////////////////////////////////////////
	memset(psbuffer->srecv_buffer, '\0', sizeof(psbuffer->srecv_buffer));
	psbuffer->srlength = m_pTRANSSockets->Receive(psbuffer->srecv_buffer, RECV_BUFF_SIZE);
	if (0 > psbuffer->srlength) dwHandleValue = -1;
	else {
		psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
		if(STATUS_OK != NSDTProtocol::ParsePostRecv(&ResponseValue, psbuffer->head_buffer)) dwHandleValue = -1;
		else NValueLayer::PostResponse(&pTRANSSession->inte_value, &ResponseValue);
	}

	// return dwHandleValue? false: true;
	return 0;
}

// LOG_DEBUG(_T("vvvvvvvvvvvvvvvvvvv"));
DWORD CSDTPUtility::Getsync(struct TaskNode *task_node) {
	TCHAR szFileName[MAX_PATH];

	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	nstriutility::full_path(szFileName, pTRANSSession->list_inform->szLocation, task_node->szFileName);

	LOG_DEBUG(_T("get sync get sums section."));
	if (GetCsum(task_node)) return -1;

	switch(TNODE_RANGE_CODE(task_node->control_code)) {
	case RANGE_TYPE_BLOCK:
		break;
	case RANGE_TYPE_CHUNK:
		LOG_DEBUG(_T("get sync match chunk block sums section."));
		if(BlockMatchChunkRange(&pTRANSSession->control, &pTRANSSession->inte_value, szFileName)) 
			return -1;
		LOG_DEBUG(_T("get sync post build data section."));
		if (Post(task_node)) return -1;
		break;
	case RANGE_TYPE_FILE:
		LOG_DEBUG(_T("get sync match block file sums section."));
		if(BlockMatchFileRange(&pTRANSSession->control, &pTRANSSession->inte_value, szFileName)) 
			return -1;
		LOG_DEBUG(_T("get sync post build data section."));
		if (Post(task_node)) return -1;
		break;
	}

	return 0;
}

#define OPEN_MULTI_PUTSUMS_SEND_SE(TASK_NODE, PCHECK_SUMS, CHECKSUM_TYPE) \
	switch(CHECKSUM_TYPE) { \
	case SIMPLE_CSUM: \
	m_pTRANSSessionBzl->OpenPutSimpleCsumsSendSe(TASK_NODE, (struct simple_sums *)PCHECK_SUMS); \
	break; \
	case COMPLEX_CSUM: \
	m_pTRANSSessionBzl->OpenPutComplexCsumsSendSe(TASK_NODE, (struct complex_sums *)PCHECK_SUMS); \
	break; \
}

DWORD CSDTPUtility::PutCsum(OUT vector<struct local_match *> &vlocal_match, struct TaskNode *task_node, void *psums, DWORD dwCsumType) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	OPEN_MULTI_PUTSUMS_SEND_SE(task_node, psums, dwCsumType)
		NValueLayer::PutCsumRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildPutCsumSend(psbuffer, &RequestValue);
	LOG_DEBUG(_T("put checksum send."));
	///////////////////////////////////////////////////////////////////////////////	
	size_t nSendLength = 0;
	DWORD dwHandleValue = -1;
	DWORD dwFirstEntry = 0;
	do{
		if (!dwFirstEntry) {
			dwHandleValue = NTRANSHandler::HandlePutCsumSend(pTRANSSession, psbuffer->body_buffer, &psbuffer->blen);
			nSendLength = psbuffer->hlen + psbuffer->blen;
			dwFirstEntry++;
		} else {
			nSendLength = RECV_BUFF_SIZE;
			dwHandleValue = NTRANSHandler::HandlePutCsumSend(pTRANSSession, psbuffer->srecv_buffer, &nSendLength);
		}
		if(-1 != dwHandleValue) {
			psbuffer->srlength = m_pTRANSSockets->Send(psbuffer->srecv_buffer, nSendLength);
			if (0 > psbuffer->srlength) dwHandleValue = -1;
		}
	} while(dwHandleValue > 0);
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->ClosePutCsumSendSe();
	// if(dwHandleValue) return false;
	LOG_DEBUG(_T("put checksum receive."));
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenPutCsumRecvSe(task_node);
	///////////////////////////////////////////////////////////////////////////////
	dwHandleValue = -1;
	dwFirstEntry = 0;
	do {
		memset(psbuffer->srecv_buffer, '\0', sizeof(psbuffer->srecv_buffer));
		psbuffer->srlength = m_pTRANSSockets->Receive(psbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > psbuffer->srlength) break;

		if (!dwFirstEntry) {
			psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
			psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);
			//////////////////////////////////////////////////////////////////////////////////////////////////
			if(STATUS_OK != NSDTProtocol::ParsePutCsumRecv(&ResponseValue, psbuffer->head_buffer)) break;
			NValueLayer::PutCsumResponse(&pTRANSSession->inte_value, &ResponseValue);
			///////////////////////////////////////////////////////////////////////////////////////////////////
			dwHandleValue = NTRANSHandler::HandlePutCsumRecv(pTRANSSession, psbuffer->body_buffer, psbuffer->blen);
			dwFirstEntry++;
		} else dwHandleValue = NTRANSHandler::HandlePutCsumRecv(pTRANSSession, psbuffer->srecv_buffer, psbuffer->srlength);
	} while(dwHandleValue > 0);
	LOG_DEBUG(_T("bulid local match begain."));
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->ClosePutCsumRecvSe(vlocal_match);
	LOG_DEBUG(_T("put checksum OK."));
	// return dwHandleValue? false: true;

	return 0;
}

DWORD CSDTPUtility::Putsync(struct TaskNode *task_node) {
	vector<struct local_match *> vlocal_match;
	struct FileBuilder builder; 
	TCHAR szFileName[MAX_PATH];

	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct InternalValue *psvalue = &pTRANSSession->inte_value;
	memset(&builder, '\0', sizeof(FileBuilder));
	nstriutility::full_path(szFileName, pTRANSSession->list_inform->szLocation, task_node->szFileName);

	switch(TNODE_RANGE_CODE(task_node->control_code)) {
	case RANGE_TYPE_BLOCK:
		break;
	case RANGE_TYPE_CHUNK: {
		struct complex_sums *psums_complex = BuildChunkComplexCsum(szFileName, task_node->offset);
		if(!psums_complex) return -1;

		LOG_DEBUG(_T("put sync put sums section."));
		if(PutCsum(vlocal_match, task_node, (void*)psums_complex, COMPLEX_CSUM))
			return -1;
		DeleteComplexCsum(psums_complex);
		}
		break;
	case RANGE_TYPE_FILE: {
		struct complex_sums *psums_complex = BuildFileComplexCsum(szFileName, task_node->offset);
		if(!psums_complex) return -1;
		LOG_DEBUG(_T("put sync put sums section."));
		if(PutCsum(vlocal_match, task_node, (void*)psums_complex, COMPLEX_CSUM))
			return -1;	
		DeleteComplexCsum(psums_complex);
		}
		break;
	}

	LOG_DEBUG(_T("put sync build local data section."));
	_tcscpy_s(builder.file_name, szFileName);
	builder.build_type = psvalue->range_type; 
	builder.offset = psvalue->offset;
	builder.length = psvalue->content_length; 
	_tcscpy_s(builder.build_cache, task_node->build_cache);
	if(NBuildUtility::ContentBuilder(&builder, &vlocal_match)) return -1;

	LOG_DEBUG(_T("put sync build local data OK."));
	DeleteMatchVec(&vlocal_match);

	///////////////////////////////////////////////////////////////////////////////
	NFileUtility::SetFileAttrib(szFileName, psvalue->file_length, &psvalue->last_write);

	return 0;
}

DWORD CSDTPUtility::Final(DWORD dwFinalType) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	NValueLayer::FinalRequest(&RequestValue, dwFinalType, pTRANSSession->list_inform);
	NSDTProtocol::BuildFinalSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->SendReceive(psbuffer->srecv_buffer, psbuffer->srecv_buffer);
	if (0 > psbuffer->srlength) return -1;
	///////////////////////////////////////////////////////////////////////////////
	pTRANSSession->inte_value.final_status = dwFinalType;
	psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
	switch(dwFinalType) {
	case FINAL_STATUS_END:
		if(STATUS_OK != NSDTProtocol::ParseFinalEndRecv(&ResponseValue, psbuffer->head_buffer)) return -1;	
		NValueLayer::FinalEndResponse(&pTRANSSession->inte_value, &ResponseValue);
		break;
	case FINAL_STATUS_SUCCESS:
		if(STATUS_OK != NSDTProtocol::ParseFinalSuccessRecv(&ResponseValue, psbuffer->head_buffer)) return -1;
		NValueLayer::FinalSuccessResponse(&pTRANSSession->inte_value, &ResponseValue);
		break;
	case FINAL_STATUS_FINISH:
		if(STATUS_OK != NSDTProtocol::ParseFinalFinishRecv(&ResponseValue, psbuffer->head_buffer)) return -1;	
		NValueLayer::FinalFinishResponse(&pTRANSSession->inte_value, &ResponseValue);
		break;
	}

	return 0;
}

DWORD CSDTPUtility::GetFileVerifyCsums(HANDLE hFileVerify, struct TaskNode *task_node) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenFileVerifySe(task_node, hFileVerify);
	NValueLayer::GetCsumRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildGetCsumSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->Send(psbuffer->srecv_buffer, strlen(psbuffer->srecv_buffer));
	if (0 > psbuffer->srlength) return -1;
	///////////////////////////////////////////////////////////////////////////////
	DWORD dwHandleValue = -1;
	DWORD dwFirstEntry = 0;
	do{
		memset(psbuffer->srecv_buffer, '\0', sizeof(psbuffer->srecv_buffer));
		psbuffer->srlength = m_pTRANSSockets->Receive(psbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > psbuffer->srlength) break;

		if (!dwFirstEntry) {
			psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
			psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);
			if(STATUS_OK != NSDTProtocol::ParseGetCsumRecv(&ResponseValue, psbuffer->head_buffer)) break;
			NValueLayer::GetCsumResponse(&pTRANSSession->inte_value, &ResponseValue);

			dwHandleValue = NTRANSHandler::HandleGetCsumRecv(pTRANSSession, psbuffer->body_buffer, psbuffer->blen);
			dwFirstEntry++;
		} else dwHandleValue = NTRANSHandler::HandleGetCsumRecv(pTRANSSession, psbuffer->srecv_buffer, psbuffer->srlength);
	} while(dwHandleValue > 0);
	///////////////////////////////////////////////////////////////////////////////

	return 0;
}

DWORD CSDTPUtility::GetChunkVerifyCsums(HANDLE hFileVerify, struct TaskNode *task_node) {
	struct TRANSSession *pTRANSSession = m_pTRANSSessionBzl->GetTRANSSession();
	struct SessionBuffer *psbuffer = &pTRANSSession->buffer;	
	//////////////////////////////////////////////
	struct ServRequest RequestValue;
	struct ServResponse ResponseValue;
	///////////////////////////////////////////////////////////////////////////////
	m_pTRANSSessionBzl->OpenChunkVerifySe(task_node, hFileVerify);
	NValueLayer::GetCsumRequest(&RequestValue, pTRANSSession->list_inform, &pTRANSSession->inte_value);
	NSDTProtocol::BuildGetCsumSend(psbuffer, &RequestValue);
	///////////////////////////////////////////////////////////////////////////////
	psbuffer->srlength = m_pTRANSSockets->Send(psbuffer->srecv_buffer, strlen(psbuffer->srecv_buffer));
	if (0 > psbuffer->srlength) return -1;
	///////////////////////////////////////////////////////////////////////////////
	DWORD dwHandleValue = -1;
	DWORD dwFirstEntry = 0;
	do{
		memset(psbuffer->srecv_buffer, '\0', sizeof(psbuffer->srecv_buffer));
		psbuffer->srlength = m_pTRANSSockets->Receive(psbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > psbuffer->srlength) break;

		if (!dwFirstEntry) {
			psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
			psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);
			if(STATUS_OK != NSDTProtocol::ParseGetCsumRecv(&ResponseValue, psbuffer->head_buffer)) break;
			NValueLayer::GetCsumResponse(&pTRANSSession->inte_value, &ResponseValue);

			dwHandleValue = NTRANSHandler::HandleGetCsumRecv(pTRANSSession, psbuffer->body_buffer, psbuffer->blen);
			dwFirstEntry++;
		} else dwHandleValue = NTRANSHandler::HandleGetCsumRecv(pTRANSSession, psbuffer->srecv_buffer, psbuffer->srlength);
	}while(dwHandleValue > 0);
	///////////////////////////////////////////////////////////////////////////////

	return 0;
}