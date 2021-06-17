/* 
 * File:   main.c
 * Author: Administrator
 *
 * Created on 2012
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "utility.h"
#include "clear.h"
#include "logger.h"
#include "auth_ctrl.h"
#include "lock_pool.h"
#include "config.h"

/*
 * 
 */
#define ONCE_COUNT 100

int initialize_env(struct erase_config *eraconfig, char *xml_config);
int finalize_env();

void clear_backup(struct erase_config *eraconfig) {
    ////////////////////////////////////////////////////////////////////////////
    struct auth_value avalue[ONCE_COUNT];
    char validation[VALID_LENGTH];
    struct erase_space *eraspa = &eraconfig->eraspa;

    unsigned int uid = eraspa->start_uid;
    int count, auth_num, once_count;
    for (count = 0; count < eraspa->number; uid += once_count, count += once_count) {
        if (count + ONCE_COUNT <= eraspa->number) once_count = ONCE_COUNT;
        else once_count = eraspa->number - count;
        LOG_INFO("++++++++++++++++++++++++++++++++++++++++");
        if (0 < (auth_num = get_auth_value(avalue, &eraconfig->auth_db, uid, once_count))) {
            LOG_INFO("auth: [%d,%d] value: %d", uid, once_count, auth_num);
            int index;
            for (index = 0; index < auth_num; index++) {
                LOG_INFO("------------------------------");
                LOG_INFO("clear user uid: %d", avalue[index].uid);
                if (!valid_cache_locked(validation, &avalue[index])) {
                    clear_unlink(&avalue[index], &eraconfig->eralink, &eraconfig->auth_db);
                    clear_handle(&avalue[index], eraspa);
                    unlocked_cache(&avalue[index], validation);
                }
                sleep(eraspa->sleep);
            }
        } else LOG_INFO("auth: [%d,%d] value: %d", uid, once_count, auth_num);
    }
}

void print_erase(struct erase_config *eraconfig) {
    struct erase_space *eraspa = &eraconfig->eraspa;
    LOG_INFO("start_uid: %d number: %d sleep: %d anchor_day: %d", eraspa->start_uid, eraspa->number, eraspa->sleep, eraspa->anchor_day);
    LOG_INFO("backup_day: %d max_length: %d delim_count: %d path: %s", eraspa->backup.backup_day, eraspa->backup.max_length, eraspa->local.delim_count, eraspa->local.path);
    struct erase_link *eralink = &eraconfig->eralink;
    LOG_INFO("unlink_day: %d bomb_day: %d", eralink->unlink_day, eralink->bomb_day);
    struct auth_config *aconfig = &eraconfig->auth_db;
    LOG_INFO("sin_addr: %s sin_port: %d database: %s  user_name: %s", aconfig->address.sin_addr, aconfig->address.sin_port, aconfig->database, aconfig->user_name);
    log_config *lconfig = &eraconfig->runtime_log;
    LOG_INFO("debug_level: %d logfile: %s", lconfig->debug_level, lconfig->logfile);
}

int main(int argc, char** argv) {
    char xml_config[MAX_PATH * 3];

    if (parse_args(xml_config, argc, argv)) return ERR_FAULT;
    if (FILE_EXISTS(xml_config)) {
        printf("%s configuration file not exists\n", xml_config);
        return EXIT_FAILURE;
    }

    struct erase_config eraconfig;
    if (initialize_env(&eraconfig, xml_config)) {
        LOG_INFO("initialize_serv failed!");
        return ERR_FAULT;
    }
    LOG_INFO("initialize_serv OK");

    char timestr[TIME_LENGTH];
    time_t utctime = time(NULL);
    UTC_TIME_TEXT(timestr, utctime)
    LOG_INFO("today: %s epoch day: %d", timestr, time(NULL) / SECONDS_ONEDAY);

    ////////////////////////////////////////////////////////////////////////////
    LOG_INFO("******************** configuration ********************");
    print_erase(&eraconfig);
    LOG_INFO("******************** clear backup ********************");
    clear_backup(&eraconfig);
    LOG_INFO("******************** finished ********************");

    ////////////////////////////////////////////////////////////////////////////
    if (finalize_env(xml_config)) {
        LOG_INFO("finalize_serv failed!");
        return ERR_FAULT;
    }
    LOG_INFO("finalize_serv OK");

    return (EXIT_SUCCESS);
}

int initialize_env(struct erase_config *eraconfig, char *xml_config) {
    // init serv gui module
    parse_create(eraconfig, xml_config);

    // init log module
    log_init(eraconfig->runtime_log.logfile, eraconfig->runtime_log.debug_level);

    return ERR_SUCCESS;
}

int finalize_env() {
    log_fini();

    // uninit client gui module
    parse_destroy();

    return ERR_SUCCESS;
}