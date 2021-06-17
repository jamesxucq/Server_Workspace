/* epoll_serv.c
 * Created on: 2010-3-19
 * Author: Divad
#include "sserv_bzl.h"
 */
#include <sys/timerfd.h>  
#include <sys/signalfd.h>
#include <sys/time.h> // for test
#include <signal.h>  
#include <unistd.h> 

#include "hand_epoll.h"
#include "excep.h"
#include "third_party.h"

#include "epoll_serv.h"

// #define EPOLL_SERV_PORT 8090 // default port
#define LISTENQ 20

// check timeout
#define CONNECT_ACTIVE 256
#define TIMEOUT_CHECK_COUNT 3

// int max_events = 512;
#define MAX_EVENTS 512
#define FD_CONTINUE   0

//
char gserv_address[256];
int gbind_port;

// set event

void event_set(struct event_data *evd, int sockfd) {
    if (sockfd) evd->sockfd = sockfd;
    evd->evloop = 0;
    evd->last_active = time(NULL);
    evd->evstat = 0;
    // _LOG_WARN("event Set ok[fd:%d]", evd->sockfd); // disable by james 20120410
}

// set event last_active
#define EVENT_ACTIVE(EVD) EVD->last_active = time(NULL);

// add an event to epoll

void event_add(int epofd, int events, struct event_data *evd) {
    struct epoll_event ev = {0, {0}};
    ev.data.ptr = evd;
    ev.events = events;
//
    if (epoll_ctl(epofd, EPOLL_CTL_ADD, evd->sockfd, &ev))
        _LOG_WARN("event add failed[fd:%d] error[%d]:%s", evd->sockfd, errno, strerror(errno));
}

// add/mod an event to epoll

void event_mod(int epofd, int events, struct event_data *evd) {
    struct epoll_event ev = {0, {0}};
    ev.data.ptr = evd;
    ev.events = events;
//
    if (epoll_ctl(epofd, EPOLL_CTL_MOD, evd->sockfd, &ev))
        _LOG_WARN("event mod failed[fd:%d] error[%d]:%s", evd->sockfd, errno, strerror(errno));
}

// delete an event from epoll

void event_del(int epofd, struct event_data *evd) {
    struct epoll_event ev = {0, {0}};
    ev.data.ptr = evd;
    if (epoll_ctl(epofd, EPOLL_CTL_DEL, evd->sockfd, &ev))
        { _LOG_WARN("event del fail[fd:%d] error[%d]:%s", evd->sockfd, errno, strerror(errno)); }
}

#define EVENT_DEL(EPO_FD, EPO_LIST, EVD) { \
    /* _LOG_DEBUG("event del, close:%d", EVD->sockfd); */ \
    event_del(EPO_FD, EVD); \
    close(EVD->sockfd); \
    if (!set_levd_list(EPO_LIST, EVD)) { _LOG_WARN("levd_list del fail."); } \
}

// accept new connections from clients
#define ACCEPT_CONNECT(EPO_FD, EPO_LIST, EVD, LISTEN_FD) { \
    struct sockaddr_in sin; \
    socklen_t len = sizeof (struct sockaddr_in); \
    int aifd; \
    struct levd_list *levd_list; \
\
/*_LOG_WARN("accept connect!");*/ \
    for (;;) { /* accept */ \
        if ((aifd = accept(LISTEN_FD, (struct sockaddr*) &sin, &len)) == -1) { \
            if ((EAGAIN != errno) && (EINTR != errno)) { \
                _LOG_WARN("%s: bad accept", __func__); \
            } \
            break; \
        } \
/* _LOG_DEBUG("accept:%d", aifd); */ \
        do { \
            levd_list = get_levd_list(EPO_LIST); \
            if (!levd_list) { \
                _LOG_WARN("%s:max connection limit[%d].", __func__, max_events); \
                break; \
            } \
            /* memset(levd_list->evd, '\0', sizeof(struct event_data)); */ \
            if (EVD->bev) INITIAL_EVENT(EVD->bev) \
            /* set nonblocking */ \
            if (fcntl(aifd, F_SETFL, fcntl(aifd, F_GETFL) | O_NONBLOCK) < 0) \
                break; \
            /* add a read event for receive data */ \
            event_set(levd_list->evd, aifd); \
            event_add(EPO_FD, EPOLLIN | EPOLLET, levd_list->evd); \
_LOG_INFO("new connection[%s:%d][time:%d][fd:%d]", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port), levd_list->evd->last_active, aifd); \
        } while(0); \
    } \
}

