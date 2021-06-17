#include "StdAfx.h"
#include "stdlib.h"

#include "common_macro.h"
#include "client_macro.h"
#include "Encoding.h"
#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"
#include "ListVObj.h"

#include "ValueLayer.h"

#define VALUE_TOKEN  ' '

VOID NValueLayer::RegistRequest(struct RegistRequest *pReqValue, struct ListData *ldata) {
	strcpy_s(pReqValue->szResIdentity, ldata->szUserName);
	sprintf_s(pReqValue->szUserAgent, "SWD-Windows/%s", ldata->szVersion);
	strcpy_s(pReqValue->szLocalIdenti, ldata->szLocalIdenti);
	NEncoding::AuthChks(pReqValue->szAuthorize, ldata->szUserName, ldata->szPassword);
}


DWORD ContentValueType(char *szContentText) {
	if (!szContentText || '\0'==*szContentText) 
		return CONTENT_TYPE_INVA;
	else if (!strcmp("text/xml", szContentText)) 
		return CONTENT_TYPE_XML;
	else if (!strcmp("application/octet-stream", szContentText)) 
		return CONTENT_TYPE_OCTET;
	else if (!strcmp("text/plain", szContentText)) 
		return CONTENT_TYPE_PLAIN;

	return CONTENT_TYPE_INVA;
}

VOID NValueLayer::RegistResponse(struct ValInteValue *ival, struct RegistResponse *pResValue) {
	if (OK == pResValue->status_code) {
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
	}
}

VOID NValueLayer::SettingsRequest(struct RegistRequest *pReqValue, struct ListData *ldata) {
	strcpy_s(pReqValue->szResIdentity, ldata->szUserName);
	strcpy_s(pReqValue->szLocalIdenti, ldata->szLocalIdenti);
	NEncoding::AuthChks(pReqValue->szAuthorize, ldata->szUserName, ldata->szPassword);
}

VOID NValueLayer::SettingsResponse(struct ValInteValue *ival, struct RegistResponse *pResValue) {
	if (OK == pResValue->status_code) {
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
	}
}

VOID NValueLayer::LinkRequest(struct RegistRequest *pReqValue, struct ListData *ldata) {
	strcpy_s(pReqValue->szResIdentity, ldata->szUserName);
	strcpy_s(pReqValue->szLocalIdenti, ldata->szLocalIdenti);
	NEncoding::AuthChks(pReqValue->szAuthorize, ldata->szUserName, ldata->szPassword);
}

VOID NValueLayer::LinkResponse(struct RegistResponse *pResValue) {

}

VOID NValueLayer::UnlinkRequest(struct RegistRequest *pReqValue, struct ListData *ldata) {
	strcpy_s(pReqValue->szResIdentity, ldata->szUserName);
	strcpy_s(pReqValue->szLocalIdenti, ldata->szLocalIdenti);
	NEncoding::AuthChks(pReqValue->szAuthorize, ldata->szUserName, ldata->szPassword);
}

VOID NValueLayer::UnlinkResponse(struct RegistResponse *pResValue) {

}

//
VOID NValueLayer::QueryRequest(struct AuthRequest *pReqValue, struct ListData *ldata) {
	sprintf_s(pReqValue->szResIdentity, "%lu", ldata->uiUID);
	strcpy_s(pReqValue->szAuthorize, ldata->_szAuthsum_);
}

VOID NValueLayer::QueryResponse(struct ComInteValue *ival, struct AuthResponse *pResValue) {
	if (OK == pResValue->status_code) {
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
	}
}

//
VOID NValueLayer::InitialRequest(struct ServRequest *pReqValue, struct ListData *ldata) {
	sprintf_s(pReqValue->szResIdentity, "%lu", ldata->uiUID);
	sprintf_s(pReqValue->szParams, "valid=%s", ldata->szReqValid);	
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::InitialResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::KeepAliveRequest(struct ServRequest *pReqValue, struct ListData *ldata) {
	sprintf_s(pReqValue->szResIdentity, "%lu", ldata->uiUID);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::KeepAliveResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

char *HeadlistTextType(char *szHeadlistText, DWORD dwHeadlistValue) {
	// static TCHAR szGetlistSType[MIN_TEXT_LEN];
	switch(dwHeadlistValue) {
		case HLIST_ANCH_FILES:
			strcpy_s(szHeadlistText, MIN_TEXT_LEN, "anchor-files");
			break;
		case HLIST_RECU_FILES:
			strcpy_s(szHeadlistText, MIN_TEXT_LEN, "recursion-files");
			break;
		case HLIST_RECU_DIRES:
			strcpy_s(szHeadlistText, MIN_TEXT_LEN, "recursion-directories");
			break;
		case HLIST_LIST_DIREC:
			strcpy_s(szHeadlistText, MIN_TEXT_LEN, "list-directory");
			break;
		case HLIST_INVA:
		default:
			strcpy_s(szHeadlistText, MIN_TEXT_LEN, "invalid");
			break;
	}
	return szHeadlistText;
}

VOID NValueLayer::HeadListRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, ival->data_buffer);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szParams, "subentry=list value=%s", HeadlistTextType(text_buffer, ival->list_type));	
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::HeadListResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
		char *poval;
		strcon::utf8_ustr(ival->data_buffer, struti::name_value(&poval, pResValue->szAttach, VALUE_TOKEN));
