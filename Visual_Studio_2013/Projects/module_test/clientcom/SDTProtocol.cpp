#include "StdAfx.h"

#include "common_macro.h"
#include "ErrPrint.h"
#include "Encoding.h"
#include "StringUtility.h"
#include "FileUtility.h"
#include "ValueLayer.h"
#include "SDTProtocol.h"

#include "logger_ansi.h"
#include "third_party.h"

#define HEAD_TOKEN		"\r\n\r\n"
#define LINE_TOKEN		"\r\n"
#define FIELD_TOKEN		" "
#define SDTP_TOKEN		"SDTP/1.1"
#define ONLY_HEADER_SECTION		0

#define FINISH_HEADER_BUFFER \
	psbuffer->hlen = strlen(header_txt); \
	psbuffer->blen = ONLY_HEADER_SECTION;

#define FINISH_HDBD_BUFFER \
	psbuffer->hlen = strlen(header_txt); \
	psbuffer->body_buffer = header_txt + psbuffer->hlen; \
	psbuffer->blen = RECV_BUFF_SIZE - psbuffer->hlen;


DWORD NSDTProtocol::GetHeaderValue(char **head_buffer, char *szReceiveTxt) {
	if(!szReceiveTxt) return ERR_FAULT;

	char *szHeaderEnd = strstr(szReceiveTxt, "\r\n\r\n");
	if(!szHeaderEnd) return ERR_FAULT;
	szHeaderEnd += 2;
	*szHeaderEnd++ = '\0';
	*szHeaderEnd++ = '\0';

	*head_buffer = szReceiveTxt;
	return szHeaderEnd - szReceiveTxt;
}


DWORD NSDTProtocol::GetBodyValue(char **body_buffer, char *szReceiveTxt, int iLength) {
	if(!szReceiveTxt) return ERR_FAULT;

	char *pBodyStart = szReceiveTxt + strlen(szReceiveTxt) + 2;
	*body_buffer = pBodyStart;

	return iLength - (pBodyStart - szReceiveTxt);
}

/*
OPTIONS user_name SDTP/1.1\r\n
Command: register\r\n
User-Agent: xxxxx\r\n
From: xxxx\r\n
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildRegistSend(struct SessionBuffer *psbuffer, struct RegistRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	strcat_s(header_txt, LINE_LENGTH, "Command: register"LINE_TOKEN);
	sprintf_s(line_txt, "User-Agent: %s"LINE_TOKEN, RequestValue->szUserAgent);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "From: %s"LINE_TOKEN, RequestValue->szLocalIdenti);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, RequestValue->szAuthorization);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("regist send header: %s\r\n", header_txt);
	return 0;
}


/*
<?xml version="1.0" encoding="UTF-8"?>
<register version="1.2.1">
<client_version>1.12</client_version>
<uid>12345321</uid>
<cache_host>123.354.345.345</cache_host>
<cache_port>325</cache_port>
<server_host>123.354.345.345</server_host>
<server_port>325</server_port>
<access_key>2343ewr544r454rewfa45qwee</access_key>
</register>
*/
/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxxxxxxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized\r\n\r\n
*/
DWORD NSDTProtocol::ParseRegistRecv(struct RegistResponse *ResponseValue, char *regist_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*regist_recv=='\0') return -1;

	LOG_TRACE_ANSI("regist recv header: %s\r\n", regist_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(regist_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Content-Type"))
				strcpy_s(ResponseValue->szContentType, pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				strcpy_s(ResponseValue->szContentLength, pValue);
		}	
	}

	return ResponseValue->status_code;
}

