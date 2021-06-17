/* sdtp_utility.h
 * Created on: 2010-4-11
 * Author: David
 */

#ifndef SDTPUTILITY_H_
#define SDTPUTILITY_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_macro.h"
#include "author_macro.h"
#include "session.h"
#include "value_layer.h"
#include "event.h"

//
int parse_query_recv(struct cache_request *req_value, char *query_recv);
/*
<?xml version="1.0" encoding="UTF-8"?>
<query version="1.2.1">
<uid>12345321</uid> 
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
#define QUERY_SUCCESS \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value.content_length); \
    sprintf(header, "Content-Type: %s"HEAD_TOKEN, res_value.content_type); \
    FINISH_HDBD_BUFFER

#define BUILD_QUERY_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, QUERY_SUCCESS) \
    _LOG_INFO("build query send:\n%s", (char *)evdat->head_buffer); \
}

//
int parse_addi_recv(struct cache_request *req_value, char *addi_recv);
/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
 */
#define ADD_SUCCESS \
    sprintf(header, "Execute: %s"HEAD_TOKEN, res_value.execute_status); \
    FINISH_HEADER_BUFFER

#define BUILD_ADDI_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, ADD_SUCCESS) \
    _LOG_INFO("build add send:\n%s", header); \
}

//
int parse_clear_recv(struct cache_request *req_value, char *clear_recv);
/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
// SDTP/1.1 409 Conflict 
 */
#define CLEAR_SUCCESS \
    sprintf(header, "Execute: %s"HEAD_TOKEN, res_value.execute_status); \
    FINISH_HEADER_BUFFER

#define BUILD_CLEAR_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, CLEAR_SUCCESS) \
    _LOG_INFO("build clear send:\n%s", header); \
}

//
int parse_list_recv(struct cache_request *req_value, char *list_recv);
/*
<?xml version="1.0" encoding="UTF-8"?>
<list version="1.2.1">
<server weight="5">xxxxxx</server>
<server weight="5">xxxxxxxxx</server>
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
#define LIST_SUCCESS \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value.content_length); \
    sprintf(header, "Content-Type: %s"HEAD_TOKEN, res_value.content_type); \
    FINISH_HDBD_BUFFER

#define BUILD_LIST_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, LIST_SUCCESS) \
    _LOG_INFO("build list send:\n%s", (char *)evdat->head_buffer); \
}

//
int parse_status_recv(struct cache_request *req_value, char *status_recv);
/*
<?xml version="1.0" encoding="UTF-8"?>
<cache_status version="1.2.1">
<cached_anchor>xxxxxx</cached_anchor>
<location>xxxxxxxxx</location>
</cache_status>
 */
/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxx\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
 */
#define STATUS_SUCCESS \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value.content_length); \
    sprintf(header, "Content-Type: %s"HEAD_TOKEN, res_value.content_type); \
    FINISH_HDBD_BUFFER

#define BUILD_STATUS_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, STATUS_SUCCESS) \
    _LOG_INFO("build status send:\n%s", (char *)evdat->head_buffer); \
}

//
int parse_set_anchor_recv(struct cache_request *req_value, char *anchor_recv);
/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
// SDTP/1.1 409 Conflict
 */
#define SANCHOR_SUCCESS \
    sprintf(header, "Execute: %s"HEAD_TOKEN, res_value.execute_status); \
    FINISH_HEADER_BUFFER

#define BUILD_SET_ANCHOR_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, SANCHOR_SUCCESS) \
    _LOG_INFO("build set anchor send:\n%s", header); \
}

//
int parse_set_locked_recv(struct cache_request *req_value, char *locked_recv);
/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
// SDTP/1.1 409 Conflict
 */
#define SLOCKED_SUCCESS \
    sprintf(header, "Execute: %s"HEAD_TOKEN, res_value.execute_status); \
    FINISH_HEADER_BUFFER

#define BUILD_SET_LOCKED_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, SLOCKED_SUCCESS) \
    _LOG_INFO("build set locked send:\n%s", header); \
}
//
int parse_keep_alive_recv(struct cache_request *req_value, char *kalive_recv);
/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
 */
#define KALIVE_SUCCESS \
    sprintf(header, "Execute: %s"HEAD_TOKEN, res_value.execute_status); \
    FINISH_HEADER_BUFFER

#define BUILD_KEEP_ALIVE_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, KALIVE_SUCCESS) \
    _LOG_INFO("build keep alive send:\n%s", header); \
}

//
#define BUILD_UNUSE_SEND(evdat) { \
    char *header = (char *)evdat->head_buffer; \
    strcpy(header, "SDTP/1.1 501 Not Implemented"HEAD_TOKEN); \
    FINISH_HEADER_BUFFER \
    _LOG_INFO("build unuse send:\n%s", header); \
}

#ifdef	__cplusplus
}
#endif

#endif /* SDTPUTILITY_H_ */
