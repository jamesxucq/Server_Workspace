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
#include "ReplyCommand.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
CReplyValue::CReplyValue(void):
m_pReplySockets(NULL)
{
	memset(&m_tReplySocksArgu, 0, sizeof(struct ReplySocksArgu));
	// memset(&m_tNetworkConf, 0, sizeof(struct NetworkConfig));
}

CReplyValue::~CReplyValue(void) {
}

CReplyValue objValidate;
CReplyValue objCommand;

DWORD CReplyValue::InitReplyValue(struct ReplySocksArgu *pReplySocksArgu) {
	if(!pReplySocksArgu) return -1;

	memcpy(&m_tReplySocksArgu, pReplySocksArgu, sizeof(struct ReplySocksArgu));
	m_pReplySockets = NTRANSSockets::Factory(pReplySocksArgu);

	return 0;
}

DWORD CReplyValue::FinishReplyValue() {
	NTRANSSockets::DestroyObject(m_pReplySockets);
	m_pReplySockets = NULL;

	return 0;
}

DWORD CReplyValue::SetSocketsAddress(char *szServAddress, int iServPort) {
	m_pReplySockets->SetAddress(szServAddress, iServPort);
	return 0;
}

int CReplyValue::ReplySendReceive(struct SessionBuffer *psbuffer) {
	if(!psbuffer) return -1;

	SetSocketsAddress(m_tReplySocksArgu.ReplyAddress.sin_addr, m_tReplySocksArgu.ReplyAddress.sin_port);
	psbuffer->srlength = m_pReplySockets->SendReceive(psbuffer->srecv_buffer, psbuffer->srecv_buffer);

	psbuffer->hlen = NSDTProtocol::GetHeaderValue(&psbuffer->head_buffer, psbuffer->srecv_buffer);
	psbuffer->blen = NSDTProtocol::GetBodyValue(&psbuffer->body_buffer, psbuffer->srecv_buffer, psbuffer->srlength);

	return psbuffer->srlength;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
DWORD NValidateBzl::Register(struct ValidateSession *pValSession) {
	struct RegistRequest RequestValue;
	struct RegistResponse ResponseValue;

	if(!pValSession) return -1;

	struct ListInform *pListInform = pValSession->list_inform;
	NValueLayer::RegistRequest(&RequestValue, pValSession->list_inform);
	NSDTProtocol::BuildRegistSend(&pValSession->buffer, &RequestValue);
	if (0 > objValidate.ReplySendReceive(&pValSession->buffer)) return -1;

	struct RegistValue tRegistValue;
	if (STATUS_OK != NSDTProtocol::ParseRegistRecv(&ResponseValue, pValSession->buffer.head_buffer)) return -1;
	else {
		NValueLayer::RegistResponse(&pValSession->inte_value, &ResponseValue);
		if (!pValSession->inte_value.content_length || CONTENT_TYPE_XML != pValSession->inte_value.content_type) return -1;
		if (NReplyHandler::HandleRegistRecv(&tRegistValue, pValSession->buffer.body_buffer, pValSession->buffer.blen))
			return -1;
	}

	/////////////////////////////////////////////////////////////////////////////
	pListInform->uiUID = tRegistValue.uiUID;
	strcpy_s(pListInform->AuthAddress.sin_addr, HOSTNAME_LENGTH, tRegistValue.cache_address.sin_addr);
	pListInform->AuthAddress.sin_port = tRegistValue.cache_address.sin_port;
	strcpy_s(pListInform->access_key, KEY_LENGTH, tRegistValue.access_key);
	NEncoding::AuthCsum(pListInform->_sAuthsum_, tRegistValue.uiUID, tRegistValue.access_key);

	return 0;
}

DWORD NValidateBzl::Settings(TCHAR *client_version, int *pool_size, struct ValidateSession *pValSession) {
	struct RegistRequest RequestValue;
	struct RegistResponse ResponseValue;

	if(!pool_size || !pValSession) return -1;

	NValueLayer::SettingsRequest(&RequestValue, pValSession->list_inform);
	NSDTProtocol::BuildSettingsSend(&pValSession->buffer, &RequestValue);
	if (0 > objValidate.ReplySendReceive(&pValSession->buffer)) return -1;

	struct ClientValue tClientValue;
	if (STATUS_OK != NSDTProtocol::ParseSettingsRecv(&ResponseValue, pValSession->buffer.head_buffer)) return -1;
	else {
		NValueLayer::SettingsResponse(&pValSession->inte_value, &ResponseValue);
		if (!pValSession->inte_value.content_length || CONTENT_TYPE_XML != pValSession->inte_value.content_type) return -1;
		if (NReplyHandler::HandleSettingsRecv(&tClientValue, pValSession->buffer.body_buffer, pValSession->buffer.blen))
			return -1;
	}

	/////////////////////////////////////////////////////////////////////////////
	_tcscpy_s(client_version, MIN_TEXT_LEN, tClientValue.client_version);
	*pool_size = tClientValue.pool_size;

	return 0;
}


DWORD NValidateBzl::Link(struct ValidateSession *pValSession) {
	struct RegistRequest RequestValue;
	struct RegistResponse ResponseValue;

	if(!pValSession) return -1;

	NValueLayer::LinkRequest(&RequestValue, pValSession->list_inform);
	NSDTProtocol::BuildLinkSend(&pValSession->buffer, &RequestValue);
	if (0 > objValidate.ReplySendReceive(&pValSession->buffer)) return -1;

	if(STATUS_OK != NSDTProtocol::ParseLinkRecv(&ResponseValue, pValSession->buffer.head_buffer)) return -1;
	else NValueLayer::LinkResponse(&ResponseValue); 

	return 0;
}

DWORD NValidateBzl::Unlink(struct ValidateSession *pValSession) {
	struct RegistRequest RequestValue;
	struct RegistResponse ResponseValue;

	if(!pValSession) return -1;

	NValueLayer::UnlinkRequest(&RequestValue, pValSession->list_inform);
	NSDTProtocol::BuildUnlinkSend(&pValSession->buffer, &RequestValue);
	if (0 > objValidate.ReplySendReceive(&pValSession->buffer)) return -1;

	if(STATUS_OK != NSDTProtocol::ParseUnlinkRecv(&ResponseValue, pValSession->buffer.head_buffer)) return -1;
	else NValueLayer::UnlinkResponse(&ResponseValue);

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

DWORD NCommandBzl::QueryCached(UINT *cached_anchor, struct CommandSession *pComSession) {
	DWORD cache_locked = SERV_STATUS_FAULT;
	//////////////////////////////////////////////
	struct AuthRequest RequestValue;
	struct AuthResponse ResponseValue;

	if(!cached_anchor || !pComSession) return -1;

	NValueLayer::QueryRequest(&RequestValue, pComSession->list_inform);
	NSDTProtocol::BuildQuerySend(&pComSession->buffer, &RequestValue);
	if (0 > objCommand.ReplySendReceive(&pComSession->buffer)) return -1;

	struct QueryValue tQueryValue;
	DWORD status_code = NSDTProtocol::ParseQueryRecv(&ResponseValue, pComSession->buffer.head_buffer);
LOG_DEBUG(_T("query cached status_code:%d"), status_code);
	if (STATUS_OK!=status_code && STATUS_UNAUTHORIZED!=status_code) return -1;
	else if(STATUS_OK == status_code) {
		NValueLayer::QueryResponse(&pComSession->inte_value, &ResponseValue);
		if (!pComSession->inte_value.content_length || CONTENT_TYPE_XML != pComSession->inte_value.content_type) return -1;
		if (NReplyHandler::HandleQueryRecv(&tQueryValue, pComSession->buffer.body_buffer, pComSession->buffer.blen))
			return -1;
	} else if(STATUS_UNAUTHORIZED == status_code) cache_locked = SERV_STATUS_NOTAUTH;

	/////////////////////////////////////////////////////////////////////////////
	if(STATUS_OK == status_code) {
		strcpy_s(pComSession->list_inform->ServAddress.sin_addr, HOSTNAME_LENGTH, tQueryValue.address.sin_addr);
		pComSession->list_inform->ServAddress.sin_port = tQueryValue.address.sin_port;
		cache_locked = tQueryValue.serv_locked? SERV_STATUS_LOCKED: SERV_STATUS_OK;
		*cached_anchor = tQueryValue.cached_anchor;
		strcpy_s(pComSession->list_inform->szSessionID, SESSION_LENGTH, tQueryValue.session_id);
	}
LOG_DEBUG(_T("query cached cache_locked:%d"), cache_locked);
	return cache_locked;
}