#define CONTRO_SIGNAL(SIFD) { \
    struct signalfd_siginfo fdsi; \
    ssize_t rlen; \
    for(;;) { \
        rlen = read(SIFD, &fdsi, sizeof(struct signalfd_siginfo)); \
        if (sizeof(struct signalfd_siginfo) == rlen) { \
            if (SIGINT == fdsi.ssi_signo) { _LOG_WARN("GOT SIGINT!"); } \
            else if (SIGPIPE == fdsi.ssi_signo) { _LOG_WARN("GOT SIGPIPE!"); } \
            else { _LOG_WARN("read unexpected signal!"); } \
        } else { \
            if((EAGAIN != errno) && (EINTR != errno)) \
                { _LOG_WARN("signal fd read error! sifd:%d", SIFD); } \
            break; \
        } \
    } \
}

// alarm catch function
#define CHECK_TIMEOUT(EPO_FD, EPO_LIST, CHECK_TATOL, TIME_OUT, NOW) { \
    struct levd_list *levd_head = NULL; \
    struct levd_list *levd_list, *next_elist; \
    /* a simple timeout check here, every time 100, better to use a mini-heap, and add timer event */ \
    int inde; \
    for (inde = 0, levd_list = EPO_LIST->chkpos; levd_list && inde<CHECK_TATOL; inde++, levd_list = (levd_head==next_elist?0L:next_elist)) { /* doesn't check listen fd */ \
        next_elist = levd_list->next == levd_list ? 0L: levd_list->next; \
        if ((NOW - levd_list->evd->last_active) >= TIME_OUT) { /* 60s timeout */ \
            /* decrease auth cache refere */ \
            _LOG_WARN("[fd:%d] timeout[%d--%d].", levd_list->evd->sockfd, levd_list->evd->last_active, NOW); \
            ((excep)levd_list->evd->bev->handle[4])(levd_list->evd->bev->seion); \
            EVENT_DEL(EPO_FD, EPO_LIST, levd_list->evd) \
        } else if (!levd_head) levd_head = levd_list; \
    } \
}

// sockets connect timeout check here
#define CONTRO_TIMEOUT(EPO_FD, EPO_LIST, CHECK_TATOL, TIME_OUT, TIFD) { \
/*_LOG_DEBUG("check timeout!");*/ \
    uint64_t exp; \
    ssize_t rlen = read(TIFD, &exp, sizeof(uint64_t)); \
    if (sizeof (uint64_t) == rlen) { \
/*_LOG_DEBUG("check timeout! exp:%llu time(NULL):%lu", exp, time(NULL));*/ \
        long now = time(NULL); \
        CHECK_TIMEOUT(EPO_FD, EPO_LIST, CHECK_TATOL, TIME_OUT, now); \
    } else { _LOG_WARN("timer read error! tifd:%d", TIFD); } \
}

#define ACCEPT_CONTRO(EPO_FD, EPO_LIST, COLFD, EVD, CHECK_TATOL, TIME_OUT)  { \
/*_LOG_WARN("accept contro!");*/ \
    if(COLFD[0] == EVD->sockfd) \
        ACCEPT_CONNECT(EPO_FD, EPO_LIST, EVD, COLFD[0]) \
    else if(COLFD[1] == EVD->sockfd) \
        CONTRO_TIMEOUT(EPO_FD, EPO_LIST, CHECK_TATOL, TIME_OUT, COLFD[1])\
    else if(COLFD[2] == EVD->sockfd) CONTRO_SIGNAL(COLFD[2]) \
}

