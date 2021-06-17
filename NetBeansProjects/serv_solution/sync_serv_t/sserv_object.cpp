/* sserv_object.c
 * Created on: 2010-3-10
 * Author: Divad
 */
#include "sserv_object.h"
#include "build_utility.h"
#include "finalize.h"
#include "fasd_list.h"
#include "versi_utility.h"

#include "third_party.h" // test
//

//
static uint64 _max_modi_siz_;
static uint64 _max_dele_siz_;

//
int initialize_hander(bakup_config *bakup_ctrl) {
    _max_modi_siz_ = bakup_ctrl->max_modify << 20;
    _modi_interv_ = bakup_ctrl->modi_interv;
fprintf(stderr, "max_modify:%dMB modi_interv:%dsec\n", bakup_ctrl->max_modify, bakup_ctrl->modi_interv);
    _max_dele_siz_ = bakup_ctrl->max_delete << 20;
    _dele_interv_ = bakup_ctrl->dele_interv;
fprintf(stderr, "max_delete:%dMB dele_interv:%dsec\n", bakup_ctrl->max_delete, bakup_ctrl->dele_interv);

    return ERR_SUCCESS; //succ
}

int finalize_hander() {
    return ERR_SUCCESS; //succ
}

//
int hand_post_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    if (!ival->content_length) return HAND_FINISH;
    evdat->blen = fwrite(evdat->body_buffer, 1, evdat->blen, sctrl->req_data);
    //
    sctrl->rwrite_tatol += evdat->blen;
    if (ival->content_length != sctrl->rwrite_tatol) 
        return HAND_UNDONE;
    FINAL_POST_RECV(bev, sctrl, ival, _max_modi_siz_)
    return HAND_FINISH;
}

int hand_put_file_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    //
    if(ival->is_dire) {
        struct list_data *ldata = bev->seion->ldata;
        CREAT_DIRE_PATH(ldata->location, ival->file_name, 0777)
    } else {
        struct seion_ctrl *sctrl = &bev->seion->sctrl;
        struct evdata *evdat = &bev->evdat;
        //
        if (!ival->content_length) return HAND_FINISH;
        // log_binary_code(evdat->body_buffer, evdat->blen);
// _LOG_DEBUG("content_length:%d rwrite_tatol:%d", ival->content_length, sctrl->rwrite_tatol);
// _LOG_DEBUG("blen:%d body_buffer:%d req_data:%d", evdat->blen, evdat->body_buffer, sctrl->req_data);
        evdat->blen = fwrite(evdat->body_buffer, 1, evdat->blen, sctrl->req_data);
        if (!evdat->blen) { _LOG_WARN("put file error!"); }
        //
        sctrl->rwrite_tatol += evdat->blen;    
        // if(evdat->blen != 8192) _LOG_DEBUG("sctrl->rwrite_tatol:%d evdat->blen:%d", sctrl->rwrite_tatol, evdat->blen);
        if (ival->content_length != sctrl->rwrite_tatol) 
            return HAND_UNDONE;    
    }
///
    return HAND_FINISH;
}

int hand_put_chks_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    if (!ival->content_length) return HAND_FINISH;
// _LOG_DEBUG("content_length:%d rwrite_tatol:%d", ival->content_length, sctrl->rwrite_tatol);
// _LOG_DEBUG("blen:%d body_buffer:%d req_data:%d", evdat->blen, evdat->body_buffer, sctrl->req_data);
    evdat->blen = fwrite(evdat->body_buffer, 1, evdat->blen, sctrl->req_data);
    //
    sctrl->rwrite_tatol += evdat->blen;
    if (ival->content_length != sctrl->rwrite_tatol) 
        return HAND_UNDONE;
    return HAND_FINISH;
}

static int(*recv_put_table[])(struct _event*) = {
    NULL,
    hand_put_chks_recv,
    hand_put_file_recv
};

int hand_put_recv(struct _event *bev) {
    int haval = recv_put_table[bev->seion->ival.sub_entry](bev);
    if (!haval) FINAL_PUT_RECV(bev);
// _LOG_TRACE("hand put recv! haval:%d", haval);
    return haval;
}

int (*recv_handler_tab[])(struct _event*) = {
    NULL, NULL, NULL, NULL, NULL,
    hand_post_recv,
    hand_put_recv,
    NULL, NULL, NULL, NULL
};

