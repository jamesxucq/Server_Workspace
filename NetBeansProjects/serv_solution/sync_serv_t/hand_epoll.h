#ifndef HAND_EPOLL_H
#define HAND_EPOLL_H

// #ifdef	__cplusplus
// extern "C" {
// #endif

#include "common_header.h"
#include "epoll_serv.h"

int syncingbzl_create(struct serv_config *config);
int syncingbzl_destroy();
//
int initial_recv(struct _event *bev);
int kalive_recv(struct _event *bev);
int head_recv(struct _event *bev);
int get_recv(struct _event *bev);
int post_recv(struct _event *bev);
int put_recv(struct _event *bev);
int copy_recv(struct _event *bev);
int move_recv(struct _event *bev);
int delete_recv(struct _event *bev);
int final_recv(struct _event *bev);
extern int(*recv_initialize_tab[])(struct _event*);

//
int unuse_send(struct _event *bev);
int initial_send(struct _event *bev);
int kalive_send(struct _event *bev);
int head_send(struct _event *bev);
int get_send(struct _event *bev);
int post_send(struct _event *bev);
int put_send(struct _event *bev);
int copy_send(struct _event *bev);
int move_send(struct _event *bev);
int delete_send(struct _event *bev);
int final_send(struct _event *bev);
extern int(*send_initialize_tab[])(struct _event*);

// #ifdef	__cplusplus
// }
// #endif

#endif /* HAND_EPOLL_H */