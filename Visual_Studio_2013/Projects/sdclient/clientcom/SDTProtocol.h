#pragma once

#include "Session.h"


enum STATUS_CODE {
	// Informational
	CONTINUE = 100, // 100 Continue
	SWITCHINT_PROTOCOLS = 101, // 101 Switching Protocols
	PROCESSING = 102, // * 102 Processing // WebDAV
	// Successful
	OK = 200, // 200 OK
	CREATED = 201, // 201 Created
	ACCEPTED = 202, // 202 Accepted
	NON_AUTHORITATIVE_INFORMATION = 203, // 203 Non - Authoritative Information
	NO_CONTENT = 204, // 204 No Content
	RESET_CONTENT = 205, // 205 Reset Content
	PARTIAL_CONTENT = 206, // 206 Partial Content
	MULTI_STATUS = 207, // * 207 Multi - Status // WebDAV
	// Redirection
	MULTIPLE_CHOICES = 300, // 300 Multiple Choices
	MOVED_PERMANENTLY = 301, // 301 Moved Permanently
	FOUND = 302, // 302 Found
	SEE_OTHER = 303, // 303 See Other
	NOT_MODIFIED = 304, // 304 Not Modified
	USE_PROXY = 305, // 305 Use Proxy
	TEMPORARY_REDIRECT = 307, // 307 Temporary Redirect
	// Client Error
	BAD_REQUEST = 400, // 400 Bad Request
	UNAUTHORIZED = 401, // 401 Unauthorized
	FORBIDDEN = 403, // 403 Forbidden
	NOT_FOUND = 404, // 404 Not Found
	METHOD_NOT_ALLOWED = 405, // 405 Method Not Allowed
	NOT_ACCEPTABLE = 406, // 406 Not Acceptable
	PROXY_AUTHENTICATION_REQUIRED = 407, // 407 Proxy Authentication Required
	REQUEST_TIMEOUT = 408, // 408 Request Timeout
	CONFLICT = 409, // 409 Conflict
	GONE = 410, // 410 Gone
	LENGTH_REQUIRED = 411, // 411 Length Required
	PRECONDITION_FAILED = 412, // 412 Precondition Failed
	REQUEST_ENTITY_TOO_LARGE = 413, // 413 Request Entity Too Large
	REQUEST_URI_TOO_LOOG = 414, // 414 Request URI Too Long
	REQUESTED_RANGE_NOT_SATISFIABLE = 416, // 416 Requested Range Not Satisfiable
	EXPECTATION_FAILED = 417, // 417 Expectation Failed
	TOO_MANY_CONNECTIONS = 421, // * 421 There are too many connections from your internet address
	UNPROCESSABLE_ENTITY = 422, // * 422 Unprocessable Entity
	LOCKED = 423, // * 423 Locked
	FAILED_DEPENDENCY = 424, // * 424 Failed Dependency
	UNORDERED_COLLECTION = 425, // * 425 Unordered Collection
	UPGRADE_REQUIRED = 426, // * 426 Upgrade Required
	RETRY_WITH = 449, // * 449 Retry With
	// Server Error
	INTERNAL_SERVER_ERROR = 500, // 500 Internal Server Error
	NOT_IMPLEMENTED = 501, // 501 Not Implemented
	BAD_GATEWAY = 502, // 502 Bad Gateway
	SERVICE_UNAVAILABLE = 503, // 503 Service Unavailable
	GATEWAY_TIMEOUT = 504, // 504 Gateway Timeout
	HTTP_VERSION_NOT_SUPPORTED = 505, // 505 HTTP Version Not Supported
	VARIANT_ALSO_NEGOTIATES = 506, // * 506 Variant Also Negotiates
	INSUFFICIENT_STORAGE = 507, // * 507 Insufficient Storage // WebDAV
	BANDWIDTH_LIMIT_EXCEEDED = 509, // * 509 Bandwidth Limit Exceeded
	NOT_EXTENDED = 510, // * 510 Not Extended
};


//
struct RegistRequest {
	char		szResIdentity[MAX_PATH];
	char		szUserAgent[USERAGENT_LENGTH];
	char		szLocalIdenti[IDENTI_LENGTH];
	char		szAuthorize[MIN_TEXT_LEN];
};

struct RegistResponse {
	int			status_code;
	//
	char		szContentLength[DIGIT_LENGTH];
	char		szContentType[CONTENT_TYPE_LENGTH];
};

struct AuthRequest {
	char		szResIdentity[MAX_PATH];
	char		szAuthorize[MIN_TEXT_LEN];
};

struct AuthResponse {
	int			status_code;
	//
	char		szContentLength[DIGIT_LENGTH];
	char		szContentType[CONTENT_TYPE_LENGTH];
};

