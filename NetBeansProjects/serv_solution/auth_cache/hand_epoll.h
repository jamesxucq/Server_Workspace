#ifndef HANDEPOLL_H_
#define HANDEPOLL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common_header.h"
#include "epoll_serv.h"

    int cachebzl_create(struct cache_config *config);
    int cachebzl_destroy();
    //
    void query_recv(struct _event *bev);
    void addi_recv(struct _event *bev);
    void clear_recv(struct _event *bev);
    void list_recv(struct _event *bev);
    void status_recv(struct _event *bev);
    void sanchor_recv(struct _event *bev);
    void slocked_recv(struct _event *bev);
    void kalive_recv(struct _event *bev);

    extern void (*recv_null_code[])(struct _event*);
    extern void (*recv_options_code[])(struct _event*);
    extern void (*recv_control_code[])(struct _event*);
    extern void (*recv_kalive_code[])(struct _event*);
    extern void (**receive_tab[])(struct _event*);

    //
    void unused_send(struct _event *bev);
    void query_send(struct _event *bev);
    void addi_send(struct _event *bev);
    void clear_send(struct _event *bev);
    void list_send(struct _event *bev);
    void status_send(struct _event *bev);
    void sanchor_send(struct _event *bev);
    void slocked_send(struct _event *bev);
    void kalive_send(struct _event *bev);

    extern void (*send_null_code[])(struct _event*);
    extern void (*send_options_code[])(struct _event*);
    extern void (*send_control_code[])(struct _event*);
    extern void (*send_kalive_code[])(struct _event*);
    extern void (**send_tab[])(struct _event*);

#ifdef __cplusplus
}
#endif

#endif /* HANDEPOLL_H_ */