/* event.h
 * Author: Administrator
 * Created on 2012
 */

#ifndef EVENT_H
#define	EVENT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "session.h" 
    
//
struct evdata {
    BYTE *head_buffer;
    size_t hlen;
    BYTE *body_buffer;
    size_t blen;
};

struct _event {
    struct session *seion;
    //
    struct evdata evdat;
    void (*handle[3])(struct _event *);
};
typedef void (*excep)(struct session *);
typedef void (*event)(struct _event *);
//
#define CREATE_EVENT(BEV) \
    BEV = (struct _event *) malloc(sizeof (struct _event)); \
    if(!BEV) return ERR_FAULT; \
    memset(BEV, '\0', sizeof (struct _event)); \
    BEV->seion = (struct session *) malloc(sizeof (struct session)); \
    if(!BEV->seion) { \
        free(BEV); \
        return ERR_FAULT; \
    } \
    memset(BEV->seion, '\0', sizeof (struct session));

#define DESTORY_EVENT(BEV) \
    if(BEV->seion) free(BEV->seion); \
    BEV->seion = NULL; \
    if(BEV) return free(BEV); \
    BEV = NULL;

#define INITIAL_EVENT(BEV) \
    memset(BEV->seion, '\0', sizeof (struct session));

//
#define INITIAL_EVENT_HANDLE(handle, method, command) \
    handle[0] = receive_tab[method][command]; \
    handle[1] = send_tab[method][command]; \
    handle[2] = (event)exception_tab[method][command];
//

#ifdef	__cplusplus
}
#endif

#endif	/* EVENT_H */

