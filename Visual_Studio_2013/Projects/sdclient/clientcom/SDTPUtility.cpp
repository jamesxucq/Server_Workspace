#include "StdAfx.h"

#include "client_macro.h"
#include "third_party.h"
#include "Session.h"
#include "TRANSSeionBzl.h"
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
m_pTRANSSeionBzl(NULL),
m_pTRANSSocke(NULL)
{
}

CSDTPUtility::~CSDTPUtility(void) {
}

DWORD CSDTPUtility::Create(struct SessionArgu *pSeionArgu, struct SocketsArgu *pSockeArgu) {
	if(!pSeionArgu || !pSockeArgu) return ((DWORD)-1);
	//
	m_pTRANSSeionBzl = NTRANSSessionBzl::Factory(pSeionArgu);
	if (!m_pTRANSSeionBzl) return ((DWORD)-1);
	//
	CONVERT_SOCKETS_ARGU(pSockeArgu, m_pTRANSSeionBzl->GetListData());
	m_pTRANSSocke = NTRANSSockets::Factory(pSockeArgu); // Todo: 20110411
// _LOG_ANSI("sin_addr:%s\r\n", pSockeArgu->tServAddress.sin_addr);
// _LOG_DEBUG(_T("sin_port:%d"), pSockeArgu->tServAddress.sin_port);
	if (m_pTRANSSocke->Connect(pSockeArgu->tServAddress.sin_addr, pSockeArgu->tServAddress.sin_port))
		return ((DWORD)-1);
// _LOG_DEBUG(_T("connect ok!"));
	return 0x00;
}

VOID CSDTPUtility::Destroy() {
	if(m_pTRANSSocke) {
// _LOG_DEBUG(_T("close sockets!"));
		m_pTRANSSocke->Close();
		NTRANSSockets::DestroyObject(m_pTRANSSocke);
	}
	m_pTRANSSocke = NULL;
	//
	if(m_pTRANSSeionBzl) NTRANSSessionBzl::DestroyObject(m_pTRANSSeionBzl);
	m_pTRANSSeionBzl = NULL;
}

int CSDTPUtility::Initial() {  // -1 fault 0 success 1 network

	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	NValueLayer::InitialRequest(&tReqValue, pTRANSSeion->ldata);
	NSDTProtocol::BuildInitialSend(sbuffer, &tReqValue);
	//
// _LOG_DEBUG(_T("initial send!"));
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) return PROTO_NETWORK;
	//
// _LOG_DEBUG(_T("initial receive ok!"));
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
	if(OK == NSDTProtocol::ParseInitialRecv(&tResValue, sbuffer->head_buffer)) {
		NValueLayer::InitialResponse(&pTRANSSeion->ivalue, &tResValue);
		handle_value = PROTO_OKAY;
	} else handle_value = PROTO_FAULT;
	//
// _LOG_DEBUG(_T("initial handle_value:%d"), handle_value);
	return handle_value;
}

int CSDTPUtility::KeepAlive() {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	NValueLayer::KeepAliveRequest(&tReqValue, pTRANSSeion->ldata);
	NSDTProtocol::BuildKeepAliveSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) return PROTO_NETWORK;
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
	if(OK == NSDTProtocol::ParseKeepAliveRecv(&tResValue, sbuffer->head_buffer)) {
		NValueLayer::KeepAliveResponse(&pTRANSSeion->ivalue, &tResValue);
		handle_value = PROTO_OKAY;
	} else handle_value = PROTO_FAULT;
	//
	return handle_value;
}

int CSDTPUtility::HeadList(unsigned __int64 *cleng, TCHAR *list_chks, DWORD dwListType, DWORD dwAnchor, TCHAR *szDirectory) { // -1 fault 0 success 1 network 2 processing 4 not found
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	if (m_pTRANSSeionBzl->OpenHeadListSe(dwListType, dwAnchor, szDirectory))
		return PROTO_FAULT;
	NValueLayer::HeadListRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildHeadListSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseHeadListSe();
		return PROTO_NETWORK;
	}
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
	DWORD proto_value = NSDTProtocol::ParseHeadListRecv(&tResValue, sbuffer->head_buffer);
	if(OK == proto_value) {
		NValueLayer::HeadListResponse(&pTRANSSeion->ivalue, &tResValue);
		// NTRANSHandler::HandleHeadListRecv(pTRANSSeion);
		handle_value = PROTO_OKAY;
	} else {
		if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
		else if(NOT_FOUND == proto_value) handle_value = PROTO_NOT_FOUND;
		else handle_value = PROTO_FAULT;
	}
	//
	m_pTRANSSeionBzl->CloseHeadListSe();
	*cleng = pTRANSSeion->ivalue.content_length;
	_tcscpy_s(list_chks, MD5_TEXT_LENGTH, pTRANSSeion->ivalue.data_buffer);
_LOG_DEBUG(_T("*cleng:%d list_chks:%s"), *cleng, list_chks);
_LOG_DEBUG(_T("handle_value:%d"), handle_value); // disable by james 20140410
	return handle_value;
}

