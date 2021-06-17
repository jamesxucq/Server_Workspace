/*
 * session.h
 *
 *  Created on: 2010-4-30
 *      Author: Divad
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
#include "regis_macro.h"

struct inte_value {
    char client_id[IDENTI_LENGTH];
    char user_agent[MID_TEXT_LEN];
    //
    char authorize[AUTHOR_LENGTH];
    off_t content_length;
    unsigned int content_type;
    //
    char user_name[_USERNAME_LENGTH_];
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
extern int _lease_time_;

//
#define BZERO_SCTRL(sctrl) \
    memset(sctrl, '\0', sizeof(struct seion_ctrl));

//
void open_regis_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_REGIS_SE(sctrl) { \
    DEL_REGIS_VALUE(sctrl->send_buffer) \
    /* _LOG_INFO("close regis session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_settings_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_SETTINGS_SE(sctrl) { \
    DEL_SETTINGS_VALUE(sctrl->send_buffer) \
    /* _LOG_INFO("close setting session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_link_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_LINK_SE(sctrl) { \
    /* _LOG_INFO("close link session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

void open_unlink_se(struct seion_ctrl *sctrl, struct inte_value *ival);
#define CLOSE_UNLINK_SE(sctrl) { \
    /* _LOG_INFO("close unlink session."); // disable by james 20120410 */ \
    BZERO_SCTRL(sctrl) \
}

/* session Ctrl */

#ifdef	__cplusplus
}
#endif

#endif /* SESSION_H_ */
