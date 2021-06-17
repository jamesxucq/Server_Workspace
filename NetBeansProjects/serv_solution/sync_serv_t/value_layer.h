/* value_layer.h
 * Author: Divad
 * Created on 2011-11-17
 */

#ifndef VALUE_LAYER_H
#define VALUE_LAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
#include "common_macro.h"
#include "text_value.h"
#include "session.h"

#ifdef __cplusplus
extern "C" {
#endif

    //

    struct serv_request {
        char res_identity[MAX_PATH * 3];
        char last_write[TIME_LENGTH];
        char params[MAX_PATH * 3];
        char content_range[MIN_TEXT_LEN];
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
        char cache_verify[CACHE_VERIFY_LENGTH];
        char seion_id[SESSION_LENGTH];
    };

    struct serv_response {
        enum STATUS_CODE status_code;
        //
        char file_size[DIGIT_LENGTH];
        char last_write[TIME_LENGTH];
        char attach[MIN_TEXT_LEN];
        char content_range[MIN_TEXT_LEN];
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
        char seion_id[SESSION_LENGTH];
    };

    //

#define ITEM_TOKEN  ' '
#define PATH_TOKEN  0

#define INITIAL_REQUEST(INTE_VALUE, REQ_VALUE) { \
    INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
    strcpy((char *)INTE_VALUE->data_buffer, name_value(NULL, REQ_VALUE.params, ITEM_TOKEN)); \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

    //#define CONTENT_TYPE_INVA            0x0000
    //#define CONTENT_TYPE_XML                0x0001
    //#define CONTENT_TYPE_OCTET              0x0002
    //#define CONTENT_TYPE_PLAIN              0x0003
    extern const char *content_table[];

#define INITIAL_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

#define KALIVE_REQUEST(INTE_VALUE, REQ_VALUE) { \
    INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

#define KALIVE_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

    static const struct value_node subentry_table[] = {
        {"chks", ENTRY_CHKS},
        {"file", ENTRY_FILE},
        {"anchor", ENTRY_ANCHOR},
        {"list", ENTRY_LIST},
        {NULL, 0}
    };

    static const struct value_node anchor_value_table[] = {
        {"last-anchor", GANCH_LAST_ANCH},
        {NULL, 0}
    };

    static const struct value_node chks_value_table[] = {
        {"adler32&md5", COMPLEX_CHKS},
        {"md5", SIMPLE_CHKS},
        {"sha1", WHHLE_CHKS},
        {"invalid", INVA_CHKS},
        {NULL, 0}
    };

    static const struct value_node cache_value_table[] = {
        {"no-cache", UPDATE_CACHE},
        {"cache", RIVER_CACHED},
        {"invalid", INVA_CACHE},
        {NULL, 0}
    };

    static const struct value_node list_value_table[] = {
        {"anchor-files", GLIST_ANCH_FILES},
        {"recursion-files", GLIST_RECU_FILES},
        {"recursion-directories", GLIST_RECU_DIRES},
        {"list-directory", GLIST_LIST_DIREC},
        {NULL, 0}
    };

    static const struct value_node range_value_table[] = {
        {"block", RANGE_TYPE_BLOCK},
        {"chunk", RANGE_TYPE_CHUNK},
        {"file", RANGE_TYPE_FILE},
        {"invalid", RANGE_TYPE_INVA},
        {NULL, 0}
    };

    static inline int range_value_type(uint64 *offset, char *range_txt) {
        int range_type = RANGE_TYPE_INVA;
        char *offsetsp;

        if (*range_txt == '\0') return RANGE_TYPE_INVA;
        offsetsp = strchr(range_txt, '=');
        if (offsetsp) *offsetsp = '\0';
        else return RANGE_TYPE_INVA;

        TEXT_VALUE(range_type, range_value_table, range_txt)
                *offset = atoll(++offsetsp);

        return range_type;
    }

#define VALUE_UNUSED    0
#define HEAD_REQUEST(INTE_VALUE, REQ_VALUE) { \
    char *value_txt = name_value(NULL, REQ_VALUE.params, ITEM_TOKEN); \
    TEXT_VALUE(INTE_VALUE->sub_entry, subentry_table, value_txt) \
    switch (INTE_VALUE->sub_entry) { \
        case ENTRY_CHKS: \
            value_txt = name_value(NULL, NULL, ITEM_TOKEN); \
            TEXT_VALUE(INTE_VALUE->svalue, chks_value_table, value_txt) \
            strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
            break; \
        case ENTRY_FILE: \
            INTE_VALUE->svalue = VALUE_UNUSED; \
            strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
            break; \
        case ENTRY_ANCHOR: \
            value_txt = name_value(NULL, NULL, ITEM_TOKEN); \
            TEXT_VALUE(INTE_VALUE->svalue, anchor_value_table, value_txt) \
            INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
            break; \
        case ENTRY_LIST: \
            value_txt = name_value(NULL, NULL, ITEM_TOKEN); \
            TEXT_VALUE(INTE_VALUE->svalue, list_value_table, value_txt) \
            strcpy((char *)INTE_VALUE->data_buffer, REQ_VALUE.res_identity); \
            break; \
    } \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

#define HEAD_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        switch (INTE_VALUE->sub_entry) { \
            case ENTRY_CHKS: \
                sprintf(RES_VALUE.attach, "md5=%s", INTE_VALUE->data_buffer); \
                break; \
            case ENTRY_FILE: \
                UTC_TIME_TEXT(RES_VALUE.last_write, INTE_VALUE->last_write) \
                break; \
            case ENTRY_ANCHOR: \
            case ENTRY_LIST: \
                sprintf(RES_VALUE.attach, "md5=%s", INTE_VALUE->data_buffer); \
                break; \
        } \
        sprintf(RES_VALUE.content_length, "%u", INTE_VALUE->content_length); \
        strcpy(RES_VALUE.content_type, content_table[INTE_VALUE->content_type]); \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}


