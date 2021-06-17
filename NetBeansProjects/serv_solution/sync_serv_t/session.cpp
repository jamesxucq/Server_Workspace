#include "auth_bzl.h"
#include "opera_bzl.h"
#include "auth_cache.h"
#include "list_cache.h"
#include "chks_bzl.h"

#include "session.h"

//
#define IS_DIRE_PATH(STR) ('/' == *strlchr(STR))

//
int _seion_timeout_;
int _seion_loop_;

//
int initialize_seion(int seion_timeout) {
    _seion_timeout_ = seion_timeout;
fprintf(stderr, "seion_timeout:%d\n", seion_timeout);
    _seion_loop_ = 0x00;
    return ERR_SUCCESS; //succ
}

int finalize_seion() {
    return ERR_SUCCESS; //succ
}

//
void open_initial_se(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival) {
// _LOG_TRACE("open initial session.");
    //
    struct list_data *ldata;
    if ((ldata = get_adata_bzl(ival->uid, sctrl->saddr.sin_addr, sctrl->saddr.sin_port, (char *) ival->data_buffer))) {
        DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
        seion->ldata = ldata;
        strcpy(ldata->req_valid, (char *) ival->data_buffer);
        strcpy(sctrl->seion_id, ldata->seion_id);
        //
        if (strcmp(ldata->seion_id, ival->seion_id)) {
            // _LOG_INFO("sctrl->seion_id:%s, ival->seion_id:%s", sctrl->seion_id, ival->seion_id); // delete by james 20130409
            erase_chks_cache_bzl(ival->uid);
            opera_reconn_exception(ival->uid);
            auth_reconn_exception(ival->uid);
            sctrl->status_code = UNAUTHORIZED;
        } else {
            // seion->rope = get_chks_cache_bzl(ldata->uid, ldata->location);
            if (!(seion->aope = get_opera_cache_bzl(ldata->uid)))
                sctrl->status_code = INTERNAL_SERVER_ERROR;
        }
    } else sctrl->status_code = UNAUTHORIZED;
}

void open_kalive_se(struct list_data *ldata) {
    DIRECT_SESSION_TIMEOUT(ldata, _seion_timeout_)
// _LOG_TRACE("open kalive session.");
}

int open_post_se(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open post session.");
            //
    if (!(sctrl->req_data = tmpfile64())) return ERR_FAULT;
    FILE_SIZE_PATH(ival->origin_length, ldata->location, ival->file_name)
    ival->act_time = time(NULL);
    return ERR_SUCCESS;
}

// for test
FILE *swd_tmpfile() {
    static int name_count;
    char file_name[16];
    //
    sprintf(file_name, "./tmp/tmp%d", ++name_count);
    FILE *fp = fopen(file_name, "wb+");
    //
    return fp;
}

int open_put_file(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open put session.");
            //
            // _LOG_INFO("put file name:%s offset:%d", ival->file_name, ival->offset); // delete by james 20130409
    if(IS_DIRE_PATH(ival->file_name)) ival->is_dire = TRUE;
    else { //
        ival->is_dire = FALSE;
        if (RANGE_TYPE_INVA != ival->range_type)
            CREAT_FILE_DIRPATH_EXT(ldata->location, ival->file_name, 0777)
            if (!(sctrl->req_data = tmpfile64())) return ERR_FAULT;
        // if (!(sctrl->req_data = swd_tmpfile())) return ERR_FAULT; // for test
        ival->act_time = time(NULL);
        //
        struct riv_live *plive;
        plive = get_chks_cache_bzl(ldata->uid, ldata->location);
        riv_insert_bzl(ival->file_name, plive);
    }
    //
    return ERR_SUCCESS;
}

int open_put_chks(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open put sums session.");
    FILE_SIZE_TIME_PATH(ival->file_size, ival->last_write, ldata->location, ival->file_name)
            //
    if (!(sctrl->req_data = tmpfile64())) return ERR_FAULT;
    return ERR_SUCCESS;
}

int (*open_put_se[])(struct seion_ctrl*, struct list_data*, struct inte_value*) = {
    NULL,
    open_put_chks,
    open_put_file
};

//
int open_copy_se(struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open copy session.");
    //
    CREAT_FILE_DIRPATH_EXT(ldata->location, (char *) ival->data_buffer, 0777)
    ival->act_time = time(NULL);
    FILE_SIZE_PATH(ival->file_size, ldata->location, ival->file_name)
            //
    struct riv_live *plive;
    plive = get_chks_cache_bzl(ldata->uid, ldata->location);
    riv_copy_bzl((char *) ival->data_buffer, ival->file_name, plive);
    // _LOG_TRACE("riv copy new:%s exis:%s", (char *) ival->data_buffer, ival->file_name);
    return ERR_SUCCESS;
}

int open_move_se(struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open move session.");
    //
    CREAT_FILE_DIRPATH_EXT(ldata->location, (char *) ival->data_buffer, 0777)
    ival->act_time = time(NULL);
    //
    struct riv_live *plive;
    plive = get_chks_cache_bzl(ldata->uid, ldata->location);
    riv_move_bzl((char *) ival->data_buffer, ival->file_name, plive);
    // _LOG_TRACE("riv move new:%s exis:%s", (char *) ival->data_rivfs copy file:buffer, ival->file_name);
    return ERR_SUCCESS;
}

int open_delete_se(struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open delete session.");
    //
    ival->act_time = time(NULL);
    // FILE_SIZE_PATH(ival->file_size, ldata->location, ival->file_name)
    FILE_SIZE_PATH(ival->file_size, ldata->location, ival->file_name)
    if(IS_DIRE_PATH(ival->file_name)) ival->is_dire = TRUE; 
    else ival->is_dire = FALSE;
// _LOG_DEBUG("delete file size:%llu", ival->file_size);
            //
    struct riv_live *plive;
    plive = get_chks_cache_bzl(ldata->uid, ldata->location);
    riv_remove_bzl(ival->file_name, plive);
    return ERR_SUCCESS;
}

void open_final_null(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(seion->ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open final invalid session.");
    sctrl->status_code = BAD_REQUEST;
}

void open_final_end(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival) {
    struct list_data *ldata = seion->ldata;
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open final end session.");
    //
    del_opera_cache_end(ldata->uid);
    erase_auth_cache_end(ldata->uid);
}

void open_final_success(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival) {
    struct list_data *ldata = seion->ldata;
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open final success session.");
    struct actno_ope *aope = seion->aope;
    //
    uint32 ins_anchor = add_anchor_bzl(ldata->location, aope);
    if (ins_anchor) {
        clear_list_cache(ldata->list_cache, ldata->location);
        erase_chks_cache_bzl(ldata->uid);
        del_opera_cache_finish(ldata->uid);
        sprintf((char *) ival->data_buffer, "%u", ins_anchor);
        erase_auth_cache_success(ldata->uid, ins_anchor, ldata->req_valid);
        seion->ldata = NULL;
    } else sctrl->status_code = INTERNAL_SERVER_ERROR;
}

void open_final_finish(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival) {
    struct list_data *ldata = seion->ldata;
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
// _LOG_TRACE("open final finish session.");
    //
    clear_list_cache(ldata->list_cache, ldata->location);
    erase_chks_cache_bzl(ldata->uid);
    del_opera_cache_finish(ldata->uid);
    erase_auth_cache_finish(ldata->uid, ldata->last_anchor, ldata->req_valid, ldata->anchor_cached);
    seion->ldata = NULL;
}

void (*open_final_se[])(struct seion_ctrl *, struct session *, struct inte_value *) = {
    open_final_null,
    open_final_end,
    open_final_success,
    open_final_finish
};