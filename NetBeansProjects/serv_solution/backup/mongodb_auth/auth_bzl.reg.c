/*
 * authbzl.c
 *
 *  Created on: 2010-3-10
 *      Author: Divad
 */

#include "rserv_bzl.h"
#include "auth_ctrl.h"
#include "auth_bzl.h"
#include "linked_status.h"

//
#define CREATE_AUTH_CONFIG(ACONFIG, RCONFIG)                                    \
    strcpy((ACONFIG)->address.sin_addr, (RCONFIG)->auth_db.address.sin_addr);   \
    (ACONFIG)->address.sin_port = (RCONFIG)->auth_db.address.sin_port;          \
    strcpy((ACONFIG)->database, (RCONFIG)->auth_db.database);                   \
    strcpy((ACONFIG)->password, (RCONFIG)->auth_db.password);                   \
    strcpy((ACONFIG)->user_name, (RCONFIG)->auth_db.user_name);

static mongoc_client_t *reconn_client(mongoc_client_t *client, struct auth_config *config) {
    mongoc_client_destroy (client);
    mongoc_cleanup ();
    //
    mongoc_init ();
    char uristr[URI_LENGTH];
    sprintf(uristr, "mongodb://%s:%s@%s:%d/%s", config->user_name, config->password, 
            config->address.sin_addr, config->address.sin_port, config->database);
    mongoc_client_t *client_new = mongoc_client_new (uristr);
    return client_new;
}

int authbzl_create(struct regist_config *config) {
    CREATE_AUTH_CONFIG(&_auth_config_, config);
    //
    mongoc_init ();
    char uristr[URI_LENGTH];
    sprintf(uristr, "mongodb://%s:%s@%s:%d/%s", _auth_config_.user_name, 
            _auth_config_.password, _auth_config_.address.sin_addr, 
            _auth_config_.address.sin_port, _auth_config_.database);
    _auth_client_ = mongoc_client_new (uristr);
    if(!_auth_client_) {
        _LOG_WARN("connect auth_db clent failed!");
        return ERR_FAULT;
    }
    //
    if (volid_auth_dbase()) {
        _LOG_WARN("connect auth_db failed!");
        return ERR_FAULT;
    }
    _LOG_INFO("connect auth_db ok!");
//
    return ERR_SUCCESS; //succ
}

int authbzl_destroy() {
    mongoc_client_destroy (_auth_client_);
    mongoc_cleanup ();
    return ERR_SUCCESS; //succ
}

inline struct auth_config *get_auth_config_bzl() {
    return &_auth_config_;
}

int get_avalue_by_uname_bzl(struct auth_value *avalue, unsigned int *clinked, char *user_name, char *client_id) {
    struct linked_status link_stat;
    int mon_value = get_avalue_by_uname(avalue, _auth_client_, _auth_config_.database, user_name);
    if (mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = get_avalue_by_uname(avalue, _auth_client_, _auth_config_.database, user_name);
        if (mon_value) {
            _LOG_WARN("get auth value failed.");
            return -1;
        }
    }
//
    if (find_linked_status(&link_stat, avalue->linked_status, client_id)) {
        if (LINKED_STATUS_UNLINK == link_stat.link_stat || BOMB_STATUS_DONE == link_stat.data_bomb) {
            _LOG_WARN("client unlink or bomb done.");
            return -1;
        }
        *clinked = 0x01;
    } else *clinked = 0x00;
    decode_base64_text(avalue->password, avalue->password, avalue->_salt_);
    return 0;
}

inline long int update_accesskey_bzl(struct auth_value * value) {
    int mon_value = update_access_key(_auth_client_, _auth_config_.database, value->uid, value->access_key, value->lease_time);
    if (mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = update_access_key(_auth_client_, _auth_config_.database, value->uid, value->access_key, value->lease_time);
    } 
    return mon_value;
}

