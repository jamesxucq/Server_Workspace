/*
 * File: value_layer.h
 * Author: Divad
 *
 * Created on 2011
 */

#ifndef VALUE_LAYER_H
#define VALUE_LAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_macro.h"
#include "session.h"

    //

    struct regis_request {
        char res_identity[MAX_PATH * 3];
        char user_agent[MIN_TEXT_LEN];
        char from[IDENTI_LENGTH];
        char authorize[AUTHOR_LENGTH];
    };

    struct regis_response {
        enum STATUS_CODE status_code;
        //
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
    };

    //
#define REGIS_REQUEST(INTE_VALUE, REQ_VALUE) { \
    strcpy(INTE_VALUE->user_name, REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->client_id, REQ_VALUE.from); \
    strcpy(INTE_VALUE->user_agent, REQ_VALUE.user_agent); \
    strcpy(INTE_VALUE->authorize, REQ_VALUE.authorize); \
}

    // #define CONTENT_TYPE_UNKNOWN            0x0000
    // #define CONTENT_TYPE_XML                0x0001
    // #define CONTENT_TYPE_OCTET              0x0002
    // #define CONTENT_TYPE_PLAIN              0x0003
    extern const char *content_table[];

#define REGIS_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        sprintf(RES_VALUE.content_length, "%lu", INTE_VALUE->content_length); \
        strcpy(RES_VALUE.content_type, content_table[INTE_VALUE->content_type]); \
    } \
}

#define SETTINGS_REQUEST(INTE_VALUE, REQ_VALUE) { \
    strcpy(INTE_VALUE->user_name, REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->client_id, REQ_VALUE.from); \
    strcpy(INTE_VALUE->authorize, REQ_VALUE.authorize); \
}

#define SETTINGS_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        sprintf(RES_VALUE.content_length, "%lu", INTE_VALUE->content_length); \
        strcpy(RES_VALUE.content_type, content_table[INTE_VALUE->content_type]); \
    } \
}

#define LINK_REQUEST(INTE_VALUE, REQ_VALUE) { \
    strcpy(INTE_VALUE->user_name, REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->client_id, REQ_VALUE.from); \
    strcpy(INTE_VALUE->authorize, REQ_VALUE.authorize); \
}

#define LINK_RESPONSE(RES_VALUE, SDTP_STATUS) { \
    RES_VALUE.status_code = SDTP_STATUS; \
}

#define UNLINK_REQUEST(INTE_VALUE, REQ_VALUE) { \
    strcpy(INTE_VALUE->user_name, REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->client_id, REQ_VALUE.from); \
    strcpy(INTE_VALUE->authorize, REQ_VALUE.authorize); \
}

#define UNLINK_RESPONSE(RES_VALUE, SDTP_STATUS) { \
    RES_VALUE.status_code = SDTP_STATUS; \
}

#ifdef __cplusplus
}
#endif

#endif /* VALUE_LAYER_H */

