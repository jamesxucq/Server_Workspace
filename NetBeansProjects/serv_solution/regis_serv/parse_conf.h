/*
 * parse_conf.h
 *
 *  Created on: 2010-3-8
 *      Author: Divad
 */

#ifndef _PARSECONF_H_
#define _PARSECONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common_header.h"
#include "auth_stru.h"

    typedef struct {
        char logfile[MAX_PATH * 3];
        int debug_level;
    } log_config;

    struct regis_config {
        //Server info
        char user_name[_USERNAME_LENGTH_];
        char group_name[GROUPNAME_LENGTH];
        uid_t _user_id_;
        gid_t _group_id_;
        short bind_port[MAX_BIND_PORT];
        int max_connecting;
        int time_out;
        int lease_time;
        char lock_file[MAX_PATH * 3];
        char pid_file[MAX_PATH * 3];
        //Auth DB
        struct auth_config auth_db;
        struct user_ping ping_auth;
        // log config
        log_config runtime_log;
        log_config access_log;
    };

    extern struct regis_config _regis_config_;
    extern char _config_path_[MAX_PATH * 3];

    int regis_config_from_xml(struct regis_config *config, const char *xml_config);
    void serialize(char *sbuf, struct regis_config *config);

#ifdef __cplusplus
}
#endif

#endif /* _PARSECONF_H_ */
