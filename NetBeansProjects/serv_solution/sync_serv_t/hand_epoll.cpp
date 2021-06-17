#include <time.h>

#include "sync_serv.h"
#include "opera_bzl.h"
#include "sserv_bzl.h"

#include "hand_epoll.h"
#include "common_header.h"
#include "sdtp_utility.h"
#include "match_utility.h"
#include "gseion.h"
#include "dseion.h"
#include "session.h"

#define EXCEP_EXIT(SCTRL, STATUS) { \
    SCTRL->status_code = STATUS; \
    return LOOP_STOP; \
}
//
#define INIT_STATUS_OKAY(ctrl) ctrl->status_code = OK;
//

int syncingbzl_create(struct serv_config *config) {
    // start the sync thread
    initialize_seion(config->seion_timeout);
    initialize_hander(&config->bakup_ctrl);
    return ERR_SUCCESS; //succ
}
//

int syncingbzl_destroy() {
    finalize_hander();
    finalize_seion();
    return ERR_SUCCESS; //succ
}

//epoll serv handle recv and create send

int initial_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_initial_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        INITIAL_REQUEST(ival, req_value)
        //
        strcpy(sctrl->saddr.sin_addr, gserv_address);
    sctrl->saddr.sin_port = gbind_port;
    open_initial_se(sctrl, bev->seion, ival);
    return LOOP_STOP;
}

int kalive_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_kalive_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        KALIVE_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            //
            open_kalive_se(bev->seion->ldata);
    return LOOP_STOP;
}

int head_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_head_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        HEAD_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            //
            if (open_head_se[ival->sub_entry][ival->svalue](sctrl, bev->seion->ldata, ival))
                EXCEP_EXIT(sctrl, INTERNAL_SERVER_ERROR)
                //
                return LOOP_STOP;
}

int get_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_get_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        GET_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            /* _LOG_DEBUG("ival->sub_entry:%d ival->svalue:%d", ival->sub_entry, ival->svalue); */
            if (open_get_se[ival->sub_entry][ival->svalue](sctrl, bev->seion->ldata, ival))
                EXCEP_EXIT(sctrl, INTERNAL_SERVER_ERROR)
                //
                return LOOP_STOP;
}

int post_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_post_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        POST_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            //
            if (open_post_se(sctrl, bev->seion->ldata, ival))
                EXCEP_EXIT(sctrl, INTERNAL_SERVER_ERROR)
                if (hand_post_recv(bev)) return LOOP_CONTINUE;
    //
    return LOOP_STOP;
}

int put_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_put_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        PUT_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            //
            if (open_put_se[ival->sub_entry](sctrl, bev->seion->ldata, ival))
                EXCEP_EXIT(sctrl, INTERNAL_SERVER_ERROR)
                if (hand_put_recv(bev)) return LOOP_CONTINUE;
    //
    return LOOP_STOP;
}

int copy_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_copy_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        COPY_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            //
            if (open_copy_se(bev->seion->ldata, ival))
                EXCEP_EXIT(sctrl, INTERNAL_SERVER_ERROR)
                if (hand_copy_recv(bev)) EXCEP_EXIT(sctrl, NOT_FOUND)
                    //
                    return LOOP_STOP;
}

int move_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_move_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        MOVE_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            //
            if (open_move_se(bev->seion->ldata, ival))
                EXCEP_EXIT(sctrl, INTERNAL_SERVER_ERROR)
                if (hand_move_recv(bev)) EXCEP_EXIT(sctrl, NOT_FOUND)
                    //
                    return LOOP_STOP;
}

int delete_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_delete_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        DELETE_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            //
            if (open_delete_se(bev->seion->ldata, ival))
                EXCEP_EXIT(sctrl, INTERNAL_SERVER_ERROR)
                if (hand_dele_recv(bev)) EXCEP_EXIT(sctrl, NOT_FOUND)
                    //
                    return LOOP_STOP;
}

int final_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct serv_request req_value;
    if (parse_final_recv(&req_value, (char *) bev->evdat.head_buffer))
        EXCEP_EXIT(sctrl, BAD_REQUEST)
        FINAL_REQUEST(ival, req_value)
        if (strcmp(sctrl->seion_id, ival->seion_id))
            EXCEP_EXIT(sctrl, UNAUTHORIZED)
            //
            open_final_se[ival->sub_entry](sctrl, bev->seion, ival);
    //
    return LOOP_STOP;
}

