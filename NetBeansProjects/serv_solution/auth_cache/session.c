#include <utime.h>
#include "aserv_bzl.h"
#include "auth_bzl.h"
#include "session.h"

//
int _seion_timeout_;

//
#define AUTH_ACTIVE_SET(auth_data) auth_data->last_active = time(NULL);

//
void open_query_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct auth_data *adata;
    //
    if ((adata = get_adata_bzl(ival->uid))) {
        if (strcmp(ival->authorize, adata->_author_chks_)) {
            adata = acache_update_bzl(ival->uid);
            if (!adata) sctrl->status_code = BAD_REQUEST;
            if (strcmp(ival->authorize, adata->_author_chks_)) sctrl->status_code = UNAUTHORIZED;
        }
        if (OK == sctrl->status_code) {
            long now = time(NULL); // session timeout check here
            if ((now - adata->last_active) >= _seion_timeout_) { // 60s timeout
                adata->last_active = now;
                adata->serv_locked = FALSE;
            }
            if(!adata->serv_locked) {
                serv_server *light_server;
                if ((light_server = get_light_server())) {
                    struct xml_query query;
                    query.uid = adata->uid;
                    query.cached_anchor = adata->cached_anchor;
                    strcpy(query.req_valid, adata->req_valid);
                    _LOG_TRACE("adata->req_valid:%s", adata->req_valid);
                    strcpy(query.seion_id, adata->seion_id);
                    strcpy(query.saddr.sin_addr, light_server->sin_addr);
                    query.saddr.sin_port = light_server->sin_port;
                    NEW_QUERY_VALUE(sctrl->send_buffer)
                    if(sctrl->send_buffer) {
                        query_xml_value((char *) sctrl->send_buffer, &query);
                        ival->content_type = CONTENT_TYPE_XML;
                        ival->content_length = strlen((char *) sctrl->send_buffer);                
                    } else sctrl->status_code = INTERNAL_SERVER_ERROR;
                } else sctrl->status_code = VARIANT_ALSO_NEGOTIATES;
            } else sctrl->status_code = LOCKED;
        }
    } else sctrl->status_code = BAD_REQUEST;
}

void open_addi_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    listen_admin *admin;
    char author_chks[MD5_TEXT_LENGTH];
    //
    if ((admin = get_listen_admin())) {
_LOG_TRACE("admin->user_name:%s admin->password:%s", admin->user_name, admin->password);
char buffer[MID_TEXT_LEN];
sprintf(buffer, "%s:%s", admin->user_name, admin->password);
_LOG_TRACE("buffer:%s", buffer);
        reply_auth_chks(author_chks, admin->user_name, admin->password);
_LOG_TRACE("ival->authorize:%s author_chks:%s", ival->authorize, author_chks);
        if (!strcmp(ival->authorize, author_chks)) {
            switch (ival->subtype) {
                case ADDI_TYPE_SERVER:
                    addi_server(ival->caddr.sin_addr, ival->caddr.sin_port);
                    ival->execute_status = EXECUTE_SUCCESS;
                    break;
                case ADDI_TYPE_ADMIN:
                    addi_admin(ival->caddr.sin_addr);
                    ival->execute_status = EXECUTE_SUCCESS;
                    break;
            }
        } else sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = INTERNAL_SERVER_ERROR;
}

#define HANDLE_CLEAR_VALUE(value) \
    switch (value) { \
    case -1: \
        sctrl->status_code = INTERNAL_SERVER_ERROR; \
        break; \
    case 0: \
        ival->execute_status = EXECUTE_SUCCESS; \
        break; \
    case 1: \
        ival->execute_status = EXECUTE_EXCEPTION; \
        break; \
}

void open_clear_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    listen_admin *admin;
    char author_chks[MD5_TEXT_LENGTH];
    //
    if ((admin = get_listen_admin())) {
        reply_auth_chks(author_chks, admin->user_name, admin->password);
        if (!strcmp(ival->authorize, author_chks)) {
            switch (ival->subtype) {
                case CLEAR_TYPE_ALLUSRS:
                    HANDLE_CLEAR_VALUE(adata_clear_bzl())
                    break;
                case CLEAR_TYPE_SINGLEUSR:
                    HANDLE_CLEAR_VALUE(adata_erase_bzl(ival->uid))
                    break;
                case CLEAR_TYPE_SERVER:
                    HANDLE_CLEAR_VALUE(clear_server_bzl(ival->caddr.sin_addr, ival->caddr.sin_port))
                    break;
                case CLEAR_TYPE_ADMIN:
                    HANDLE_CLEAR_VALUE(CLEAR_ADMIN_BZL(ival->caddr.sin_addr))
                    break;
            }
        } else sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = INTERNAL_SERVER_ERROR;
}