/*
OPTIONS user_name SDTP/1.1\r\n
Command: settings\r\n
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildSettingsSend(struct SessionBuffer *psbuffer, struct RegistRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	strcat_s(header_txt, LINE_LENGTH, "Command: settings"LINE_TOKEN);
	sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, RequestValue->szAuthorization);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("cinform send header: %s\r\n", header_txt);
	return 0;
}

/*
<?xml version="1.0" encoding="UTF-8"?>
<client_settings version="1.2.1">
<uid>12345321</uid>
<pool_size>1</pool_size>
</client_settings>
*/
/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxxxxxxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized\r\n\r\n
*/
DWORD NSDTProtocol::ParseSettingsRecv(struct RegistResponse *ResponseValue, char *settings_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*settings_recv=='\0') return -1;

	LOG_TRACE_ANSI("cinform recv header: %s\r\n", settings_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(settings_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Content-Type"))
				strcpy_s(ResponseValue->szContentType, pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				strcpy_s(ResponseValue->szContentLength, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
OPTIONS user_name SDTP/1.1\r\n
Command: link\r\n
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildLinkSend(struct SessionBuffer *psbuffer, struct RegistRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	strcat_s(header_txt, LINE_LENGTH, "Command: link"LINE_TOKEN);
	sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, RequestValue->szAuthorization);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("link send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n\r\n 
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 404 Not Found
*/
DWORD NSDTProtocol::ParseLinkRecv(struct RegistResponse *ResponseValue, char *link_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Found
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*link_recv=='\0') return -1;

	LOG_TRACE_ANSI("link recv header: %s\r\n", link_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(link_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	return ResponseValue->status_code;
}


/*
OPTIONS user_name SDTP/1.1\r\n
Command: unlink\r\n
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildUnlinkSend(struct SessionBuffer *psbuffer, struct RegistRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	strcat_s(header_txt, LINE_LENGTH, "Command: unlink"LINE_TOKEN);
	sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, RequestValue->szAuthorization);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("getlist send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n\r\n 
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 403 Forbidden
*/
DWORD NSDTProtocol::ParseUnlinkRecv(struct RegistResponse *ResponseValue, char *unlink_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Forbidden
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*unlink_recv=='\0')return -1;

	LOG_TRACE_ANSI("unlink recv header: %s\r\n", unlink_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(unlink_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	return ResponseValue->status_code;
}


/*
OPTIONS user_id SDTP/1.1\r\n
Command: query\r\n
Authorization: uid:access_key\r\n\r\n
*/
DWORD NSDTProtocol::BuildQuerySend(struct SessionBuffer *psbuffer, struct AuthRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	strcat_s(header_txt, LINE_LENGTH, "Command: query"LINE_TOKEN);
	sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, RequestValue->szAuthorization);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("query send header: %s\r\n", header_txt);
	return 0;
}


/*
<?xml version="1.0" encoding="UTF-8"?>
<query version="1.2.1">
<uid>12345321</uid>
<address port="325">123.354.345.345</address>
<serv_locked>true</serv_locked>
<cached_anchor>1</cached_anchor>
<session_id>123456789</session_id>
</query>
 */
/*
SDTP/1.1 200 OK\r\n 
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxxxxxxx\r\n\r\n 
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 404 Not Found
*/
DWORD NSDTProtocol::ParseQueryRecv(struct AuthResponse *ResponseValue, char *query_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Found
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*query_recv=='\0') return -1;
	// *pCachedAnchor = 0;

	LOG_TRACE_ANSI("query recv head: %s", query_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(query_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Content-Type"))
				strcpy_s(ResponseValue->szContentType, pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				strcpy_s(ResponseValue->szContentLength, pValue);
		}
	}

	return ResponseValue->status_code;
}


/*SDTP/1.1 400 Bad Request\r\n\r\n*/
/*BOOL NSDTProtocol::ParseUnuseRecv( CString &UnuseRecvTxt)
{return TRUE;}
*/
/*SDTP/1.1 400 Bad Request\r\n\r\n*/
/*BOOL NSDTProtocol::ParseDefaultRecv( CString &DefaultRecvTxt)
{return TRUE;}
*/


/*
INITIAL user_id SDTP/1.1\r\n
Authorization: uid:access_key\r\n
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildInitialSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "INITIAL %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Authorization: %s"LINE_TOKEN, RequestValue->szAuthorization);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("initial send header: %s\r\n", header_txt);
	return 0;
}

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 404 Not Found\r\n\r\n
//SDTP/1.1 600 Wait\r\n\r\n
*/
DWORD NSDTProtocol::ParseInitialRecv(struct ServResponse *ResponseValue, char *initial_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Found; 5 Wait
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*initial_recv=='\0') return -1;

	LOG_TRACE_ANSI("init recv header: %s\r\n", initial_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(initial_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}
	}

	return ResponseValue->status_code;
}

/*
GET user_id SDTP/1.1\r\n
Params: subtype=anchor value=last-anchor\r\n // file // list // checksum // anchor
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildGetAnchorSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "GET %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Params: %s"LINE_TOKEN, RequestValue->szParams);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("get anchor send header: %s\r\n", header_txt);
	return 0;
}

/*
<?xml version="1.0" encoding="UTF-8"?>
<anchor version="1.2.1">
<uid>12345321</uid> 
<last_anchor>xxxxx</last_anchor>
</anchor>
*/
/*
SDTP/1.1 200 OK\r\n
Content-Length: xxxxx\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Session: xxx\r\n \r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 404 Not Found
//SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseGetAnchorRecv(struct ServResponse *ResponseValue, char *getlist_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*getlist_recv=='\0') return -1;

	LOG_TRACE_ANSI("get anchor recv header: %s\r\n", getlist_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(getlist_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Content-Type"))
				strcpy_s(ResponseValue->szContentType, pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				strcpy_s(ResponseValue->szContentLength, pValue);
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
GET Content SDTP/1.1\r\n
Params: subtype=list value=anchor-files\r\n // file // list // checksum//anchor-files //recursion-files //recursion-directories //list-files
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildGetListSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "GET %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Params: %s"LINE_TOKEN, RequestValue->szParams);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("getlist send header: %s\r\n", header_txt);
	return 0;
}

/*
<?xml version="1.0" encoding="UTF-8"?>
<directories version="1.2.1">
<dir_name>xxxxxxxxxx</dir_name>
<dir_name>xxxxxxxxxx</dir_name>
</directories>
*/
/*
/*
<?xml version="1.0" encoding="UTF-8"?>
<files_attrib version="1.2.1">
<file modify_type='A' file_size="343" last_write="05-25-2010 10:55:33 GMT">xxxxxxxxxx</file>
<file modify_type='A' file_size="343" last_write="05-25-2010 10:55:33 GMT">xxxxxxxxxx</file>
</files_attrib>
*/
/*
SDTP/1.1 200 OK\r\n
Content-Length: xxxxx\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Session: xxx\r\n \r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 404 Not Found
//SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseGetListRecv(struct ServResponse *ResponseValue, char *getlist_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error

	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*getlist_recv=='\0') return -1;

	LOG_TRACE_ANSI("getlist recv header: %s\r\n", getlist_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(getlist_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Content-Type"))
				strcpy_s(ResponseValue->szContentType, pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				strcpy_s(ResponseValue->szContentLength, pValue);
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
GET FileName SDTP/1.1\r\n
Params: subtype=file\r\n // file // list // checksum
Range: file=4455\r\n /chunk / block
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildGetFileSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "GET %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Params: %s"LINE_TOKEN, RequestValue->szParams);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Range: %s"LINE_TOKEN, RequestValue->szContentRange);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("get send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Content-Range: file 4455/8888\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseGetFileRecv(struct ServResponse *ResponseValue, char *get_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*get_recv=='\0') return -1;

	LOG_TRACE_ANSI("get recv header: %s\r\n", get_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(get_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;			
			else if(!strcmp(pLineTxt, "Last-Modified"))
				strcpy_s(ResponseValue->szLastWrite, pValue);		
			else if(!strcmp(pLineTxt, "Content-Range"))
				strcpy_s(ResponseValue->szContentRange, pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				strcpy_s(ResponseValue->szContentLength, pValue);
			else if(!strcmp(pLineTxt, "Content-Type"))
				strcpy_s(ResponseValue->szContentType, pValue);
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
GET FileName SDTP/1.1\r\n
Params: subtype=checksum value=md5\r\n // file // list // checksum/adler32 /adler32&md5 //  add by james 2011-09-15   
Range: file=4455\r\n /chunk /block
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildGetCsumSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "GET %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Params: %s"LINE_TOKEN, RequestValue->szParams);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Range: %s"LINE_TOKEN, RequestValue->szContentRange);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("getsums send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseGetCsumRecv(struct ServResponse *ResponseValue, char *getsum_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error

	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*getsum_recv=='\0') return -1;

	LOG_TRACE_ANSI("getsums recv head: %s", getsum_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(getsum_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Content-Type"))
				strcpy_s(ResponseValue->szContentType, pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				strcpy_s(ResponseValue->szContentLength, pValue);
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}
	}

	return ResponseValue->status_code;
}


/*
POST FileName SDTP/1.1\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n			//add by james 2010-05-25
Content-Range: file 4455/455255\r\n /chunk
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildPostSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "POST %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Last-Modified: %s"LINE_TOKEN, RequestValue->szLastWrite);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Content-Range: %s"LINE_TOKEN, RequestValue->szContentRange);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Content-Length: %s"LINE_TOKEN, RequestValue->szContentLength);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Content-Type: %s"LINE_TOKEN, RequestValue->szContentType);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HDBD_BUFFER

		LOG_TRACE_ANSI("post send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParsePostRecv(struct ServResponse *ResponseValue, char *post_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error

	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*post_recv=='\0') return -1;

	LOG_TRACE_ANSI("post recv header: %s\r\n", post_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(post_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
PUT FileName SDTP/1.1\r\n
Params: subtype=file\r\n // file // checksum					// add by james 2011-10-28
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n			//add by james 2010-05-25
Content-Range: file 4455/55244\r\n /chunk
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildPutFileSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "PUT %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Params: %s"LINE_TOKEN, RequestValue->szParams);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Last-Modified: %s"LINE_TOKEN, RequestValue->szLastWrite);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Content-Range: %s"LINE_TOKEN, RequestValue->szContentRange);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Content-Length: %s"LINE_TOKEN, RequestValue->szContentLength);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Content-Type: %s"LINE_TOKEN, RequestValue->szContentType);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HDBD_BUFFER

	LOG_TRACE_ANSI("put send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParsePutFileRecv(struct ServResponse *ResponseValue, char *put_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error

	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*put_recv=='\0') return -1;

	LOG_TRACE_ANSI("put recv header: %s\r\n", put_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(put_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
PUT FileName SDTP/1.1\r\n
Params: subtype=checksum value=md5\r\n // file // checksum/adler32 /adler32&md5 //  add by james 2011-09-15
Range: file=4455\r\n /chunk /block
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildPutCsumSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "PUT %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Params: %s"LINE_TOKEN, RequestValue->szParams);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Range: %s"LINE_TOKEN, RequestValue->szContentRange);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Content-Length: %s"LINE_TOKEN, RequestValue->szContentLength);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Content-Type: %s"LINE_TOKEN, RequestValue->szContentType);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HDBD_BUFFER

		LOG_TRACE_ANSI("putsums send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Content-Range: file 4455/8000\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParsePutCsumRecv(struct ServResponse *ResponseValue, char *putsum_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error

	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*putsum_recv=='\0') return -1;

	LOG_TRACE_ANSI("putsums recv header: %s\r\n", putsum_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(putsum_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Last-Modified"))
				strcpy_s(ResponseValue->szLastWrite, pValue);
			else if(!strcmp(pLineTxt, "Content-Range"))
				strcpy_s(ResponseValue->szContentRange, pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				strcpy_s(ResponseValue->szContentLength, pValue);
			else if(!strcmp(pLineTxt, "Content-Type"))
				strcpy_s(ResponseValue->szContentType, pValue);
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}
	}

	return ResponseValue->status_code;
}


/*
MOVE FileName SDTP/1.1\r\n
Params: destination=xxxxxxx\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildMoveSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "MOVE %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Params: %s"LINE_TOKEN, RequestValue->szParams);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Last-Modified: %s"LINE_TOKEN, RequestValue->szLastWrite);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("move send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 : Expectation Failed
*/
DWORD NSDTProtocol::ParseMoveRecv(struct ServResponse *ResponseValue, char *move_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*move_recv=='\0') return -1;

	LOG_TRACE_ANSI("move recv header: %s\r\n", move_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(move_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
DELETE FileName SDTP/1.1\r\n
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildDeleteSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "DELETE %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("delete send header: %s\r\n", header_txt);
	return true;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 : Expectation Failed
*/
DWORD NSDTProtocol::ParseDeleteRecv(struct ServResponse *ResponseValue, char *delete_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*delete_recv=='\0') return -1;

	LOG_TRACE_ANSI("delete recv header: %s\r\n", delete_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(delete_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
FINAL user_id SDTP/1.1\r\n
Params: subtype=end\r\n //successful //finish
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildFinalSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue) {
	char line_txt[LINE_LENGTH];
	char *header_txt = psbuffer->head_buffer;

	sprintf_s(line_txt, "FINAL %s SDTP/1.1"LINE_TOKEN, RequestValue->szResourceIdenti);
	strcpy_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Params: %s"LINE_TOKEN, RequestValue->szParams);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, RequestValue->szSessionID);
	strcat_s(header_txt, LINE_LENGTH, line_txt);
	FINISH_HEADER_BUFFER

		LOG_TRACE_ANSI("final send header: %s\r\n", header_txt);
	return 0;
}


/*
SDTP/1.1 200 OK\r\n\r\n
Params: anchor=xxxxx\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 :  Expectation Failed
*/
DWORD NSDTProtocol::ParseFinalSuccessRecv(struct ServResponse *ResponseValue, char *final_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*final_recv=='\0') return -1;

	LOG_TRACE_ANSI("final recv header: %s\r\n", final_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(final_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Params"))
				strcpy_s(ResponseValue->szParams, pValue);
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		}	
	}

	return ResponseValue->status_code;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 :  Expectation Failed
*/
DWORD NSDTProtocol::ParseFinalEndRecv(struct ServResponse *ResponseValue, char *final_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*final_recv=='\0') return -1;

	LOG_TRACE_ANSI("final recv header: %s\r\n", final_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(final_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		} 	
	}

	return ResponseValue->status_code;
}

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 :  Expectation Failed
*/
DWORD NSDTProtocol::ParseFinalFinishRecv(struct ServResponse *ResponseValue, char *final_recv) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	char *pLineTxt = NULL;
	char *pValue = NULL;

	if(*final_recv=='\0') return -1;

	LOG_TRACE_ANSI("final recv header: %s\r\n", final_recv);
	char *splittoken;
	pLineTxt = nstriutility::strsplit(final_recv, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = nstriutility::split_status(pLineTxt);
	// if(strcmp(pLineTxt, SDTP_TOKEN)) return -1;
	if(!pValue) return -1;
	else ResponseValue->status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == ResponseValue->status_code) {
		while(pLineTxt = nstriutility::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = nstriutility::split_value(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Session"))
				strcpy_s(ResponseValue->szSessionID, pValue);
		} 	
	}

	return ResponseValue->status_code;
}