int get_sdata_by_uname_bzl(struct set_data *sdata, unsigned int *clinked, char *user_name, char *client_id) {
    struct set_value svalue;
    struct linked_status link_stat;
    int mon_value = get_svalue_by_uname(&svalue, _auth_client_, _auth_config_.database, user_name);
    if (mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = get_svalue_by_uname(&svalue, _auth_client_, _auth_config_.database, user_name);
        if (mon_value) {
            _LOG_WARN("get auth value failed.");
            return -1;
        }
    } 
//
    if (find_linked_status(&link_stat, svalue.linked_status, client_id)) {
        if (LINKED_STATUS_UNLINK == link_stat.link_stat || BOMB_STATUS_DONE == link_stat.data_bomb) {
            _LOG_WARN("client unlink or bomb done.");
            return -1;
        } else if (BOMB_STATUS_TRUE == link_stat.data_bomb) {
            if (modify_databomb(svalue.linked_status, client_id, BOMB_STATUS_DONE)) {
                mon_value = update_linked_status(_auth_client_, _auth_config_.database, svalue.uid, svalue.linked_status);
                if (mon_value) {
                    _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
                    mon_value = update_linked_status(_auth_client_, _auth_config_.database, svalue.uid, svalue.linked_status);
                    if (mon_value) {
                        _LOG_WARN("update linked status failed.");
                        return -1;
                    }
                }
            }
        }
        *clinked = 0x01;
    } else *clinked = 0x00;
    //
    sdata->uid = svalue.uid;
    decode_base64_text(sdata->password, svalue.password, svalue._salt_);
    strcpy(sdata->version, svalue.version);
    sdata->pool_size = svalue.pool_size;
    sdata->data_bomb = link_stat.data_bomb;
    return 0;
}

int app_linkstat_bzl(unsigned int uid, char *client_id) {
    char linked_status[LARGE_TEXT_SIZE];
    int mon_value = get_linked_status(_auth_client_, _auth_config_.database, linked_status, uid);
    if (mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = get_linked_status(_auth_client_, _auth_config_.database, linked_status, uid);
        if (mon_value) {
            _LOG_WARN("get linked status failed.");
            return -1;
        }
    }
    //
    insert_linkstat_text(linked_status, client_id);
    mon_value = update_linked_status(_auth_client_, _auth_config_.database, uid, linked_status);
    if (mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = update_linked_status(_auth_client_, _auth_config_.database, uid, linked_status);
        if (mon_value) {
            _LOG_WARN("update linked status failed.");
            return -1;
        }
    }
    //
    return 0;
}

int get_lvalue_link_bzl(struct link_value *lvalue, char *user_name, char *client_id) {
    int mon_value = get_lvalue_by_uname(lvalue, _auth_client_, _auth_config_.database, user_name);
    if (mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = get_lvalue_by_uname(lvalue, _auth_client_, _auth_config_.database, user_name);
        if (mon_value) {
            _LOG_WARN("get auth value failed.");
            return -1;
        }
    } 
//
    if (!modify_linkstat_databomb(lvalue->linked_status, client_id, LINKED_STATUS_LINKED, BOMB_STATUS_FALSE))
        add_linkstat_text(lvalue->linked_status, client_id);
    mon_value = update_linked_status(_auth_client_, _auth_config_.database, lvalue->uid, lvalue->linked_status);
    if (mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = update_linked_status(_auth_client_, _auth_config_.database, lvalue->uid, lvalue->linked_status);
        if (mon_value) {
            _LOG_WARN("update linked status failed.");
            return -1;
        }
    }
    decode_base64_text(lvalue->password, lvalue->password, lvalue->_salt_);
    return 0;
}

int lvalue_unlink_bzl(struct link_value *lvalue, char *user_name, char *client_id) {
    int mon_value = get_lvalue_by_uname(lvalue, _auth_client_, _auth_config_.database, user_name);
    if (mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = get_lvalue_by_uname(lvalue, _auth_client_, _auth_config_.database, user_name);
        if (mon_value) {
            _LOG_WARN("get auth value failed.");
            return -1;
        }
    } 
//
    if (modify_linkstat_databomb(lvalue->linked_status, client_id, LINKED_STATUS_UNLINK, BOMB_STATUS_FALSE)) {
        mon_value = update_linked_status(_auth_client_, _auth_config_.database, lvalue->uid, lvalue->linked_status);
        if (mon_value) {
            _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
            mon_value = update_linked_status(_auth_client_, _auth_config_.database, lvalue->uid, lvalue->linked_status);
            if (mon_value) {
                _LOG_WARN("update linked status failed.");
                return -1;
            }
        }
    }
    decode_base64_text(lvalue->password, lvalue->password, lvalue->_salt_);
    return 0;
}

inline int volid_auth_dbase() {
    int mon_value = ping_auth_dbase(_auth_client_, _auth_config_.database);
    if(mon_value) {
        _auth_client_ = reconn_client(_auth_client_, &_auth_config_);
        mon_value = ping_auth_dbase(_auth_client_, _auth_config_.database);
    }
    return mon_value;
}

