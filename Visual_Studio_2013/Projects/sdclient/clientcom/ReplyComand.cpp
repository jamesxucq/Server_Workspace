#include "StdAfx.h"

#include "client_macro.h"
#include "Session.h"
#include "ValueLayer.h"
#include "TRANSSockets.h"
#include "SDTProtocol.h"
#include "StringUtility.h"

#include "Encoding.h"
#include "third_party.h"
#include "ReplyHandler.h"
#include "ReplyComand.h"

//
CReplyValue::CReplyValue(void):
m_pReplySocke(NULL)
{
	memset(&m_tReplyArgu, 0, sizeof(struct ReplyArgu));
	// memset(&m_tNetworkConf, 0, sizeof(struct NetworkConfig));
}

CReplyValue::~CReplyValue(void) {
}

CReplyValue objValid;
CReplyValue objComand;

DWORD CReplyValue::InitReplyValue(struct ReplyArgu *pReplyArgu) {
	memcpy(&m_tReplyArgu, pReplyArgu, sizeof(struct ReplyArgu));
	m_pReplySocke = NTRANSSockets::Factory(pReplyArgu);
	return 0x00;
}

DWORD CReplyValue::FinishReplyValue() {
	NTRANSSockets::DestroyObject(m_pReplySocke);
	m_pReplySocke = NULL;
	return 0x00;
}

int CReplyValue::ReplySendReceive(struct SeionBuffer *sbuffer) {
	m_pReplySocke->SetAddress(m_tReplyArgu.tReplyAddress.sin_addr, m_tReplyArgu.tReplyAddress.sin_port);
	sbuffer->srlength = m_pReplySocke->SendReceive(sbuffer->srecv_buffer, sbuffer->srecv_buffer);
//
	sbuffer->hlen = NSDTProtocol::GetHeaderValue(&sbuffer->head_buffer, sbuffer->srecv_buffer);
	sbuffer->blen = NSDTProtocol::GetBodyValue(&sbuffer->body_buffer, sbuffer->srecv_buffer, sbuffer->srlength);
//
	return sbuffer->srlength;
}

//
DWORD NValidBzl::InitValid(struct ReplyArgu *pReplyArgu) { 
	// Initialize Winsock
	NTRANSSockets::InitialWinsock();
	return objValid.InitReplyValue(pReplyArgu); 
}
	
DWORD NValidBzl::FinishValid() {
	// Finish Winsock
	NTRANSSockets::FinishWinsock();
	return objValid.FinishReplyValue(); 
}


DWORD NValidBzl::Register(struct ValidSeion *pValSeion) {
	struct RegistRequest tReqValue;
	struct RegistResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct RegistRequest));
	memset(&tResValue, '\0', sizeof(struct RegistResponse));
//
	struct ListData *ldata = pValSeion->ldata;
	NValueLayer::RegistRequest(&tReqValue, pValSeion->ldata);
	NSDTProtocol::BuildRegistSend(&pValSeion->buffer, &tReqValue);
	if (0 > objValid.ReplySendReceive(&pValSeion->buffer)) return ((DWORD)-1);
//
	struct RegistValue tRegistValue;
	if (OK != NSDTProtocol::ParseRegistRecv(&tResValue, pValSeion->buffer.head_buffer)) return ((DWORD)-1);
	else {
		NValueLayer::RegistResponse(&pValSeion->ivalue, &tResValue);
		if (!pValSeion->ivalue.content_length || CONTENT_TYPE_XML != pValSeion->ivalue.content_type) return ((DWORD)-1);
		if (NReplyHandler::HandleRegistRecv(&tRegistValue, pValSeion->buffer.body_buffer, pValSeion->buffer.blen))
			return ((DWORD)-1);
	}
	//
	ldata->uiUID = tRegistValue.uiUID;
	strcpy_s(ldata->tAuthAddress.sin_addr, HOSTNAME_LENGTH, tRegistValue.cache_address.sin_addr);
	ldata->tAuthAddress.sin_port = tRegistValue.cache_address.sin_port;
	strcpy_s(ldata->access_key, KEY_LENGTH, tRegistValue.access_key);
	NEncoding::AuthChks(ldata->_szAuthsum_, tRegistValue.uiUID, tRegistValue.access_key);
//
	return 0x00;
}

DWORD NValidBzl::Settings(TCHAR *client_version, int *pool_size, int *data_bomb, struct ValidSeion *pValSeion) {
	struct RegistRequest tReqValue;
	struct RegistResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct RegistRequest));
	memset(&tResValue, '\0', sizeof(struct RegistResponse));
//
	NValueLayer::SettingsRequest(&tReqValue, pValSeion->ldata);
	NSDTProtocol::BuildSettingsSend(&pValSeion->buffer, &tReqValue);
	if (0 > objValid.ReplySendReceive(&pValSeion->buffer)) return ((DWORD)-1);