int CSDTPUtility::HeadFile(unsigned __int64 *cleng, FILETIME *last_write, TCHAR *szFileName) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenHeadFileSe(szFileName);
	NValueLayer::HeadFileRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildHeadFileSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseGetAnchorSe();
		return PROTO_NETWORK;
	}
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
	if(OK == NSDTProtocol::ParseHeadFileRecv(&tResValue, sbuffer->head_buffer)) {
		NValueLayer::HeadFileResponse(&pTRANSSeion->ivalue, &tResValue);
		// NTRANSHandler::HandleHeadFileRecv(pTRANSSeion);
		handle_value = PROTO_OKAY;
	} else handle_value = PROTO_FAULT;
	//
	m_pTRANSSeionBzl->CloseHeadFileSe();
	*cleng = pTRANSSeion->ivalue.content_length;
	memcpy(last_write, &pTRANSSeion->ivalue.last_write, sizeof(FILETIME));
	// _LOG_DEBUG(_T("handle_value:%d"), handle_value); // disable by james 20140410
	return handle_value;
}

int CSDTPUtility::GetAnchor(DWORD *slast_anchor) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	struct XmlAnchor tXmlAnchor;
	//
	if (m_pTRANSSeionBzl->OpenGetAnchorSe(&tXmlAnchor)) return PROTO_FAULT;
	NValueLayer::GetAnchorRequest(&tReqValue, pTRANSSeion->ldata);
	NSDTProtocol::BuildGetAnchorSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseGetAnchorSe();
		return PROTO_NETWORK;
	}
// BinaryLogger::LogBinaryCode(sbuffer->srecv_buffer, sbuffer->srlength);
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
// BinaryLogger::LogBinaryCode(sbuffer->head_buffer, sbuffer->hlen);
	sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
// BinaryLogger::LogBinaryCode(sbuffer->body_buffer, sbuffer->blen);
// _LOG_DEBUG(_T("sbuffer->srlength:%d sbuffer->hlen:%d sbuffer->blen:%d "), sbuffer->srlength, sbuffer->hlen, sbuffer->blen);
	if(OK == NSDTProtocol::ParseGetAnchorRecv(&tResValue, sbuffer->head_buffer)) {
		NValueLayer::GetAnchorResponse(&pTRANSSeion->ivalue, &tResValue);
		NTRANSHandler::HandleGetAnchorRecv(pTRANSSeion, sbuffer->body_buffer, sbuffer->blen);
		handle_value = PROTO_OKAY;
	} else handle_value = PROTO_FAULT;
	//
	m_pTRANSSeionBzl->CloseGetAnchorSe();
	*slast_anchor = tXmlAnchor.slast_anchor;
// _LOG_DEBUG(_T("handle_value:%d tXmlAnchor.slast_anchor:%u"), handle_value, tXmlAnchor.slast_anchor);
	return handle_value;
}

// FilesVec sattent
int CSDTPUtility::GetList(struct ListNode *list_node) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	if (m_pTRANSSeionBzl->OpenGetListSe(list_node))
		return PROTO_FAULT;
	NValueLayer::GetListRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildGetListSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, strlen(sbuffer->srecv_buffer));
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseGetListSe();
		return PROTO_NETWORK;
	}
	//
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > sbuffer->srlength) {
			handle_value = PROTO_NETWORK;
			break;
		}
		//
		if (!first_entry) {
			sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
			sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
			//
			if(OK != NSDTProtocol::ParseGetListRecv(&tResValue, sbuffer->head_buffer)) {
				handle_value = PROTO_FAULT;
				break;
			}
			NValueLayer::GetListResponse(&pTRANSSeion->ivalue, &tResValue);
			handle_value = NTRANSHandler::HandleGetListRecv(pTRANSSeion, sbuffer->body_buffer, sbuffer->blen);
			//
			first_entry++;
		} else handle_value = NTRANSHandler::HandleGetListRecv(pTRANSSeion, sbuffer->srecv_buffer, sbuffer->srlength);
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->CloseGetListSe();
	// _LOG_DEBUG(_T("handle_value:%d"), handle_value); // disable by james 20140410
	return handle_value;
}

int CSDTPUtility::GetFile(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenGetFileSe(task_node);
	NValueLayer::GetFileRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildGetFileSend(sbuffer, &tReqValue);
	//
// _LOG_ANSI("sbuffer->srecv_buffer:%s", sbuffer->srecv_buffer);
	sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, strlen(sbuffer->srecv_buffer));
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseGetFileSe();
_LOG_DEBUG(_T("send close get file"));
		return PROTO_NETWORK;
	}
// _LOG_DEBUG(_T("send sbuffer->srlength:%d"), sbuffer->srlength);
	//
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
// _LOG_DEBUG(_T("receive sbuffer->srlength:%d"), sbuffer->srlength);
		if (0 > sbuffer->srlength) {
_LOG_DEBUG(_T("receive close get file"));
			handle_value = PROTO_NETWORK;
			break;
		}
		//
		if (!first_entry) {
			sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
			sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
//
			DWORD proto_value = NSDTProtocol::ParseGetFileRecv(&tResValue, sbuffer->head_buffer);
			if(OK != proto_value) {
				if(NOT_FOUND == proto_value) handle_value = PROTO_NOT_FOUND;
				else handle_value = PROTO_FAULT;
				break;
			}
			NValueLayer::GetFileResponse(&pTRANSSeion->ivalue, &tResValue);
			handle_value = NTRANSHandler::HandleGetFileRecv(pTRANSSeion, sbuffer->body_buffer, sbuffer->blen);
			//
			first_entry++;
		} else handle_value = NTRANSHandler::HandleGetFileRecv(pTRANSSeion, sbuffer->srecv_buffer, sbuffer->srlength);
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->CloseGetFileSe();
// _LOG_DEBUG(_T("get file handle_value:%d"), handle_value); // disable by james 20140410
	return handle_value;
}


