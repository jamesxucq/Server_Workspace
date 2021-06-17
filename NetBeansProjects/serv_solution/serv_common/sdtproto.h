/*
// Informational
100 Continue 
101 Switching Protocols 
 * 102 Processing // WebDAV
// Successful
200 OK 
201 Created 
202 Accepted 
203 Non-Authoritative Information 
204 No Content 
205 Reset Content 
206 Partial Content
 * 207 Multi-Status // WebDAV
// Redirection
300 Multiple Choices 
301 Moved Permanently 
302 Found 
303 See Other 
304 Not Modified 
305 Use Proxy 
307 Temporary Redirect 
// Client Error
400 Bad Request 
401 Unauthorized 
403 Forbidden 
404 Not Found 
405 Method Not Allowed 
406 Not Acceptable 
407 Proxy Authentication Required 
408 Request Timeout 
409 Conflict 
410 Gone 
411 Length Required 
412 Precondition Failed 
413 Request Entity Too Large 
414 Request URI Too Long URI
416 Requested Range Not Satisfiable 
417 Expectation Failed
 * 421 There are too many connections from your internet address
 * 422 Unprocessable Entity
 * 423 Locked
 * 424 Failed Dependency
 * 425 Unordered Collection
 * 426 Upgrade Required
 * 449 Retry With
// Server Error
500 Internal Server Error 
501 Not Implemented 
502 Bad Gateway
503 Service Unavailable 
504 Gateway Timeout 
505 HTTP Version Not Supported 
 * 506 Variant Also Negotiates
 * 507 Insufficient Storage // WebDAV
 * 509 Bandwidth Limit Exceeded
 * 510 Not Extended
 */

#ifndef SDTPPROTO_H
#define	SDTPPROTO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"
    //    

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

    // register server sdtp method

    enum REGIS_METHOD {
        RGTP_INVA,
        RGTP_OPTIONS // OPT_SETTINGS, OPT_LINK, OPT_UNLINK,
    };

    enum REGIS_OPTIONS {
        ROP_INVA,
        ROP_REGIS,
        ROP_SETTINGS,
        ROP_LINK,
        ROP_UNLINK,
    };
    //

    enum CACHE_METHOD {
        CATP_INVA,
        CATP_OPTIONS, // OPT_SETTINGS, OPT_LINK, OPT_UNLINK,
        INTE_CONTROL,
        CATP_KALIVE
    };

    enum CACHE_OPTIONS {
        COP_INVA,
        COP_QUERY,
        COP_STATUS,
    };

    enum CACHE_CONTROL {
        CTRL_INVA,
        CTRL_SANCHOR,
        CTRL_SLOCKED,
        // 
        CTRL_ADD,
        CTRL_CLEAR,
        CTRL_LIST
    };

    // server server sdtp method
    // SDTP_ATTRIB, // Locked Shared ReadOnly

    enum SERVER_METHOD {
        SVTP_INVA,
        SVTP_INITIAL,
        SVTP_KALIVE,
        SVTP_HEAD,
        SVTP_GET, // ENTRY_ANCHOR, ENTRY_FILE, ENTRY_LIST, TYPE_CHKS
        SVTP_POST,
        SVTP_PUT, // TYPE_PUTFILE, TYPE_PUTSUMS
        SVTP_COPY,
        SVTP_MOVE,
        SVTP_DELETE,
        SVTP_FINAL // end success finish
    };

    //

    enum SUBENTRY {
        ENTRY_INVA,
        ENTRY_CHKS,
        ENTRY_FILE,
        ENTRY_ANCHOR,
        ENTRY_LIST
    };

    //
#define SDTP_EVDAT_HEAD(EVDAT, EVD) { \
    GET_HEADER_VALUE(EVDAT, EVD) \
/*_LOG_INFO("recv hdbd (EVDAT)->hlen:%d\n", (EVDAT)->hlen); */ \
}

#define SDTP_EVDAT_HDBD(EVDAT, EVD) { \
    GET_HEADER_VALUE(EVDAT, EVD) \
/*_LOG_INFO("recv hdbd (EVDAT)->hlen:%d\n", (EVDAT)->hlen); */ \
    GET_BODY_VALUE(EVDAT, EVD) \
/*_LOG_INFO("recv hdbd (EVDAT)->blen:%d\n", (EVDAT)->blen); */ \
}

#define SDTP_EVDAT_BODY(EVDAT, EVD) { \
    (EVDAT)->body_buffer = (EVD)->buffer; \
    (EVDAT)->blen = (EVD)->rsleng; \
/*_LOG_INFO("recv body (EVDAT)->blen:%d\n", (EVDAT)->blen); */ \
}

#define SDTP_EVDAT_INIT(EVDAT, EVD) { \
    (EVDAT)->head_buffer = (EVD)->buffer; \
    (EVDAT)->hlen = 0; \
    (EVDAT)->body_buffer = (EVD)->buffer; \
    (EVDAT)->blen = EVD_SOCKET_BUFF; \
/*_LOG_INFO("send start (EVD)->rsleng:%d\n", (EVD)->rsleng); */ \
}

#define SDTP_EVDAT_END(EVDAT, EVD) { \
    (EVD)->sleng = 0; \
    (EVD)->rsleng = (EVDAT)->hlen + (EVDAT)->blen; \