//
#define INITIAL_RECV_EVENT(EVD, EVDAT) \
    if(EVD->evstat) { \
        SDTP_EVDAT_BODY(EVDAT, EVD); \
        EVD->evstat = EVD->bev->handle[1](EVD->bev); \
    } else { \
/*_LOG_DEBUG("---- EVD->sockfd:%d EVD->evstat:%d", EVD->sockfd, EVD->evstat);*/ \
        SDTP_EVDAT_HDBD(EVDAT, EVD); \
        EVD->bev->method = proto_method((char *)EVDAT->head_buffer, EVDAT->hlen); \
/*_LOG_DEBUG("---- method:%d", EVD->bev->method);*/ \
        INITIAL_EVENT_HANDLE(EVD->bev->handle, EVD->bev->method) \
/*_LOG_DEBUG("---- EVD->bev->handle[0]:%X", EVD->bev->handle[0]);*/ \
        EVD->evstat = EVD->bev->handle[0](EVD->bev); \
/*_LOG_DEBUG("---- EVD->evstat:%d", EVD->evstat);*/ \
    }
// receive data
#define RECEIVE_DATA(EVD, EPO_FD, EPO_LIST) { \
/*_LOG_WARN("receive data!");*/ \
    for (;;) { /* receive data */ \
/*struct timeval t_start,t_end; gettimeofday(&t_start, NULL);*/\
        EVD->rsleng = recv(EVD->sockfd, EVD->buffer, EVD_SOCKET_BUFF, 0); \
/*gettimeofday(&t_end, NULL); fprintf(stderr, "%ld\n", t_end.tv_usec - t_start.tv_usec);*/\
        if (0 < EVD->rsleng) { \
            struct evdata *evdat = &EVD->bev->evdat; \
            INITIAL_RECV_EVENT(EVD, evdat) \
            if (!EVD->evstat) { /* change to send event */ \
/*_LOG_WARN("[fd:%d] recv over, modify to send.", EVD->sockfd);*/ \
                event_set(EVD, FD_CONTINUE); \
                event_mod(EPO_FD, EPOLLOUT | EPOLLET, EVD); \
                break; \
            } else if(!POW2N_MOD(EVD->evloop++, CONNECT_ACTIVE)) EVENT_ACTIVE(EVD) \
        } else if (!EVD->rsleng) { \
            /* _LOG_WARN("[fd:%d] recv closed gracefully.", EVD->sockfd); // disable by james 20120410 */ \
            EVENT_DEL(EPO_FD, EPO_LIST, EVD) \
            break; \
        } else { \
            if ((EAGAIN != errno) && (EINTR != errno)) { \
_LOG_WARN("recv[fd:%d] error[%d]:%s", EVD->sockfd, errno, strerror(errno)); \
                ((excep)EVD->bev->handle[4])(EVD->bev->seion); \
                EVENT_DEL(EPO_FD, EPO_LIST, EVD) \
            } \
            break; \
        } \
    } \
}

//
#define INITIAL_SEND_EVENT(EVD, EVDAT) \
    SDTP_EVDAT_INIT(EVDAT, EVD) \
    if(EVD->evstat) EVD->evstat = EVD->bev->handle[3](EVD->bev); \
    else EVD->evstat = EVD->bev->handle[2](EVD->bev); \
    SDTP_EVDAT_END(EVDAT, EVD)
