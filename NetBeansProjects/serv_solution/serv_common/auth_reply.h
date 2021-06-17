/* 
 */
#ifndef AUTHREPLY_H
#define AUTHREPLY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "text_value.h"
#include "sdtproto.h"
#include "common_macro.h"
#include "server_macro.h"
    //   

    struct socks_request {
        char res_identity[MAX_PATH * 3];
        char req_valid[VALID_LENGTH];
        char params[MIN_TEXT_LEN];
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
    };

    struct socks_response {
        int status_code;
        //        
        char params[MIN_TEXT_LEN];
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
        char execute_status[EXECUTE_LENGTH];
    };

#define STATUS_REQUEST(REQ_VALUE, UID, ADDR, PORT, VALID) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    sprintf(REQ_VALUE.params, "addr=%s port=%d", ADDR, PORT); \
    strcpy(REQ_VALUE.req_valid, VALID); \
}

//
#define STATUS_RESPONSE(CONTENT_TYPE, CONTENT_LEN, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(CONTENT_TYPE, content_value_table, RES_VALUE.content_type) \
        CONTENT_LEN = atoi(RES_VALUE.content_length); \
    } \
}

#define SET_ANCHOR_REQUEST(REQ_VALUE, UID, LAST_ANCHOR, VALID) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    sprintf(REQ_VALUE.params, "anchor=%u", LAST_ANCHOR); \
    strcpy(REQ_VALUE.req_valid, VALID); \
}

    //
    extern const struct value_node execute_value_table[];
#define SET_ANCHOR_RESPONSE(SET_STATUS, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(SET_STATUS, execute_value_table, RES_VALUE.execute_status) \
    } \
}

    extern const char *bool_text_table[];
#define SET_LOCKED_REQUEST(REQ_VALUE, UID, SYNC_LOCKED, VALID) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    sprintf(REQ_VALUE.params, "locked=%s", bool_text_table[SYNC_LOCKED]); \
    strcpy(REQ_VALUE.req_valid, VALID); \
}

#define SET_LOCKED_RESPONSE(SET_STATUS, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(SET_STATUS, execute_value_table, RES_VALUE.execute_status) \
    } \
}

#define KEEP_ALIVE_REQUEST(REQ_VALUE, UID, VALID) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    strcpy(REQ_VALUE.req_valid, VALID); \
}

#define KEEP_ALIVE_RESPONSE(KEEP_STATUS, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(KEEP_STATUS, execute_value_table, RES_VALUE.execute_status) \
    }\
}


#ifdef __cplusplus
}
#endif

#endif /* AUTHREPLY_H */

