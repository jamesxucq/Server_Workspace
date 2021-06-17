#include "listxml.h"
#include "list_cache.h"
#include "auth_bzl.h"
#include "auth_cache.h"
#include "chks_bzl.h"

#include "gseion.h"

//

int open_gnull_null(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open gnull session.");
    sctrl->status_code = BAD_REQUEST;
    return ERR_SUCCESS;
}

int (*open_get_null[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_gnull_null
};

//

int open_gchks_null(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open gchks session.");
    sctrl->status_code = BAD_REQUEST;
    return ERR_SUCCESS;
}

static uint32(*build_simple_chks[])(FILE **, struct riv_live *, char *, uint64, uint32) = {
    NULL,
    build_file_simple_bzl,
    build_chunk_simple_bzl,
    NULL
};

static enum STATUS_CODE simple_chks_code[] = {
    OK,
    NO_CONTENT,
    NOT_FOUND,
    PROCESSING,
    INTERNAL_SERVER_ERROR
};

int open_gchks_simple(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open gsums simple session.");
            //
            struct riv_live *plive;
    plive = get_chks_cache_bzl(ldata->uid, ldata->location);
    uint32 opval = build_simple_chks[ival->range_type](&sctrl->res_data, plive, ival->file_name, ival->offset, ival->cache_verify);
    if (!opval) {
        FILE_SIZE_STREAM(ival->content_length, sctrl->res_data)
        ival->content_type = CONTENT_TYPE_OCTET;
        rewind(sctrl->res_data);
    } else sctrl->status_code = simple_chks_code[opval];
    //
    return ERR_SUCCESS;
}

static uint32(*build_complex_chks[])(FILE **, char *, uint64) = {
    NULL,
    build_file_complex_bzl,
    build_chunk_complex_bzl,
    NULL
};

static enum STATUS_CODE complex_chks_code[] = {
    OK,
    NO_CONTENT,
    NOT_FOUND,
    INTERNAL_SERVER_ERROR
};

int open_gchks_complex(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    char file_name[MAX_PATH * 3];
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open gsums complex session.");
    //
    POOL_PATH(file_name, ldata->location, ival->file_name);
    uint32 opval = build_complex_chks[ival->range_type](&sctrl->res_data, file_name, ival->offset);
    if (!opval) {
        FILE_SIZE_STREAM(ival->content_length, sctrl->res_data)
        ival->content_type = CONTENT_TYPE_OCTET;
        rewind(sctrl->res_data);
    } else sctrl->status_code = complex_chks_code[opval];
    //
    return ERR_SUCCESS;
}

static uint32(*build_whole_chks[])(FILE **, struct riv_live *, char *, uint32) = {
    NULL,
    build_file_whole_bzl,
    NULL,
    NULL
};

static enum STATUS_CODE whole_chks_code[] = {
    OK,
    NO_CONTENT,
    NOT_FOUND,
    PROCESSING,
    INTERNAL_SERVER_ERROR
};

int open_gchks_whole(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open gsums whole session.");
            //
            struct riv_live *plive;
    plive = get_chks_cache_bzl(ldata->uid, ldata->location);
    uint32 opval = build_whole_chks[ival->range_type](&sctrl->res_data, plive, ival->file_name, ival->cache_verify);
    if (!opval) {
        FILE_SIZE_STREAM(ival->content_length, sctrl->res_data)
        ival->content_type = CONTENT_TYPE_OCTET;
        rewind(sctrl->res_data);
    } else sctrl->status_code = whole_chks_code[opval];
    //
    return ERR_SUCCESS;
}

int (*open_get_chks[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_gchks_null,
    open_gchks_simple,
    open_gchks_complex,
    open_gchks_whole
};

//
static uint32 range_type_value[] = {
    0, // RANGE_TYPE_INVA
    0, // RANGE_TYPE_FILE
    CHUNK_SIZE, // RANGE_TYPE_CHUNK
    BLOCK_SIZE // RANGE_TYPE_BLOCK
};

int open_gfile_file(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    char file_name[MAX_PATH * 3];
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open get file session.");
    //
    POOL_PATH(file_name, ldata->location, ival->file_name);
    // _LOG_TRACE("get file name:%s", file_name); // disable by james 20120410
    sctrl->res_data = fopen64(file_name, "rb");
    if (!sctrl->res_data) {
        _LOG_ERROR("not open file:%s", file_name);
        return ERR_FAULT;
    }
    // _LOG_TRACE("open get file. name:%s", file_name);
    if (fseeko64(sctrl->res_data, ival->offset, SEEK_SET))
        return ERR_FAULT;
    //
    FILE_SIZE_TIME_STREAM(ival->file_size, ival->last_write, sctrl->res_data)
    if (RANGE_TYPE_FILE == ival->range_type)
        ival->content_length = ival->file_size - ival->offset;
    else ival->content_length = range_type_value[ival->range_type];
    ival->content_type = CONTENT_TYPE_OCTET;
    //
    return ERR_SUCCESS;
}

int (*open_get_file[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_gfile_file
};

//
static enum STATUS_CODE open_list_code[] = {
    OK,
    PROCESSING,
    NOT_FOUND,
    INTERNAL_SERVER_ERROR
};

int open_glist_null(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("close get list session.");
    sctrl->status_code = BAD_REQUEST;
    return ERR_SUCCESS;
}

int open_glist_recu_files(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open glist recu files session.");
            //
            int opval = build_recursive_file(&sctrl->res_data, ldata->location, (char *) ival->data_buffer);
    if (!opval) {
        ldata->list_cache |= CACHE_RECURSIVE_FILE;
        FILE_SIZE_STREAM(ival->content_length, sctrl->res_data)
        ival->content_type = CONTENT_TYPE_XML;
        rewind(sctrl->res_data);
    } else sctrl->status_code = open_list_code[opval];
    //
    return ERR_SUCCESS;
}

int open_glist_list_directory(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open glist list files session.");
            //
            int opval = build_list_directory(&sctrl->res_data, ldata->location, (char *) ival->data_buffer);
    if (!opval) {
        ldata->list_cache |= CACHE_LIST_DIREC;
        FILE_SIZE_STREAM(ival->content_length, sctrl->res_data)
        ival->content_type = CONTENT_TYPE_XML;
        rewind(sctrl->res_data);
    } else sctrl->status_code = open_list_code[opval];
    //
    return ERR_SUCCESS;
}

int open_glist_recu_dires(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open glist recu dires session.");
            //
            int opval = build_recu_directory(&sctrl->res_data, ldata->location, (char *) ival->data_buffer);
    if (!opval) {
        ldata->list_cache |= CACHE_RECU_DIRECTORY;
        FILE_SIZE_STREAM(ival->content_length, sctrl->res_data)
        ival->content_type = CONTENT_TYPE_XML;
        rewind(sctrl->res_data);
    } else sctrl->status_code = open_list_code[opval];
    //
    return ERR_SUCCESS;
}

int open_glist_anch_files(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
            // _LOG_TRACE("open glist anchor session.");
            //
            unsigned int anchor = atoi((const char *) ival->data_buffer);
    if (0 < anchor) {
        int opval = build_anchor_file(&sctrl->res_data, ldata->location, anchor);
        if (!opval) {
            ldata->list_cache |= CACHE_ANCHOR_FILE;
            FILE_SIZE_STREAM(ival->content_length, sctrl->res_data)
            ival->content_type = CONTENT_TYPE_XML;
            rewind(sctrl->res_data);
        } else sctrl->status_code = open_list_code[opval];
    } else sctrl->status_code = NOT_FOUND;
    //
    return ERR_SUCCESS;
}

int (*open_get_list[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_glist_null,
    open_glist_recu_files,
    open_glist_list_directory,
    open_glist_recu_dires,
    open_glist_anch_files
};

//

int open_ganchor_null(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open ganchor null session.");
    sctrl->status_code = BAD_REQUEST;
    return ERR_SUCCESS;
}

int open_ganchor_last(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    struct xml_anchor anchor;
    DELAY_SESSION_TIMEOUT(ldata, _seion_loop_, _seion_timeout_)
    // _LOG_TRACE("open ganchor last session.");
    //
    anchor.uid = ldata->uid;
    anchor.last_anchor = ldata->last_anchor;
    NEW_ANCHOR_VALUE(sctrl->send_buffer);
    if (!sctrl->send_buffer) return ERR_FAULT;
    anchor_xml_value((char *) sctrl->send_buffer, &anchor);
    _LOG_TRACE("ganchor last:%s", (char *) sctrl->send_buffer);
    //
    ival->content_length = strlen((char *) sctrl->send_buffer);
    ival->content_type = CONTENT_TYPE_XML;
    //
    return ERR_SUCCESS;
}

int (*open_get_anchor[])(struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_ganchor_null,
    open_ganchor_last
};

int (**open_get_se[]) (struct seion_ctrl *, struct list_data *, struct inte_value *) = {
    open_get_null,
    open_get_chks,
    open_get_file,
    open_get_anchor,
    open_get_list
};

//

void close_gnull_null(struct seion_ctrl * sctrl) {
    // _LOG_TRACE("close ganchor NULL session.");
    BZERO_SCTRL(sctrl)
}

void (*close_get_null[])(struct seion_ctrl *) = {
    close_gnull_null
};

//

void close_gchks_null(struct seion_ctrl *sctrl) {
    // _LOG_TRACE("close gsums invalid session.");
    BZERO_SCTRL(sctrl)
}

void close_gchks_simple(struct seion_ctrl *sctrl) {
    // _LOG_TRACE("close gsums simple session.");
    RIV_CLOSE_BZL(sctrl->res_data);
    BZERO_SCTRL(sctrl)
}

void close_gchks_complex(struct seion_ctrl *sctrl) {
    // _LOG_TRACE("close gsums complex session.");
    CHKS_CLOSE_BZL(sctrl->res_data);
    BZERO_SCTRL(sctrl)
}

void close_gchks_whole(struct seion_ctrl *sctrl) {
    // _LOG_TRACE("close gsums sha1 session.");
    CHKS_CLOSE_BZL(sctrl->res_data);
    BZERO_SCTRL(sctrl)
}

void (*close_get_chks[])(struct seion_ctrl *) = {
    close_gchks_null,
    close_gchks_simple,
    close_gchks_complex,
    close_gchks_whole
};

//

void close_gfile_file(struct seion_ctrl * sctrl) {
    // _LOG_TRACE("close get file. send file count:%d", sctrl->rwrite_tatol);
    FCLOSE_SAFETY(sctrl->res_data);
    BZERO_SCTRL(sctrl)
}

void (*close_get_file[])(struct seion_ctrl *) = {
    close_gfile_file
};

//

void close_ganchor_null(struct seion_ctrl *sctrl) {
    // _LOG_TRACE("close gsums invalid session.");
    BZERO_SCTRL(sctrl)
}

void close_ganchor_last(struct seion_ctrl * sctrl) {
    // _LOG_TRACE("close ganchor last session. get anchor send file size:%d", sctrl->rwrite_tatol);
    DELE_ANCHOR_VALUE(sctrl->send_buffer);
    BZERO_SCTRL(sctrl)
}

void (*close_get_anchor[])(struct seion_ctrl *) = {
    close_ganchor_null,
    close_ganchor_last
};

//

void close_glist_null(struct seion_ctrl * sctrl) {
    // _LOG_TRACE("close glist invalid session");
    BZERO_SCTRL(sctrl)
}

void close_glist_recu_files(struct seion_ctrl * sctrl) {
    // _LOG_TRACE("close get list session. get list send file count:%d", sctrl->rwrite_tatol);
    LISTX_CLOSE(sctrl->res_data);
    BZERO_SCTRL(sctrl)
}

void close_glist_list_directory(struct seion_ctrl * sctrl) {
    // _LOG_TRACE("close get list session. get list send file count:%d", sctrl->rwrite_tatol);
    LISTX_CLOSE(sctrl->res_data);
    BZERO_SCTRL(sctrl)
}

void close_glist_recu_dires(struct seion_ctrl * sctrl) {
    // _LOG_TRACE("close get list session. get list send file count:%d", sctrl->rwrite_tatol);
    LISTX_CLOSE(sctrl->res_data);
    BZERO_SCTRL(sctrl)
}

void close_glist_anch_files(struct seion_ctrl * sctrl) {
    // _LOG_TRACE("close get list session. get list send file count:%d", sctrl->rwrite_tatol);
    LISTX_CLOSE(sctrl->res_data);
    BZERO_SCTRL(sctrl)
}

//
void (*close_get_list[])(struct seion_ctrl *) = {
    close_glist_null,
    close_glist_recu_files,
    close_glist_list_directory,
    close_glist_recu_dires,
    close_glist_anch_files
};

void (**close_get_se[]) (struct seion_ctrl *) = {
    close_get_null,
    close_get_chks,
    close_get_file,
    close_get_anchor,
    close_get_list
};



