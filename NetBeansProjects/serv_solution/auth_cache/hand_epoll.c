#include "aserv_bzl.h"
#include "auth_bzl.h"

#include "sdtp_utility.h"
#include "hand_epoll.h"

#define FAULT_EXIT(STATUS) { \
    sctrl->status_code = STATUS; \
    return; \
}
//
#define INIT_STATUS_OKAY(ctrl) ctrl->status_code = OK;
//

int cachebzl_create(struct cache_config *config) {
    _seion_timeout_ = config->seion_timeout;
    return ERR_SUCCESS; //succ
}

int cachebzl_destroy() {
    return ERR_SUCCESS; //succ
}

//epoll serv handle recv and create send
//
int hand_query_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl);
int hand_list_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl);
int hand_status_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl);
//

void query_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct cache_request req_value;
    if (parse_query_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        QUERY_REQUEST(ival, req_value);
    open_query_se(sctrl, ival);
}

void addi_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct cache_request req_value;
    if (parse_addi_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        ADD_REQUEST(ival, req_value);
    open_addi_se(sctrl, ival);
}

void clear_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct cache_request req_value;
    if (parse_clear_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        CLEAR_REQUEST(ival, req_value);
    open_clear_se(sctrl, ival);
}

void list_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct cache_request req_value;
    if (parse_list_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        LIST_REQUEST(ival, req_value);
    open_list_se(sctrl, ival);
}

void status_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct cache_request req_value;
    if (parse_status_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        STATUS_REQUEST(ival, req_value);
    open_status_se(sctrl, ival);
}

void sanchor_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct cache_request req_value;
    if (parse_set_anchor_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        SETANCHOR_REQUEST(ival, req_value);
    open_set_anchor_se(sctrl, ival);
}

void slocked_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct cache_request req_value;
    if (parse_set_locked_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        SETLOCKED_REQUEST(ival, req_value);
    open_set_locked_se(sctrl, ival);
}

void kalive_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct cache_request req_value;
    if (parse_keep_alive_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        KEEPALIVE_REQUEST(ival, req_value);
    open_keep_alive_se(sctrl, ival);
}

void (*recv_null_code[])(struct _event*) = {
    NULL
};


void (*recv_options_code[])(struct _event*) = {
    NULL,
    query_recv,
    status_recv
};

void (*recv_control_code[])(struct _event*) = {
    NULL,
    sanchor_recv,
    slocked_recv,
    addi_recv,
    clear_recv,
    list_recv
};

void (*recv_kalive_code[])(struct _event*) = {
    kalive_recv
};


void (**receive_tab[])(struct _event*) = {
    recv_null_code,
    recv_options_code,
    recv_control_code,
    recv_kalive_code
};

//

void unused_send(struct _event *bev) {
    struct evdata *evdat = &bev->evdat;
    BUILD_UNUSE_SEND(evdat)
}

void query_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct cache_response res_value;
    QUERY_RESPONSE(res_value, sctrl->status_code, ival);
    BUILD_QUERY_SEND(evdat, res_value);
    hand_query_send(evdat, ival, sctrl);
    CLOSE_QUERY_SE(sctrl);
}

void addi_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct cache_response res_value;
    ADDI_RESPONSE(res_value, sctrl->status_code, ival);
    BUILD_ADDI_SEND(evdat, res_value);
    CLOSE_ADDI_SE(sctrl);
}

void clear_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct cache_response res_value;
    CLEAR_RESPONSE(res_value, sctrl->status_code, ival);
    BUILD_CLEAR_SEND(evdat, res_value);
    CLOSE_CLEAR_SE(sctrl);
}

void list_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct cache_response res_value;
    LIST_RESPONSE(res_value, sctrl->status_code, ival);
    BUILD_LIST_SEND(evdat, res_value);
    hand_list_send(evdat, ival, sctrl);
    CLOSE_LIST_SE(sctrl);
}

void status_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct cache_response res_value;
    STATUS_RESPONSE(res_value, sctrl->status_code, ival);
    BUILD_STATUS_SEND(evdat, res_value);
    hand_status_send(evdat, ival, sctrl);
    CLOSE_STATUS_SE(sctrl);
}

void sanchor_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct cache_response res_value;
    SETANCHOR_RESPONSE(res_value, sctrl->status_code, ival);
    BUILD_SET_ANCHOR_SEND(evdat, res_value);
    CLOSE_SET_ANCHOR_SE(sctrl);
}

void slocked_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct cache_response res_value;
    SETLOCKED_RESPONSE(res_value, sctrl->status_code, ival);
    BUILD_SET_LOCKED_SEND(evdat, res_value);
    CLOSE_SET_LOCKED_SE(sctrl);
}

//

int hand_query_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl) {
    if (!ival->content_length) return ERR_SUCCESS;
    //
    if (ival->content_length > evdat->blen) return ERR_FAULT;
    evdat->blen = ival->content_length;
    memcpy(evdat->body_buffer, sctrl->send_buffer, evdat->blen);
    //
    return ERR_SUCCESS;
}

int hand_list_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl) {
    if (!ival->content_length) return ERR_SUCCESS;
    //
    if (ival->content_length > evdat->blen) return ERR_FAULT;
    evdat->blen = ival->content_length;
    memcpy(evdat->body_buffer, sctrl->send_buffer, evdat->blen);
    //
    return ERR_SUCCESS;
}

int hand_status_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl) {
    if (!ival->content_length) return ERR_SUCCESS;
    //
    if (ival->content_length > evdat->blen) return ERR_FAULT;
    evdat->blen = ival->content_length;
    memcpy(evdat->body_buffer, sctrl->send_buffer, evdat->blen);
    //
    return ERR_SUCCESS;
}

void kalive_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct cache_response res_value;
    KEEPALIVE_RESPONSE(res_value, sctrl->status_code, ival);
    BUILD_KEEP_ALIVE_SEND(evdat, res_value);
    CLOSE_KEEP_ALIVE_SE(sctrl);
}

//

void (*send_null_code[])(struct _event*) = {
    unused_send
};

void (*send_options_code[])(struct _event*) = {
    unused_send,
    query_send,
    status_send
};

void (*send_control_code[])(struct _event*) = {
    unused_send,
    sanchor_send,
    slocked_send,
    addi_send,
    clear_send,
    list_send
};

void (*send_kalive_code[])(struct _event*) = {
    kalive_send
};

void (**send_tab[])(struct _event*) = {
    send_null_code,
    send_options_code,
    send_control_code,
    send_kalive_code
};
