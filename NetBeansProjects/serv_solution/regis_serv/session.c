#include "rserv_bzl.h"
#include "auth_bzl.h"
#include "session.h"
#include "settings_xml.h"

//
int _lease_time_;

//
void open_regis_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct auth_value avalue;
    char author_chks[MD5_TEXT_LENGTH];
    unsigned int clinked = 0x00;
    //
    if (!get_avalue_by_uname_bzl(&avalue, &clinked, ival->user_name, ival->client_id)) {
        long now = time(NULL); // lease time timeout check here
        if (avalue.lease_time && ((now-avalue.lease_time)>=_lease_time_)) {
            strcpy(avalue.access_key, random_accesskey());
            avalue.lease_time = now;
            update_accesskey_bzl(&avalue);
        } // else sctrl->status_code = BAD_REQUEST; // delete by james 20131101
        reply_auth_chks(author_chks, ival->user_name, avalue.password);
_LOG_DEBUG("ival->authorize:%s author_chks:%s", ival->authorize, author_chks);
// _LOG_DEBUG("user_name:%s password:%s", ival->user_name, avalue.password);
        if (!strcmp(ival->authorize, author_chks)) {
            if(!clinked) app_linkstat_bzl(avalue.uid, ival->client_id);
            struct xml_register regis;
            regis.uid = avalue.uid;
            strcpy(regis.cache_addr.sin_addr, avalue.saddr.sin_addr);
            regis.cache_addr.sin_port = avalue.saddr.sin_port;
            strcpy(regis.access_key, avalue.access_key);
            NEW_REGIS_VALUE(sctrl->send_buffer)
            if(sctrl->send_buffer) {
                regis_xml_value((char *) sctrl->send_buffer, &regis);
                ival->content_type = CONTENT_TYPE_XML;
                ival->content_length = strlen((char *) sctrl->send_buffer);            
            } else sctrl->status_code = INTERNAL_SERVER_ERROR;
        } else sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = BAD_REQUEST;
}

void open_settings_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct set_data sdata;
    char author_chks[MD5_TEXT_LENGTH];
    unsigned int clinked = 0x00;
    //
    if (!get_sdata_by_uname_bzl(&sdata, &clinked, ival->user_name, ival->client_id)) {
        reply_auth_chks(author_chks, ival->user_name, sdata.password);
_LOG_DEBUG("ival->authorize:%s author_chks:%s", ival->authorize, author_chks);
// _LOG_DEBUG("user_name:%s password:%s", ival->user_name, sdata.password);
        if (!strcmp(ival->authorize, author_chks)) {
            if(!clinked) app_linkstat_bzl(sdata.uid, ival->client_id);
            struct xml_settings settings;
            settings.uid = sdata.uid;
            settings.pool_size = sdata.pool_size;
            strcpy(settings.client_version, sdata.version);
            settings.data_bomb = sdata.data_bomb;
            NEW_SETTINGS_VALUE(sctrl->send_buffer)
            if(sctrl->send_buffer) {
                settings_xml_value((char *) sctrl->send_buffer, &settings);
                ival->content_type = CONTENT_TYPE_XML;
                ival->content_length = strlen((char *) sctrl->send_buffer);
            } else sctrl->status_code = INTERNAL_SERVER_ERROR;
        } else sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = BAD_REQUEST;
}

void open_link_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct link_value lvalue;
    char author_chks[MD5_TEXT_LENGTH];
    //
    if (!get_lvalue_link_bzl(&lvalue, ival->user_name, ival->client_id)) {
        reply_auth_chks(author_chks, ival->user_name, lvalue.password);
_LOG_DEBUG("ival->authorize:%s author_chks:%s", ival->authorize, author_chks);
// _LOG_DEBUG("user_name:%s password:%s", ival->user_name, lvalue.password);
        if (strcmp(ival->authorize, author_chks)) sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = BAD_REQUEST;
}

void open_unlink_se(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct link_value lvalue;
    char author_chks[MD5_TEXT_LENGTH];
    //
    if (!lvalue_unlink_bzl(&lvalue, ival->user_name, ival->client_id)) {
        reply_auth_chks(author_chks, ival->user_name, lvalue.password);
_LOG_DEBUG("ival->authorize:%s author_chks:%s", ival->authorize, author_chks);
// _LOG_DEBUG("user_name:%s password:%s", ival->user_name, lvalue.password);
        if (strcmp(ival->authorize, author_chks)) sctrl->status_code = UNAUTHORIZED;
    } else sctrl->status_code = BAD_REQUEST;
}