//
int hand_copy_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct list_data *ldata = bev->seion->ldata;
    char exist_name[MAX_PATH * 3], new_name[MAX_PATH * 3];
    //
    POOL_PATH(exist_name, ldata->location, ival->file_name);
    POOL_PATH(new_name, ldata->location, ival->data_buffer);
    // _LOG_TRACE("hand copy new:%s exis:%s", new_name, exist_name);
    int cpval = ERR_SUCCESS;
    IS_DIRECTORY(exist_name)
    if (TRUE == S_ISDIR(statb.st_mode)) {
        if (recu_dires_copy(exist_name, new_name) < 0) {
            cpval = ERR_FAULT;
            _LOG_WARN("copy directory error! %s", exist_name);
        }
    } else {
        int cpsta = file_copy_ext(exist_name, new_name);
        if (0x01 == cpsta)
            bev->seion->sctrl.status_code = PROCESSING;
        else if (-1 == cpsta) {
            cpval = ERR_FAULT;
            _LOG_WARN("copy file error! %s", exist_name);
        }
    }
    return cpval;
}

int hand_move_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct list_data *ldata = bev->seion->ldata;
    char exist_name[MAX_PATH * 3], new_name[MAX_PATH * 3];
    //
    POOL_PATH(exist_name, ldata->location, ival->file_name);
    POOL_PATH(new_name, ldata->location, ival->data_buffer);
    // _LOG_TRACE("hand move new:%s exis:%s", new_name, exist_name);
    if (rename(exist_name, new_name) < 0) {
        _LOG_WARN("move server file error! %s", exist_name);
        return ERR_FAULT;
    }
    //
    return ERR_SUCCESS;
}

int hand_dele_recv(struct _event *bev) {
    struct inte_value *ival = &bev->seion->ival;
    struct list_data *ldata = bev->seion->ldata;
    char path_name[MAX_PATH * 3];
    //
    int deval = ERR_SUCCESS;
    if (ival->is_dire) {
        POOL_PATH(path_name, ldata->location, ival->file_name);
        if (recu_rmdir(path_name) < 0) {
            deval = ERR_FAULT;
            _LOG_WARN("delete path error! errno:%d path:%s", errno, ival->file_name);
        }
    } else {
        if(_max_dele_siz_ < ival->file_size) {
            POOL_PATH(path_name, ldata->location, ival->file_name);
            unlink(path_name);
_LOG_DEBUG("unlink file:%s", ival->file_name);
        } else {
            int desta = delete_backup(ldata->location, ival->file_name, ival->act_time);
_LOG_DEBUG("desta:%d", desta);
            if (0x01 == desta)
                bev->seion->sctrl.status_code = PROCESSING;
            else if (-1 == desta) {
                deval = ERR_FAULT;
                _LOG_WARN("delete file error! %s", ival->file_name);
            }        
        }
    }
    //
    return deval;
}

//

int hand_get_anchor_send(struct _event *bev) {
    int send_value = HAND_UNDONE;
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    if (!ival->content_length) {
        evdat->blen = 0x00;
        return HAND_FINISH;
    }
    if (ival->content_length <= (sctrl->rwrite_tatol + evdat->blen)) {
        evdat->blen = ival->content_length - sctrl->rwrite_tatol;
        send_value = HAND_FINISH;
    }
// _LOG_DEBUG("send_value:%d", send_value);
// _LOG_DEBUG("content_length:%d rwrite_tatol:%d", ival->content_length, sctrl->rwrite_tatol);
// _LOG_DEBUG("blen:%d body_buffer:%d send_buffer:%d", evdat->blen, evdat->body_buffer, sctrl->send_buffer);
    memcpy(evdat->body_buffer, sctrl->send_buffer + sctrl->rwrite_tatol, evdat->blen);
    //
    sctrl->rwrite_tatol += evdat->blen;
    return send_value;
}

int hand_get_list_send(struct _event *bev) {
    int send_value = HAND_UNDONE;
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    if (!ival->content_length) {
        evdat->blen = 0x00;
        return HAND_FINISH;
    }
    if (ival->content_length <= (sctrl->rwrite_tatol + evdat->blen)) {
        evdat->blen = ival->content_length - sctrl->rwrite_tatol;
        send_value = HAND_FINISH;
    }
    evdat->blen = fread(evdat->body_buffer, 1, evdat->blen, sctrl->res_data);
    if (!evdat->blen) { _LOG_WARN("get server list error!"); }
    //
    sctrl->rwrite_tatol += evdat->blen;
    return send_value;
}