struct ServRequest {
	char		szResIdentity[MAX_PATH];
	char		szLastWrite[TIME_LENGTH];
	char		szParams[MAX_TEXT_LEN];
	char		szContentRange[MIN_TEXT_LEN];
	char		szContentLength[MIN_TEXT_LEN];
	char		szContentType[CONTENT_TYPE_LENGTH];
	char		szCacheVerify[CACHE_VERIFY_LENGTH];
	char		szSessionID[SESSION_LENGTH];
};

struct ServResponse {
	int			status_code;
	//
	char		szLastWrite[TIME_LENGTH];
	char		szAttach[MIN_TEXT_LEN];
	char		szContentRange[MIN_TEXT_LEN];
	char		szContentLength[MIN_TEXT_LEN];
	char		szContentType[CONTENT_TYPE_LENGTH];
	char		szSessionID[SESSION_LENGTH];
};

namespace NSDTProtocol
{
	//
	DWORD GetHeaderValue(char **head_buffer, char *szReceiveTxt);
	DWORD GetBodyValue(char **body_buffer, char *szReceiveTxt, int iLeng);

	//
	DWORD BuildRegistSend(struct SeionBuffer *sbuffer, struct RegistRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized
	DWORD ParseRegistRecv( struct RegistResponse *pResValue, char *regist_recv);

	DWORD BuildSettingsSend(struct SeionBuffer *sbuffer, struct RegistRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized
	DWORD ParseSettingsRecv( struct RegistResponse *pResValue, char *settings_recv);

	DWORD BuildLinkSend(struct SeionBuffer *sbuffer, struct RegistRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found
	DWORD ParseLinkRecv( struct RegistResponse *pResValue, char *link_recv);

	DWORD BuildUnlinkSend(struct SeionBuffer *sbuffer, struct RegistRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Forbidden
	DWORD ParseUnlinkRecv( struct RegistResponse *pResValue, char *unlink_recv);

	//
	DWORD BuildQuerySend(struct SeionBuffer *sbuffer, struct AuthRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found
	DWORD ParseQueryRecv( struct AuthResponse *pResValue, char *query_recv);

	//
	DWORD BuildInitialSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found; 5 Wait
	DWORD ParseInitialRecv( struct ServResponse *pResValue, char *initial_recv);

	DWORD BuildKeepAliveSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Found; 5 Wait
	DWORD ParseKeepAliveRecv( struct ServResponse *pResValue, char *kalive_recv);

	DWORD BuildHeadListSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseHeadListRecv( struct ServResponse *pResValue, char *headlist_recv);

	DWORD BuildHeadFileSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseHeadFileRecv( struct ServResponse *pResValue, char *head_recv);

	DWORD BuildGetAnchorSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseGetAnchorRecv( struct ServResponse *pResValue, char *getlist_recv);

	DWORD BuildGetListSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseGetListRecv( struct ServResponse *pResValue, char *getlist_recv);

	DWORD BuildGetFileSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseGetFileRecv( struct ServResponse *pResValue, char *get_recv);

	DWORD BuildGetChksSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParseGetChksRecv( struct ServResponse *pResValue, char *getsum_recv);

	DWORD BuildPostSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParsePostRecv( struct ServResponse *pResValue, char *post_recv);

	DWORD BuildPutFileSend(struct SeionBuffer *sbuffer,  ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParsePutFileRecv( struct ServResponse *pResValue, char *put_recv);

	DWORD BuildPutDireSend(struct SeionBuffer *sbuffer,  ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParsePutDireRecv( struct ServResponse *pResValue, char *put_recv);

	DWORD BuildPutChksSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error
	DWORD ParsePutChksRecv( struct ServResponse *pResValue, char *putsum_recv);

	DWORD BuildMoveSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseMoveRecv( struct ServResponse *pResValue, char *move_recv);

	DWORD BuildCopySend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseCopyRecv( struct ServResponse *pResValue, char *move_recv);

	DWORD BuildDeleteSend(struct SeionBuffer *sbuffer, struct ServRequest *pReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseDeleteRecv( struct ServResponse *pResValue, char *delete_recv);

	DWORD BuildFinalSend(struct SeionBuffer *sbuffer, struct ServRequest *tReqValue);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseFinalSuccessRecv(struct ServResponse *pResValue, char *final_recv);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseFinalEndRecv( struct ServResponse *pResValue, char *final_recv);
	// -1 error; 0 succ; 1 Bad Request; 3 Not Authorized; 5 Internal Server Error; 7 Expectation Failed
	DWORD ParseFinalFinishRecv( struct ServResponse *pResValue, char *final_recv);
	//
};
