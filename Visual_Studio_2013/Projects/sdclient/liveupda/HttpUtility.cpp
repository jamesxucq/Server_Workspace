#include "StdAfx.h"

#include "LiveUtility.h"
#include "LiveSocks.h"
#include "HttpUtility.h"

//
// GET /active-update.xml HTTP/1.1\r\nHost: 192.168.1.103\r\nAccept: */*\r\nUser-Agent: LiveUpdate/1.21.1\r\n\r\n
VOID BuildRequest(char *szReqLine, char *szHost, char *szReqURI) {
	sprintf(szReqLine, "GET %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nUser-Agent: LiveUpdate/1.21.1\r\n\r\n", szReqURI, szHost);
}

#define LINE_TOKEN		"\r\n"

struct ServResponse {
	int			status_code;
	//
	char		szLastWrite[TIME_LENGTH];
	char		szContentRange[MIN_TEXT_LEN];
	char		szContentLength[DIGIT_LENGTH];
	char		szContentType[CONTENT_TYPE_LENGTH];
};

/*
HTTP/1.1 200 OK
Server: nginx/1.1.12
Date: Sat, 06 Oct 2012 06:36:00 GMT
Content-Type: text/xml
Content-Length: 1599
Last-Modified: Sat, 06 Oct 2012 01:07:15 GMT
Connection: keep-alive
Accept-Ranges: bytes
*/
DWORD ParseResponse(struct ServResponse *pResValue, char *szResponseTxt) { // -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	char *pLineTxt = NULL;
	char *pValue = NULL;
//
	if('\0' == szResponseTxt[0]) return ((DWORD)-1);
// _LOG_ANSI("get anchor recv header: %s\r\n", szResponseTxt);
	char *splittok;
	pLineTxt = struti::strsplit(szResponseTxt, LINE_TOKEN, &splittok);
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
			else if(!strcmp(pLineTxt, "Last-Modified"))
				strcpy_s(pResValue->szLastWrite, pValue);
			else if(!strcmp(pLineTxt, "Accept-Ranges"))
				strcpy_s(pResValue->szContentRange, pValue);
		}	
	}
//
	return pResValue->status_code;
}

BOOL HandleReceive(HANDLE hTo, char *szBuffer, DWORD dwLength) {
	DWORD dwWritenSize = 0x00;
	WriteFile(hTo, szBuffer, dwLength, &dwWritenSize, NULL);
	if(dwWritenSize != dwLength) {
		_LOG_WARN(_T("fatal write error: %08x!"), GetLastError());
		return FALSE;		
	}
	return TRUE;
}

#define	HEAD_BODY(bady_start, srlength, buffer) { \
	char *toke = strstr(buffer, "\r\n\r\n"); \
	if(!toke) return ((DWORD)-1); \
	*(toke + 2) = '\0'; \
	bady_start = toke + 4; \
	srlength = srlength - (bady_start - buffer); \
}

DWORD NHttpUtility::HttpGet(HANDLE hTo, CLiveSocks *pLiveSocks, char *szHost, char *szReqURI) {
	char szBuffer[LOAD_BUFF_SIZE];
	DWORD srlength;
//
	BuildRequest(szBuffer, szHost, szReqURI);
	srlength = pLiveSocks->Send(szBuffer, strlen(szBuffer));
	srlength = pLiveSocks->Receive(szBuffer, LOAD_BUFF_SIZE);
	char *bady_start;
	HEAD_BODY(bady_start, srlength, szBuffer)
//
	DWORD conLength = 0, rcvLength = 0;
	struct ServResponse tResValue;
	if(200 == ParseResponse(&tResValue, szBuffer)) {
		conLength = atoi(tResValue.szContentLength);
		HandleReceive(hTo, bady_start, srlength);
		rcvLength += srlength;

		while(conLength != rcvLength) {
			srlength = pLiveSocks->Receive(szBuffer, LOAD_BUFF_SIZE);
			HandleReceive(hTo, szBuffer, srlength);
			rcvLength += srlength;
		}	
	}
//
	return 0x00;
}


//
DWORD NHttpUtility::HttpDownd(HANDLE hTo, char *szReqURI, ServAddress *pUpdateAddress, struct NetworkConfig *pNetworkConfig) {
	char address[HOSTNAME_LENGTH];
	DWORD dwGetResult = 0;
	//
	if(NLiveSocks::SWD_IsNetworkAlive()) return ((DWORD)-1);
	// if(!lutility::URL_split(address, &iport, req_uri, szReqURI)) return ((DWORD)-1);
	strcon::ustr_ansi(address, pUpdateAddress->sin_addr); 
	//
	// Initialize Winsock
	NLiveSocks::InitialWinsock();
	//
	struct LiveArgu tLiveArgu;
	CONVERT_LIVESOCKS_ARGU(&tLiveArgu, pNetworkConfig);
	CLiveSocks *pLiveSocks = NLiveSocks::Factory(&tLiveArgu);
	if(!pLiveSocks) { 
		NLiveSocks::FinishWinsock();
		return ((DWORD)-1);
	}
//
	pLiveSocks->Connect(address, pUpdateAddress->sin_port);
	if(NHttpUtility::HttpGet(hTo, pLiveSocks, address, szReqURI)) dwGetResult = ((DWORD)-1);
	pLiveSocks->Close();
	//
	// Finish Winsock
	NLiveSocks::FinishWinsock();
	//
	return dwGetResult;
}