// _LOG_DEBUG(_T("ival->data_buffer:%s"), ival->data_buffer);
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
} 

VOID NValueLayer::HeadFileRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	strcpy_s(pReqValue->szParams, "subentry=file");
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::HeadFileResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		timcon::ansi_ftim(&(ival->last_write), pResValue->szLastWrite);
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

char *GetAnchorTextType(char *szGetAnchorText, DWORD dwGetAnchorValue) {
	// static TCHAR szGetlistSType[MIN_TEXT_LEN];
	switch(dwGetAnchorValue) {
		case GANCH_LAST_ANCH:
			strcpy_s(szGetAnchorText, MIN_TEXT_LEN, "last-anchor");
			break;
		case GANCH_INVA:
		default:
			strcpy_s(szGetAnchorText, MIN_TEXT_LEN, "invalid");
			break;
	}
	return szGetAnchorText;
}

VOID NValueLayer::GetAnchorRequest(struct ServRequest *pReqValue, struct ListData *ldata) {
	sprintf_s(pReqValue->szResIdentity, "%lu", ldata->uiUID);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szParams, "subentry=anchor value=%s", GetAnchorTextType(text_buffer, GANCH_LAST_ANCH));
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::GetAnchorResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

char *RangeTextType(char *szRangeText, DWORD dwRangeValue) {
	// static TCHAR szRangeSType[MIN_TEXT_LEN];
	switch(dwRangeValue) {
		case RANGE_TYPE_FILE:
			strcpy_s(szRangeText, MIN_TEXT_LEN, "file");
			break;
		case RANGE_TYPE_CHUNK:
			strcpy_s(szRangeText, MIN_TEXT_LEN, "chunk");
			break;
		case RANGE_TYPE_BLOCK:
			strcpy_s(szRangeText, MIN_TEXT_LEN, "block");
			break;
		case RANGE_TYPE_INVA:
		default:
			strcpy_s(szRangeText, MIN_TEXT_LEN, "invalid");
			break;
	}
	return szRangeText;
}

char *ListTextType(char *szListText, DWORD dwListValue) {
	switch(dwListValue) {
		case LIST_ANCH_FILES:
			strcpy_s(szListText, MIN_TEXT_LEN, "anchor-files");
			break;
		case LIST_RECU_FILES:
			strcpy_s(szListText, MIN_TEXT_LEN, "recursion-files");
			break;
		case LIST_RECU_DIRES:
			strcpy_s(szListText, MIN_TEXT_LEN, "recursion-directories");
			break;
		case LIST_LIST_DIREC:
			strcpy_s(szListText, MIN_TEXT_LEN, "list-directory");
			break;
		case LIST_INVA:
		default:
			strcpy_s(szListText, MIN_TEXT_LEN, "invalid");
			break;
	}
	return szListText;
}

char *IdentityTextType(char *szIdentityText, DWORD dwIdentityValue, DWORD dwLastAnchor, TCHAR *szDirectory) {
	// static TCHAR szGetlistSType[MIN_TEXT_LEN];
	switch(dwIdentityValue) {
		case LIST_ANCH_FILES:
			sprintf_s(szIdentityText, MAX_PATH, "%u", dwLastAnchor);
			break;
		case LIST_RECU_FILES:
		case LIST_RECU_DIRES:
		case LIST_LIST_DIREC:
			strcon::ustr_utf8(szIdentityText, szDirectory);
			break;
		case LIST_INVA:
		default:
			strcpy_s(szIdentityText, MAX_PATH, "");
			break;
	}
	return szIdentityText;
}

