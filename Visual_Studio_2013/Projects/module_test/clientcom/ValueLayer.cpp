#include "StdAfx.h"
#include "stdlib.h"

#include "common_macro.h"
#include "client_macro.h"
#include "Encoding.h"
#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "ValueLayer.h"

#define VALUE_TOKEN  ' '

void NValueLayer::RegistRequest(struct RegistRequest *RequestValue, struct ListInform *list_inform) {
	strcpy_s(RequestValue->szResourceIdenti, list_inform->szUserName);
	sprintf_s(RequestValue->szUserAgent, "SDISK-Windows/%s", list_inform->szVersion);
	strcpy_s(RequestValue->szLocalIdenti, list_inform->szLocalIdenti);
	// _tcscpy_s(RequestValue->szAuthorization, list_inform->szPassword);
	NEncoding::AuthCsum(RequestValue->szAuthorization, list_inform->szUserName, list_inform->szPassword);
}


DWORD ContentValueType(char *szContentText) {
	if (!szContentText || !strcmp("", szContentText)) 
		return CONTENT_TYPE_UNKNOWN;
	else if (!strcmp("text/xml", szContentText)) 
		return CONTENT_TYPE_XML;
	else if (!strcmp("application/octet-stream", szContentText)) 
		return CONTENT_TYPE_OCTET;
	else if (!strcmp("text/plain", szContentText)) 
		return CONTENT_TYPE_PLAIN;

	return CONTENT_TYPE_UNKNOWN;
}

void NValueLayer::RegistResponse(struct ValInteValue *psvalue, struct RegistResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		psvalue->content_length = atoi(ResponseValue->szContentLength);
		psvalue->content_type = ContentValueType(ResponseValue->szContentType);
	}
}

void NValueLayer::SettingsRequest(struct RegistRequest *RequestValue, struct ListInform *list_inform) {
	strcpy_s(RequestValue->szResourceIdenti, list_inform->szUserName);
	// _tcscpy_s(RequestValue->szAuthorization, list_inform->szPassword);
	NEncoding::AuthCsum(RequestValue->szAuthorization, list_inform->szUserName, list_inform->szPassword);
}

void NValueLayer::SettingsResponse(struct ValInteValue *psvalue, struct RegistResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		psvalue->content_length = atoi(ResponseValue->szContentLength);
		psvalue->content_type = ContentValueType(ResponseValue->szContentType);
	}
}

void NValueLayer::LinkRequest(struct RegistRequest *RequestValue, struct ListInform *list_inform) {
	strcpy_s(RequestValue->szResourceIdenti, list_inform->szUserName);
	// _tcscpy_s(RequestValue->szAuthorization, list_inform->szPassword);
	NEncoding::AuthCsum(RequestValue->szAuthorization, list_inform->szUserName, list_inform->szPassword);
}

void NValueLayer::LinkResponse(struct RegistResponse *ResponseValue) {

}

void NValueLayer::UnlinkRequest(struct RegistRequest *RequestValue, struct ListInform *list_inform) {
	strcpy_s(RequestValue->szResourceIdenti, list_inform->szUserName);
	// _tcscpy_s(RequestValue->szAuthorization, list_inform->szPassword);
	NEncoding::AuthCsum(RequestValue->szAuthorization, list_inform->szUserName, list_inform->szPassword);
}

void NValueLayer::UnlinkResponse(struct RegistResponse *ResponseValue) {

}

///////////////////////////////////////////////////////////////////////
void NValueLayer::QueryRequest(struct AuthRequest *RequestValue, struct ListInform *list_inform) {
	sprintf_s(RequestValue->szResourceIdenti, "%lu", list_inform->uiUID);
	strcpy_s(RequestValue->szAuthorization, list_inform->_sAuthsum_);
}

void NValueLayer::QueryResponse(struct ComInteValue *psvalue, struct AuthResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		psvalue->content_length = atoi(ResponseValue->szContentLength);
		psvalue->content_type = ContentValueType(ResponseValue->szContentType);
	}
}

