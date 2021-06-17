/* 
 * File:   config.h
 * Author: Administrator
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "auth_ctrl.h"
    
typedef struct {
        char logfile[MAX_PATH * 3];
        int debug_level;
} log_config;
//
struct aupa_config {
    int pool_size;
    char location[MAX_PATH * 3];
    char auth_host[MID_TEXT_LEN];
    char version[MID_TEXT_LEN];
    struct auth_config auth_db;
    log_config runtime_log;
};

    int parse_create(struct aupa_config *config, char *xml_config);
    inline int parse_destroy();

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