//
	struct ClientValue tClientValue;
	if (OK != NSDTProtocol::ParseSettingsRecv(&tResValue, pValSeion->buffer.head_buffer)) return ((DWORD)-1);
	else {
		NValueLayer::SettingsResponse(&pValSeion->ivalue, &tResValue);
		if (!pValSeion->ivalue.content_length || CONTENT_TYPE_XML != pValSeion->ivalue.content_type) return ((DWORD)-1);
		if (NReplyHandler::HandleSettingsRecv(&tClientValue, pValSeion->buffer.body_buffer, pValSeion->buffer.blen))
			return ((DWORD)-1);
	}
	//
	_tcscpy_s(client_version, VERSION_LENGTH, tClientValue.client_version);
	*pool_size = tClientValue.pool_size;
	*data_bomb = tClientValue.data_bomb;
//
	return 0x00;
}

DWORD NValidBzl::Link(struct ValidSeion *pValSeion) {
	struct RegistRequest tReqValue;
	struct RegistResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct RegistRequest));
	memset(&tResValue, '\0', sizeof(struct RegistResponse));
//
	NValueLayer::LinkRequest(&tReqValue, pValSeion->ldata);
	NSDTProtocol::BuildLinkSend(&pValSeion->buffer, &tReqValue);
	if (0 > objValid.ReplySendReceive(&pValSeion->buffer)) return ((DWORD)-1);
//
	if(OK != NSDTProtocol::ParseLinkRecv(&tResValue, pValSeion->buffer.head_buffer)) return ((DWORD)-1);
	else NValueLayer::LinkResponse(&tResValue); 
//
	return 0x00;
}

DWORD NValidBzl::Unlink(struct ValidSeion *pValSeion) {
	struct RegistRequest tReqValue;
	struct RegistResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct RegistRequest));
	memset(&tResValue, '\0', sizeof(struct RegistResponse));
//
	NValueLayer::UnlinkRequest(&tReqValue, pValSeion->ldata);
	NSDTProtocol::BuildUnlinkSend(&pValSeion->buffer, &tReqValue);
	if (0 > objValid.ReplySendReceive(&pValSeion->buffer)) return ((DWORD)-1);
//
	if(OK != NSDTProtocol::ParseUnlinkRecv(&tResValue, pValSeion->buffer.head_buffer)) return ((DWORD)-1);
	else NValueLayer::UnlinkResponse(&tResValue);
//
	return 0x00;
}

//
DWORD NComandBzl::InitComand(struct ReplyArgu *pReplyArgu){ 
	// Initialize Winsock
	NTRANSSockets::InitialWinsock();
	//
	return objComand.InitReplyValue(pReplyArgu); 
}
	
DWORD NComandBzl::FinishComand() { 
	// Finish Winsock
	NTRANSSockets::FinishWinsock();
	//
	return objComand.FinishReplyValue(); 
}


DWORD NComandBzl::QueryCached(UINT *cached_anchor, struct ComandSeion *pCmdSeion) {
	struct AuthRequest tReqValue;
	struct AuthResponse tResValue;
	memset(&tReqValue, '\0', sizeof(struct AuthRequest));
	memset(&tResValue, '\0', sizeof(struct AuthResponse));
//
	NValueLayer::QueryRequest(&tReqValue, pCmdSeion->ldata);
	NSDTProtocol::BuildQuerySend(&pCmdSeion->buffer, &tReqValue);
	if (0 > objComand.ReplySendReceive(&pCmdSeion->buffer)) return WORKER_STATUS_NOTCONN;
//
	struct QueryValue tQueryValue;
	memset(&tQueryValue, '\0', sizeof(struct QueryValue));
	DWORD status_code = NSDTProtocol::ParseQueryRecv(&tResValue, pCmdSeion->buffer.head_buffer);
// _LOG_DEBUG(_T("query cached status_code:%d"), status_code); // disable by james 20130410
	if(OK == status_code) {
		NValueLayer::QueryResponse(&pCmdSeion->ivalue, &tResValue);
		if (!pCmdSeion->ivalue.content_length || CONTENT_TYPE_XML != pCmdSeion->ivalue.content_type) return ((DWORD)-1);
		if (NReplyHandler::HandleQueryRecv(&tQueryValue, pCmdSeion->buffer.body_buffer, pCmdSeion->buffer.blen))
			return ((DWORD)-1);
	} else if(UNAUTHORIZED == status_code) return WORKER_STATUS_NOTAUTH;
	else if(LOCKED == status_code) return WORKER_STATUS_LOCKED;
	//
	if(OK == status_code) {
		strcpy_s(pCmdSeion->ldata->tServAddress.sin_addr, HOSTNAME_LENGTH, tQueryValue.address.sin_addr);
		pCmdSeion->ldata->tServAddress.sin_port = tQueryValue.address.sin_port;
// static int requ_tatol;
// _LOG_DEBUG(_T("++++++++++ sin_port:%d requ_tatol:%d"), tQueryValue.address.sin_port, ++requ_tatol);
		*cached_anchor = tQueryValue.cached_anchor;
		strcpy_s(pCmdSeion->ldata->szReqValid, VALID_LENGTH, tQueryValue.req_valid);
		strcpy_s(pCmdSeion->ldata->szSessionID, SESSION_LENGTH, tQueryValue.session_id);
	}
// _LOG_DEBUG(_T("query cached cache_locked:%d"), cache_locked); // disable by james 20130410
	return WORKER_STATUS_OKAY;
}