///////////////////////////////////////////////////////////////////////
void NValueLayer::InitialRequest(struct ServRequest *RequestValue, struct ListInform *list_inform) {
	sprintf_s(RequestValue->szResourceIdenti, "%lu", list_inform->uiUID);
	strcpy_s(RequestValue->szAuthorization, list_inform->_sAuthsum_);
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::InitialResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

char *GetAnchorTextType(char *szGetAnchorText, DWORD dwGetAnchorValue) {
	// static TCHAR szGetlistSType[MIN_TEXT_LEN];
	switch(dwGetAnchorValue) {
		case GANCH_LAST_ANCH:
			strcpy_s(szGetAnchorText, MIN_TEXT_LEN, "last-anchor");
			break;
		case GANCH_UNKNOWN:
		default:
			strcpy_s(szGetAnchorText, MIN_TEXT_LEN, "unknown");
			break;
	}
	return szGetAnchorText;
}
void NValueLayer::GetAnchorRequest(struct ServRequest *RequestValue, struct ListInform *list_inform) {
	sprintf_s(RequestValue->szResourceIdenti, "%lu", list_inform->uiUID);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(RequestValue->szParams, "subentry=anchor value=%s", GetAnchorTextType(text_buffer, GANCH_LAST_ANCH));
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::GetAnchorResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		psvalue->content_length = atoi(ResponseValue->szContentLength);
		psvalue->content_type = ContentValueType(ResponseValue->szContentType);
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

char *GetlistTextType(char *szGetlistText, DWORD dwGetlistValue) {
	// static TCHAR szGetlistSType[MIN_TEXT_LEN];
	switch(dwGetlistValue) {
		case GLIST_ANCH_FILES:
			strcpy_s(szGetlistText, MIN_TEXT_LEN, "anchor-files");
			break;
		case GLIST_RECU_FILES:
			strcpy_s(szGetlistText, MIN_TEXT_LEN, "recursion-files");
			break;
		case GLIST_RECU_DIRES:
			strcpy_s(szGetlistText, MIN_TEXT_LEN, "recursion-directories");
			break;
		case GLIST_LIST_FILES:
			strcpy_s(szGetlistText, MIN_TEXT_LEN, "list-files");
			break;
		case GLIST_UNKNOWN:
		default:
			strcpy_s(szGetlistText, MIN_TEXT_LEN, "unknown");
			break;
	}
	return szGetlistText;
}
void NValueLayer::GetListRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue) {
	nstriconv::unicode_utf8(RequestValue->szResourceIdenti, psvalue->data_buffer);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(RequestValue->szParams, "subentry=list value=%s", GetlistTextType(text_buffer, psvalue->list_type));	
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::GetListResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		psvalue->content_length = atoi(ResponseValue->szContentLength);
		psvalue->content_type = ContentValueType(ResponseValue->szContentType);
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
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
		case RANGE_TYPE_UNKNOWN:
		default:
			strcpy_s(szRangeText, MIN_TEXT_LEN, "unknown");
			break;
	}
	return szRangeText;
}
void NValueLayer::GetFileRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue) {
	nstriconv::unicode_utf8(RequestValue->szResourceIdenti, nstriutility::path_windows_unix(psvalue->file_name));
	strcpy_s(RequestValue->szParams, "subentry=file");
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(RequestValue->szContentRange, "%s=%lu", RangeTextType(text_buffer, psvalue->range_type), psvalue->offset);
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

inline DWORD ContentRangeValueType(off_t *length, off_t *offset, char *range_txt) {
	DWORD range_type = RANGE_TYPE_UNKNOWN;

	if (!range_txt || !strcmp("", range_txt)) return RANGE_TYPE_UNKNOWN;
	char *offsetsp = strchr(range_txt, ' ');
	if (offsetsp) *offsetsp = '\0';
	else return RANGE_TYPE_UNKNOWN;

	if (!strcmp("block", range_txt)) range_type = RANGE_TYPE_BLOCK;
	else if (!strcmp("chunk", range_txt)) range_type = RANGE_TYPE_CHUNK;
	else if (!strcmp("file", range_txt)) range_type = RANGE_TYPE_FILE;

	char *lensp = strchr(++offsetsp, '/');
	if (lensp) *lensp = '\0';
	else return RANGE_TYPE_UNKNOWN;

	*offset = atol(offsetsp);
	*length = atol(++lensp);

	return range_type;
}

void NValueLayer::GetFileResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		ntimeconv::ansi_filetime(&(psvalue->last_write), ResponseValue->szLastWrite);
		psvalue->range_type = ContentRangeValueType(&psvalue->file_length, &psvalue->offset, ResponseValue->szContentRange);
		psvalue->content_length = atoi(ResponseValue->szContentLength);
		psvalue->content_type = ContentValueType(ResponseValue->szContentType);
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

char *CsumTextType(char *szCsumText, DWORD dwCsumValue) {
	// static TCHAR szRangeSType[MIN_TEXT_LEN];
	switch(dwCsumValue) {
		case SIMPLE_CSUM:
			strcpy_s(szCsumText, MIN_TEXT_LEN, "md5");
			break;
		case COMPLEX_CSUM:
			strcpy_s(szCsumText, MIN_TEXT_LEN, "adler32&md5");
			break;
		case UNKNOWN_CSUM:
		default:
			strcpy_s(szCsumText, MIN_TEXT_LEN, "unknown");
			break;
	}
	return szCsumText;
}
void NValueLayer::GetCsumRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue) {
	nstriconv::unicode_utf8(RequestValue->szResourceIdenti, nstriutility::path_windows_unix(psvalue->file_name));
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(RequestValue->szParams, "subentry=checksum value=%s", CsumTextType(text_buffer, psvalue->csum_type));	
	sprintf_s(RequestValue->szContentRange, "%s=%lu", RangeTextType(text_buffer, psvalue->range_type), psvalue->offset);
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::GetCsumResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		psvalue->content_length = atoi(ResponseValue->szContentLength);
		psvalue->content_type = ContentValueType(ResponseValue->szContentType);
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
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
		case CONTENT_TYPE_UNKNOWN:
		default:
			strcpy_s(szContentText, CONTENT_TYPE_LENGTH, "unknown");
			break;
	}
	return szContentText;
}
void NValueLayer::PostRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue) {
	nstriconv::unicode_utf8(RequestValue->szResourceIdenti, nstriutility::path_windows_unix(psvalue->file_name));
	ntimeconv::filetime_ansi(RequestValue->szLastWrite, &psvalue->last_write);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(RequestValue->szContentRange, "%s %lu/%lu", RangeTextType(text_buffer, psvalue->range_type), psvalue->offset, psvalue->file_length);
	sprintf_s(RequestValue->szContentLength, "%lu", psvalue->content_length);
	ContentTextType(RequestValue->szContentType, psvalue->content_type);
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::PostResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

void NValueLayer::PutFileRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue) {
	nstriconv::unicode_utf8(RequestValue->szResourceIdenti, nstriutility::path_windows_unix(psvalue->file_name));
	strcpy_s(RequestValue->szParams, "subentry=file");
	ntimeconv::filetime_ansi(RequestValue->szLastWrite, &psvalue->last_write);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(RequestValue->szContentRange, "%s %lu/%lu", RangeTextType(text_buffer, psvalue->range_type), psvalue->offset, psvalue->file_length);
	sprintf_s(RequestValue->szContentLength, "%lu", psvalue->content_length);
	ContentTextType(RequestValue->szContentType, psvalue->content_type);
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::PutFileResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

void NValueLayer::PutCsumRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue) {
	nstriconv::unicode_utf8(RequestValue->szResourceIdenti, nstriutility::path_windows_unix(psvalue->file_name));
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(RequestValue->szParams, "subentry=checksum value=%s", CsumTextType(text_buffer, psvalue->csum_type));	
	sprintf_s(RequestValue->szContentRange, "%s=%lu", RangeTextType(text_buffer, psvalue->range_type), psvalue->offset);
	sprintf_s(RequestValue->szContentLength, "%lu", psvalue->content_length);
	ContentTextType(RequestValue->szContentType, psvalue->content_type);
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);

}