int CSDTPUtility::PutFile(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
// _LOG_DEBUG(_T("put file task_node->node_inde:%u control_code:%08x"), task_node->node_inde, task_node->control_code); // disable by james 20140410
	//
	DWORD openValue = m_pTRANSSeionBzl->OpenPutFileSe(task_node);
	if(openValue) {
_LOG_DEBUG(_T("open put file."));
		if(OPEN_FAULT == openValue) return PROTO_FAULT;
		else if(OPEN_NOT_FOUND == openValue) return PROTO_CREATE;
		else if(OPEN_VIOLATION == openValue) return PROTO_LOCKED;
	}
	NValueLayer::PutFileRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildPutFileSend(sbuffer, &tReqValue);
	//
	size_t send_length = 0;
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		if (!first_entry) {
			handle_value = NTRANSHandler::HandlePutFileSend(pTRANSSeion, sbuffer->body_buffer, &sbuffer->blen);
			send_length = sbuffer->hlen + sbuffer->blen;
			first_entry++;
// _LOG_DEBUG(_T("xxxxx1 handle_value:%d first_entry:%d blen:%d body_buffer:|%s|"), handle_value, first_entry, sbuffer->blen, sbuffer->body_buffer);
// _LOG_DEBUG(_T("xxxxx1 handle_value:%d first_entry:%d blen:%d"), handle_value, first_entry, sbuffer->blen);
		} else {
			send_length = RECV_BUFF_SIZE;
			handle_value = NTRANSHandler::HandlePutFileSend(pTRANSSeion, sbuffer->srecv_buffer, &send_length);
// _LOG_DEBUG(_T("xxxxx2 handle_value:%d first_entry:%d blen:%d body_buffer:|%s|"), handle_value, first_entry, sbuffer->blen, sbuffer->body_buffer);
// _LOG_DEBUG(_T("xxxxx2 handle_value:%d first_entry:%d send_length:%d"), handle_value, first_entry, send_length);
		}
		if(PROTO_FAULT != handle_value) {
			// BinaryLogger::LogBinaryCode(sbuffer->body_buffer, sbuffer->blen);
// _LOG_DEBUG(_T("---------------- task_node->node_inde:%d length:%d handle_value:%d"), task_node->node_inde, send_length, handle_value);
			sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, send_length);
// _LOG_DEBUG(_T("---------------- task_node->node_inde:%d sbuffer->srlength:%d"), task_node->node_inde, sbuffer->srlength);
			if (0 > sbuffer->srlength) handle_value = PROTO_NETWORK;
		}
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->ClosePutFileSe(task_node);
// _LOG_DEBUG(_T("close handle_value:%08x"), handle_value);
	if (handle_value) return handle_value;
	//
	sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
	if (0 <= sbuffer->srlength) {
		sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
		if(OK != NSDTProtocol::ParsePutFileRecv(&tResValue, sbuffer->head_buffer)) handle_value = PROTO_FAULT;
		NValueLayer::PutFileResponse(&pTRANSSeion->ivalue, &tResValue);
	} else handle_value = PROTO_NETWORK;
	//
_LOG_DEBUG(_T("put file handle_value:%08x"), handle_value);
	return handle_value;
}

int CSDTPUtility::MoveLocal(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	ListData *ldata = m_pTRANSSeionBzl->GetTRANSSession()->ldata;
	TCHAR szExistName[MAX_PATH];
	TCHAR szNewName[MAX_PATH];
	//
	struti::full_path(szExistName, ldata->szLocation, task_node->szFileName1);
	struti::full_path(szNewName, ldata->szLocation, task_node->szFileName2);
	if(!NFileUtility::FileExist(szExistName)) return PROTO_FAULT;
	//
    NDireUtility::MakeFileFolder(szNewName);
	if(!MoveFileEx(szExistName, szNewName, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED|MOVEFILE_WRITE_THROUGH)) {
_LOG_WARN(_T("move local file %s new file %s error!"), szExistName, szNewName);
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return PROTO_FAULT;
		} else { return PROTO_LOCKED; }
	}
	//
	return PROTO_OKAY;
}

#define MAX_COPY_LEN ((int)1 << 28) // 256M

int CSDTPUtility::CopyLocal(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	ListData *ldata = m_pTRANSSeionBzl->GetTRANSSession()->ldata;
	TCHAR szExistName[MAX_PATH];
	TCHAR szNewName[MAX_PATH];
	//
	struti::full_path(szExistName, ldata->szLocation, task_node->szFileName1);
	struti::full_path(szNewName, ldata->szLocation, task_node->szFileName2);
	if(!NFileUtility::FileExist(szExistName)) return PROTO_FAULT;
	//
    NDireUtility::MakeFileFolder(szNewName);
	DWORD copy_value = NFileUtility::BlockCopyFile(szExistName, szNewName, MAX_COPY_LEN);
	if(copy_value) {
_LOG_DEBUG(_T("copy value local."));
		if(BCOPY_CONTINUE == copy_value) return PROTO_PROCESSING;
		else if(BCOPY_NOT_FOUND == copy_value) return PROTO_FAULT;
		else if(BCOPY_VIOLATION == copy_value) return PROTO_LOCKED;
		else { return PROTO_FAULT; }
// _LOG_WARN(_T("copy local file %s new file %s error!"), szExistName, szNewName);
	}
	//
	return PROTO_OKAY;
}

