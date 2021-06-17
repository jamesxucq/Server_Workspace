#ifndef OPERABZL_H
#define	OPERABZL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_header.h"
#include "parse_conf.h"
#include "session.h"

    //    
    int operabzl_create(struct serv_config *config);
    int operabzl_destroy();
    //
#define GET_LAST_ANCHOR_BZL            load_last_anchor
    //
#define ADD_ADDI_BZL(OPERA, ISDIRE, FILE_NAME, LENGTH, ACTION_TIME) \
    add_action_addi(OPERA, ISDIRE, FILE_NAME, LENGTH, ACTION_TIME);
    //
#define ADD_DELETE_BZL(OPERA, ISDIRE, FILE_NAME, LENGTH, ACTION_TIME) \
    add_action_delete(OPERA, ISDIRE, FILE_NAME, LENGTH, ACTION_TIME);
    //
#define ADD_MODIFY_BZL(OPERA, FILE_NAME, LENGTH, ORIGIN_LENGTH, ACTION_TIME) \
    add_action_modify(OPERA, FILE_NAME, LENGTH, ORIGIN_LENGTH, ACTION_TIME);
    //
#define ADD_MOVE_BZL(OPERA, FILE_NAME, NEW_NAME, ACTION_TIME) \
    add_action_move(OPERA, FILE_NAME, NEW_NAME, ACTION_TIME);
    //
#define ADD_COPY_BZL(OPERA, FILE_NAME, NEW_NAME, LENGTH, ACTION_TIME) \
    add_action_copy(OPERA, FILE_NAME, NEW_NAME, LENGTH, ACTION_TIME);
    //
    uint32 add_anchor_bzl(char *location, struct actno_ope* aope);
    int flush_anchor_bzl(char *location, struct actno_ope* aope);
    //
    int creat_active_alist(fsdo_lsth activ_alsth, fsdo_lsth idx_lsth);
    //
#define  ANCHOR_NOFOND   0x01
#define  SEARCH_ANCHOR   0x02
#define  ALIST_ERROR     0x04
#define GET_ALIST_BY_ANCHOR_BZL      get_alist_by_anchor
    int get_alist_by_anchor(fsdo_lsth alsth, char *location, uint32 anchor);
    //
    struct actno_ope *get_opera_cache_bzl(int uid);
    int valid_opera_referen(int uid);
    void opera_epoll_exception(int uid);
    void opera_reconn_exception(int uid);
    int del_opera_cache_end(int uid);
    int del_opera_cache_finish(int uid);
    //
    int valid_today_action(struct actno_ope* aope, char *location, char *file_name, time_t act_time);

#ifdef	__cplusplus
}
#endif

#endif	/* OPERABZL_H */

