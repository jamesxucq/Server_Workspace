/* epoll_serv.h
 * Created on: 2010-3-19
 * Author: David
 */

#ifndef EPOLLSERV_H_
#define EPOLLSERV_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "common_header.h"

#include "parse_conf.h"
#include "evd_list.h"
#include "event.h"

struct event_data {
    int sockfd;
    int evctrl; // 0:data 1:ctrl
    BYTE *buffer; // recv data buffer
    int rsleng;
    long last_active; // last active time
    //
    struct _event *bev;
};
//
#define CREATE_SRECVBUF(SRE_BUFF) \
    SRE_BUFF = (BYTE *) malloc(EVD_SOCKET_BUFF); \
    if(!SRE_BUFF) return ERR_FAULT; \
    memset(SRE_BUFF, '\0', EVD_SOCKET_BUFF);

#define DESTORY_SRECVBUF(SRE_BUFF) \
    if(SRE_BUFF) return free(SRE_BUFF); \
    SRE_BUFF = NULL;
//
int epoll_cache(struct cache_config *config, short alloc_port);
int init_event_data(struct event_data *evds, int max_events);

#ifdef	__cplusplus
}
#endif

#endif /* EPOLLSERV_H_ */
