#ifndef _PARSECONF_H_
#define _PARSECONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common_header.h"

//
    typedef struct {
        char logfile[MAX_PATH * 3];
        log_lvl_t debug_level;
    } log_config;

    typedef struct {
        int max_modify;
        int modi_interv;
        int max_delete;
        int dele_interv;
    } bakup_config;

    struct serv_config {
        // server info
        char user_name[_USERNAME_LENGTH_];
        char group_name[GROUPNAME_LENGTH];
        uid_t _user_id_;
        gid_t _group_id_;
        char serv_address[SERVNAME_LENGTH];
        // short bind_port[MAX_BIND_PORT];
        int max_connecting;
        int time_out;
        int seion_timeout;
        char lock_file[MAX_PATH * 3];
        char pid_file[MAX_PATH * 3];
        anchor_files anchor;
        // server config
        struct ssev_config ssev_db;
        struct user_ping ping_ssev;
        // backup control
        bakup_config bakup_ctrl;
        // log config
        log_config runtime_log;
        log_config access_log;
    };

    extern struct serv_config _serv_config_;
    extern char _config_path_[MAX_PATH * 3];

    int serv_config_from_xml(struct serv_config *config, const char *xml_config);
    void serialize(char *sbuf, struct serv_config *config);

#ifdef __cplusplus
}
#endif

#endif /* _PARSECONF_H_ */