// send data
#define SEND_MESSAGE(EVD, EPO_FD, EPO_LIST) { \
/*_LOG_WARN("send data!");*/ \
    for (;;) { /* send data */ \
        if (!(SEND_CONTINUE & EVD->evstat)) { \
            struct evdata *evdat = &EVD->bev->evdat; \
            INITIAL_SEND_EVENT(EVD, evdat) \
        } \
/*struct timeval t_start,t_end; gettimeofday(&t_start, NULL);*/\
        int cleng = send(EVD->sockfd, EVD->buffer + EVD->sleng, EVD->rsleng - EVD->sleng, 0); \
/*gettimeofday(&t_end, NULL); fprintf(stderr, "%ld\n", t_end.tv_usec - t_start.tv_usec);*/\
/*_LOG_WARN("cleng:%d EVD->rsleng:%d EVD->sleng:%d", cleng, EVD->rsleng, EVD->sleng);*/ \
/* log_binary_code(EVD->buffer + EVD->sleng, EVD->rsleng - EVD->sleng); */ \
        if (0 < cleng) { \
            EVD->sleng += cleng; \
            if (EVD->sleng != EVD->rsleng) EVD->evstat |= SEND_CONTINUE; \
            else { \
                EVD->evstat &= SEND_SUCCESS; \
                if (!EVD->evstat) { /* change to receive event */ \
                    if(SVTP_FINAL != EVD->bev->method) { \
/*_LOG_WARN("[fd:%d] send over, modify to recv. method:%d", EVD->sockfd, EVD->bev->method);*/ \
                        event_set(EVD, FD_CONTINUE); \
                        event_mod(EPO_FD, EPOLLIN | EPOLLET, EVD); \
                    } else { /*_LOG_WARN("[fd:%d] send over, delete.", EVD->sockfd);*/ \
                        EVENT_DEL(EPO_FD, EPO_LIST, EVD) \
                    } \
                    break; \
                } else if(!POW2N_MOD(EVD->evloop++, CONNECT_ACTIVE)) EVENT_ACTIVE(EVD) \
            } \
        } else { \
            if ((EAGAIN != errno) && (EINTR != errno)) { \
_LOG_WARN("[fd:%d] send exception.", EVD->sockfd); \
/* _LOG_WARN("send[fd:%d] error[%d]:%s", EVD->sockfd, errno, strerror(errno)); */ \
                ((excep)EVD->bev->handle[4])(EVD->bev->seion); \
                EVENT_DEL(EPO_FD, EPO_LIST, EVD) \
            } else EVD->evstat |= SEND_CONTINUE; \
            break; \
        } \
    } \
}

// except
#define CLOSE_EXCEPTION(EVD, EPO_FD, EPO_LIST, EVENTS) { \
_LOG_WARN("[fd:%d] epoll hup or err, everts:%x, loop closed.", EVD->sockfd, EVENTS.events); \
    if(EVENTS.events & (EPOLLERR | EPOLLHUP)) \
        ((excep)EVD->bev->handle[4])(EVD->bev->seion); \
    EVENT_DEL(EPO_FD, EPO_LIST, EVD) \
}

int init_listen_socket(int *colfd, int epofd, struct event_data *list_evd, short port) {
    int lisfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lisfd == -1) {
        _LOG_WARN("creat socket fd error!");
        return ERR_FAULT;
    }
    //
    fcntl(lisfd, F_SETFL, fcntl(lisfd, F_GETFL) | O_NONBLOCK); // set non-blocking
    _LOG_INFO("server listen fd:%d", lisfd);
    //
    memset(list_evd, '\0', sizeof (struct event_data));
    list_evd->evctrl = 0x01;
    event_set(list_evd, lisfd);
    event_add(epofd, EPOLLIN | EPOLLET, list_evd);
    //
    // bind & listen
    struct sockaddr_in sin;
    bzero(&sin, sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
    if (bind(lisfd, (const struct sockaddr*) &sin, sizeof (struct sockaddr)) == -1) {
        _LOG_WARN("bind error!");
        return ERR_SOCK_BIND;
    }
    if (listen(lisfd, LISTENQ) == -1) {
        _LOG_WARN("listen error!");
        return ERR_FAULT;
    }
    //EVD->rsleng:
    colfd[0] = lisfd;
    return ERR_SUCCESS;
}