int hand_get_file_send(struct _event *bev) {
    int send_value = HAND_UNDONE;
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    if (!ival->content_length) {
        evdat->blen = 0x00;
        return HAND_FINISH;
// _LOG_DEBUG("ival->content_length:0");
    }
    if (ival->content_length <= (sctrl->rwrite_tatol + evdat->blen)) {
        evdat->blen = ival->content_length - sctrl->rwrite_tatol;
        send_value = HAND_FINISH;
// _LOG_DEBUG("content_length:%d rwrite_tatol:%d blen:%d", ival->content_length, sctrl->rwrite_tatol, evdat->blen);
    }
    evdat->blen = fread(evdat->body_buffer, 1, evdat->blen, sctrl->res_data);
    if (!evdat->blen) { _LOG_WARN("get server file error!"); }
    //
    sctrl->rwrite_tatol += evdat->blen;
    return send_value;
}

int hand_get_chks_send(struct _event *bev) {
    int send_value = HAND_UNDONE;
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
    //
    if (!ival->content_length) {
        evdat->blen = 0x00;
        return HAND_FINISH;
    }
    if (ival->content_length <= (sctrl->rwrite_tatol + evdat->blen)) {
        evdat->blen = ival->content_length - sctrl->rwrite_tatol;
        send_value = HAND_FINISH;
    }
// _LOG_DEBUG("send_value:%d", send_value);
// _LOG_DEBUG("content_length:%d rwrite_tatol:%d", ival->content_length, sctrl->rwrite_tatol);
// _LOG_DEBUG("blen:%d body_buffer:%d res_data:%d", evdat->blen, evdat->body_buffer, sctrl->res_data);
    evdat->blen = fread(evdat->body_buffer, 1, evdat->blen, sctrl->res_data);
    if (!evdat->blen) { _LOG_WARN("get chks file error!"); }
    //
    sctrl->rwrite_tatol += evdat->blen;
    return send_value;
}

static int(*send_get_table[])(struct _event*) = {
    NULL,
    hand_get_chks_send,
    hand_get_file_send,
    hand_get_anchor_send,
    hand_get_list_send
};

int hand_get_send(struct _event *bev) {
// _LOG_TRACE("hand get send! 2");
    int haval = send_get_table[bev->seion->ival.sub_entry](bev);
    if (!haval) FINAL_GET_SEND(bev);
// _LOG_TRACE("hand get send! haval:%d", haval);
    return haval;
}

int hand_put_chks_send(struct _event *bev) {
    int send_value = HAND_UNDONE;
    struct inte_value *ival = &bev->seion->ival;
    struct seion_ctrl *sctrl = &bev->seion->sctrl;
    struct evdata *evdat = &bev->evdat;
// _LOG_DEBUG("ival->content_length:%d", ival->content_length);
    if (!ival->content_length) {
        evdat->blen = 0x00;
        return HAND_FINISH;
    }
    if (ival->content_length <= (sctrl->rwrite_tatol + evdat->blen)) {
        evdat->blen = ival->content_length - sctrl->rwrite_tatol;
        send_value = HAND_FINISH;
    }
// _LOG_DEBUG("send_value:%d", send_value);
// _LOG_DEBUG("content_length:%d rwrite_tatol:%d", ival->content_length, sctrl->rwrite_tatol);
// _LOG_DEBUG("blen:%d body_buffer:%d res_data:%d", evdat->blen, evdat->body_buffer, sctrl->res_data);
    evdat->blen = fread(evdat->body_buffer, 1, evdat->blen, sctrl->res_data);
    // log_binary_code(evdat->body_buffer, evdat->blen);
    //
    sctrl->rwrite_tatol += evdat->blen;
// _LOG_DEBUG("sctrl->rwrite_tatol:%d evdat->blen:%d ival->content_length:%d", sctrl->rwrite_tatol, evdat->blen, ival->content_length);
    return send_value;
}

inline int hand_put_file_send(struct _event *bev) {
    return HAND_FINISH;
}

static int(*send_put_table[])(struct _event*) = {
    NULL,
    hand_put_chks_send,
    hand_put_file_send
};

int hand_put_send(struct _event *bev) {
    int haval = send_put_table[bev->seion->ival.sub_entry](bev);
    if (!haval) FINAL_PUT_SEND(bev);
// _LOG_TRACE("hand put send! haval:%d", haval);
    return haval;
}

//
int (*send_handler_tab[])(struct _event*) = {
    NULL, NULL, NULL, NULL,
    hand_get_send,
    NULL,
    hand_put_send,
    NULL, NULL, NULL, NULL
};