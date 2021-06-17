/* 
 * File:   dseion.h
 * Author: Administrator
 *
 * Created on 11:14
 */

#ifndef DSEION_H
#define	DSEION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_header.h"
#include "server_macro.h"
#include "session.h"    

    //
    int open_hnull_null(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
    extern int (*open_head_null[])(struct seion_ctrl *, struct list_data *, struct inte_value *);

    //
    int open_hfile_file(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
    extern int (*open_head_file[])(struct seion_ctrl *, struct list_data *, struct inte_value *);

    //
    int open_hlist_null(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
    int open_hlist_recu_files(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
    int open_hlist_list_directory(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
    int open_hlist_recu_dires(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
    int open_hlist_anch_files(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
    extern int (*open_head_list[])(struct seion_ctrl *, struct list_data *, struct inte_value *);
    extern int (**open_head_se[])(struct seion_ctrl *, struct list_data *, struct inte_value *);

    //
    void close_head_null(struct seion_ctrl *sctrl);
    void close_head_file(struct seion_ctrl *sctrl);
    void close_head_list(struct seion_ctrl *sctrl);
    extern void (*close_head_se[])(struct seion_ctrl *);

#endif	/* DSEION_H */

