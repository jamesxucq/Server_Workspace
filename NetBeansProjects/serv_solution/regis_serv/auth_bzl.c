/*
 * authbzl.c
 *
 *  Created on: 2010-3-10
 *      Author: Divad
 */

#include "rserv_bzl.h"
#include "auth_bzl.h"
#include "linked_status.h"


#define CREATE_AUTH_CONFIG(ACONFIG, RCONFIG)                                    \
    strcpy((ACONFIG)->address.sin_addr, (RCONFIG)->auth_db.address.sin_addr);   \
    (ACONFIG)->address.sin_port = (RCONFIG)->auth_db.address.sin_port;          \
    strcpy((ACONFIG)->database, (RCONFIG)->auth_db.database);                   \
    strcpy((ACONFIG)->password, (RCONFIG)->auth_db.password);                   \
    strcpy((ACONFIG)->user_name, (RCONFIG)->auth_db.user_name);

int authbzl_create(struct regis_config *config) {
    CREATE_AUTH_CONFIG(&_auth_config_, config);
    if (volid_auth_dbase(config->ping_auth.param_str)) {
        _LOG_INFO("connect auth_db failed!");
        return ERR_FAULT;
    }
    _LOG_INFO("connect auth_db ok!");
    return ERR_SUCCESS; //succ
}

int authbzl_destroy() {
    return ERR_SUCCESS; //succ
}

inline struct auth_config *get_auth_config_bzl() {
    return &_auth_config_;
}

int get_avalue_by_uname_bzl(struct auth_value *avalue, unsigned int *clinked, char *user_name, char *client_id) {
    struct linked_status link_stat;
    if (get_avalue_by_uname(avalue, &_auth_config_, user_name)) {
        _LOG_INFO("get auth value failed.");
        return -1;
    } else {
        if (find_linked_status(&link_stat, avalue->link_con, client_id)) {
            if (LINKED_STATUS_UNLINK == link_stat.link_stat || BOMB_STATUS_DONE == link_stat.data_bomb) {
                _LOG_INFO("client unlink or bomb done.");
                return -1;
            }
            *clinked = 0x01;
        } else *clinked = 0x00;
        decode_base64_text(avalue->password, avalue->password, avalue->_salt_);
    }
    return 0;
}

inline long int update_accesskey_bzl(struct auth_value * value) {
   // if()
    return update_access_key(&_auth_config_, value->uid, value->access_key, value->lease_time);
}

int get_sdata_by_uname_bzl(struct set_data *sdata, unsigned int *clinked, char *user_name, char *client_id) {
    struct set_value svalue;
    struct linked_status link_stat;
    if (get_svalue_by_uname(&svalue, &_auth_config_, user_name)) {
        _LOG_INFO("get auth value failed.");
        return -1;
    } else {
        if (find_linked_status(&link_stat, svalue.link_con, client_id)) {
            if (LINKED_STATUS_UNLINK == link_stat.link_stat || BOMB_STATUS_DONE == link_stat.data_bomb) {
                _LOG_INFO("client unlink or bomb done.");
                return -1;
            } else if (BOMB_STATUS_TRUE == link_stat.data_bomb) {
                if (modify_databomb(svalue.link_con, client_id, BOMB_STATUS_DONE)) {
                    CONVERT_SQLTEXT(svalue.link_con)
                    if (update_linked_status(&_auth_config_, svalue.uid, SQL_TEXT)) {
                        _LOG_INFO("update linked status failed.");
                        return -1;
                    }
                }
            }
            *clinked = 0x01;
        } else *clinked = 0x00;
    }
    //
    sdata->uid = svalue.uid;
    decode_base64_text(sdata->password, svalue.password, svalue._salt_);
    strcpy(sdata->version, svalue.version);
    sdata->pool_size = svalue.pool_size;
    sdata->data_bomb = link_stat.data_bomb;
    return 0;
}

int app_linkstat_bzl(unsigned int uid, char *client_id) {
    char link_con[LARGE_TEXT_SIZE];
    if (get_linked_status(&_auth_config_, link_con, uid)) {
        _LOG_INFO("get auth value failed.");
        return -1;
    } else {
        insert_linkstat_text(link_con, client_id);
        CONVERT_SQLTEXT(link_con)
        if (update_linked_status(&_auth_config_, uid, SQL_TEXT)) {
            _LOG_INFO("update linked status failed.");
            return -1;
        }
    }
    //
    return 0;
}

int get_lvalue_link_bzl(struct link_value *lvalue, char *user_name, char *client_id) {
    if (get_lvalue_by_uname(lvalue, &_auth_config_, user_name)) {
        _LOG_INFO("get auth value failed.");
        return -1;
    } else {
        if (!modify_linkstat_databomb(lvalue->link_con, client_id, LINKED_STATUS_LINKED, BOMB_STATUS_FALSE))
            add_linkstat_text(lvalue->link_con, client_id);
        CONVERT_SQLTEXT(lvalue->link_con)
        if (update_linked_status(&_auth_config_, lvalue->uid, SQL_TEXT)) {
            _LOG_INFO("update linked status failed.");
            return -1;
        }
        decode_base64_text(lvalue->password, lvalue->password, lvalue->_salt_);
    }
    return 0;
}

int lvalue_unlink_bzl(struct link_value *lvalue, char *user_name, char *client_id) {
    if (get_lvalue_by_uname(lvalue, &_auth_config_, user_name)) {
        _LOG_INFO("get auth value failed.");
        return -1;
    } else {
        if (modify_linkstat_databomb(lvalue->link_con, client_id, LINKED_STATUS_UNLINK, BOMB_STATUS_FALSE)) {
            CONVERT_SQLTEXT(lvalue->link_con)
            if (update_linked_status(&_auth_config_, lvalue->uid, SQL_TEXT)) {
                _LOG_INFO("update linked status failed.");
                return -1;
            }
        }
        decode_base64_text(lvalue->password, lvalue->password, lvalue->_salt_);
    }
    return 0;
}


