#pragma once

#include "Session.h"
//#include "ParseConfig.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////    

enum SDTP_METHOD {
	SDTP_NONE = 0,
	/////////////////// register server sdtp method
	SDTP_OPTIONS, // OPT_CSETTING, OPT_LINK, OPT_UNLINK,
	/////////////////// worker server sdtp method
	SDTP_INITIAL,
	SDTP_HEAD,
	SDTP_GET, // INTE_GETANCH, INTE_GET, INTE_GETLIST, INTE_GETSUMS
	SDTP_POST,
	SDTP_PUT, // INTE_PUT, INTE_PUTSUMS
	SDTP_COPY,
	SDTP_MOVE,
	SDTP_DELETE,
	SDTP_FINAL,
	///////////////////
	SDTP_UNUSE
};

enum OPTIONS {
	OPT_NONE = 0,
	//////////////////////////////////////      
	// SDTP_OPTIONS,
	OPT_REGIST,
	OPT_CSETTING,
	OPT_LINK,
	OPT_UNLINK,
	//////////////////////////////////////
	OPT_QUERY,
	////////////////////////////////////// 
	OPT_UNUSE
};

enum SUBENTRY {
	INTE_NONE = 0,
	///////////////////
	// SDTP_GET, 
	INTE_GETANCH,
	INTE_GET,
	INTE_GETLIST,
	INTE_GETSUMS,
	// SDTP_PUT,   
	INTE_PUT,
	INTE_PUTSUMS,
	///////////////////
	INTE_UNUSE
};

////////////////////////////////////////////////////////////////////////////////    

enum CONTROLS {
	CTRL_NONE = 0,
	//////////////////////////////////////
	CTRL_ADD,
	CTRL_CLEAR,
	CTRL_LIST,
	//////////////////////////////////////
	CTRL_STATUS,
	CTRL_SANCHOR,
	CTRL_SLOCKED,
	CTRL_KALIVE,
	///////////////////
	CTRL_UNUSE
};

enum STATUS_CODE {
	// Informational
	STATUS_CONTINUE = 100, // 100 Continue 
	STATUS_SWITCHINT_PROTOCOLS = 101, // 101 Switching Protocols
	STATUS_PROCESSING = 102, // * 102 Processing // WebDAV
	// Successful
	STATUS_OK = 200, // 200 OK
	STATUS_CREATED = 201, // 201 Created
	STATUS_ACCEPTED = 202, // 202 Accepted
	STATUS_NON_AUTHORITATIVE_INFORMATION = 203, // 203 Non - Authoritative Information
	STATUS_NO_CONTENT = 204, // 204 No Content
	STATUS_RESET_CONTENT = 205, // 205 Reset Content
	STATUS_PARTIAL_CONTENT = 206, // 206 Partial Content
	STATUS_MULTI_STATUS = 207, // * 207 Multi - Status // WebDAV
	// Redirection
	STATUS_MULTIPLE_CHOICES = 300, // 300 Multiple Choices
	STATUS_MOVED_PERMANENTLY = 301, // 301 Moved Permanently
	STATUS_FOUND = 302, // 302 Found
	STATUS_SEE_OTHER = 303, // 303 See Other
	STATUS_NOT_MODIFIED = 304, // 304 Not Modified
	STATUS_USE_PROXY = 305, // 305 Use Proxy
	STATUS_TEMPORARY_REDIRECT = 307, // 307 Temporary Redirect
	// Client Error
	STATUS_BAD_REQUEST = 400, // 400 Bad Request
	STATUS_UNAUTHORIZED = 401, // 401 Unauthorized
	STATUS_FORBIDDEN = 403, // 403 Forbidden
	STATUS_NOT_FOUND = 404, // 404 Not Found
	STATUS_METHOD_NOT_ALLOWED = 405, // 405 Method Not Allowed
	STATUS_NOT_ACCEPTABLE = 406, // 406 Not Acceptable
	STATUS_PROXY_AUTHENTICATION_REQUIRED = 407, // 407 Proxy Authentication Required
	STATUS_REQUEST_TIMEOUT = 408, // 408 Request Timeout
	STATUS_CONFLICT = 409, // 409 Conflict
	STATUS_GONE = 410, // 410 Gone
	STATUS_LENGTH_REQUIRED = 411, // 411 Length Required
	STATUS_PRECONDITION_FAILED = 412, // 412 Precondition Failed
	STATUS_REQUEST_ENTITY_TOO_LARGE = 413, // 413 Request Entity Too Large
	STATUS_REQUEST_URI_TOO_LOOG = 414, // 414 Request URI Too Long
	STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416, // 416 Requested Range Not Satisfiable
	STATUS_EXPECTATION_FAILED = 417, // 417 Expectation Failed
	STATUS_TOO_MANY_CONNECTIONS = 421, // * 421 There are too many connections from your internet address
	STATUS_UNPROCESSABLE_ENTITY = 422, // * 422 Unprocessable Entity
	STATUS_LOCKED = 423, // * 423 Locked
	STATUS_FAILED_DEPENDENCY = 424, // * 424 Failed Dependency
	STATUS_UNORDERED_COLLECTION = 425, // * 425 Unordered Collection
	STATUS_UPGRADE_REQUIRED = 426, // * 426 Upgrade Required
	STATUS_RETRY_WITH = 449, // * 449 Retry With
	// Server Error
	STATUS_INTERNAL_SERVER_ERROR = 500, // 500 Internal Server Error
	STATUS_NOT_IMPLEMENTED = 501, // 501 Not Implemented
	STATUS_BAD_GATEWAY = 502, // 502 Bad Gateway
	STATUS_SERVICE_UNAVAILABLE = 503, // 503 Service Unavailable
	STATUS_GATEWAY_TIMEOUT = 504, // 504 Gateway Timeout
	STATUS_HTTP_VERSION_NOT_SUPPORTED = 505, // 505 HTTP Version Not Supported
	STATUS_VARIANT_ALSO_NEGOTIATES = 506, // * 506 Variant Also Negotiates
	STATUS_INSUFFICIENT_STORAGE = 507, // * 507 Insufficient Storage // WebDAV
	STATUS_BANDWIDTH_LIMIT_EXCEEDED = 509, // * 509 Bandwidth Limit Exceeded
	STATUS_NOT_EXTENDED = 510, // * 510 Not Extended
};


