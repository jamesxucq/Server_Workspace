#include "StdAfx.h"

#include "tools_macro.h"
#include "StringUtility.h"
#include "SDTProtocol.h"

#include "Logger.h"
#include "logger_ansi.h"

#define HEAD_TOKEN		"\r\n\r\n"
#define LINE_TOKEN		"\r\n"
#define FIELD_TOKEN		" "
#define SDTP_TOKEN		"SDTP/1.1"
#define ONLY_HEADER_SECTION		0

#define COMPLETE_HEADER_BUFFER \
	strcpy_s(psbuffer->head_buffer, strlen(szHeaderTxt)+1, szHeaderTxt); \
	psbuffer->hlen = strlen(psbuffer->head_buffer); \
	psbuffer->blen = ONLY_HEADER_SECTION;
//
#define COMPLETE_BODY_BUFFER \
	strcpy_s(psbuffer->head_buffer, strlen(szHeaderTxt)+1, szHeaderTxt); \
	psbuffer->hlen = strlen(psbuffer->head_buffer); \
	psbuffer->body_buffer = psbuffer->srecv_buffer + psbuffer->hlen; \
	psbuffer->blen = SOCKET_BUFFER - psbuffer->hlen;
//
DWORD NSDTProtocol::GetHeaderValue(char **head_buffer, char *szReceiveTxt)
{
	if(!szReceiveTxt) return RETURN_ERROR;
//
	char *szHeaderEnd = strstr(szReceiveTxt, "\r\n\r\n");
	if(!szHeaderEnd) return RETURN_ERROR;
	szHeaderEnd += 2;
	*szHeaderEnd++ = '\0';
	*szHeaderEnd++ = '\0';
//
	*head_buffer = szReceiveTxt;
	return szHeaderEnd - szReceiveTxt;
}


DWORD NSDTProtocol::GetBodyValue(char **body_buffer, char *szReceiveTxt, int iLength)
{
	if(!szReceiveTxt) return RETURN_ERROR;
//
	char *pBodyStart = szReceiveTxt + strlen(szReceiveTxt) + 2;
	*body_buffer = pBodyStart;
//
	return iLength - (pBodyStart - szReceiveTxt);
}

