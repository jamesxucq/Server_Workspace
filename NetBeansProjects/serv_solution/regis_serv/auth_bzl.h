/*
 * File:   auth_bzl.h
 * Author: David
 *
 * Created on: 2010-3-10
 */

#ifndef AUTHBZL_H
#define	AUTHBZL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_header.h"
#include "auth_ctrl.h"
#include "parse_conf.h"

    struct set_data {
        unsigned int uid;
        char password[PASSWORD_LENGTH];
        char version[VERSION_LENGTH];
        int pool_size;
        int data_bomb;
    };

    int authbzl_create(struct regis_config *config);
    int authbzl_destroy();

    /*auth_db operator section*/
    inline struct auth_config *get_auth_config_bzl();
    int get_avalue_by_uname_bzl(struct auth_value *avalue, unsigned int *clinked, char *user_name, char *client_id);
    inline long int update_accesskey_bzl(struct auth_value *value);
    int get_sdata_by_uname_bzl(struct set_data *sdata, unsigned int *clinked, char *user_name, char *client_id);
    int app_linkstat_bzl(unsigned int uid, char *client_id);
    int set_cache_anchor_bzl(int uid);

    int get_lvalue_link_bzl(struct link_value *lvalue, char *user_name, char *client_id);
    int lvalue_unlink_bzl(struct link_value *lvalue, char *user_name, char *client_id);

#define volid_auth_dbase(user_ping) ping_auth_dbase(&_auth_config_, user_ping)

#ifdef	__cplusplus
}
#endif

#endif	/* AUTHBZL_H */