int CSDTPUtility::MoveServer(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenMoveServSe(task_node);
	NValueLayer::MoveRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildMoveSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) return PROTO_NETWORK;
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	DWORD proto_value = NSDTProtocol::ParseMoveRecv(&tResValue, sbuffer->head_buffer);
	if(OK == proto_value) {
		NValueLayer::MoveResponse(&pTRANSSeion->ivalue, &tResValue);
		handle_value = PROTO_OKAY;
	} else {
		if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
		else if(NOT_FOUND == proto_value) handle_value = PROTO_FAULT;
		else handle_value = PROTO_FAULT;
	}
	//
	return handle_value;
}

int CSDTPUtility::CopyServer(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenCopyServSe(task_node);
	NValueLayer::CopyRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildCopySend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) return PROTO_NETWORK;
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	DWORD proto_value = NSDTProtocol::ParseCopyRecv(&tResValue, sbuffer->head_buffer);
	if(OK == proto_value) {
		NValueLayer::CopyResponse(&pTRANSSeion->ivalue, &tResValue);
		handle_value = PROTO_OKAY;
	} else {
		if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
		else if(NOT_FOUND == proto_value) handle_value = PROTO_FAULT;
		else handle_value = PROTO_FAULT;
	}
	//
	return handle_value;
}

int CSDTPUtility::DeleLocaFile(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	ListData *ldata = m_pTRANSSeionBzl->GetTRANSSession()->ldata;
	TCHAR szDeleName[MAX_PATH];
	//
	struti::full_path(szDeleName, ldata->szLocation, task_node->szFileName1);
	if(!NFileUtility::FileExist(szDeleName)) return PROTO_OKAY;
	//
	if(!DeleteFile(szDeleName)) {
_LOG_WARN(_T("delete client file %s error!"), szDeleName);
		if(ERROR_SHARING_VIOLATION==GetLastError() || ERROR_LOCK_VIOLATION==GetLastError()) {
			return PROTO_LOCKED;
		}
	}
		// return DeleteFile(DeliName)? true: false; // delete by james 20110307	
	//
	return PROTO_OKAY;
}

int CSDTPUtility::DeleServFile(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenDeleServSe(task_node);
	NValueLayer::DeleteRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildDeleteSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) return PROTO_NETWORK;
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	DWORD proto_value = NSDTProtocol::ParseDeleteRecv(&tResValue, sbuffer->head_buffer);
	if(OK == proto_value) {
		NValueLayer::DeleteResponse(&pTRANSSeion->ivalue, &tResValue);
		handle_value = PROTO_OKAY;
	} else {
		if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
		else if(NOT_FOUND == proto_value) handle_value = PROTO_NOT_FOUND;
		else handle_value = PROTO_FAULT;
	}
	//
	return handle_value;
}

int CSDTPUtility::GetChks(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenGetChksSe(task_node);
	NValueLayer::GetChksRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildGetChksSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, strlen(sbuffer->srecv_buffer));
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseGetChksSe();
		return PROTO_NETWORK;
	}
	//
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > sbuffer->srlength) {
			handle_value = PROTO_NETWORK;
			break;
		}
		//
		if (!first_entry) {
			sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
			sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
			DWORD proto_value = NSDTProtocol::ParseGetChksRecv(&tResValue, sbuffer->head_buffer);
			if(OK != proto_value) {
				if(NO_CONTENT == proto_value) handle_value = PROTO_NO_CONTENT;
				else if(NOT_FOUND == proto_value) handle_value = PROTO_NOT_FOUND;
				else handle_value = PROTO_FAULT;
				break;
			}
			NValueLayer::GetChksResponse(&pTRANSSeion->ivalue, &tResValue);
			handle_value = NTRANSHandler::HandleGetChksRecv(pTRANSSeion, sbuffer->body_buffer, sbuffer->blen);
			//
			first_entry++;
		} else handle_value = NTRANSHandler::HandleGetChksRecv(pTRANSSeion, sbuffer->srecv_buffer, sbuffer->srlength);
_LOG_DEBUG(_T("handle_value:%d node_inde:%d"), handle_value, task_node->node_inde); // disable by james 20140410
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->CloseGetChksSe();
_LOG_DEBUG(_T("handle_value:%d node_inde:%d"), handle_value, task_node->node_inde); // disable by james 20140410
	return handle_value;
}

int CSDTPUtility::Post(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenPostSe(task_node);
	NValueLayer::PostRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildPostSend(sbuffer, &tReqValue);
// _LOG_DEBUG(_T("sbuffer->hlen=%d"), sbuffer->hlen); // disable by james 20140410
	//
	size_t send_length = 0;
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		if (!first_entry) {
			handle_value = NTRANSHandler::HandlePostSend(pTRANSSeion, sbuffer->body_buffer, &sbuffer->blen);
			send_length = sbuffer->hlen + sbuffer->blen;
			first_entry++;
		} else {
			send_length = RECV_BUFF_SIZE;
			handle_value = NTRANSHandler::HandlePostSend(pTRANSSeion, sbuffer->srecv_buffer, &send_length);
		}
		if(PROTO_FAULT != handle_value) {
// _LOG_DEBUG(_T("handle_value=%d, send_length=%d"), handle_value, send_length); // disable by james 20140410
			sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, send_length);
			if (0 > sbuffer->srlength) handle_value = PROTO_NETWORK;
// _LOG_DEBUG(_T("sbuffer->srlength=%d"), sbuffer->srlength); // disable by james 20140410
		}
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->ClosePostSe();
	// if(handle_value) return false;
	//
	sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
	if (0 <= sbuffer->srlength) {
		sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
		DWORD proto_value = NSDTProtocol::ParsePostRecv(&tResValue, sbuffer->head_buffer);
		if(OK == proto_value) {
			NValueLayer::PostResponse(&pTRANSSeion->ivalue, &tResValue);
			handle_value = PROTO_OKAY;
		} else {
			if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
			else handle_value = PROTO_FAULT;
		}
	} else handle_value = PROTO_NETWORK;
	//
	return handle_value;
}

