#include "rserv_bzl.h"
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

int regisbzl_create(struct regis_config *regis_config) {
    _lease_time_ = regis_config->lease_time * 60;
    return ERR_SUCCESS; //succ
}

int regisbzl_destroy() {
    return ERR_SUCCESS; //succ
}

//epoll serv handle recv and create send
//
int hand_regis_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl);
int hand_settings_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl);
//

void regis_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct regis_request req_value;
    if (parse_regis_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        REGIS_REQUEST(ival, req_value);
    open_regis_se(sctrl, ival);
}

void settings_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct regis_request req_value;
    if (parse_settings_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        SETTINGS_REQUEST(ival, req_value);
    open_settings_se(sctrl, ival);
}

void link_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct regis_request req_value;
    if (parse_link_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        LINK_REQUEST(ival, req_value);
    open_link_se(sctrl, ival);
}

void unlink_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    INIT_STATUS_OKAY(sctrl)
            //
            struct regis_request req_value;
    if (parse_unlink_recv(&req_value, (char *) bev->evdat.head_buffer))
        FAULT_EXIT(BAD_REQUEST)
        UNLINK_REQUEST(ival, req_value);
    open_unlink_se(sctrl, ival);
}

void (*recv_null_code[])(struct _event*) = {
    NULL
};

void (*recv_options_code[])(struct _event*) = {
    NULL,
    regis_recv,
    settings_recv,
    link_recv,
    unlink_recv
};

void (**receive_tab[])(struct _event*) = {
    recv_null_code,
    recv_options_code
};

//

void unuse_send(struct _event *bev) {
    struct evdata *evdat = &bev->evdat;
    BUILD_UNUSE_SEND(evdat)
}

void regis_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct regis_response res_value;
    REGIS_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_REGIS_SEND(evdat, res_value)
    hand_regis_send(evdat, ival, sctrl);
    CLOSE_REGIS_SE(sctrl);
}

void settings_send(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct regis_response res_value;
    SETTINGS_RESPONSE(res_value, sctrl->status_code, ival)
    BUILD_SETTINGS_SEND(evdat, res_value);
    hand_settings_send(evdat, ival, sctrl);
    CLOSE_SETTINGS_SE(sctrl);
}

void link_send(struct _event *bev) {
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct regis_response res_value;
    LINK_RESPONSE(res_value, sctrl->status_code)
    BUILD_LINK_SEND(evdat, res_value);
    CLOSE_LINK_SE(sctrl);
}

void unlink_send(struct _event *bev) {
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    struct regis_response res_value;
    UNLINK_RESPONSE(res_value, sctrl->status_code)
    BUILD_UNLINK_SEND(evdat, res_value);
    CLOSE_UNLINK_SE(sctrl);
}

//

int hand_regis_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl) {
    if (!ival->content_length) return ERR_SUCCESS;
    //
    if (ival->content_length > evdat->blen) return ERR_FAULT;
    evdat->blen = ival->content_length;
    memcpy(evdat->body_buffer, sctrl->send_buffer, evdat->blen);
    //
    return ERR_SUCCESS;
}

int hand_settings_send(struct evdata *evdat, const struct inte_value *ival, struct seion_ctrl *sctrl) {
    if (!ival->content_length) return ERR_SUCCESS;
    //
    if (ival->content_length > evdat->blen) return ERR_FAULT;
    evdat->blen = ival->content_length;
    memcpy(evdat->body_buffer, sctrl->send_buffer, evdat->blen);
    //
    return ERR_SUCCESS;
}

//
void (*send_null_code[])(struct _event*) = {
    unuse_send
};

void (*send_options_code[])(struct _event*) = {
    unuse_send, regis_send, settings_send, link_send, unlink_send
};

void (**send_tab[])(struct _event*) = {
    send_null_code,
    send_options_code
};

