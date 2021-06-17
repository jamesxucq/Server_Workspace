/* 
 * File:   linked_status.h
 * Author: Administrator
 *
 * Created on 2012
 */

#ifndef LINKED_STATUS_H
#define	LINKED_STATUS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_macro.h"
#include "regis_macro.h"

#define BOMB_STATUS_FALSE   0
#define BOMB_STATUS_TRUE    1
#define BOMB_STATUS_DONE    2
    
#define LINKED_STATUS_NOAUTH   0
#define LINKED_STATUS_LINKED    1
#define LINKED_STATUS_UNLINK    2

    struct linked_status {
        char client_id[IDENTI_LENGTH];
        int link_stat;
        time_t time_stamp;
        int data_bomb;
    };
    
    // -1:error; 0:found; 1:not found 
    int find_linked_status(struct linked_status *link_stat, char *linked_txt, char *client_id);
    void insert_linkstat_text(char *linked_txt, char *client_id);
    void add_linkstat_text(char *linked_txt, char *client_id);
    void erase_linkstat_text(char *linked_txt, char *client_id);
    int modify_linkstat(char *linked_txt, char *client_id, int linkstat);
    int modify_databomb(char *linked_txt, char *client_id, int data_bomb);
    int modify_linkstat_databomb(char *linked_txt, char *client_id, int linkstat, int data_bomb);

#ifdef	__cplusplus
}
#endif

#endif	/* LINKED_STATUS_H */

