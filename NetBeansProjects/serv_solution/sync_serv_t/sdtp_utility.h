#ifndef SDTP_UTILITY_H_
#define SDTP_UTILITY_H_

// #ifdef	__cplusplus
// extern "C" {
// #endif

#include "common_macro.h"
#include "server_macro.h"
#include "value_layer.h"
#include "event.h"
#include "session.h"

//
int parse_initial_recv(struct serv_request *req_value, char *initial_recv);
/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 404 Not Found\r\n\r\n
//SDTP/1.1 600 Wait\r\n\r\n
 */
#define INITIAL_SUCCESS \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value.seion_id); \
    FINISH_HDBD_BUFFER

#define BUILD_INITIAL_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, INITIAL_SUCCESS) \
    /* _LOG_INFO("build initial send:\n%s", header); */ \
}
//
int parse_kalive_recv(struct serv_request *req_value, char *kalive_recv);
/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 404 Not Found\r\n\r\n
 */
#define KALIVE_SUCCESS \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value.seion_id); \
    FINISH_HEADER_BUFFER

#define BUILD_KALIVE_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, KALIVE_SUCCESS) \
    /* _LOG_INFO("build initial send:\n%s", header); // disable by james 20130507 */ \
}
//
int parse_head_recv(struct serv_request *req_value, char *head_recv);

void build_head_simple_send(struct evdata *evdat, struct serv_response *res_value);
void build_head_complex_send(struct evdata *evdat, struct serv_response *res_value);
extern void(*build_head_send[])(struct evdata*, struct serv_response*);

//
int parse_get_recv(struct serv_request *req_value, char *get_recv);

void build_get_simple_send(struct evdata *evdat, struct serv_response *res_value);
void build_get_complex_send(struct evdata *evdat, struct serv_response *res_value);
extern void(*build_get_send[])(struct evdata*, struct serv_response*);

//
int parse_post_recv(struct serv_request *req_value, char *post_recv);

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
 */
#define POST_SUCCESS \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value.seion_id); \
    FINISH_HEADER_BUFFER

#define BUILD_POST_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, POST_SUCCESS) \
    _LOG_INFO("build post send:\n%s", header); \
}

//
int parse_put_recv(struct serv_request *req_value, char *put_recv);

void build_put_simple_send(struct evdata *evdat, struct serv_response *res_value);
void build_put_complex_send(struct evdata *evdat, struct serv_response *res_value);
extern void(*build_put_send[])(struct evdata*, struct serv_response*);

//
int parse_copy_recv(struct serv_request *req_value, char *copy_recv);

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 : Expectation Failed
 */
#define COPY_SUCCESS \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value.seion_id); \
    FINISH_HEADER_BUFFER

#define BUILD_COPY_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, COPY_SUCCESS) \
    _LOG_INFO("build copy send:\n%s", header); \
}

//
int parse_move_recv(struct serv_request *req_value, char *move_recv);

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 : Expectation Failed
 */
#define MOVE_SUCCESS \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value.seion_id); \
    FINISH_HEADER_BUFFER

#define BUILD_MOVE_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, MOVE_SUCCESS) \
    _LOG_INFO("build move send:\n%s", header); \
}

//
int parse_delete_recv(struct serv_request *req_value, char *delete_recv);

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 : Expectation Failed
 */
#define DELETE_SUCCESS \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value.seion_id);\
    FINISH_HEADER_BUFFER

#define BUILD_DELETE_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, DELETE_SUCCESS) \
    _LOG_INFO("build delete send:\n%s", header); \
}

//
int parse_final_recv(struct serv_request *req_value, char *final_recv);
/*
SDTP/1.1 200 OK\r\n\r\n
Attach: anchor=xxxxx\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 :  Expectation Failed
 */

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
//417 :  Expectation Failed
 */
#define FINAL_SUCCESS(final_type) \
    char *line_txt = header; \
    if (FINAL_TYPE_SUCCESS == final_type) { \
        line_txt = lsprif(line_txt, "Attach: %s"LINE_TOKEN, res_value.attach); \
    } \
    sprintf(line_txt, "Session: %s"HEAD_TOKEN, res_value.seion_id); \
    FINISH_HEADER_BUFFER

#define BUILD_FINAL_SEND(evdat, res_value, final_type) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, FINAL_SUCCESS(final_type)) \
    /* _LOG_INFO("build final send:\n%s", (char *)evdat->head_buffer); // disable by james 20130507 */ \
}

//
#define BUILD_UNUSE_SEND(evdat) { \
    char *header = (char *)evdat->head_buffer; \
    strcpy(header, "SDTP/1.1 501 Not Implemented"HEAD_TOKEN); \
    FINISH_HEADER_BUFFER \
    _LOG_INFO("build unuse send:\n%s", header); \
}
//

// #ifdef	__cplusplus
// }
// #endif

#endif /* SDTP_UTILITY_H_ */
