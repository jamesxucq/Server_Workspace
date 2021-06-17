/*
 * sdtp_utility.h
 *
 *  Created on: 2010-4-11
 *      Author: Divad
 */

#ifndef SDTPOPT_H_
#define SDTPOPT_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_macro.h"
#include "regis_macro.h"
#include "session.h"
#include "auth_ctrl.h"
#include "value_layer.h"
#include "event.h"

//
int parse_regis_recv(struct regis_request *req_value, char *regis_recv);
/*
<?xml version="1.0" encoding="UTF-8"?>
<register version="1.2.1">
<uid>12345321</uid>
<cache address="123.354.345.345" port="325" />
<access_key>2343ewr544r454rewfa45qwee</access_key>
</register>
 */
/*
SDTP/1.1 200 OK\r\n
Content-Length: xxxxxxxxxx\r\n
Content-Type: text/xml\r\n\r\n  // application/octet-stream
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized\r\n\r\n
 */
#define REGIS_SUCCESS \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value.content_length); \
    sprintf(header, "Content-Type: %s"HEAD_TOKEN, res_value.content_type); \
    FINISH_HDBD_BUFFER

#define BUILD_REGIS_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, REGIS_SUCCESS) \
    _LOG_INFO("build regis send:\n%s", (char *)evdat->head_buffer); \
}
//
int parse_settings_recv(struct regis_request *req_value, char *settings_recv);
/*
<?xml version="1.0" encoding="UTF-8"?>
<client_setting version="1.2.1">
<uid>12345321</uid>
<pool_size>1</pool_size>
</client_setting>
 */
/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxxxxxxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized\r\n\r\n
 */
#define CINFORM_SUCCESS \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value.content_length); \
    sprintf(header, "Content-Type: %s"HEAD_TOKEN, res_value.content_type); \
    FINISH_HDBD_BUFFER

#define BUILD_SETTINGS_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, CINFORM_SUCCESS) \
    _LOG_INFO("build settings send:\n%s", (char *)evdat->head_buffer); \
}
//
int parse_link_recv(struct regis_request *req_value, char *link_recv);
/*
SDTP/1.1 200 OK\r\n\r\n 
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 404 Not Found
 */
#define LINK_SUCCESS \
    FINISH_HEADER_BUFFER

#define BUILD_LINK_SEND(evdatx, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, LINK_SUCCESS) \
    _LOG_INFO("build link send:\n%s", header); \
}

//
int parse_unlink_recv(struct regis_request *req_value, char *unlink_recv);
/*
SDTP/1.1 200 OK\r\n\r\n 
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 403 Forbidden
 */
#define UNLINK_SUCCESS \
    FINISH_HEADER_BUFFER

#define BUILD_UNLINK_SEND(evdat, res_value) { \
    char *header = (char *)evdat->head_buffer; \
    BUILD_STATUS_LINE(header, res_value.status_code, UNLINK_SUCCESS) \
    _LOG_INFO("build unlink send:\n%s", header); \
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

#endif /* SDTPOPT_H_ */
