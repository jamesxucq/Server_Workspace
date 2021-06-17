/*
 * File:   finalize.h
 * Author: Administrator
 *
 * Created on 2012
 */

#ifndef FINALIZE_H
#define	FINALIZE_H

// #ifdef	__cplusplus
// extern "C" {
// #endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common_header.h"
#include "opera_bzl.h"
#include "gseion.h"
#include "build_utility.h"
#include "event.h"
#include "session.h"

#define FINAL_POST_RECV(BEV, SCTRL, IVALU, MAX_MODI_SIZ) { \
    struct list_data *ldata = BEV->seion->ldata; \
    struct file_builder builder; \
    if(MAX_MODI_SIZ > IVALU->origin_length) { \
        if(0x01 == modify_verson(ldata->location, IVALU->file_name, IVALU->act_time)) { \
            SCTRL->status_code = PROCESSING; \
            return HAND_FINISH;\
        }\
    } \
    memset(&builder, '\0', sizeof (struct file_builder)); \
    POOL_PATH(builder.file_name, ldata->location, IVALU->file_name); \
    builder.length = IVALU->content_length; \
    builder.offset = IVALU->offset; \
/* _LOG_INFO("status code:%d", SCTRL->status_code); */ \
    if (build_content_by_file[IVALU->range_type](&builder, SCTRL->req_data)) \
        SCTRL->status_code = INTERNAL_SERVER_ERROR; \
/* _LOG_INFO("status code:%d", SCTRL->status_code); */ \
}    

#define FINAL_PUT_RECV(BEV) { \
    struct inte_value *ival = &BEV->seion->ival; \
    struct seion_ctrl *sctrl = &BEV->seion->sctrl; \
    if (ENTRY_CHKS == ival->sub_entry) { \
        if (0 > match_chks_by_file(sctrl, BEV->seion->ldata, ival)) \
            (sctrl)->status_code = INTERNAL_SERVER_ERROR; \
        RESET_PUT_CHKS_SE(sctrl) \
        if (flush_match_file(sctrl, ival)) \
            (sctrl)->status_code = INTERNAL_SERVER_ERROR; \
    } else if (ENTRY_FILE == ival->sub_entry) CLOSE_PUT_FILE_SE(BEV->seion->aope, sctrl, BEV->seion->ldata, ival) \
}

#define FINAL_GET_SEND(BEV) { \
    struct inte_value *ival = &BEV->seion->ival; \
    struct seion_ctrl *sctrl = &BEV->seion->sctrl; \
/* _LOG_INFO("xxx 1 ival->sub_entry:%d ival->svalue%d", ival->sub_entry, ival->svalue); */ \
    close_get_se[ival->sub_entry][ival->svalue](sctrl); \
/* _LOG_INFO("xxx 2"); */ \
}

#define FINAL_PUT_SEND(BEV) { \
    struct inte_value *ival = &BEV->seion->ival; \
    struct seion_ctrl *sctrl = &BEV->seion->sctrl; \
    if (ENTRY_CHKS == ival->sub_entry) CLOSE_PUT_CHKS_SE(sctrl) \
}

// #ifdef	__cplusplus
// }
// #endif

#endif	/* FINALIZE_H */