VOID NValueLayer::GetListRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	// strcon::ustr_utf8(pReqValue->szResIdentity, ival->data_buffer);
	IdentityTextType(pReqValue->szResIdentity, ival->list_type, ival->last_anchor, ival->file_name);
// _LOG_DEBUG(_T("pReqValue->szResIdentity:%s"), pReqValue->szResIdentity);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szParams, "subentry=list value=%s", ListTextType(text_buffer, ival->list_type));
	sprintf_s(pReqValue->szContentRange, "%s=%llu", RangeTextType(text_buffer, ival->range_type), ival->offset);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::GetListResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::GetFileRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	strcpy_s(pReqValue->szParams, "subentry=file");
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szContentRange, "%s=%llu", RangeTextType(text_buffer, ival->range_type), ival->offset);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

inline DWORD ContentRangeValueType(unsigned __int64 *length, unsigned __int64 *offset, char *range_txt) {
	DWORD range_type = RANGE_TYPE_INVA;

	if (!range_txt || '\0'==*range_txt) return RANGE_TYPE_INVA;
	char *offsetsp = strchr(range_txt, ' ');
	if (offsetsp) *offsetsp = '\0';
	else {
		return RANGE_TYPE_INVA;
	}

	if (!strcmp("block", range_txt)) range_type = RANGE_TYPE_BLOCK;
	else if (!strcmp("chunk", range_txt)) range_type = RANGE_TYPE_CHUNK;
	else if (!strcmp("file", range_txt)) range_type = RANGE_TYPE_FILE;

	char *lensp = strchr(++offsetsp, '/');
	if (lensp) *lensp = '\0';
	else {
		return RANGE_TYPE_INVA;
	}

	*offset = _atoi64(offsetsp);
	*length = _atoi64(++lensp);

	return range_type;
}

VOID NValueLayer::GetFileResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		timcon::ansi_ftim(&(ival->last_write), pResValue->szLastWrite);
		ival->range_type = ContentRangeValueType(&ival->file_size, &ival->offset, pResValue->szContentRange);
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

char *ChksTextType(char *szChksText, DWORD dwChksValue) {
	switch(dwChksValue) {
		case SIMPLE_CHKS:
			strcpy_s(szChksText, MIN_TEXT_LEN, "md5");
			break;
		case COMPLEX_CHKS:
			strcpy_s(szChksText, MIN_TEXT_LEN, "adler32&md5");
			break;
		case WHOLE_CHKS:
			strcpy_s(szChksText, MIN_TEXT_LEN, "sha1");
			break;
		case INVA_CHKS:
		default:
			strcpy_s(szChksText, MIN_TEXT_LEN, "invalid");
			break;
	}
	return szChksText;
}

char *CacheTextType(char *szCacheText, DWORD dwCacheValue) {
	switch(dwCacheValue) {
		case VERIFY_CACHE:
			strcpy_s(szCacheText, MIN_TEXT_LEN, "no-cache");
			break;
		case RIVER_CACHED:
			strcpy_s(szCacheText, MIN_TEXT_LEN, "cache");
			break;
		case INVA_CACHE:
		default:
			strcpy_s(szCacheText, MIN_TEXT_LEN, "invalid");
			break;
	}
	return szCacheText;
}

VOID NValueLayer::GetChksRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szParams, "subentry=chks value=%s", ChksTextType(text_buffer, ival->chksum_type));	
	sprintf_s(pReqValue->szContentRange, "%s=%llu", RangeTextType(text_buffer, ival->range_type), ival->offset);
	strcpy_s(pReqValue->szCacheVerify, CacheTextType(text_buffer, ival->cache_verify));
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::GetChksResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
// _LOG_DEBUG(_T("ival->content_length:%d"), ival->content_length);
}

inline char *ContentTextType(char *szContentText, DWORD dwContentValue) {
	switch (dwContentValue) {
		case CONTENT_TYPE_XML:
			strcpy_s(szContentText, CONTENT_TYPE_LENGTH, "text/xml");
			break;
		case CONTENT_TYPE_OCTET:
			strcpy_s(szContentText, CONTENT_TYPE_LENGTH, "application/octet-stream");
			break;
		case CONTENT_TYPE_PLAIN:
			strcpy_s(szContentText, CONTENT_TYPE_LENGTH, "text/plain");
			break;
		case CONTENT_TYPE_INVA:
		default:
			strcpy_s(szContentText, CONTENT_TYPE_LENGTH, "invalid");
			break;
	}
	return szContentText;
}

