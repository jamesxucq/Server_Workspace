/* parse_conf.h
 * Created on: 2010-3-8
 * Author: David
 */

#ifndef _PARSECONF_H_
#define _PARSECONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "auth_stru.h"

    //

    typedef struct {
        char user_name[_USERNAME_LENGTH_];
        char password[PASSWORD_LENGTH];
    } listen_admin;

    typedef struct {
        char logfile[MAX_PATH * 3];
        int debug_level;
    } log_config;

    struct cache_config {
        // server info
        char user_name[_USERNAME_LENGTH_];
        char group_name[GROUPNAME_LENGTH];
        uid_t _user_id_;
        gid_t _group_id_;
        char cache_address[SERVNAME_LENGTH];
        int max_connecting;
        int time_out;
        int seion_timeout;
        char lock_file[MAX_PATH * 3];
        char pid_file[MAX_PATH * 3];
        // listen syncing server address
        listen_admin admin;
        struct addr_list *alist;
        // server config
        struct ssev_config ssev_db;
        struct user_ping ping_ssev;
        // auth DB
        struct auth_config auth_db;
        struct user_ping ping_auth;
        // log config
        log_config runtime_log;
        log_config access_log;
    };

    extern struct cache_config _cache_config_;
    extern char _config_path_[MAX_PATH * 3];

    int cache_config_from_xml(struct cache_config *config, const char *xml_config);
    void serialize(char *sbuf, struct cache_config *config);

#ifdef __cplusplus
}
#endif

#endif /* _PARSECONF_H_ */
