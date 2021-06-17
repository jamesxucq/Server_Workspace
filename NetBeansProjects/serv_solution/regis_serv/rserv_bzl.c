/*
 * aserv_bzl.c
 *
 *  Created on: 2010-3-10
 *      Author: Divad
 */

#include "rserv_bzl.h"

int servbzl_create(struct regis_config **config, char *xml_config) {
    *config = load_regis_config(xml_config);
    if (!(*config)) {
        _LOG_WARN("Load config failed!");
        return ERR_FAULT;
    }
    _LOG_INFO("Load config ok!");

    return ERR_SUCCESS; //succ
}

int servbzl_destroy() {

    return ERR_SUCCESS; //succ
}

struct regis_config *load_regis_config(const char *config_path) {
    if (!config_path) return NULL;
//
    strcpy(_config_path_, (char *) config_path);
    if (regis_config_from_xml(&_regis_config_, config_path)) return NULL;
    find_user_group(_regis_config_.user_name, _regis_config_.group_name, &_regis_config_._user_id_, &_regis_config_._group_id_);
//
    return &_regis_config_;
}

inline unsigned int get_port_tatol() {
    unsigned int port_tatol;
    for (port_tatol = 0; 0 != _regis_config_.bind_port[port_tatol]; port_tatol++);
    return port_tatol;
}

inline void set_regis_config_file(const char *config_path) {
    strcpy(_config_path_, (char *) config_path);
}

inline char *get_regis_config_file() {
    return _config_path_;
}

inline void set_regis_config(struct regis_config *config) {
    memcpy(&_regis_config_, config, sizeof (struct regis_config));
}

inline struct regis_config *get_regis_config() {
    return &_regis_config_;
}
