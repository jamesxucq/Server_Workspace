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
	sbuffer->hlen = strlen(header_txt); \
	sbuffer->blen = ONLY_HEADER_SECTION;

#define FINISH_HDBD_BUFFER \
	sbuffer->hlen = strlen(header_txt); \
	sbuffer->body_buffer = header_txt + sbuffer->hlen; \
	sbuffer->blen = RECV_BUFF_SIZE - sbuffer->hlen;


DWORD NSDTProtocol::GetHeaderValue(char **head_buffer, char *szReceiveTxt) {
    if(!szReceiveTxt) return ERR_FAULT;
    //
    char *header_end = strstr(szReceiveTxt, "\r\n\r\n");
    if(!header_end) return ERR_FAULT;
    header_end += 2;
    *header_end++ = '\0';
    *header_end++ = '\0';
    //
    *head_buffer = szReceiveTxt;
    return header_end - szReceiveTxt;
}


DWORD NSDTProtocol::GetBodyValue(char **body_buffer, char *szReceiveTxt, int iLeng) {
    if(!szReceiveTxt) return ERR_FAULT;
    char *body_start = szReceiveTxt + strlen(szReceiveTxt) + 2;
    *body_buffer = body_start;
    return iLeng - (body_start - szReceiveTxt);
}

/*
OPTIONS user_name SDTP/1.1\r\n
Command: register\r\n
User-Agent: xxxxx\r\n
From: xxxx\r\n
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildRegistSend(struct SeionBuffer *sbuffer, struct RegistRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    strcat_s(header_txt, LINE_LENGTH, "Command: register"LINE_TOKEN);
    sprintf_s(line_txt, "User-Agent: %s"LINE_TOKEN, pReqValue->szUserAgent);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "From: %s"LINE_TOKEN, pReqValue->szLocalIdenti);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, pReqValue->szAuthorize);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    // _LOG_ANSI("regist send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
<?xml version="1.0" encoding="UTF-8"?>
<register version="1.2.1">
<uid>12345321</uid>
<cache_address port="325">123.354.345.345</cache_address>
<access_key>2343ewr544r454rewfa45qwee</access_key>
</register>
*/
/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxxxxxxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized\r\n\r\n
*/
DWORD NSDTProtocol::ParseRegistRecv(struct RegistResponse *pResValue, char *regist_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == regist_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("regist recv header: %s\r\n", regist_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(regist_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
OPTIONS user_name SDTP/1.1\r\n
Command: settings\r\n
From: xxxx\r\n
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildSettingsSend(struct SeionBuffer *sbuffer, struct RegistRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    strcat_s(header_txt, LINE_LENGTH, "Command: settings"LINE_TOKEN);
    sprintf_s(line_txt, "From: %s"LINE_TOKEN, pReqValue->szLocalIdenti);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, pReqValue->szAuthorize);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    //
    // _LOG_ANSI("cinform send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
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
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized\r\n\r\n
*/
DWORD NSDTProtocol::ParseSettingsRecv(struct RegistResponse *pResValue, char *settings_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == settings_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("cinform recv header: %s\r\n", settings_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(settings_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
OPTIONS user_name SDTP/1.1\r\n
Command: link\r\n
From: xxxx\r\n
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildLinkSend(struct SeionBuffer *sbuffer, struct RegistRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    strcat_s(header_txt, LINE_LENGTH, "Command: link"LINE_TOKEN);
    sprintf_s(line_txt, "From: %s"LINE_TOKEN, pReqValue->szLocalIdenti);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, pReqValue->szAuthorize);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    // _LOG_ANSI("link send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
*/
DWORD NSDTProtocol::ParseLinkRecv(struct RegistResponse *pResValue, char *link_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Found
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == link_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("link recv header: %s\r\n", link_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(link_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    return pResValue->status_code;
}


/*
OPTIONS user_name SDTP/1.1\r\n
Command: unlink\r\n
From: xxxx\r\n
Authorization: user:password\r\n\r\n
*/
DWORD NSDTProtocol::BuildUnlinkSend(struct SeionBuffer *sbuffer, struct RegistRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    strcat_s(header_txt, LINE_LENGTH, "Command: unlink"LINE_TOKEN);
    sprintf_s(line_txt, "From: %s"LINE_TOKEN, pReqValue->szLocalIdenti);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, pReqValue->szAuthorize);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    // _LOG_ANSI("unlink send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 403 Forbidden
*/
DWORD NSDTProtocol::ParseUnlinkRecv(struct RegistResponse *pResValue, char *unlink_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Forbidden
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == unlink_recv[0])return ((DWORD)-1);
    // _LOG_ANSI("unlink recv header: %s\r\n", unlink_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(unlink_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    return pResValue->status_code;
}


/*
OPTIONS user_id SDTP/1.1\r\n
Command: query\r\n
Authorization: uid:access_key\r\n\r\n
*/
DWORD NSDTProtocol::BuildQuerySend(struct SeionBuffer *sbuffer, struct AuthRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    strcat_s(header_txt, LINE_LENGTH, "Command: query"LINE_TOKEN);
    sprintf_s(line_txt, "Authorization: %s"HEAD_TOKEN, pReqValue->szAuthorize);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    // _LOG_ANSI("query send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
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
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
*/
DWORD NSDTProtocol::ParseQueryRecv(struct AuthResponse *pResValue, char *query_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Found
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == query_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("query recv head: %s", query_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(query_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*SDTP/1.1 400 Bad Request\r\n\r\n*/
/*BOOL NSDTProtocol::ParseUnuseRecv( CString &csUnuseRecvTxt)
{ return TRUE; }
*/
/*SDTP/1.1 400 Bad Request\r\n\r\n*/
/*BOOL NSDTProtocol::ParseDefaultRecv( CString &csDefaultRecvTxt)
{ return TRUE; }
*/

/*
INITIAL user_id SDTP/1.1\r\n
Params: valid=xxxxxxxxxx\r\n
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildInitialSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "INITIAL %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
// _LOG_ANSI("initial send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found\r\n\r\n
// SDTP/1.1 600 Wait\r\n\r\n
*/
DWORD NSDTProtocol::ParseInitialRecv(struct ServResponse *pResValue, char *initial_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Found; 5 Wait
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == initial_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("init recv header: %s\r\n", initial_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(initial_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
INITIAL user_id SDTP/1.1\r\n
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildKeepAliveSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "KALIVE %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
// _LOG_ANSI("kalive send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found\r\n\r\n
*/
DWORD NSDTProtocol::ParseKeepAliveRecv(struct ServResponse *pResValue, char *kalive_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Found; 5 Wait
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == kalive_recv[0]) return ((DWORD)-1);
// _LOG_ANSI("kalive recv header: %s\r\n", kalive_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(kalive_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
HEAD Content SDTP/1.1\r\n
Params: subentry=list value=anchor-files\r\n // file // list // chks // anchor-files // recursion-files // recursion-directories // list-directory
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildHeadListSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "HEAD %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    //
// _LOG_ANSI("headlist send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}

/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxx\r\n
Attach: md5=xxxxxxxxxxxxxxx\r\n
Session: xxx\r\n \r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 404 Not Found
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseHeadListRecv(struct ServResponse *pResValue, char *headlist_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == headlist_recv[0]) return ((DWORD)-1);
// _LOG_ANSI("headlist recv header: %s\r\n", headlist_recv); // disable by james 20130507
    //
    char *splittok;
    pLineTxt = struti::strsplit(headlist_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
            else if(!strcmp(pLineTxt, "Attach"))
                strcpy_s(pResValue->szAttach, pValue);
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
HEAD FileName SDTP/1.1\r\n
Params: subentry=file\r\n // file // list // chks
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildHeadFileSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "HEAD %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    //
    // _LOG_ANSI("get send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Content-Length: xxxxx\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseHeadFileRecv(struct ServResponse *pResValue, char *head_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == head_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("head recv header: %s\r\n", head_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(head_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Last-Modified"))
                strcpy_s(pResValue->szLastWrite, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
GET user_id SDTP/1.1\r\n
Params: subentry=anchor value=last-anchor\r\n // file // list // chks // anchor
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildGetAnchorSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "GET %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    //
// _LOG_ANSI("get anchor send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
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
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 404 Not Found
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseGetAnchorRecv(struct ServResponse *pResValue, char *getlist_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
_LOG_DEBUG(_T("parse get anchor recv."));
    if('\0' == getlist_recv[0]) return ((DWORD)-1);
// _LOG_ANSI("get anchor recv header: %s\r\n", getlist_recv); // disable by james 20130507
    //
    char *splittok;
    pLineTxt = struti::strsplit(getlist_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
GET Content SDTP/1.1\r\n
Params: subentry=list value=anchor-files\r\n // file // list // chks // anchor-files // recursion-files // recursion-directories // list-directory
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildGetListSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "GET %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Range: %s"LINE_TOKEN, pReqValue->szContentRange);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    //
// _LOG_ANSI("getlist send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}

/*
<?xml version="1.0" encoding="UTF-8"?>
<directories version="1.2.1">
<dire_name>xxxxxxxxxx</dire_name>
<dire_name>xxxxxxxxxx</dire_name>
</directories>
*/
/*
/*
<?xml version="1.0" encoding="UTF-8"?>
<inodes_attrib version="1.2.1">
<inode action_type='A' file_size="343" last_write="05-25-2010 10:55:33 GMT">xxxxxxxxxx</inode>
<inode action_type='A' file_size="343" last_write="05-25-2010 10:55:33 GMT">xxxxxxxxxx</inode>
</inodes_attrib>
*/
/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxx\r\n
Session: xxx\r\n \r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 404 Not Found
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseGetListRecv(struct ServResponse *pResValue, char *getlist_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == getlist_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("getlist recv header: %s\r\n", getlist_recv); // disable by james 20130507
    //
    char *splittok;
    pLineTxt = struti::strsplit(getlist_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
GET FileName SDTP/1.1\r\n
Params: subentry=file\r\n // file // list // chks
Range: file=4455\r\n /chunk / block
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildGetFileSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "GET %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Range: %s"LINE_TOKEN, pReqValue->szContentRange);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
// _LOG_ANSI("get send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Content-Length: xxxxx\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Content-Range: file 4455/8888\r\n
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseGetFileRecv(struct ServResponse *pResValue, char *get_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == get_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("get recv header: %s\r\n", get_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(get_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Last-Modified"))
                strcpy_s(pResValue->szLastWrite, pValue);
            else if(!strcmp(pLineTxt, "Content-Range"))
                strcpy_s(pResValue->szContentRange, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
GET FileName SDTP/1.1\r\n
Params: subentry=chks value=md5\r\n // file // list // chks/md5 /adler32&md5 /sha1 //  add by james 2011-09-15
Range: file=4455\r\n /chunk /block
Cache-Verify: no-cache\r\n //no-cache /cache
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildGetChksSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "GET %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Range: %s"LINE_TOKEN, pReqValue->szContentRange);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Cache-Verify: %s"LINE_TOKEN, pReqValue->szCacheVerify);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
// _LOG_ANSI("getsums send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Content-Length: xxxxx\r\n
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParseGetChksRecv(struct ServResponse *pResValue, char *getsum_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
// _LOG_DEBUG(_T("in parse get chks"));
	if('\0' == getsum_recv[0]) return ((DWORD)-1);
// _LOG_ANSI("getsums recv head: %s", getsum_recv); // disable by james 20130507
	char *splittok;
    pLineTxt = struti::strsplit(getsum_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
	if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
POST FileName SDTP/1.1\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Content-Length: xxxxx\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n			// add by james 2010-05-25
Content-Range: file 4455/455255\r\n /chunk
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildPostSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "POST %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Last-Modified: %s"LINE_TOKEN, pReqValue->szLastWrite);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Content-Range: %s"LINE_TOKEN, pReqValue->szContentRange);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Content-Length: %s"LINE_TOKEN, pReqValue->szContentLength);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Content-Type: %s"LINE_TOKEN, pReqValue->szContentType);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HDBD_BUFFER
    // _LOG_ANSI("post send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParsePostRecv(struct ServResponse *pResValue, char *post_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == post_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("post recv header: %s\r\n", post_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(post_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
PUT FileName SDTP/1.1\r\n
Params: subentry=file\r\n // file // chks					// add by james 2011-10-28
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n			// add by james 2010-05-25
Content-Range: file 4455/55244\r\n /chunk
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildPutFileSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "PUT %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Last-Modified: %s"LINE_TOKEN, pReqValue->szLastWrite);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Content-Range: %s"LINE_TOKEN, pReqValue->szContentRange);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Content-Length: %s"LINE_TOKEN, pReqValue->szContentLength);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Content-Type: %s"LINE_TOKEN, pReqValue->szContentType);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HDBD_BUFFER
    // _LOG_ANSI("put send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParsePutFileRecv(struct ServResponse *pResValue, char *put_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == put_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("put recv header: %s\r\n", put_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(put_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
PUT FileName SDTP/1.1\r\n
Params: subentry=file\r\n // file // chks					// add by james 2011-10-28
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n			// add by james 2010-05-25
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildPutDireSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "PUT %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Last-Modified: %s"LINE_TOKEN, pReqValue->szLastWrite);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    // _LOG_ANSI("put send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParsePutDireRecv(struct ServResponse *pResValue, char *put_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == put_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("put recv header: %s\r\n", put_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(put_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
PUT FileName SDTP/1.1\r\n
Params: subentry=chks value=md5\r\n // file // chks/adler32 /adler32&md5 //  add by james 2011-09-15
Range: file=4455\r\n /chunk /block
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildPutChksSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "PUT %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Range: %s"LINE_TOKEN, pReqValue->szContentRange);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Content-Length: %s"LINE_TOKEN, pReqValue->szContentLength);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Content-Type: %s"LINE_TOKEN, pReqValue->szContentType);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HDBD_BUFFER
    // _LOG_ANSI("putsums send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Content-Range: file 4455/8000\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
*/
DWORD NSDTProtocol::ParsePutChksRecv(struct ServResponse *pResValue, char *putsum_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == putsum_recv[0]) return ((DWORD)-1);
// _LOG_ANSI("putsums recv header: %s\r\n", putsum_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(putsum_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Last-Modified"))
                strcpy_s(pResValue->szLastWrite, pValue);
            else if(!strcmp(pLineTxt, "Content-Range"))
                strcpy_s(pResValue->szContentRange, pValue);
            else if(!strcmp(pLineTxt, "Content-Length"))
                strcpy_s(pResValue->szContentLength, pValue);
            else if(!strcmp(pLineTxt, "Content-Type"))
                strcpy_s(pResValue->szContentType, pValue);
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
MOVE FileName SDTP/1.1\r\n
Params: desti=xxxxxxx\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildMoveSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "MOVE %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Last-Modified: %s"LINE_TOKEN, pReqValue->szLastWrite);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    // _LOG_ANSI("move send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
// 417 : Expectation Failed
*/
DWORD NSDTProtocol::ParseMoveRecv(struct ServResponse *pResValue, char *move_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == move_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("move recv header: %s\r\n", move_recv);
    char *splittok;
    pLineTxt = struti::strsplit(move_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
COPY FileName SDTP/1.1\r\n
Params: desti=xxxxxxx\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildCopySend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "COPY %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Last-Modified: %s"LINE_TOKEN, pReqValue->szLastWrite);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    // _LOG_ANSI("copy send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
// 417 : Expectation Failed
*/
DWORD NSDTProtocol::ParseCopyRecv(struct ServResponse *pResValue, char *move_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == move_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("move recv header: %s\r\n", move_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(move_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
DELETE FileName SDTP/1.1\r\n
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildDeleteSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "DELETE %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
    // _LOG_ANSI("delete send header: %s\r\n", header_txt); // disable by james 20130507
    return true;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
// 417 : Expectation Failed
*/
DWORD NSDTProtocol::ParseDeleteRecv(struct ServResponse *pResValue, char *delete_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == delete_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("delete recv header: %s\r\n", delete_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(delete_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
FINAL user_id SDTP/1.1\r\n
Params: subtype=end\r\n // successful // finish
Session: xxx\r\n\r\n
*/
DWORD NSDTProtocol::BuildFinalSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue) {
    char line_txt[LINE_LENGTH];
    char *header_txt = sbuffer->head_buffer;
    //
    sprintf_s(line_txt, "FINAL %s SDTP/1.1"LINE_TOKEN, pReqValue->szResIdentity);
    strcpy_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Params: %s"LINE_TOKEN, pReqValue->szParams);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    sprintf_s(line_txt, "Session: %s"HEAD_TOKEN, pReqValue->szSessionID);
    strcat_s(header_txt, LINE_LENGTH, line_txt);
    FINISH_HEADER_BUFFER
// _LOG_ANSI("final send header: %s\r\n", header_txt); // disable by james 20130507
    return 0x00;
}


/*
SDTP/1.1 200 OK\r\n\r\n
Attach: anchor=xxxxx\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
// 417 :  Expectation Failed
*/
DWORD NSDTProtocol::ParseFinalSuccessRecv(struct ServResponse *pResValue, char *final_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == final_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("final recv header: %s\r\n", final_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(final_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Attach"))
                strcpy_s(pResValue->szAttach, pValue);
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}


/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
// 417 :  Expectation Failed
*/
DWORD NSDTProtocol::ParseFinalEndRecv(struct ServResponse *pResValue, char *final_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == final_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("final recv header: %s\r\n", final_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(final_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
// SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 401 Not Authorized
// SDTP/1.1 500 Internal Server Error
// 417 :  Expectation Failed
*/
DWORD NSDTProtocol::ParseFinalFinishRecv(struct ServResponse *pResValue, char *final_recv) {  // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
    char *pLineTxt = NULL;
    char *pValue = NULL;
    //
    if('\0' == final_recv[0]) return ((DWORD)-1);
    // _LOG_ANSI("final recv header: %s\r\n", final_recv); // disable by james 20130507
    char *splittok;
    pLineTxt = struti::strsplit(final_recv, LINE_TOKEN, &splittok);
    //
    pValue = struti::split_status(pLineTxt);
    // if(strcmp(pLineTxt, SDTP_TOKEN)) return ((DWORD)-1);
    if(!pValue) return ((DWORD)-1);
    else pResValue->status_code = atol(pValue);
    //
    if(200 == pResValue->status_code) {
        while(pLineTxt = struti::strsplit(NULL, LINE_TOKEN, &splittok)) {
            pValue = struti::split_value(pLineTxt);
            if(!pValue) continue;
            else if(!strcmp(pLineTxt, "Session"))
                strcpy_s(pResValue->szSessionID, pValue);
        }
    }
    //
    return pResValue->status_code;
}