#define GET_REQUEST(INTE_VALUE, REQ_VALUE) { \
    char *value_txt = name_value(NULL, REQ_VALUE.params, ITEM_TOKEN); \
    TEXT_VALUE(INTE_VALUE->sub_entry, subentry_table, value_txt) \
    switch (INTE_VALUE->sub_entry) { \
        case ENTRY_CHKS: \
            value_txt = name_value(NULL, NULL, ITEM_TOKEN); \
            TEXT_VALUE(INTE_VALUE->svalue, chks_value_table, value_txt) \
            strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
            INTE_VALUE->range_type = range_value_type(&INTE_VALUE->offset, REQ_VALUE.content_range); \
            TEXT_VALUE(INTE_VALUE->cache_verify, cache_value_table, REQ_VALUE.cache_verify) \
            break; \
        case ENTRY_FILE: \
            INTE_VALUE->svalue = VALUE_UNUSED; \
            strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
            INTE_VALUE->range_type = range_value_type(&INTE_VALUE->offset, REQ_VALUE.content_range); \
            break; \
        case ENTRY_ANCHOR: \
            value_txt = name_value(NULL, NULL, ITEM_TOKEN); \
            TEXT_VALUE(INTE_VALUE->svalue, anchor_value_table, value_txt) \
            INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
            break; \
        case ENTRY_LIST: \
            value_txt = name_value(NULL, NULL, ITEM_TOKEN); \
            TEXT_VALUE(INTE_VALUE->svalue, list_value_table, value_txt) \
            strcpy((char *)INTE_VALUE->data_buffer, REQ_VALUE.res_identity); \
            break; \
    } \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

    //#define RANGE_TYPE_INVA              0x0000 // invalid
    //#define RANGE_TYPE_FILE                 0x0001 // file
    //#define RANGE_TYPE_CHUNK                0x0002 // chunk
    //#define RANGE_TYPE_BLOCK                0x0003 // block 
    extern const char *range_text_table[];

#define GET_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        switch (INTE_VALUE->sub_entry) { \
            case ENTRY_CHKS: \
                break; \
            case ENTRY_FILE: \
                UTC_TIME_TEXT(RES_VALUE.last_write, INTE_VALUE->last_write) \
                sprintf(RES_VALUE.content_range, "%s %lu/%lu", range_text_table[INTE_VALUE->range_type], INTE_VALUE->offset, INTE_VALUE->file_size); \
                break; \
            case ENTRY_ANCHOR: \
            case ENTRY_LIST: \
                break; \
        } \
        sprintf(RES_VALUE.content_length, "%u", INTE_VALUE->content_length); \
        strcpy(RES_VALUE.content_type, content_table[INTE_VALUE->content_type]); \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

//
    static inline int content_range_value(uint64 *length, uint64 *offset, char *range_txt) {
        int range_type = RANGE_TYPE_INVA;
        //
        if (*range_txt == '\0') return RANGE_TYPE_INVA;
        char *offsetsp = strchr(range_txt, ' ');
        if (offsetsp) *offsetsp = '\0';
        else return RANGE_TYPE_INVA;
        //
        TEXT_VALUE(range_type, range_value_table, range_txt)
                //
                char *lensp = strchr(++offsetsp, '/');
        if (lensp) *lensp = '\0';
        else return RANGE_TYPE_INVA;
        //
        *offset = atoll(offsetsp);
        *length = atoll(++lensp);
        //
        return range_type;
    }

