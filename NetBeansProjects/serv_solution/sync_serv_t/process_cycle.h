/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef PROCESS_CYCLE_H
#define PROCESS_CYCLE_H

#include "parse_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

//
int master_process_cycle(struct serv_config *config, short bind_port[], unsigned int tatol);
int single_process_cycle(struct serv_config *config, short bind_port[]);


#ifdef __cplusplus
}
#endif

#endif /* PROCESS_CYCLE_H */

