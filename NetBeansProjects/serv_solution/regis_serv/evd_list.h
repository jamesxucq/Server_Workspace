#ifndef _EVDLIST_H_
#define _EVDLIST_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_header.h"

struct levd_list {
    struct levd_list *next;
    struct event_data *evd;
};

struct evd_list {
    struct levd_list *active;
    struct levd_list *sleep;
};

//
//
#include "epoll_serv.h"
int init_evd_list(struct evd_list *elist, struct event_data *evds, int events_tatol);
void dele_evd_list(struct evd_list *elist);

inline struct levd_list *get_levd_list(struct evd_list *elist);
inline struct levd_list *set_levd_list(struct evd_list *elist, struct event_data *evd);

#ifdef	__cplusplus
}
#endif

#endif /* _EVDLIST_H_ */