#define RETURN_HANDLE_VALUE(HANDLE_VALUE, MATCH_VALUE) \
	else if(MATCH_NOT_FOUND == MATCH_VALUE) HANDLE_VALUE = PROTO_CREATE; \
	else if(MATCH_VIOLATION == MATCH_VALUE) HANDLE_VALUE = PROTO_LOCKED; \
	else HANDLE_VALUE = PROTO_FAULT;

int CSDTPUtility::GetSynchron(struct TaskNode *task_node) { // -1 fault 0 success 1 network
	TCHAR szFileName[MAX_PATH];
	int handle_value = PROTO_FAULT;
	//
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struti::full_path(szFileName, pTRANSSeion->ldata->szLocation, task_node->szFileName1);
	//
// _LOG_DEBUG(_T("get sync get sums section.")); // disable by james 20140410
	handle_value = GetChks(task_node);
// _LOG_DEBUG(_T("handle_value:%d node_inde:%d"), handle_value, task_node->node_inde);
	if (handle_value && (PROTO_NO_CONTENT != handle_value))
		return handle_value;
	//
	DWORD matchValue = MATCH_SUCCESS;
	switch(TNODE_RANGE_BYTE(task_node->control_code)) {
	case RANGE_TYPE_BLOCK:
		break;
	case RANGE_TYPE_CHUNK:
// _LOG_DEBUG(_T("get sync match chunk block sums section.")); // disable by james 20140410
		if(!handle_value) {
			matchValue = MUtil::BlockMatchChunk(&pTRANSSeion->control, &pTRANSSeion->ivalue, szFileName);
			if(!matchValue) {
// _LOG_DEBUG(_T("chunk get sync post build data section.")); // disable by james 20140410
				do {
					handle_value = Post(task_node);
// _LOG_DEBUG(_T("handle_value:%d"), handle_value);
				} while(PROTO_PROCESSING == handle_value);
			} RETURN_HANDLE_VALUE(handle_value, matchValue)
		} else if(PROTO_NO_CONTENT == handle_value) {
			matchValue = MUtil::MatchZeroChunk(&pTRANSSeion->control, &pTRANSSeion->ivalue, szFileName);
			if(!matchValue) {
// _LOG_DEBUG(_T("chunk get sync post build data section.")); // disable by james 20140410
				do {
					handle_value = Post(task_node);
// _LOG_DEBUG(_T("handle_value:%d"), handle_value);
				} while(PROTO_PROCESSING == handle_value);
			} RETURN_HANDLE_VALUE(handle_value, matchValue)
		}
		MUtil::CloseBlockMatch(&pTRANSSeion->control);
		break;
	case RANGE_TYPE_FILE:
// _LOG_DEBUG(_T("get sync match block file sums section.")); // disable by james 20140410
		if(!handle_value) {
			matchValue = MUtil::BlockMatchFile(&pTRANSSeion->control, &pTRANSSeion->ivalue, szFileName);
			if(!matchValue) {
// _LOG_DEBUG(_T("file get sync post build data section.")); // disable by james 20140410
				do {
					handle_value = Post(task_node);
// _LOG_DEBUG(_T("handle_value:%d"), handle_value);
				} while(PROTO_PROCESSING == handle_value);
			} RETURN_HANDLE_VALUE(handle_value, matchValue)
		} else if(PROTO_NO_CONTENT == handle_value) {
			matchValue = MUtil::MatchZeroFile(&pTRANSSeion->control, &pTRANSSeion->ivalue, szFileName);
			if(!matchValue) {
// _LOG_DEBUG(_T("file get sync post build data section.")); // disable by james 20140410
				do {
					handle_value = Post(task_node);
// _LOG_DEBUG(_T("handle_value:%d"), handle_value);
				} while(PROTO_PROCESSING == handle_value);
			} RETURN_HANDLE_VALUE(handle_value, matchValue)
		}
		MUtil::CloseBlockMatch(&pTRANSSeion->control);
		break;
	}
	//
// _LOG_DEBUG(_T("handle_value:%d"), handle_value);
	return handle_value;
}

#define OPEN_MULTI_PUTSUMS_SEND_SE(TASK_NODE, PCHECKS, CHECKS_TYPE) \
	switch(CHECKS_TYPE) { \
	case SIMPLE_CHKS: \
	m_pTRANSSeionBzl->OpenPutSimpChksSendSe(TASK_NODE, (struct simple_head *)PCHECKS); \
	break; \
	case COMPLEX_CHKS: \
	m_pTRANSSeionBzl->OpenPutCompChksSendSe(TASK_NODE, (struct complex_head *)PCHECKS); \
	break; \
	case WHOLE_CHKS: \
	break; \
}

int CSDTPUtility::PutChks(vector<struct local_match *> &vlocal_match, struct TaskNode *task_node, VOID *chk_sums, DWORD dwChksType) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	OPEN_MULTI_PUTSUMS_SEND_SE(task_node, chk_sums, dwChksType)
	NValueLayer::PutChksRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildPutChksSend(sbuffer, &tReqValue);
