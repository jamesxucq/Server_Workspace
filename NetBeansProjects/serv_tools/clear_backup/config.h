/* 
 * File:   config.h
 * Author: Administrator
 *
 * Created on 2012年9月12日, 上午9:27
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "auth_ctrl.h"
#include "clear.h"

    typedef struct {
        char logfile[MAX_PATH * 3];
        int debug_level;
    } log_config;

    struct erase_config {
        struct erase_space eraspa;
        struct erase_link eralink;
        struct auth_config auth_db;
        log_config runtime_log;
    };

    int parse_create(struct erase_config *config, char *xml_config);
    inline int parse_destroy();

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