#define POST_REQUEST(INTE_VALUE, REQ_VALUE) { \
    strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
    UTC_TIME_VALUE(INTE_VALUE->last_write, REQ_VALUE.last_write) \
    INTE_VALUE->range_type = content_range_value(&INTE_VALUE->file_size, &INTE_VALUE->offset, REQ_VALUE.content_range); \
    INTE_VALUE->content_length = atoi(REQ_VALUE.content_length); \
    TEXT_VALUE(INTE_VALUE->content_type, content_value_table, REQ_VALUE.content_type) \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

#define POST_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

    static const struct value_node put_entry_table[] = {
        {"chks", ENTRY_CHKS},
        {"file", ENTRY_FILE},
        {NULL, 0}
    };

#define PUT_REQUEST(INTE_VALUE, REQ_VALUE) { \
    char *value_txt = name_value(NULL, REQ_VALUE.params, ITEM_TOKEN); \
    TEXT_VALUE(INTE_VALUE->sub_entry, put_entry_table, value_txt) \
    strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
    switch (INTE_VALUE->sub_entry) { \
        case ENTRY_CHKS: \
            value_txt = name_value(NULL, NULL, ITEM_TOKEN); \
            TEXT_VALUE(INTE_VALUE->svalue, chks_value_table, value_txt) \
            INTE_VALUE->range_type = range_value_type(&INTE_VALUE->offset, REQ_VALUE.content_range); \
            break; \
        case ENTRY_FILE: \
            UTC_TIME_VALUE(INTE_VALUE->last_write, REQ_VALUE.last_write) \
            INTE_VALUE->range_type = content_range_value(&INTE_VALUE->file_size, &INTE_VALUE->offset, REQ_VALUE.content_range); \
            break; \
    } \
    INTE_VALUE->content_length = atoi(REQ_VALUE.content_length); \
    TEXT_VALUE(INTE_VALUE->content_type, content_value_table, REQ_VALUE.content_type) \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

#define PUT_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        switch (INTE_VALUE->sub_entry) { \
            case ENTRY_CHKS: \
                UTC_TIME_TEXT(RES_VALUE.last_write, INTE_VALUE->last_write) \
                sprintf(RES_VALUE.content_range, "%s %lu/%lu", range_text_table[INTE_VALUE->range_type], INTE_VALUE->offset, INTE_VALUE->file_size); \
                sprintf(RES_VALUE.content_length, "%u", INTE_VALUE->content_length); \
                strcpy(RES_VALUE.content_type, content_table[INTE_VALUE->content_type]); \
                break; \
            case ENTRY_FILE: \
                break; \
        } \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

#define COPY_REQUEST(INTE_VALUE, REQ_VALUE) { \
    strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
    UTC_TIME_VALUE(INTE_VALUE->last_write, REQ_VALUE.last_write) \
    strcpy((char *)INTE_VALUE->data_buffer, name_value(NULL, REQ_VALUE.params, PATH_TOKEN)); \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

#define COPY_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

#define MOVE_REQUEST(INTE_VALUE, REQ_VALUE) { \
    strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
    UTC_TIME_VALUE(INTE_VALUE->last_write, REQ_VALUE.last_write) \
    strcpy((char *)INTE_VALUE->data_buffer, name_value(NULL, REQ_VALUE.params, PATH_TOKEN)); \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

#define MOVE_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

#define DELETE_REQUEST(INTE_VALUE, REQ_VALUE) { \
    strcpy(INTE_VALUE->file_name, REQ_VALUE.res_identity); \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

#define DELETE_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

    static const struct value_node final_value_table[] = {
        {"invalid", FINAL_TYPE_INVA},
        {"end", FINAL_TYPE_END},
        {"successful", FINAL_TYPE_SUCCESS},
        {"finish", FINAL_TYPE_FINISH},
        {NULL, 0}
    };

#define FINAL_REQUEST(INTE_VALUE, REQ_VALUE) { \
    INTE_VALUE->uid = atol(REQ_VALUE.res_identity); \
    char *entry_txt = name_value(NULL, REQ_VALUE.params, ITEM_TOKEN); \
    TEXT_VALUE(INTE_VALUE->sub_entry, final_value_table, entry_txt) \
    strcpy(INTE_VALUE->seion_id, REQ_VALUE.seion_id); \
}

#define FINAL_RESPONSE(RES_VALUE, SDTP_STATUS, INTE_VALUE) { \
    RES_VALUE.status_code = SDTP_STATUS; \
    if (OK == RES_VALUE.status_code) { \
        if (FINAL_TYPE_SUCCESS & INTE_VALUE->sub_entry) sprintf(RES_VALUE.attach, "anchor=%s", INTE_VALUE->data_buffer); \
        strcpy(RES_VALUE.seion_id, INTE_VALUE->seion_id); \
    } \
}

#ifdef __cplusplus
}
#endif

#endif /* VALUE_LAYER_H */