int(*recv_initialize_tab[])(struct _event*) = {
    NULL,
    initial_recv,
    kalive_recv,
    head_recv,
    get_recv,
    post_recv,
    put_recv,
    copy_recv,
    move_recv,
    delete_recv,
    final_recv
};

//

int unuse_send(struct _event *bev) {
    struct evdata *evdat = &bev->evdat;
    BUILD_UNUSE_SEND(evdat)
            //
    return LOOP_STOP;
}

int initial_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct serv_response res_value;
    INITIAL_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_INITIAL_SEND(evdat, res_value);
    //
    CLOSE_INITIAL_SE(sctrl);
    return LOOP_STOP;
}

int kalive_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct serv_response res_value;
    KALIVE_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_KALIVE_SEND(evdat, res_value);
    //
    CLOSE_KALIVE_SE(sctrl);
    return LOOP_STOP;
}

int head_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    //
    struct serv_response res_value;
    HEAD_RESPONSE(res_value, sctrl->status_code, ival)
    build_head_send[ival->sub_entry](&bev->evdat, &res_value);
    //
    close_head_se[ival->sub_entry](sctrl);
    return LOOP_STOP;
}

int get_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    //
    struct serv_response res_value;
    GET_RESPONSE(res_value, sctrl->status_code, ival)
    build_get_send[ival->sub_entry](&bev->evdat, &res_value);
    //
    // _LOG_INFO("build_get_send! 2");
    if (OK == sctrl->status_code) {
        // _LOG_INFO("hand get send! 1");
        if (hand_get_send(bev)) return LOOP_CONTINUE;
    }
    // _LOG_INFO("build_get_send! 3");
    return LOOP_STOP;
}

int post_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct serv_response res_value;
    POST_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_POST_SEND(evdat, res_value);
    //
    if (OK == sctrl->status_code) {
        SET_FATTRIB_BZL(bev->seion->ldata->location, ival)
        ADD_MODIFY_BZL(bev->seion->aope, ival->file_name, ival->file_size, ival->origin_length, ival->act_time);
    }
    CLOSE_POST_SE(sctrl);
    return LOOP_STOP;
}

int put_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    //
    struct serv_response res_value;
    PUT_RESPONSE(res_value, sctrl->status_code, ival)
    build_put_send[ival->sub_entry](&bev->evdat, &res_value);
    //
    if (OK == sctrl->status_code) {
        if (hand_put_send(bev)) return LOOP_CONTINUE;
    }
    return LOOP_STOP;
}

int copy_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct serv_response res_value;
    COPY_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_COPY_SEND(evdat, res_value);
    //
    if (OK == sctrl->status_code) {
        // plawi(bev->seion->ldata->location, (char *)ival->data_buffer);
        SET_MODTIME_BZL(bev->seion->ldata->location, ival)
        // plawi(bev->seion->ldata->location, (char *)ival->data_buffer);
        ADD_COPY_BZL(bev->seion->aope, ival->file_name, (char *) ival->data_buffer, ival->file_size, ival->act_time);
    }
    CLOSE_COPY_SE(sctrl);
    return LOOP_STOP;
}

int move_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct serv_response res_value;
    MOVE_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_MOVE_SEND(evdat, res_value);
    //
    if (OK == sctrl->status_code) {
        SET_MODTIME_BZL(bev->seion->ldata->location, ival)
        ADD_MOVE_BZL(bev->seion->aope, ival->file_name, (char *) ival->data_buffer, ival->act_time);
    }
    CLOSE_MOVE_SE(sctrl);
    return LOOP_STOP;
}

int delete_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct serv_response res_value;
    DELETE_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_DELETE_SEND(evdat, res_value);
    //
    if (OK == sctrl->status_code) {
        ADD_DELETE_BZL(bev->seion->aope, ival->is_dire, ival->file_name, ival->file_size, ival->act_time);
    }
    CLOSE_DELETE_SE(sctrl);
    return LOOP_STOP;
}

int final_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct serv_response res_value;
    FINAL_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_FINAL_SEND(evdat, res_value, ival->sub_entry);
    //
    CLOSE_FINAL_SE(sctrl);
    return LOOP_STOP;
}

int(*send_initialize_tab[])(struct _event*) = {
    unuse_send,
    initial_send,
    kalive_send,
    head_send,
    get_send,
    post_send,
    put_send,
    copy_send,
    move_send,
    delete_send,
    final_send
};
