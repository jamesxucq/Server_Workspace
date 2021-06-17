#ifndef SSERV_OBJECT_H
#define SSERV_OBJECT_H

//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common_header.h"
#include "parse_conf.h"
#include "options.h"
#include "event.h"
#include "session.h"

//
int initialize_hander(bakup_config *bakup_ctrl);
int finalize_hander();

//
// struct inte_value *ival, struct seion_ctrl *sctrl, struct evdata *evdat, struct list_data *ldata
int hand_post_recv(struct _event *bev);
int hand_put_recv(struct _event *bev);
extern int (*recv_handler_tab[])(struct _event*);
//
int hand_copy_recv(struct _event *bev);
int hand_move_recv(struct _event *bev);
int hand_dele_recv(struct _event *bev);

/* handle recv and creat send */
int hand_get_send(struct _event *bev);
int hand_put_send(struct _event *bev);
extern int (*send_handler_tab[])(struct _event*);

#endif /* SSERV_OBJECT_H */
