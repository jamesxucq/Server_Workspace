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

#include <mongoc.h>
#include "common_macro.h"
#include "string_utility.h"
#include "addr_list.h"
#include "auth_stru.h"

//
extern struct auth_config _auth_config_;
extern mongoc_client_t *_auth_client_;

//
int ping_auth_dbase(mongoc_client_t *client, char *dabe_name);

//
int get_avalue_by_uname(struct auth_value *avalue, mongoc_client_t *client, char *dabe_name, const char *user_name);
int get_cvalue_by_uid(struct cache_value *cvalue, mongoc_client_t *client, char *dabe_name, unsigned int uid);
int update_access_key(mongoc_client_t *client, char *dabe_name, unsigned int uid, char *access_key, long int lease_time);
int get_linked_status(mongoc_client_t *client, char *dabe_name, char *linked_status, unsigned int uid);
int update_linked_status(mongoc_client_t *client, char *dabe_name, unsigned int uid, char *linked_status);

//inline void set_struct auth_config(struct struct auth_config *pAuthConf);
#define DELETE_AUTH_VALUE(AUTH_VALUE)     { free(AUTH_VALUE); AUTH_VALUE = NULL; }

//
int get_svalue_by_uname(struct set_value *svalue, mongoc_client_t *client, char *dabe_name, const char *user_name);
int get_lvalue_by_uname(struct link_value *lvalue, mongoc_client_t *client, char *dabe_name, const char *user_name);

//
int get_ssev_by_auth(struct addr_list **alsth, struct ssev_config *config, const char *cache_id);
int get_auth_by_ssev(char *cache_id, struct ssev_config *config, const char *serv_addr, int serv_port);

#ifdef	__cplusplus
}
#endif

#endif /* AUTHCTRL_H_ */
