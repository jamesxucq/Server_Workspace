#pragma once

#include "Session.h"
#include "ParseConf.h"

#define EXECU_TYPE_UNKNOWN			-1
#define EXECU_TYPE_FAILED			0x0000
#define EXECU_TYPE_SUCCESS			0x0001
#define EXECU_TYPE_EXCEPTION		0x0003
#define EXECU_TYPE_CONFLICT			0x0005
//
enum STATUS_CODE {
	EXCEPTION = 0x00,
	OK = 200, // 200 OK(Successful)
	MOVED_PERMANENTLY = 301, // 301 Moved Permanently
	BAD_REQUEST = 400, // 400 Bad Request
	NOT_AUTHORIZED = 401, // 401 Not Authorized
	FORBIDDEN = 403, // 403 Forbidden
	NOT_FOUND = 404, // 404 Not Found
	CONFLICT = 409, // 409 Conflict
	INTERNAL_SERVER = 500, // 500 Internal Server Error
	EXPECTATION_FAILED = 506, // 506 Expectation Failed
};

//
namespace NSDTProtocol
{
	////////////////////////////////////////////////////////////
	DWORD GetHeaderValue(char **head_buffer, char *szReceiveTxt);
	DWORD GetBodyValue(char **body_buffer, char *szReceiveTxt, int iLength);//
	/////////////////////////////////////////////////////////////////////
	DWORD BuildAddSend(struct SessionBuffer *psbuffer, DWORD dwAddType, char *szAddress, char *szAuthsum);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found
	DWORD ParseAddRecv(DWORD *dwAddedStatus, char *szAddRecvTxt);
	/////////////////////////////////////////////////////////////////////
	DWORD BuildClearSend(struct SessionBuffer *psbuffer, DWORD dwClearType, char *szAdminAddress, char *szAuthsum);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found
	DWORD ParseClearRecv(DWORD *dwClearedStatus, char *szClearRecvTxt);
	/////////////////////////////////////////////////////////////////////
	DWORD BuildListSend(struct SessionBuffer *psbuffer, DWORD dwListType, char *szAuthsum);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found
	DWORD ParseListRecv(DWORD *dwContentType, DWORD *dwContentLength, DWORD *dwClearedStatus, char *szListRecvTxt);
};