_LOG_DEBUG(_T("put chks send.")); // disable by james 20140410
	//
	size_t send_length = 0;
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		if (!first_entry) {
			handle_value = NTRANSHandler::HandlePutChksSend(pTRANSSeion, sbuffer->body_buffer, &sbuffer->blen);
			send_length = sbuffer->hlen + sbuffer->blen;
			first_entry++;
		} else {
			send_length = RECV_BUFF_SIZE;
			handle_value = NTRANSHandler::HandlePutChksSend(pTRANSSeion, sbuffer->srecv_buffer, &send_length);
		}
		if(PROTO_FAULT != handle_value) {
			sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, send_length);
			if (0 > sbuffer->srlength) handle_value = PROTO_NETWORK;
		}
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->ClosePutChksSendSe();
	// if(handle_value) return false;
_LOG_DEBUG(_T("put chks receive.")); // disable by james 20140410
	//
	m_pTRANSSeionBzl->OpenPutChksRecvSe(task_node);
	//
	handle_value = PROTO_NETWORK;
	first_entry = 0x00;
	do {
		sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > sbuffer->srlength) {
			handle_value = PROTO_NETWORK;
			break;
		}
		//
		if (!first_entry) {
			sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
			sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
			//
			if(OK != NSDTProtocol::ParsePutChksRecv(&tResValue, sbuffer->head_buffer)) {
				handle_value = PROTO_FAULT;
				break;
			}
			NValueLayer::PutChksResponse(&pTRANSSeion->ivalue, &tResValue);
			//
			handle_value = NTRANSHandler::HandlePutChksRecv(pTRANSSeion, sbuffer->body_buffer, sbuffer->blen);
			first_entry++;
		} else handle_value = NTRANSHandler::HandlePutChksRecv(pTRANSSeion, sbuffer->srecv_buffer, sbuffer->srlength);
	} while(0 < handle_value);
// _LOG_DEBUG(_T("bulid local match begain.")); // disable by james 20140410
	//
	m_pTRANSSeionBzl->ClosePutChksRecvSe(vlocal_match);
_LOG_DEBUG(_T("put chks handle_value:%08X"), handle_value); // disable by james 20140410
	return handle_value;
}

int CSDTPUtility::PutSynchron(struct TaskNode *task_node) {  // -1 fault 0 success 1 network
	vector<struct local_match *> vlocal_match;
	struct FileBuilder builder;
	TCHAR szFileName[MAX_PATH];
	//
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct InteValue *ival = &pTRANSSeion->ivalue;
	memset(&builder, '\0', sizeof(FileBuilder));
	struti::full_path(szFileName, pTRANSSeion->ldata->szLocation, task_node->szFileName1);
	//
_LOG_DEBUG(_T("put sync"));
	int handle_value = PROTO_FAULT;
	DWORD dwChksValue = BCHKS_SUCCESS;
	struct complex_head *phcomp;
	switch(TNODE_RANGE_BYTE(task_node->control_code)) {
	case RANGE_TYPE_BLOCK:
		break;
	case RANGE_TYPE_CHUNK: {
		dwChksValue = FileChks::BuildChunkComplexChks(&phcomp, szFileName, task_node->offset);
		if(dwChksValue) {
_LOG_DEBUG(_T("head complex chunk."));
			if(BCHKS_NOT_FOUND == dwChksValue) return PROTO_CREATE;
			else if(BCHKS_VIOLATION == dwChksValue) return PROTO_LOCKED;	
			else { return PROTO_FAULT; }
		}
		//
_LOG_DEBUG(_T("c put sync put sums section.")); // disable by james 20140410
		if(handle_value = PutChks(vlocal_match, task_node, (VOID*)phcomp, COMPLEX_CHKS)) {
			FileChks::DeleComplexChks(phcomp);	
			MUtil::DeleMatchVec(&vlocal_match);
			return handle_value;
		}
		FileChks::DeleComplexChks(phcomp);
	}
		break;
	case RANGE_TYPE_FILE: {
		dwChksValue = FileChks::BuildFileComplexChks(&phcomp, szFileName, task_node->offset);
		if(dwChksValue) {
_LOG_DEBUG(_T("head complex file."));
			if(BCHKS_NOT_FOUND == dwChksValue) return PROTO_CREATE;
			else if(BCHKS_VIOLATION == dwChksValue) return PROTO_LOCKED;	
			else { return PROTO_FAULT; }
		}
_LOG_DEBUG(_T("f put sync put sums section.")); // disable by james 20140410
		if(handle_value = PutChks(vlocal_match, task_node, (VOID*)phcomp, COMPLEX_CHKS)) {
			FileChks::DeleComplexChks(phcomp);	
			MUtil::DeleMatchVec(&vlocal_match);
			return handle_value;
		}
		FileChks::DeleComplexChks(phcomp);
	}
		break;
	}
	//
_LOG_DEBUG(_T("put sync build local data section.")); // disable by james 20140410
	_tcscpy_s(builder.file_name, szFileName);
	builder.build_type = ival->range_type;
	builder.offset = ival->offset;
	builder.length = ival->content_length;
	_tcscpy_s(builder.builder_cache, task_node->builder_cache);
_LOG_DEBUG(_T("builder :%08X :%s :%llu :%u :%s"), builder.build_type, builder.file_name, builder.offset, builder.length, builder.builder_cache);
	DWORD buildValue = NBuildUtility::ContentBuilder(&builder, &vlocal_match);
_LOG_DEBUG(_T("put sync build local data OK. buildValue:%08X"), buildValue); // disable by james 20140410
	MUtil::DeleMatchVec(&vlocal_match);
	if(buildValue) {
_LOG_DEBUG(_T("build value error."));
		if(BUILD_FAULT == buildValue) return PROTO_FAULT;
		else if(BUILD_NOT_FOUND == buildValue) return PROTO_CREATE;
		else if(BUILD_VIOLATION == buildValue) return PROTO_LOCKED;
	}
	// set last_write at overwrite and finish trans
_LOG_DEBUG(_T("put sync ret okay"));
	return PROTO_OKAY;
}

