/* 
 * File:   linked.h
 * Author: Administrator
 *
 * Created on 2012
 */

#ifndef LINKED_H
#define	LINKED_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "macro.h"
#include "clear.h"
    
#define BOMB_STATUS_FALSE   0
#define BOMB_STATUS_TRUE    1
#define BOMB_STATUS_DONE    2
    
#define LINKED_STATUS_INITIAL   0
#define LINKED_STATUS_LINKED    1
#define LINKED_STATUS_UNLINK    2

    struct linked_status {
        char client_id[IDENTI_LENGTH];
        int link_stat;
        time_t time_stamp;
        int data_bomb;
    };

    int clear_linked_status(char *linked_txt, struct erase_link *eralink);

#ifdef	__cplusplus
}
#endif

#endif	/* LINKED_H */

