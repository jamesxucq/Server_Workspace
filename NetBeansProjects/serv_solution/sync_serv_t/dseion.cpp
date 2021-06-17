#include "listxml.h"
#include "list_cache.h"
#include "auth_bzl.h"

#include "session.h"
#include "dseion.h"

//

int open_hnull_null(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open head session.");
    //
    sctrl->status_code = BAD_REQUEST;
    //
    return ERR_SUCCESS;
}

int (*open_head_null[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_hnull_null
};

//

int open_hfile_file(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open head session.");
    //
    // FILE_SIZE_PATH(ival->file_size, ldata->location, ival->file_name)
    FILE_SIZE_TIME_PATH(ival->file_size, ival->last_write, ldata->location, ival->file_name)
    ival->content_length = ival->file_size;
    ival->content_type = CONTENT_TYPE_OCTET;
    // _LOG_INFO("get file name:%s", file_name); // disable by james 20120410
    //
    return ERR_SUCCESS;
}

int (*open_head_file[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_hfile_file
};

//

static enum STATUS_CODE open_list_code[] = {
    OK,
    PROCESSING,
    NOT_FOUND,
    INTERNAL_SERVER_ERROR
};

int open_hlist_null(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open head session.");
    sctrl->status_code = BAD_REQUEST;
    return ERR_SUCCESS;
}

int open_hlist_recu_files(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open head list recu session.");
            //
            int opval = build_recursive_file(&sctrl->res_data, ldata->location, (char *) ival->data_buffer);
    if (!opval) {
        ldata->list_cache |= CACHE_RECURSIVE_FILE;
        FILE_SIZE_STREAM_EXT(ival->file_size, (char *) ival->data_buffer, sctrl->res_data)
        _LOG_DEBUG("xml file length:%d md5:%s", ival->file_size, ival->data_buffer);
        ival->content_length = ival->file_size;
        ival->content_type = CONTENT_TYPE_XML;
    } else sctrl->status_code = open_list_code[opval];
    // _LOG_INFO("get file name:%s", file_name); // disable by james 20120410
    //
    return ERR_SUCCESS;
}

int open_hlist_list_directory(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open head list dire session.");
            //
            int opval = build_list_directory(&sctrl->res_data, ldata->location, (char *) ival->data_buffer);
    if (!opval) {
        ldata->list_cache |= CACHE_LIST_DIREC;
        FILE_SIZE_STREAM_EXT(ival->file_size, (char *) ival->data_buffer, sctrl->res_data)
        ival->content_length = ival->file_size;
        ival->content_type = CONTENT_TYPE_XML;
    } else sctrl->status_code = open_list_code[opval];
    // _LOG_INFO("get file name:%s", file_name); // disable by james 20120410
    //
    return ERR_SUCCESS;
}

int open_hlist_recu_dires(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open head list recu session.");
            //
            int opval = build_recu_directory(&sctrl->res_data, ldata->location, (char *) ival->data_buffer);
    if (!opval) {
        ldata->list_cache |= CACHE_RECU_DIRECTORY;
        FILE_SIZE_STREAM_EXT(ival->file_size, (char *) ival->data_buffer, sctrl->res_data)
        ival->content_length = ival->file_size;
        ival->content_type = CONTENT_TYPE_XML;
    } else sctrl->status_code = open_list_code[opval];
    // _LOG_INFO("get file name:%s", file_name); // disable by james 20120410
    //
    return ERR_SUCCESS;
}

int open_hlist_anch_files(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open head list anch session.");
            //
            unsigned int anchor = atoi((char *) ival->data_buffer);
    if (0 < anchor) {
        int opval = build_anchor_file(&sctrl->res_data, ldata->location, anchor);
        if (!opval) {
            ldata->list_cache |= CACHE_ANCHOR_FILE;
            FILE_SIZE_STREAM_EXT(ival->file_size, (char *) ival->data_buffer, sctrl->res_data)
            ival->content_length = ival->file_size;
            ival->content_type = CONTENT_TYPE_XML;
        } else sctrl->status_code = open_list_code[opval];
    } else sctrl->status_code = NOT_FOUND;
    // _LOG_INFO("get file name:%s", file_name); // disable by james 20120410
    //
    return ERR_SUCCESS;
}

int (*open_head_list[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_hlist_null,
    open_hlist_recu_files,
    open_hlist_list_directory,
    open_hlist_recu_dires,
    open_hlist_anch_files
};

int (**open_head_se[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_head_null, // ENTRY_INVA
    open_head_null, // ENTRY_CHKS
    open_head_file, // ENTRY_FILE
    open_head_null, // ENTRY_ANCHOR
    open_head_list // ENTRY_LIST
};

//

void close_head_null(struct seion_ctrl *sctrl) {
    // _LOG_TRACE("close head session.");
    BZERO_SCTRL(sctrl)
}

void close_head_file(struct seion_ctrl *sctrl) {
    // _LOG_TRACE("close head file session.");
    BZERO_SCTRL(sctrl)
}

void close_head_list(struct seion_ctrl *sctrl) {
    LISTX_CLOSE(sctrl->res_data)
    // _LOG_TRACE("close head list session. get_send file size:%d", sctrl->rwrite_tatol);
    BZERO_SCTRL(sctrl)
}

void (*close_head_se[])(struct seion_ctrl *) = {
    close_head_null, // ENTRY_INVA
    close_head_null, // ENTRY_CHKS
    close_head_file, // ENTRY_FILE
    close_head_null, // ENTRY_ANCHOR
    close_head_list // ENTRY_LIST
};