///////////////////////////////////////////////////////////////////////
struct RegistRequest {
	// char		szSubtype[MIN_TEXT_LEN];
	char		szResourceIdenti[MAX_PATH];
	char		szUserAgent[MIN_TEXT_LEN];
	char		szLocalIdenti[IDENTI_LENGTH];
	char		szAuthorization[MAX_TEXT_LEN];
};

struct RegistResponse {
	int			status_code;
	//////////////////////////////////////////////
	char		szContentLength[DIGIT_LENGTH];
	char		szContentType[CONTENT_TYPE_LENGTH];
};

struct AuthRequest {
	// TCHAR		szSubtype[MIN_TEXT_LEN];
	char		szResourceIdenti[MAX_PATH];
	char		szAuthorization[MAX_TEXT_LEN];
};

struct AuthResponse {
	int			status_code;
	//////////////////////////////////////////////
	char		szContentLength[DIGIT_LENGTH];
	char		szContentType[CONTENT_TYPE_LENGTH];
};

struct ServRequest {
	// TCHAR		szSubtype[MIN_TEXT_LEN];
	char		szResourceIdenti[MAX_PATH];
	char		szLastWrite[TIME_LENGTH];
	char		szAuthorization[MAX_TEXT_LEN];
	char		szParams[MAX_TEXT_LEN];
	char		szContentRange[MIN_TEXT_LEN];
	char		szContentLength[DIGIT_LENGTH];
	char		szContentType[CONTENT_TYPE_LENGTH];
	char		szSessionID[SESSION_LENGTH];
};

struct ServResponse {
	int			status_code;
	//////////////////////////////////////////////
	char		szLastWrite[TIME_LENGTH];
	char		szParams[MIN_TEXT_LEN];
	char		szContentRange[MIN_TEXT_LEN];
	char		szContentLength[DIGIT_LENGTH];
	char		szContentType[CONTENT_TYPE_LENGTH];
	char		szSessionID[SESSION_LENGTH];
};

namespace NSDTProtocol {
	////////////////////////////////////////////////////////////
	DWORD GetHeaderValue(char **head_buffer, char *szReceiveTxt);
	DWORD GetBodyValue(char **body_buffer, char *szReceiveTxt, int iLength);

	/////////////////////////////////////////////////////////////////////
	DWORD BuildRegistSend(struct SessionBuffer *psbuffer, struct RegistRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized
	DWORD ParseRegistRecv( struct RegistResponse *ResponseValue, char *regist_recv);

	DWORD BuildSettingsSend(struct SessionBuffer *psbuffer, struct RegistRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized
	DWORD ParseSettingsRecv( struct RegistResponse *ResponseValue, char *settings_recv);

	DWORD BuildLinkSend(struct SessionBuffer *psbuffer, struct RegistRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found
	DWORD ParseLinkRecv( struct RegistResponse *ResponseValue, char *link_recv);

	DWORD BuildUnlinkSend(struct SessionBuffer *psbuffer, struct RegistRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Forbidden
	DWORD ParseUnlinkRecv( struct RegistResponse *ResponseValue, char *unlink_recv);

	/////////////////////////////////////////////////////////////////////
	DWORD BuildQuerySend(struct SessionBuffer *psbuffer, struct AuthRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found
	DWORD ParseQueryRecv( struct AuthResponse *ResponseValue, char *query_recv);

	/////////////////////////////////////////////////////////////////////
	DWORD BuildInitialSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found; 5 Wait
	DWORD ParseInitialRecv( struct ServResponse *ResponseValue, char *initial_recv);

	DWORD BuildGetAnchorSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseGetAnchorRecv( struct ServResponse *ResponseValue, char *getlist_recv);

	DWORD BuildGetListSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseGetListRecv( struct ServResponse *ResponseValue, char *getlist_recv);

	DWORD BuildGetFileSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseGetFileRecv( struct ServResponse *ResponseValue, char *get_recv);

	DWORD BuildGetCsumSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseGetCsumRecv( struct ServResponse *ResponseValue, char *getsum_recv);

	DWORD BuildPostSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParsePostRecv( struct ServResponse *ResponseValue, char *post_recv);

	DWORD BuildPutFileSend(struct SessionBuffer *psbuffer,  ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParsePutFileRecv( struct ServResponse *ResponseValue, char *put_recv);

	DWORD BuildPutCsumSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParsePutCsumRecv( struct ServResponse *ResponseValue, char *putsum_recv);

	DWORD BuildMoveSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseMoveRecv( struct ServResponse *ResponseValue, char *move_recv);

	DWORD BuildDeleteSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseDeleteRecv( struct ServResponse *ResponseValue, char *delete_recv);

	DWORD BuildFinalSend(struct SessionBuffer *psbuffer, struct ServRequest *RequestValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseFinalSuccessRecv(struct ServResponse *ResponseValue, char *final_recv);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseFinalEndRecv( struct ServResponse *ResponseValue, char *final_recv);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseFinalFinishRecv( struct ServResponse *ResponseValue, char *final_recv);
	/////////////////////////////////////////////////////////////////////
};
