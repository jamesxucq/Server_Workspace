/*
 * cache_serv.c
 *
 *  Created on: 2010-3-5
 *      Author: David
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

#include "third_party.h"
#include "process_cycle.h"
#include "aserv_bzl.h"
#include "serve_bzl.h"
#include "auth_bzl.h"

#include "hand_epoll.h"
#include "epoll_serv.h"
#include "cache_serv.h"

//
int main(int argc, char *argv[]) {
    char xml_config[MAX_PATH * 3];
#ifdef _DEBUG
    fprintf(stderr, "debug %s.\n", PROGRAM_NAME);
#else
    fprintf(stderr, "release %s.\n", PROGRAM_NAME);
#endif
    //
    if (parse_args(xml_config, argc, argv)) return ERR_FAULT;
    if (FILE_EXIST(xml_config)) {
        fprintf(stderr, "%s configuration file not exist\n", xml_config);
        return EXIT_FAILURE;
    }
    //
    if (initialize_serv(xml_config)) {
        fprintf(stderr, "initialize_serv failed!\n");
        return ERR_FAULT;
    }
    fprintf(stderr, "initialize_serv ok!\n");
    struct cache_config *config = get_cache_config();
#ifndef _DEBUG
    if(daemonize(PROGRAM_NAME)) return ERR_FAULT;
#endif
    //
    short bind_port[MAX_BIND_PORT];
    int port_tatol = get_bind_port(bind_port, &config->ssev_db, config->cache_address);
    if(0x00 >= port_tatol) {
        fprintf(stderr, "parse bind port failed!\n");
        return ERR_FAULT;
    }
    // init log module
    log_inital(config->runtime_log.logfile, config->runtime_log.debug_level);
    access_inital(config->access_log.logfile);
    //
#ifndef _DEBUG
    master_process_cycle(config, bind_port, port_tatol);
#else
    _LOG_WARN("debug single mode!");
    single_process_cycle(config, bind_port);
#endif
    log_final();
    access_final();
    //
    if (finalize_serv(xml_config)) {
        _LOG_WARN("finalize_serv failed!");
        return ERR_FAULT;
    }
    _LOG_INFO("finalize_serv ok!");
    //
    return ERR_SUCCESS;
}

int initialize_serv(char *xml_config) {
    struct cache_config *config = NULL;
    // init serv gui module
    servbzl_create(&config, xml_config);
    fprintf(stderr, "configuration file name:%s\n", xml_config);
    // Start the dir watcher thread
    authbzl_create(config);
    // Start the sync thread
    cachebzl_create(config);
    //
    return ERR_SUCCESS;
}

int finalize_serv() {
    // Stop the sync thread
    cachebzl_destroy();
    // stop the dir watcher thread
    authbzl_destroy();
    // uninit client gui module
    servbzl_destroy();
    //
    return ERR_SUCCESS;
}