/*
CONTROL 192.168.1.102:8008 SDTP/1.1\r\n // 192.168.1.102
Command: add\r\n
Params: subtype=admin\r\n // admin/server 
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildAddSend(struct SessionBuffer *psbuffer, DWORD dwAddType, char *szAddress, char *szAuthsum)
{
	char szLineTxt[LINE_BUFF_SIZE];
	char szHeaderTxt[TEXT_BUFF_SIZE];
//
	sprintf_s(szLineTxt, "CONTROL %s SDTP/1.1"LINE_TOKEN, szAddress);
	strcpy_s(szHeaderTxt, szLineTxt);
	strcat_s(szHeaderTxt, "Command: add"LINE_TOKEN);
	if(ADD_TYPE_ADMIN == dwAddType)
		sprintf_s(szLineTxt, "Params: subtype=admin\r\n");
	else sprintf_s(szLineTxt, "Params: subtype=server\r\n");
	strcat_s(szHeaderTxt, szLineTxt);
	sprintf_s(szLineTxt, "Authorization: %s"HEAD_TOKEN, szAuthsum);
	strcat_s(szHeaderTxt, szLineTxt);
//
	LOG_TRACE_ANSI("add send header: %s\r\n", szHeaderTxt);
	COMPLETE_HEADER_BUFFER
//
		return 0;
}

DWORD ExecuteValueType(char *szExecuteText) {
	if (!szExecuteText || !strcmp("", szExecuteText)) 
		return EXECU_TYPE_UNKNOWN;
	else if (!strcmp("successful", szExecuteText)) 
		return EXECU_TYPE_SUCCESS;
	else if (!strcmp("exception", szExecuteText)) 
		return EXECU_TYPE_EXCEPTION;
//
	return EXECU_TYPE_UNKNOWN;
}

/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
*/
DWORD NSDTProtocol::ParseAddRecv(DWORD *dwAddedStatus, char *szAddRecvTxt) // -1 error; 0 succ; 1 Bad Request; 3 Not Found
{
	char *pLineTxt = NULL;
	char *pValue = NULL;
	DWORD status_code;
//
	if(!szAddRecvTxt || *szAddRecvTxt=='\0') 
		return EXCEPTION;
//
	LOG_TRACE_ANSI("add recv header: %s\r\n", szAddRecvTxt);
	char *splittoken;
	pLineTxt = striutil::strsplit(szAddRecvTxt, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = striutil::split_value(pLineTxt);
	if(strcmp(pLineTxt, SDTP_TOKEN)) return EXCEPTION;
	if(!pValue) return EXCEPTION;
	else status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == status_code) {
		while(pLineTxt = striutil::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = striutil::split_line(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Execute")) 
				*dwAddedStatus = ExecuteValueType(pValue);
		}	
	}else *dwAddedStatus = EXECU_TYPE_FAILED;
//
	return status_code;
}

/*
CONTROL user_id SDTP/1.1\r\n // 192.168.1.10
Command: clear\r\n
Params: subtype=all-users\r\n // all-users/single-user/server/admin
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildClearSend(struct SessionBuffer *psbuffer, DWORD dwClearType, char *szAdminAddress, IN char *szAuthsum)
{
	char szLineTxt[LINE_BUFF_SIZE];
	char szHeaderTxt[TEXT_BUFF_SIZE];
//
	sprintf_s(szLineTxt, "CONTROL %s SDTP/1.1"LINE_TOKEN, szAdminAddress);
	strcpy_s(szHeaderTxt, szLineTxt);
	strcat_s(szHeaderTxt, "Command: clear"LINE_TOKEN);
	switch(dwClearType) {
		case CLEAR_TYPE_ALLUSRS:
			strcat_s(szHeaderTxt, "Params: subtype=all-users\r\n");
			break;
		case CLEAR_TYPE_SINGLE:
			strcat_s(szHeaderTxt, "Params: subtype=single-user\r\n");
			break;	
		case CLEAR_TYPE_ADMIN:
			strcat_s(szHeaderTxt, "Params: subtype=admin\r\n");
			break;
		case CLEAR_TYPE_WORKER:
			strcat_s(szHeaderTxt, "Params: subtype=server\r\n");
			break;
	}
	sprintf_s(szLineTxt, "Authorization: %s"HEAD_TOKEN, szAuthsum);
	strcat_s(szHeaderTxt, szLineTxt);
//
	LOG_TRACE_ANSI("clear send header: %s\r\n", szHeaderTxt);
	COMPLETE_HEADER_BUFFER
//
		return 0;
}

/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
// SDTP/1.1 409 Conflict 
*/
DWORD NSDTProtocol::ParseClearRecv(DWORD *dwClearedStatus, char *szClearRecvTxt) // -1 error; 0 succ; 1 Bad Request; 3 Not Found
{
	char *pLineTxt = NULL;
	char *pValue = NULL;
	DWORD status_code;

	if(!szClearRecvTxt || *szClearRecvTxt=='\0') 
		return EXCEPTION;
//
	LOG_TRACE_ANSI("clear recv header: %s\r\n", szClearRecvTxt);
	char *splittoken;
	pLineTxt = striutil::strsplit(szClearRecvTxt, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = striutil::split_value(pLineTxt);
	if(strcmp(pLineTxt, SDTP_TOKEN)) return EXCEPTION;
	if(!pValue) return EXCEPTION;
	else status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == status_code) {
		while(pLineTxt = striutil::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = striutil::split_line(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Execute")) 
				*dwClearedStatus = ExecuteValueType(pValue);
		}	
	} else if(409 == status_code) *dwClearedStatus = EXECU_TYPE_CONFLICT;
	else *dwClearedStatus = EXECU_TYPE_FAILED;
//
	return status_code;
}

/*
CONTROL * SDTP/1.1\r\n
Command: list\r\n
Params: subtype=server\r\n // server/admin
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildListSend(struct SessionBuffer *psbuffer, DWORD dwListType, char *szAuthsum)
{
	char szLineTxt[LINE_BUFF_SIZE];
	char szHeaderTxt[TEXT_BUFF_SIZE];
//
	strcpy_s(szHeaderTxt, "CONTROL * SDTP/1.1"LINE_TOKEN);
	strcat_s(szHeaderTxt, "Command: list"LINE_TOKEN);
	if(LIST_TYPE_ADMIN == dwListType)
		strcat_s(szHeaderTxt, "Params: subtype=admin\r\n");
	else strcat_s(szHeaderTxt, "Params: subtype=server\r\n");
	sprintf_s(szLineTxt, "Authorization: %s"HEAD_TOKEN, szAuthsum);
	strcat_s(szHeaderTxt, szLineTxt);
//
	LOG_TRACE_ANSI("list send header: %s\r\n", szHeaderTxt);
	COMPLETE_HEADER_BUFFER
//
		return 0;
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
//
	return CONTENT_TYPE_UNKNOWN;
}

/*
<?xml version="1.0" encoding="UTF-8"?>
<list version="1.2.1">
<server>xxxxxx</server>
<server>xxxxxxxxx</server>
</list>
*/
/*
<?xml version="1.0" encoding="UTF-8"?>
<list version="1.2.1">
<admin>xxxxxx</admin>
<admin>xxxxxxxxx</admin>
</list>
*/

/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxx\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
*/
DWORD NSDTProtocol::ParseListRecv(DWORD *dwContentType, DWORD *dwContentLength, DWORD *dwClearedStatus, char *szListRecvTxt) // -1 error; 0 succ; 1 Bad Request; 3 Not Found
{
	char *pLineTxt = NULL;
	char *pValue = NULL;
	DWORD status_code;
//
	if(!szListRecvTxt || *szListRecvTxt=='\0') 
		return EXCEPTION;
//
	LOG_TRACE_ANSI("list recv header: %s\r\n", szListRecvTxt);
	char *splittoken;
	pLineTxt = striutil::strsplit(szListRecvTxt, LINE_TOKEN, &splittoken);
	/////////////////////////////////////////////////
	pValue = striutil::split_value(pLineTxt);
	if(strcmp(pLineTxt, SDTP_TOKEN)) return EXCEPTION;
	if(!pValue) return EXCEPTION;
	else status_code = atol(pValue);

	/////////////////////////////////////////////////
	if(200 == status_code) {
		while(pLineTxt = striutil::strsplit(NULL, LINE_TOKEN, &splittoken)) {
			pValue = striutil::split_line(pLineTxt);

			if(!pValue) continue;
			else if(!strcmp(pLineTxt, "Content-Type"))
				*dwContentType = ContentValueType(pValue);
			else if(!strcmp(pLineTxt, "Content-Length"))
				*dwContentLength = atoi(pValue);
		}
		*dwClearedStatus = EXECU_TYPE_SUCCESS;
	} else *dwClearedStatus = EXECU_TYPE_FAILED;
//
	return status_code;
}


