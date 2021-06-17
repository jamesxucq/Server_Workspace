/* value_layer.h
 * Author: David
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

    struct cache_request {
        char res_identity[MAX_PATH * 3];
        char authorize[AUTHOR_LENGTH];
        char params[MIN_TEXT_LEN];
        char req_valid[VALID_LENGTH];
    };

    struct cache_response {
        enum STATUS_CODE status_code;
        //
        char execute_status[EXECUTE_LENGTH];
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
        char seion_id[SESSION_LENGTH];
    };

    //
#define VALUE_TOKEN  ' '

#define QUERY_REQUEST(INTE_VALUE, REQ_VALUE) { \
    INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->authorize, REQ_VALUE.authorize); \
}

    // #define CONTENT_TYPE_INVALID            0x0000
    // #define CONTENT_TYPE_XML                0x0001
    // #define CONTENT_TYPE_OCTET              0x0002
    // #define CONTENT_TYPE_PLAIN              0x0003
    extern const char *content_table[];

#define QUERY_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.content_type, content_table[INTE_VALUE->content_type]); \
        sprintf(RES_VALUE.content_length, "%lu", INTE_VALUE->content_length); \
    } \
}

    void split_ipaddr(cache_address *caddr, char *addr_txt);

    struct value_node {
        const char *text;
        int value;
    };

#define TEXT_VALUE(VALUE, TABLE, TEXT) { \
    struct value_node *item; \
    for(item = (struct value_node *)TABLE; item->text; ++item) \
        if(!strcmp(item->text, TEXT)) \
            break; \
    VALUE = item->value; \
}

    static const struct value_node addi_value_table[] = {
        {"server", ADDI_TYPE_SERVER},
        {"admin", ADDI_TYPE_ADMIN},
        {NULL, 0}
    };

#define ADD_REQUEST(INTE_VALUE, REQ_VALUE) { \
    char *add_txt = name_value(NULL, REQ_VALUE.params, VALUE_TOKEN); \
    TEXT_VALUE(INTE_VALUE->subtype, addi_value_table, add_txt) \
    switch (INTE_VALUE->subtype) { \
        case ADDI_TYPE_SERVER: \
            split_ipaddr(&INTE_VALUE->caddr, REQ_VALUE.res_identity); \
            break; \
        case ADDI_TYPE_ADMIN: \
            strcpy(INTE_VALUE->caddr.sin_addr, REQ_VALUE.res_identity); \
            break; \
    } \
    strcpy(INTE_VALUE->authorize, REQ_VALUE.authorize); \
}

    //#define EXECUTE_EXCEPTION              0x0000
    //#define EXECUTE_SUCCESS                0x0001
    extern const char *execute_text_table[];

#define ADDI_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.execute_status, execute_text_table[INTE_VALUE->execute_status]); \
    } \
}

    static const struct value_node clear_value_table[] = {
        {"single-user", CLEAR_TYPE_SINGLEUSR},
        {"all-users", CLEAR_TYPE_ALLUSRS},
        {"server", CLEAR_TYPE_SERVER},
        {"admin", CLEAR_TYPE_ADMIN},
        {NULL, 0}
    };

#define CLEAR_REQUEST(INTE_VALUE, REQ_VALUE) { \
    char *clear_txt = name_value(NULL, REQ_VALUE.params, VALUE_TOKEN); \
    TEXT_VALUE(INTE_VALUE->subtype, clear_value_table, clear_txt) \
    switch (INTE_VALUE->subtype) { \
        case CLEAR_TYPE_SINGLEUSR: \
            INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
            break; \
        case CLEAR_TYPE_ALLUSRS: \
            break; \
        case CLEAR_TYPE_SERVER: \
            split_ipaddr(&INTE_VALUE->caddr, REQ_VALUE.res_identity); \
            break; \
        case CLEAR_TYPE_ADMIN: \
            strcpy(INTE_VALUE->caddr.sin_addr, REQ_VALUE.res_identity); \
            break; \
    } \
    strcpy(INTE_VALUE->authorize, REQ_VALUE.authorize); \
}

#define CLEAR_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.execute_status, execute_text_table[INTE_VALUE->execute_status]); \
    } \
}

    static const struct value_node list_value_table[] = {
        {"server", LIST_TYPE_SERVER},
        {"admin", LIST_TYPE_ADMIN},
        {NULL, 0}
    };

#define LIST_REQUEST(INTE_VALUE, REQ_VALUE) { \
    /* INTE_VALUE->uid = atol(REQ_VALUE.res_identity); */ \
    char *list_txt = name_value(NULL, REQ_VALUE.params, VALUE_TOKEN); \
    TEXT_VALUE(INTE_VALUE->subtype, list_value_table, list_txt) \
    strcpy(INTE_VALUE->authorize, REQ_VALUE.authorize); \
}

#define LIST_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        sprintf(RES_VALUE.content_length, "%lu", INTE_VALUE->content_length); \
        strcpy(RES_VALUE.content_type, content_table[INTE_VALUE->content_type]); \
    } \
}

#define STATUS_REQUEST(INTE_VALUE, REQ_VALUE) { \
    INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->caddr.sin_addr, name_value(NULL, REQ_VALUE.params, VALUE_TOKEN)); \
    INTE_VALUE->caddr.sin_port = atoi(name_value(NULL, NULL, VALUE_TOKEN)); \
    strcpy(INTE_VALUE->req_valid, REQ_VALUE.req_valid); \
}

#define STATUS_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        sprintf(RES_VALUE.content_length, "%lu", INTE_VALUE->content_length); \
        strcpy(RES_VALUE.content_type, content_table[INTE_VALUE->content_type]); \
    } \
}

#define SETANCHOR_REQUEST(INTE_VALUE, REQ_VALUE) { \
    INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
    INTE_VALUE->cached_anchor = atol(name_value(NULL, REQ_VALUE.params, VALUE_TOKEN)); \
    strcpy(INTE_VALUE->req_valid, REQ_VALUE.req_valid); \
}

#define SETANCHOR_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.execute_status, execute_text_table[INTE_VALUE->execute_status]); \
    } \
}

    static const struct value_node slocked_value_table[] = {
        {"true", 1},
        {"false", 0},
        {NULL, 0}
    };

#define SETLOCKED_REQUEST(INTE_VALUE, REQ_VALUE) { \
    INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
    char *slocked_txt = name_value(NULL, REQ_VALUE.params, VALUE_TOKEN); \
    TEXT_VALUE(INTE_VALUE->serv_locked, slocked_value_table, slocked_txt) \
    strcpy(INTE_VALUE->req_valid, REQ_VALUE.req_valid); \
}

#define SETLOCKED_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.execute_status, execute_text_table[INTE_VALUE->execute_status]); \
    } \
}

#define KEEPALIVE_REQUEST(INTE_VALUE, REQ_VALUE) { \
    INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->req_valid, REQ_VALUE.req_valid); \
}

#define KEEPALIVE_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.execute_status, execute_text_table[INTE_VALUE->execute_status]); \
    } \
}

#ifdef __cplusplus
}
#endif

#endif /* VALUE_LAYER_H */