/* _LOG_INFO("send end (EVDAT)->hlen:%d|(EVDAT)->blen:%d\n", (EVDAT)->hlen, (EVDAT)->blen); */ \
/* _LOG_INFO("send end (EVD)->rsleng:%d\n", (EVD)->rsleng); */ \
}

#define SDTP_REPLY_INIT(EVDAT, EVD) { \
    (EVDAT)->head_buffer = (EVD)->buffer; \
    (EVDAT)->hlen = 0; \
    (EVDAT)->body_buffer = (EVD)->buffer; \
    (EVDAT)->blen = EVD_SOCKET_BUFF; \
/*_LOG_INFO("send start (EVD)->rsleng:%d\n", (EVD)->rsleng); */ \
}

#define SDTP_REPLY_END(EVDAT, EVD) { \
    (EVD)->rsleng = (EVDAT)->hlen + (EVDAT)->blen; \
/*_LOG_INFO("send end (EVD)->rsleng:%d\n", (EVD)->rsleng); */ \
}

    //
#define GET_HEADER_VALUE(EVDAT, EVD) { \
    char *header_end = strstr((char *)(EVD)->buffer, "\r\n\r\n"); \
    if (!header_end) { \
        _LOG_FATAL("fatal don`t find header!"); \
        log_binary_code(EVD->buffer, EVD->rsleng); \
        return -1; \
    } \
    header_end += 2; *header_end++ = '\0'; *header_end++ = '\0'; \
    (EVDAT)->head_buffer = (EVD)->buffer; \
    (EVDAT)->hlen = header_end - (char *)(EVD)->buffer; \
}

#define GET_BODY_VALUE(EVDAT, EVD){ \
    (EVDAT)->body_buffer = (EVD)->buffer + (EVDAT)->hlen; \
    (EVDAT)->blen = (EVD)->rsleng - (EVDAT)->hlen; \
}

    //
    // enum REGIS_METHOD regis_method(char *header, int len);
    enum REGIS_METHOD regis_command(enum REGIS_OPTIONS *command, char *header, int len);
    //
    // enum CACHE_METHOD cache_method(char *header, int len);
    enum CACHE_METHOD cache_command(int *command, char *header, int len);
    //
    enum SERVER_METHOD proto_method(char *header, int len);
    //
    int get_header_value(char **head_buffer, char *receive_txt);
    int get_body_value(char **body_buffer, char *receive_txt, int len);

    //
#define HEAD_TOKEN  "\r\n\r\n"
#define LINE_TOKEN  "\r\n"
#define FIELD_TOKEN  " "
#define SDTP_VERSION  "SDTP/1.1 "   

#define ONLY_HEADER_SECTION		0
    //
#define FINISH_HEADER_BUFFER \
    evdat->hlen = strlen(header); \
    evdat->blen = ONLY_HEADER_SECTION;
    //
#define FINISH_HDBD_BUFFER \
    evdat->hlen = strlen(header); \
    evdat->body_buffer = (BYTE *)(header + evdat->hlen); \
    evdat->blen = EVD_SOCKET_BUFF - evdat->hlen;
    //
#define BUILD_STATUS_LINE(STATUS_LINE, STATUS_CODE, SUCCESS_HEADER) \
    if(OK == STATUS_CODE) { \
        STATUS_LINE = lscpy(STATUS_LINE, SDTP_VERSION"200 OK"LINE_TOKEN); \
        SUCCESS_HEADER \
    } else { \
        switch (STATUS_CODE) { \
            case PROCESSING: \
                strcpy(STATUS_LINE, SDTP_VERSION"102 Processing"HEAD_TOKEN); \
                break; \
            case NO_CONTENT: \
                strcpy(STATUS_LINE, SDTP_VERSION"204 No Content"HEAD_TOKEN); \
                break; \
            case BAD_REQUEST: \
                strcpy(STATUS_LINE, SDTP_VERSION"400 Bad Request"HEAD_TOKEN); \
                break; \
            case UNAUTHORIZED: \
                strcpy(STATUS_LINE, SDTP_VERSION"401 Unauthorized"HEAD_TOKEN); \
                break; \
            case NOT_FOUND: \
               strcpy(STATUS_LINE, SDTP_VERSION"404 Not Found"HEAD_TOKEN); \
                break; \
            case CONFLICT: \
                strcpy(STATUS_LINE, SDTP_VERSION"409 Conflict"HEAD_TOKEN); \
                break; \
            case LOCKED: \
                strcpy(STATUS_LINE, SDTP_VERSION"423 Locked"HEAD_TOKEN); \
                break; \
            case INTERNAL_SERVER_ERROR: \
                strcpy(STATUS_LINE, SDTP_VERSION"500 Internal Server Error"HEAD_TOKEN); \
                break; \
            case VARIANT_ALSO_NEGOTIATES: \
                strcpy(STATUS_LINE, SDTP_VERSION"506 Variant Also Negotiates"HEAD_TOKEN); \
                break; \
            default: break; \
        } \
    FINISH_HEADER_BUFFER \
}

#ifdef	__cplusplus
}
#endif

#endif	/* SDTPPROTO_H */

