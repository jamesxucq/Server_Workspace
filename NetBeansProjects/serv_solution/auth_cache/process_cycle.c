#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <wait.h>
#include<grp.h>
#include <pwd.h>

#include "epoll_serv.h"
#include "hand_epoll.h"
#include "process_cycle.h"

//
void worker_process_init(struct cache_config *config) {
    /*
        if (ngx_set_environment(cycle, NULL) == NULL) {
            exit(2);
        }
        if (worker >= 0 && ccf->priority != 0) {
            if (setpriority(PRIO_PROCESS, 0, ccf->priority) == -1) {
                _LOG_ERROR("setpriority(%d) failed", ccf->priority);
            }
        }
        if (ccf->rlimit_nofile != CONF_UNSET) {
            rlmt.rlim_cur = (rlim_t) ccf->rlimit_nofile;
            rlmt.rlim_max = (rlim_t) ccf->rlimit_nofile;
            if (setrlimit(RLIMIT_NOFILE, &rlmt) == -1) {
                _LOG_ERROR("setrlimit(RLIMIT_NOFILE, %i) failed", ccf->rlimit_nofile);
            }
        }
        if (ccf->rlimit_core != CONF_UNSET) {
            rlmt.rlim_cur = (rlim_t) ccf->rlimit_core;
            rlmt.rlim_max = (rlim_t) ccf->rlimit_core;
            if (setrlimit(RLIMIT_CORE, &rlmt) == -1) {
                _LOG_ERROR("setrlimit(RLIMIT_CORE, %O) failed", ccf->rlimit_core);
            }
        }
     */
    //
    if (0x00 == geteuid()) {
        if (0 == config->_group_id_) {
            _LOG_WARN("I will not set gid to 0");
            return;
        }
        //
        if (setgid(config->_group_id_) == -1) {
            _LOG_FATAL("setgid(%d) failed", config->_group_id_);
            exit(2); /* fatal */
        }
        //
        if (initgroups(config->user_name, config->_group_id_) == -1) {
            _LOG_FATAL("initgroups(%s, %d) failed", config->user_name, config->_group_id_);
        }
        if (setuid(config->_user_id_) == -1) {
            _LOG_FATAL("setuid(%d) failed", config->_user_id_);
            exit(2); /* fatal */
        }
    }
}

//
#define RETRY_START_TIMES       1024
#define RETRY_START_DELAY       32 // 32s
pid_t start_worker_process(struct cache_config *config, short alloc_port) {
        _LOG_INFO("alloc port:%d", alloc_port);
        int retry_times = 0x00;
        //
        pid_t child = fork();
        switch (child) {
            case -1:
                _LOG_WARN("fork worker failed!, alloc_port%d", alloc_port);
            case 0: // child
                worker_process_init(config);
                while (RETRY_START_TIMES > retry_times++) {
                    if (ERR_FAULT == epoll_cache(config, alloc_port)) {
                        _LOG_WARN("epoll cache start failed!");
                        break;
                    }
                    _LOG_WARN("epoll cache start retry time: %d!", retry_times);
                    sleep(RETRY_START_DELAY);
                }
                break;
            default: // parent
                break;
        }
        //
        return child;
    }

    //
#define INVA_CHILD_PID  -1

struct work_proc {
        short port;
        pid_t pid;
    };

    int master_process_cycle(struct cache_config *config, short bind_port[], unsigned int tatol) {
        struct work_proc *pstat;
        pstat = (struct work_proc *) malloc(tatol * sizeof (struct work_proc));
        unsigned int inde;
        for (inde = 0x00; tatol > inde; inde++) {
            pstat[inde].port = bind_port[inde];
            pstat[inde].pid = INVA_CHILD_PID;
        }
        //
        pid_t child_pid;
        for (;;) {
            for (inde = 0x00; tatol > inde; inde++) {
                if (INVA_CHILD_PID == pstat[inde].pid) {
                    _LOG_WARN("start worker process! port:%d", pstat[inde].port);
                    pstat[inde].pid = start_worker_process(config, pstat[inde].port);
                    if (INVA_CHILD_PID == pstat[inde].pid)
                        _LOG_WARN("start worker process failed! port:%d", pstat[inde].port);
                }
            }
            //
            child_pid = wait(NULL);
            if (INVA_CHILD_PID == child_pid) {
                _LOG_ERROR("master process wait error! exited.", pstat[inde].port);
                free(pstat);
                return ERR_FAULT;
            }
            _LOG_WARN("worker process exit! child_pid:%d", child_pid);
            for (inde = 0x00; tatol > inde; inde++) {
                if (child_pid == pstat[inde].pid) {
                    pstat[inde].pid = INVA_CHILD_PID;
                    _LOG_WARN("worker process exit! port:%d", pstat[inde].port);
                    break;
                }
            }
        }
        free(pstat);
        //
        return ERR_SUCCESS;
    }

    int single_process_cycle(struct cache_config *config, short bind_port[]) {
        if (ERR_FAULT == epoll_cache(config, bind_port[0])) {
            _LOG_WARN("epoll cache start failed!");
            return ERR_FAULT;
        }
        //
        return ERR_SUCCESS;
    }




