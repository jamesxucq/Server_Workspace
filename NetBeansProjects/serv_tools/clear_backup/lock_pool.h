/* 
 * File:   locked.h
 * Author: Administrator
 *
 * Created on 2012年9月12日, 上午9:31
 */

#ifndef LOCKED_H
#define	LOCKED_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "macro.h"
#include "auth_ctrl.h"

    int valid_cache_locked(char *validation, struct auth_value *avalue);
    int unlocked_cache(struct auth_value *avalue, char *validation);
    ////////////////////////////////////////////////////////////////////////////////    

    struct socks_request {
        char resource_identi[MAX_PATH * 3];
        char authorization[AUTHOR_LENGTH];
        char validation[VALID_LENGTH];
        char params[MIN_TEXT_LEN];
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
    };

    struct socks_response {
        int status_code;
        ////////////////////////////////////////////////////////////////////////////////        
        char params[MIN_TEXT_LEN];
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
        char execute_status[EXECUTE_LENGTH];
    };

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

#define CONTENT_TYPE_UNKNOWN            0x0000
#define CONTENT_TYPE_XML                0x0001
#define CONTENT_TYPE_OCTET              0x0002
#define CONTENT_TYPE_PLAIN              0x0003    

#ifdef	__cplusplus
}
#endif

#endif	/* LOCKED_H */