int init_contro_event(int *colfd, int epofd, struct event_data *sig_evd, struct event_data *ala_evd, int time_out) {
    // alarm fd
    struct itimerspec neval;
    neval.it_value.tv_sec = 0x03;
    neval.it_value.tv_nsec = 0;
    neval.it_interval.tv_sec = time_out;
    neval.it_interval.tv_nsec = 0;
    //
    int tifd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (tifd == -1) {
        _LOG_WARN("timerfd create error!");
        return ERR_FAULT;
    }
    if (-1 == timerfd_settime(tifd, 0, &neval, NULL)) {
        _LOG_WARN("settime error!");
        return ERR_FAULT;
    }
    //
    memset(ala_evd, '\0', sizeof (struct event_data));
    ala_evd->evctrl = 0x01;
    event_set(ala_evd, tifd);
    event_add(epofd, EPOLLIN | EPOLLET, ala_evd);
    colfd[0x01] = tifd;
    // signal fd
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT); // disable for test
    sigaddset(&mask, SIGPIPE);
    //
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        _LOG_WARN("sigprocmask");
    //
    int sifd = signalfd(-1, &mask, SFD_NONBLOCK);
    if (-1 == sifd) {
        _LOG_WARN("signalfd create error!");
        return ERR_FAULT;
    }
    //
    memset(sig_evd, '\0', sizeof (struct event_data));
    sig_evd->evctrl = 0x01;
    event_set(sig_evd, sifd);
    event_add(epofd, EPOLLIN | EPOLLET, sig_evd);
    colfd[0x02] = sifd;
    //
    return ERR_SUCCESS;
}

int init_event_data(struct event_data *evds, int max_events) {
    int inde;
    //
    if (!evds) return ERR_FAULT;
    memset(evds, '\0', sizeof (struct event_data) *(max_events + 0x03));
    for (inde = 0; inde < max_events; inde++) {
        memset(&evds[inde], '\0', sizeof (struct event_data));
        CREATE_SRECVBUF(evds[inde].buffer)
        CREATE_EVENT(evds[inde].bev)
    }
    return ERR_SUCCESS;
}

void final_event_data(struct event_data *evds, int max_events) {
    int inde;
    if (evds) {
        for (inde = 0; inde < max_events; inde++) {
            DESTORY_EVENT(evds[inde].bev)
            DESTORY_SRECVBUF(evds[inde].buffer)
        }
    }
}

#define DELETE_EVENTS(EVENTS)	free(EVENTS)
#define DELETE_EVDATS(EVDATS)	free(EVDATS)

