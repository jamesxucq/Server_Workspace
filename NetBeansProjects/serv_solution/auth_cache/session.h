/* session.h
 * Created on: 2010-4-30
 * Author: David
 */

#ifndef SESSION_H_
#define SESSION_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_header.h"
#include "common_macro.h"
#include "author_macro.h"

//
typedef struct {
    char sin_addr[SERVNAME_LENGTH];
    int sin_port;
} cache_address;

struct inte_value {
    unsigned int uid;
    char authorize[AUTHOR_LENGTH];
    char req_valid[VALID_LENGTH];
    off_t content_length;
    unsigned int content_type;
    BYTE execute_status;
    //  
    cache_address caddr;
    int subtype;
    unsigned int cached_anchor;
    BYTE serv_locked;
    // 
    BYTE command;
};

struct seion_ctrl {
    enum STATUS_CODE status_code;
    BYTE *send_buffer;
};

struct session {
    struct inte_value ival;
    struct seion_ctrl sctrl;
};

//
extern int _seion_timeout_;

//
/* session ctrl */

//
#define BZERO_SCTRL(sctrl) \
    memset(sctrl, '\0', sizeof(struct seion_ctrl));

//
void open_query_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_QUERY_SE(sctrl) { \
    DEL_QUERY_VALUE(sctrl->send_buffer) \
    /* _LOG_INFO("close query session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_addi_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_ADDI_SE(sctrl) { \
    /* _LOG_INFO("close addition session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_clear_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_CLEAR_SE(sctrl) { \
    /* _LOG_INFO("close clear session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_list_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_LIST_SE(sctrl) { \
    DEL_LIST_VALUE(sctrl->send_buffer) \
    /* _LOG_INFO("close list session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_status_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_STATUS_SE(sctrl) { \
    DEL_STATUS_VALUE(sctrl->send_buffer) \
    /* _LOG_INFO("close status session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_set_anchor_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_SET_ANCHOR_SE(sctrl) { \
    /* _LOG_INFO("close set anchor session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_set_locked_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_SET_LOCKED_SE(sctrl) { \
    /* _LOG_INFO("close set locked session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_keep_alive_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_KEEP_ALIVE_SE(sctrl) { \
    /* _LOG_INFO("close keep alive session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

#ifdef	__cplusplus
}
#endif

#endif /* SESSION_H_ */
