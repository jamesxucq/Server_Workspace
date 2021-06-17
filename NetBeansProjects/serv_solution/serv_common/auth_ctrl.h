/*
 * auth_ctrl.h
 *
 *  Created on: 2010-3-10
 *      Author: David
 */

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef AUTHCTRL_H_
#define AUTHCTRL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"
#include "string_utility.h"
#include "addr_list.h"
#include "auth_stru.h"
    
//
extern struct auth_config _auth_config_;

//
int ping_auth_dbase(struct auth_config *config, const char *ping_name);
int ping_ssev_dbase(struct auth_config *config, const char *ping_addr);

//
int get_avalue_by_uname(struct auth_value *avalue, struct auth_config *config, const char *user_name);
int get_cvalue_by_uid(struct cache_value *cvalue, struct auth_config *config, unsigned int uid);
int update_access_key(struct auth_config *config, unsigned int uid, char *access_key, long int lease_time);
int get_linked_status(struct auth_config *config, char *link_con, unsigned int uid);
int update_linked_status(struct auth_config *config, unsigned int uid, char *link_con);

//inline void set_struct auth_config(struct struct auth_config *pAuthConf);
#define DELETE_AUTH_VALUE(AUTH_VALUE)     { free(AUTH_VALUE); AUTH_VALUE = NULL; }

//
int get_svalue_by_uname(struct set_value *svalue, struct auth_config *config, const char *user_name);
int get_lvalue_by_uname(struct link_value *lvalue, struct auth_config *config, const char *user_name);

#define CONVERT_SQLTEXT(TEXT) \
    char SQL_TEXT[LARGE_TEXT_SIZE]; \
    char *toke = TEXT, *sql = SQL_TEXT; \
    while('\0' != *toke) { \
        if('\"' == *toke) {*sql = '\\'; sql++;} \
        *sql = *toke; \
        toke++; sql++; \
    } \
    *sql = '\0';

//
struct ssev_addr{
    char sin_addr[SERVNAME_LENGTH];
    char sin_port[MAX_PORT_LEN];
};
int get_ssev_by_auth(struct ssev_addr *saddr, struct ssev_config *config, const char *cache_addr, int cache_port);
int get_aport_by_auth(short *bind_port, struct ssev_config *config, const char *cache_addr);
int get_auth_by_ssev(serv_addr *caddr, struct ssev_config *config, const char *serv_addr);
int get_sport_by_ssev(char *bind_port, struct ssev_config *config, const char *serv_addr);

#ifdef	__cplusplus
}
#endif

#endif /* AUTHCTRL_H_ */