void NValueLayer::PutCsumResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		ntimeconv::ansi_filetime(&(psvalue->last_write), ResponseValue->szLastWrite);
		psvalue->range_type = ContentRangeValueType(&psvalue->file_length, &psvalue->offset, ResponseValue->szContentRange);
		psvalue->content_length = atoi(ResponseValue->szContentLength);
		psvalue->content_type = ContentValueType(ResponseValue->szContentType);
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

void NValueLayer::MoveRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue) {
	nstriconv::unicode_utf8(RequestValue->szResourceIdenti, nstriutility::path_windows_unix(psvalue->file_name));
	ntimeconv::filetime_ansi(RequestValue->szLastWrite, &psvalue->last_write);
	char text_buffer[MIN_TEXT_LEN];
	nstriconv::unicode_utf8(text_buffer, nstriutility::path_windows_unix(psvalue->data_buffer));
	sprintf_s(RequestValue->szParams, "destination=%s", text_buffer);
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::MoveResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

void NValueLayer::DeleteRequest(struct ServRequest *RequestValue, struct ListInform *list_inform, struct InternalValue *psvalue) {
	nstriconv::unicode_utf8(RequestValue->szResourceIdenti, nstriutility::path_windows_unix(psvalue->file_name));
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::DeleteResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
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
		case FINAL_STATUS_UNKNOWN:
		default:
			strcpy_s(szFinalText, MIN_TEXT_LEN, "unknown");
			break;
	}
	return szFinalText;
}

void NValueLayer::FinalRequest(struct ServRequest *RequestValue, DWORD dwFinalType, struct ListInform *list_inform) {
	sprintf_s(RequestValue->szResourceIdenti, "%lu", list_inform->uiUID);
	char text_buffer[MIN_TEXT_LEN];
	sprintf_s(RequestValue->szParams, "subtype=%s", FinalTextType(text_buffer, dwFinalType));	
	strcpy_s(RequestValue->szSessionID, list_inform->szSessionID);
}

void NValueLayer::FinalSuccessResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		psvalue->last_anchor = atoi(nstriutility::name_value(NULL, ResponseValue->szParams, VALUE_TOKEN));
		LOG_INFO(_T("final success response last_anchor:%d"), psvalue->last_anchor);
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

void NValueLayer::FinalEndResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}

void NValueLayer::FinalFinishResponse(struct InternalValue *psvalue, struct ServResponse *ResponseValue) {
	if (STATUS_OK == ResponseValue->status_code) {
		strcpy_s(psvalue->session_id, ResponseValue->szSessionID);
	}
}