int epoll_server(struct serv_config *config, short alloc_port) {
    strcpy(gserv_address, config->serv_address);
    gbind_port = alloc_port;
    _LOG_INFO("gserv_address:%s gbind_port:%d\n", gserv_address, gbind_port);
    //
    int epofd = -1, colfd[3] = {-1, -1, -1};
    // init the env
    short port = alloc_port; // EPOLL_SERV_PORT; // default port
    int max_events = config->max_connecting;
    int time_out = config->time_out;
    int check_tatol = config->max_connecting >> TIMEOUT_CHECK_COUNT;
    int ret_value = ERR_SUCCESS;
    // init event_data
    struct event_data *evds = (struct event_data *) malloc(sizeof (struct event_data) *(max_events + 0x03)); // g_evds[max_events] is used by listen fd
    if (!evds) {
        _LOG_WARN("malloc event_data error!");
        return ERR_FAULT;
    }
    memset(evds, '\0', sizeof (struct event_data) *(max_events + 0x03));
    if ((ret_value = init_event_data(evds, max_events))) {
        _LOG_WARN("init event_data error!");
        DELETE_EVDATS(evds);
        return ret_value;
    }
    // init evnets
    struct epoll_event *events = (struct epoll_event *) malloc(sizeof (struct epoll_event) *(max_events + 0x03));
    if (!events) {
        _LOG_WARN("malloc epoll event error!");
        final_event_data(evds, max_events);
        DELETE_EVDATS(evds);
        return ERR_FAULT;
    }
    memset(events, '\0', sizeof (struct epoll_event) *(max_events + 0x03));
    // init event data list
    struct evd_list *elist = (struct evd_list *) malloc(sizeof (struct evd_list));
    if (!elist) {
        _LOG_WARN("malloc event list error!");
        DELETE_EVENTS(events);
        final_event_data(evds, max_events);
        DELETE_EVDATS(evds);
        return ERR_FAULT;
    }
    memset(elist, '\0', sizeof (struct evd_list));
    if ((ret_value = init_evd_list(elist, evds, max_events))) {
        _LOG_WARN("init event list error!");
        DELETE_EVENTS(events);
        final_event_data(evds, max_events);
        DELETE_EVDATS(evds);
        return ret_value;
    }
    // create epoll
    epofd = epoll_create(max_events + 0x03);
    if (-1 == epofd) {
        _LOG_WARN("create epoll failed:%d, erron:%d", epofd, errno);
        dele_evd_list(elist);
        DELETE_EVENTS(events);
        final_event_data(evds, max_events);
        DELETE_EVDATS(evds);
        return ERR_FAULT;
    } 
    // create & bind listen socket, and add to epoll, set non-blocking
    if ((ret_value = init_listen_socket(colfd, epofd, &evds[max_events], port))) {
        dele_evd_list(elist);
        DELETE_EVENTS(events);
        final_event_data(evds, max_events);
        DELETE_EVDATS(evds);
        return ret_value;
    }
    _LOG_INFO("server running:port[%d]", port);
    // create ctrl fd
    if ((ret_value = init_contro_event(colfd, epofd, &evds[max_events + 1], &evds[max_events + 2], time_out))) {
        dele_evd_list(elist);
        DELETE_EVENTS(events);
        final_event_data(evds, max_events);
        DELETE_EVDATS(evds);
        return ret_value;
    }
    // event loop
    for (;;) { // wait for events to happen
        int ready = epoll_wait(epofd, events, max_events, -1);
        if (ready < 0) {
            if ((EAGAIN == errno) || (EINTR == errno))
                continue;
            _LOG_WARN("epoll_wait error, exit. erron: %d", errno);
            break;
        }
// _LOG_DEBUG("epoll_wait ready: %d", ready);
        int inde;
        for (inde = 0; inde < ready; inde++) {
            struct event_data *evd = (struct event_data*) events[inde].data.ptr;
// _LOG_DEBUG("events[inde].events:%x evd->sockfd:%d\n", events[inde].events, evd->sockfd);
            if (evd->evctrl) ACCEPT_CONTRO(epofd, elist, colfd, evd, check_tatol, time_out) // accept ctrl
            else if (events[inde].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
                CLOSE_EXCEPTION(evd, epofd, elist, events[inde]) // exception event
            else if (events[inde].events & EPOLLIN) RECEIVE_DATA(evd, epofd, elist) // read event
            else if (events[inde].events & EPOLLOUT) SEND_MESSAGE(evd, epofd, elist) // write event
            } // handle epoll
// _LOG_DEBUG("loop continue!");
    } // main loop An established connection was aborted by the software in your host machine.
    //
_LOG_WARN("quit event loop, error!");
    // free resource
    close(epofd);
    dele_evd_list(elist);
    DELETE_EVENTS(events);
    final_event_data(evds, max_events);
    DELETE_EVDATS(evds);
    //
_LOG_WARN("exit syncserv error!");
    return ERR_SUCCESS;
}

