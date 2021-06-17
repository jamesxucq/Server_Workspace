/* 
 * File:   main.c
 * Author: Administrator
 */

#include <stdio.h>
#include <stdlib.h>

#include "utility.h"
#include "logger.h"
#include "auth_ctrl.h"
#include "config.h"

//
int initialize_env(struct aupa_config *aupconfig, char *xml_config);
int finalize_env();

//
int dele_user(struct aupa_config *aupconfig, char *user_name) {
    char location[MAX_PATH*3];
    ////////////////////////////////////////////////////////////////////////////
    if(get_auth_value(location, &aupconfig->auth_db, user_name)) {
        LOG_INFO("get auth value failed!");
        return -1;
    }
    LOG_INFO("query user location ok.");
    //
    if(dele_user_query(&aupconfig->auth_db, user_name)) {
        LOG_INFO("dele user auth failed!");
        return -1;
    }
    LOG_INFO("dele user from database ok.");
    //
    if(recu_rmdir(location)) {
        LOG_INFO("recu rmdir failed!");
        return -1;
    }
    LOG_INFO("dele user location ok.");
    //
    return 0;
}

void print_erase(struct aupa_config *aupconfig) {
/*
    struct erase_space *eraspa = &eraconfig->eraspa;
    LOG_INFO("start_uid: %d number: %d sleep: %d anchor_day: %d", eraspa->start_uid, eraspa->number, eraspa->sleep, eraspa->anchor_day);
    LOG_INFO("backup_day: %d max_length: %d delim_count: %d path: %s", eraspa->backup.backup_day, eraspa->backup.max_length, eraspa->local.delim_count, eraspa->local.path);
    struct erase_link *eralink = &eraconfig->eralink;
    LOG_INFO("unlink_day: %d bomb_day: %d", eralink->unlink_day, eralink->bomb_day);
    struct auth_config *aconfig = &eraconfig->auth_db;
    LOG_INFO("sin_addr: %s sin_port: %d database: %s  user_name: %s", aconfig->address.sin_addr, aconfig->address.sin_port, aconfig->database, aconfig->user_name);
    log_config *lconfig = &eraconfig->runtime_log;
    LOG_INFO("debug_level: %d logfile: %s", lconfig->debug_level, lconfig->logfile);
*/
}

//
int main(int argc, char** argv) {
    char user_name[MID_TEXT_LEN];
    char xml_config[MAX_PATH * 3];
//
    if (parse_args(user_name, xml_config, argc, argv)) return ERR_FAULT;
    if (FILE_EXISTS(xml_config)) {
        printf("%s configuration file not exists\n", xml_config);
        return EXIT_FAILURE;
    }
//
    struct aupa_config aupconfig;
    if (initialize_env(&aupconfig, xml_config)) {
        LOG_INFO("initialize_serv failed!");
        return ERR_FAULT;
    }
    LOG_INFO("initialize_serv OK");
    ////////////////////////////////////////////////////////////////////////////
    LOG_INFO("******************** configuration ********************");
    print_erase(&aupconfig);
    LOG_INFO("parse config ok.");
    LOG_INFO("******************** dele user ********************");
    if(dele_user(&aupconfig, user_name)) {
        LOG_INFO("delete user failed!");
    }
    LOG_INFO("dele user ok.");
    LOG_INFO("******************** finished ********************");
    ////////////////////////////////////////////////////////////////////////////
    if (finalize_env(xml_config)) {
        LOG_INFO("finalize_serv failed!");
        return ERR_FAULT;
    }
    LOG_INFO("finalize_serv OK");
//
    return (EXIT_SUCCESS);
}

int initialize_env(struct aupa_config *aupconfig, char *xml_config) {
    // init serv gui module
    parse_create(aupconfig, xml_config);
    // init log module
    log_init(aupconfig->runtime_log.logfile, aupconfig->runtime_log.debug_level);
//
    return ERR_SUCCESS;
}

int finalize_env() {
    log_fini();
    // uninit client gui module
    parse_destroy();
//
    return ERR_SUCCESS;
}