int CSDTPUtility::Final(DWORD dwFinalType) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	NValueLayer::FinalRequest(&tReqValue, dwFinalType, pTRANSSeion->ldata);
	NSDTProtocol::BuildFinalSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) return PROTO_NETWORK;
	//
	pTRANSSeion->ivalue.final_status = dwFinalType;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	switch(dwFinalType) {
	case FINAL_STATUS_END:
		if(OK != NSDTProtocol::ParseFinalEndRecv(&tResValue, sbuffer->head_buffer)) return PROTO_FAULT;
		NValueLayer::FinalEndResponse(&pTRANSSeion->ivalue, &tResValue);
		break;
	case FINAL_STATUS_SUCCESS:
		if(OK != NSDTProtocol::ParseFinalSuccessRecv(&tResValue, sbuffer->head_buffer)) return PROTO_FAULT;
		NValueLayer::FinalSuccessResponse(&pTRANSSeion->ivalue, &tResValue);
		break;
	case FINAL_STATUS_FINISH:
		if(OK != NSDTProtocol::ParseFinalFinishRecv(&tResValue, sbuffer->head_buffer)) return PROTO_FAULT;
		NValueLayer::FinalFinishResponse(&pTRANSSeion->ivalue, &tResValue);
		break;
	}
	//
	return PROTO_OKAY;
}

int CSDTPUtility::GetFileVerifySha1(HANDLE hFileValid, struct TaskNode *task_node, DWORD dwCacheVerify) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenSha1VerifySe(task_node, hFileValid, dwCacheVerify);
	NValueLayer::GetChksRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildGetChksSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, strlen(sbuffer->srecv_buffer));
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseSha1VerifySe();
		return PROTO_NETWORK;
	}
	//
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > sbuffer->srlength) {
			handle_value = PROTO_NETWORK;
			break;
		}
		//
		if (!first_entry) {
			sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
			sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
			DWORD proto_value = NSDTProtocol::ParseGetChksRecv(&tResValue, sbuffer->head_buffer);
			if(OK != proto_value) {
				if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
				else if(NO_CONTENT == proto_value) handle_value = PROTO_NO_CONTENT;
				else if(NOT_FOUND == proto_value) handle_value = PROTO_NOT_FOUND;
				else handle_value = PROTO_FAULT;
				break;
			}
			NValueLayer::GetChksResponse(&pTRANSSeion->ivalue, &tResValue);
			//
			handle_value = NTRANSHandler::HandleGetChksRecv(pTRANSSeion, sbuffer->body_buffer, sbuffer->blen);
			first_entry++;
		} else handle_value = NTRANSHandler::HandleGetChksRecv(pTRANSSeion, sbuffer->srecv_buffer, sbuffer->srlength);
// _LOG_DEBUG(_T("handle_value:%d"), handle_value);
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->CloseSha1VerifySe();
// _LOG_DEBUG(_T("handle_value:%d"), handle_value); // disable by james 20140410
	return handle_value;
}

int CSDTPUtility::GetFileVerifyChks(HANDLE hFileValid, struct TaskNode *task_node, DWORD dwCacheVerify) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenFileVerifySe(task_node, hFileValid, dwCacheVerify);
	NValueLayer::GetChksRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildGetChksSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, strlen(sbuffer->srecv_buffer));
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseFileVerifySe();
		return PROTO_NETWORK;
	}
// _LOG_DEBUG(_T("s sbuffer->srlength:%d"), sbuffer->srlength);
// BinaryLogger::LogBinaryCode(sbuffer->srecv_buffer, sbuffer->srlength);
	//
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > sbuffer->srlength) {
			handle_value = PROTO_NETWORK;
			break;
		}
// _LOG_DEBUG(_T("r sbuffer->srlength:%d"), sbuffer->srlength);
		//
		if (!first_entry) {
			sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
			sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
// _LOG_DEBUG(_T("sbuffer->hlen:%d sbuffer->blen:%d"), sbuffer->hlen, sbuffer->blen);
			DWORD proto_value = NSDTProtocol::ParseGetChksRecv(&tResValue, sbuffer->head_buffer);
			if(OK != proto_value) {
// BinaryLogger::LogBinaryCode("|r|", 3);
// BinaryLogger::LogBinaryCode(sbuffer->srecv_buffer, sbuffer->srlength);
				if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
				else if(NO_CONTENT == proto_value) handle_value = PROTO_NO_CONTENT;
				else if(NOT_FOUND == proto_value) handle_value = PROTO_NOT_FOUND;
				else handle_value = PROTO_FAULT;
				break;
			}
			NValueLayer::GetChksResponse(&pTRANSSeion->ivalue, &tResValue);
			//
			handle_value = NTRANSHandler::HandleGetChksRecv(pTRANSSeion, sbuffer->body_buffer, sbuffer->blen);
			first_entry++;
		} else handle_value = NTRANSHandler::HandleGetChksRecv(pTRANSSeion, sbuffer->srecv_buffer, sbuffer->srlength);
// _LOG_DEBUG(_T("handle_value:%d"), handle_value);
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->CloseFileVerifySe();
// _LOG_DEBUG(_T("handle_value:%d"), handle_value); // disable by james 20140410
	return handle_value;
}

