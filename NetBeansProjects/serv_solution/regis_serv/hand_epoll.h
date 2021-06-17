#ifndef HANDEPOLL_H_
#define HANDEPOLL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common_header.h"
#include "epoll_serv.h"

    int regisbzl_create(struct regis_config *config);
    int regisbzl_destroy();
    //
    void regis_recv(struct _event *bev);
    void settings_recv(struct _event *bev);
    void link_recv(struct _event *bev);
    void unlink_recv(struct _event *bev);

    extern void(*recv_null_code[])(struct _event*);
    extern void(*recv_options_code[])(struct _event*);
    extern void(**receive_tab[])(struct _event*);

    //
    void unuse_send(struct _event *bev);
    void regis_send(struct _event *bev);
    void settings_send(struct _event *bev);
    void link_send(struct _event *bev);
    void unlink_send(struct _event *bev);

    extern void(*send_null_code[])(struct _event*);
    extern void(*send_options_code[])(struct _event*);
    extern void(**send_tab[])(struct _event*);

#ifdef __cplusplus
}
#endif

#endif /* HANDEPOLL_H_ */