VOID NValueLayer::PostRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	timcon::ftim_ansi(pReqValue->szLastWrite, &ival->last_write);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szContentRange, "%s %llu/%llu", RangeTextType(text_buffer, ival->range_type), ival->offset, ival->file_size);
	sprintf_s(pReqValue->szContentLength, "%lu", ival->content_length);
	ContentTextType(pReqValue->szContentType, ival->content_type);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::PostResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::PutFileRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	strcpy_s(pReqValue->szParams, "subentry=file");
	timcon::ftim_ansi(pReqValue->szLastWrite, &ival->last_write);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szContentRange, "%s %llu/%llu", RangeTextType(text_buffer, ival->range_type), ival->offset, ival->file_size);
	sprintf_s(pReqValue->szContentLength, "%lu", ival->content_length);
	ContentTextType(pReqValue->szContentType, ival->content_type);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::PutFileResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::PutDireRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	strcpy_s(pReqValue->szParams, "subentry=file");
	timcon::ftim_ansi(pReqValue->szLastWrite, &ival->last_write);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::PutDireResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::PutChksRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szParams, "subentry=chks value=%s", ChksTextType(text_buffer, ival->chksum_type));	
	sprintf_s(pReqValue->szContentRange, "%s=%llu", RangeTextType(text_buffer, ival->range_type), ival->offset);
	sprintf_s(pReqValue->szContentLength, "%lu", ival->content_length);
	ContentTextType(pReqValue->szContentType, ival->content_type);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);

}

VOID NValueLayer::PutChksResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		timcon::ansi_ftim(&(ival->last_write), pResValue->szLastWrite);
		ival->range_type = ContentRangeValueType(&ival->file_size, &ival->offset, pResValue->szContentRange);
		ival->content_length = atoi(pResValue->szContentLength);
		ival->content_type = ContentValueType(pResValue->szContentType);
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::MoveRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	timcon::ftim_ansi(pReqValue->szLastWrite, &ival->last_write);
	char text_buffer[MAX_PATH];
	strcon::ustr_utf8(text_buffer, struti::path_wind_unix(ival->data_buffer));
	sprintf_s(pReqValue->szParams, "desti=%s", text_buffer);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::MoveResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::CopyRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	timcon::ftim_ansi(pReqValue->szLastWrite, &ival->last_write);
	char text_buffer[MAX_PATH];
	strcon::ustr_utf8(text_buffer, struti::path_wind_unix(ival->data_buffer));
	sprintf_s(pReqValue->szParams, "desti=%s", text_buffer);
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::CopyResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::DeleteRequest(struct ServRequest *pReqValue, struct ListData *ldata, struct InteValue *ival) {
	strcon::ustr_utf8(pReqValue->szResIdentity, struti::path_wind_unix(ival->file_name));
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::DeleteResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

char *FinalTextType(char *szFinalText, DWORD dwFinalValue) {
	// static TCHAR szFinalSType[MIN_TEXT_LEN];
	switch(dwFinalValue) {
		case FINAL_STATUS_END:
			strcpy_s(szFinalText, MIN_TEXT_LEN, "end");
			break;
		case FINAL_STATUS_SUCCESS:
			strcpy_s(szFinalText, MIN_TEXT_LEN, "successful");
			break;
		case FINAL_STATUS_FINISH:
			strcpy_s(szFinalText, MIN_TEXT_LEN, "finish");
			break;
		case FINAL_STATUS_INVA:
		default:
			strcpy_s(szFinalText, MIN_TEXT_LEN, "invalid");
			break;
	}
	return szFinalText;
}

VOID NValueLayer::FinalRequest(struct ServRequest *pReqValue, DWORD dwFinalType, struct ListData *ldata) {
	sprintf_s(pReqValue->szResIdentity, "%lu", ldata->uiUID);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(pReqValue->szParams, "subtype=%s", FinalTextType(text_buffer, dwFinalType));	
	strcpy_s(pReqValue->szSessionID, ldata->szSessionID);
}

VOID NValueLayer::FinalSuccessResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		char *poval;
		ival->last_anchor = atoi(struti::name_value(&poval, pResValue->szAttach, VALUE_TOKEN));
		_LOG_INFO(_T("final success response last_anchor:%d"), ival->last_anchor);
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::FinalEndResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}

VOID NValueLayer::FinalFinishResponse(struct InteValue *ival, struct ServResponse *pResValue) {
	if (OK == pResValue->status_code) {
		strcpy_s(ival->session_id, pResValue->szSessionID);
	}
}