int CSDTPUtility::GetChunkVerifyChks(HANDLE hFileValid, struct TaskNode *task_node, DWORD dwCacheVerify) {  // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenChunkVerifySe(task_node, hFileValid, dwCacheVerify);
	NValueLayer::GetChksRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildGetChksSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->Send(sbuffer->srecv_buffer, strlen(sbuffer->srecv_buffer));
	if (0 > sbuffer->srlength) {
		m_pTRANSSeionBzl->CloseChunkVerifySe();
		return PROTO_NETWORK;
	}
	//
	int handle_value = PROTO_FAULT;
	DWORD first_entry = 0x00;
	do {
		sbuffer->srlength = m_pTRANSSocke->Receive(sbuffer->srecv_buffer, RECV_BUFF_SIZE);
		if (0 > sbuffer->srlength) {
			handle_value = PROTO_NETWORK;
			break;
		}
		//
		if (!first_entry) {
			sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
			sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
			DWORD proto_value = NSDTProtocol::ParseGetChksRecv(&tResValue, sbuffer->head_buffer);
			if(OK != proto_value) {
				if(NOT_FOUND == proto_value) handle_value = PROTO_NOT_FOUND;
				else handle_value = PROTO_FAULT;
				break;
			}
			NValueLayer::GetChksResponse(&pTRANSSeion->ivalue, &tResValue);

			handle_value = NTRANSHandler::HandleGetChksRecv(pTRANSSeion, sbuffer->body_buffer, sbuffer->blen);
			first_entry++;
		} else handle_value = NTRANSHandler::HandleGetChksRecv(pTRANSSeion, sbuffer->srecv_buffer, sbuffer->srlength);
	} while(0 < handle_value);
	//
	m_pTRANSSeionBzl->CloseChunkVerifySe();
_LOG_DEBUG(_T("handle_value:%d"), handle_value); // disable by james 20140410
	return handle_value;
}

int CSDTPUtility::GetDire(struct TaskNode *task_node) { // -1 fault 0 success 1 network
	ListData *ldata = m_pTRANSSeionBzl->GetTRANSSession()->ldata;
	TCHAR szDirePath[MAX_PATH];
	//
	struti::full_path(szDirePath, ldata->szLocation, task_node->szFileName1);
	CLEAR_LACHR(szDirePath)
	if(!NDireUtility::RecursMakeFolderW(szDirePath))
		_LOG_WARN(_T("make client dire %s error!"), szDirePath);
	//
	return PROTO_OKAY;
}

int CSDTPUtility::PutDire(struct TaskNode *task_node) { // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	// _LOG_DEBUG(_T("put file task_node->node_inde:%u control_code:%08x"), task_node->node_inde, task_node->control_code); // disable by james 20140410
	//
	if(m_pTRANSSeionBzl->OpenPutDireSe(task_node)) return PROTO_CREATE; // modify by james 20140415
	NValueLayer::PutDireRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildPutDireSend(sbuffer, &tReqValue);
	//
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) return PROTO_NETWORK;
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	DWORD proto_value = NSDTProtocol::ParsePutDireRecv(&tResValue, sbuffer->head_buffer);
	if(OK == proto_value) {
		NValueLayer::PutDireResponse(&pTRANSSeion->ivalue, &tResValue);
		handle_value = PROTO_OKAY;
	} else {
		if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
		else handle_value = PROTO_FAULT;
	}
	//
	return handle_value;
}

int CSDTPUtility::DeleLocaDire(struct TaskNode *task_node) { // -1 fault 0 success 1 network
	ListData *ldata = m_pTRANSSeionBzl->GetTRANSSession()->ldata;
	TCHAR szDeleName[MAX_PATH];
	//
	struti::full_path(szDeleName, ldata->szLocation, task_node->szFileName1);
	CLEAR_LACHR(szDeleName)
	if(!NDireUtility::FolderExistW(szDeleName)) return PROTO_OKAY;
	//
	if(!NDireUtility::DeleDirectory(szDeleName)) {
		_LOG_WARN(_T("delete client dire %s error!"), szDeleName);
		return PROTO_LOCKED;
	}
	//
	return PROTO_OKAY;
}

int CSDTPUtility::DeleServDire(struct TaskNode *task_node){ // -1 fault 0 success 1 network
	struct TRANSSession *pTRANSSeion = m_pTRANSSeionBzl->GetTRANSSession();
	struct SeionBuffer *sbuffer = &pTRANSSeion->buffer;
	//
	struct ServRequest tReqValue;
	struct ServResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct ServRequest));
	memset(&tResValue, '\0', sizeof(struct ServResponse));
	//
	m_pTRANSSeionBzl->OpenDeleServSe(task_node);
	NValueLayer::DeleteRequest(&tReqValue, pTRANSSeion->ldata, &pTRANSSeion->ivalue);
	NSDTProtocol::BuildDeleteSend(sbuffer, &tReqValue);
	//
	sbuffer->srlength = m_pTRANSSocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
	if (0 > sbuffer->srlength) return PROTO_NETWORK;
	//
	int handle_value = PROTO_FAULT;
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	DWORD proto_value = NSDTProtocol::ParseDeleteRecv(&tResValue, sbuffer->head_buffer);
	if(OK == proto_value) {
		NValueLayer::DeleteResponse(&pTRANSSeion->ivalue, &tResValue);
		handle_value = PROTO_OKAY;
	} else {
		if(PROCESSING == proto_value) handle_value = PROTO_PROCESSING;
		else if(NOT_FOUND == proto_value) handle_value = PROTO_NOT_FOUND;
		else handle_value = PROTO_FAULT;
	}
	//
	return handle_value;
}