void open_list_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    listen_admin *admin;
    char author_chks[MD5_TEXT_LENGTH];
    //
    if ((admin = get_listen_admin())) {
        reply_auth_chks(author_chks, admin->user_name, admin->password);
        if (!strcmp(ival->authorize, author_chks)) {
            NEW_LIST_VALUE(sctrl->send_buffer)
            if(sctrl->send_buffer) {
                switch (ival->subtype) {
                    case LIST_TYPE_SERVER:
                        serv_xml_value((char *) sctrl->send_buffer, get_server_list());
                        ival->execute_status = EXECUTE_SUCCESS;
                        break;
                    case LIST_TYPE_ADMIN:
                        admin_xml_value((char *) sctrl->send_buffer, get_admin_list());
                        ival->execute_status = EXECUTE_SUCCESS;
                        break;
                }
                ival->content_type = CONTENT_TYPE_XML;
                ival->content_length = strlen((char *) sctrl->send_buffer);            
            } else sctrl->status_code = INTERNAL_SERVER_ERROR;
        } else sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = INTERNAL_SERVER_ERROR;
}

void open_status_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct auth_data *adata;
    // auth error!
    _LOG_TRACE("+++++++++++++++++++++++++++++++++++++++++++++++++++");
    if ((adata = get_adata_bzl(ival->uid))) {
        _LOG_TRACE("adata->req_valid:%s, ival->req_valid:%s", adata->req_valid, ival->req_valid);
        if (!strcmp(adata->req_valid, ival->req_valid)) {
            AUTH_ACTIVE_SET(adata)
            SERVE_REFERIN_NEXT(ival->caddr.sin_addr, ival->caddr.sin_port);
            adata->serv_locked = TRUE;
            strcpy(adata->saddr.sin_addr, ival->caddr.sin_addr);
            adata->saddr.sin_port = ival->caddr.sin_port;
            // 
            struct xml_status status;
            status.uid = adata->uid;
            status.cached_anchor = adata->cached_anchor;
            strcpy(status.location, adata->location);
            strcpy(status.cached_seion, adata->seion_id);
            NEW_STATUS_VALUE(sctrl->send_buffer)
            if(sctrl->send_buffer){
                status_xml_value((char *) sctrl->send_buffer, &status);
                ival->content_type = CONTENT_TYPE_XML;
                ival->content_length = strlen((char *) sctrl->send_buffer);
                strcpy(adata->seion_id, random_seion_id());            
            } else sctrl->status_code = INTERNAL_SERVER_ERROR;
        } else {
            _LOG_TRACE("valid error! adata->req_valid:%s, ival->req_valid:%s", adata->req_valid, ival->req_valid);
// exit(1);
            sctrl->status_code = UNAUTHORIZED;
        }
    } else sctrl->status_code = BAD_REQUEST;
}

void open_set_anchor_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct auth_data *adata;
    //
    if ((adata = get_adata_bzl(ival->uid))) {
        _LOG_TRACE("adata->req_valid:%s, ival->req_valid:%s", adata->req_valid, ival->req_valid);
        if (!strcmp(adata->req_valid, ival->req_valid)) { // modify by james 20101015
            AUTH_ACTIVE_SET(adata)
            if (adata->serv_locked) {
                SERVE_REFEREN_DECREA(adata->saddr.sin_addr, adata->saddr.sin_port);
                adata->serv_locked = FALSE;
                if (ival->cached_anchor) adata->cached_anchor = ival->cached_anchor;
                ival->execute_status = EXECUTE_SUCCESS;
                strcpy(adata->req_valid, random_valid());
_LOG_TRACE("syncok! new adata->req_valid:%s", adata->req_valid);
            } else sctrl->status_code = CONFLICT;
        } else sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = BAD_REQUEST;
}

void open_set_locked_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct auth_data *adata;
    //
    if ((adata = get_adata_bzl(ival->uid))) {
        _LOG_TRACE("adata->req_valid:%s, ival->req_valid:%s", adata->req_valid, ival->req_valid);
        if (!strcmp(adata->req_valid, ival->req_valid)) { // modify by james 20101015
            AUTH_ACTIVE_SET(adata)
            if (adata->serv_locked && ival->serv_locked) sctrl->status_code = CONFLICT;
            else {
                if (adata->serv_locked && !ival->serv_locked) {
                    strcpy(adata->req_valid, random_valid());
_LOG_TRACE("set unlock, new adata->req_valid:%s", adata->req_valid);
                }
                adata->serv_locked = ival->serv_locked;
                if (adata->serv_locked) SERVE_REFERIN_NEXT(adata->saddr.sin_addr, adata->saddr.sin_port);
                else SERVE_REFEREN_DECREA(adata->saddr.sin_addr, adata->saddr.sin_port);
                ival->execute_status = EXECUTE_SUCCESS;
            }
        } else sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = BAD_REQUEST;
}

void open_keep_alive_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct auth_data *adata;
    //
    if ((adata = get_adata_bzl(ival->uid))) {
        if (!strcmp(adata->req_valid, ival->req_valid)) { // modify by james 20101015
            AUTH_ACTIVE_SET(adata)
            ival->execute_status = EXECUTE_SUCCESS;
        } else sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = BAD_REQUEST;
}
