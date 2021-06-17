/* 
 * File:   main.cpp
 * Author: Administrator
 *
 * Created on 2012年8月24日, 下午1:49
 */

#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

/******************************************************************************
 * circular doubly linked list macros                                         *
 *****************************************************************************/
#define CDL_APPEND(head, add) { \
    if (head) { \
        (add)->prev = (head)->prev; \
        (add)->next = (head); \
        (head)->prev = (add); \
        (add)->prev->next = (add); \
    } else { \
        (add)->prev = (add); \
        (add)->next = (add); \
    } \
    (head)=(add); \
}

#define CDL_DELETE(head, del) { \
    if (((head)==(del)) && ((head)->next == (head))) { \
        (head) = 0L; \
    } else { \
        (del)->next->prev = (del)->prev; \
        (del)->prev->next = (del)->next; \
        if ((del) == (head)) (head)=(del)->next; \
    } \
}

#define CDL_FOREACH(head, el) \
    for(el=head; el; el=(el->next==head?0L:el->next)) 


/*
 * 
 */
struct event_data {
    int sockfd;
    char *buffer; // recv data buffer
    int len;
    long last_active; // last active time
    /////////////////////////////////////
    int evstat;
    int slen;
};

struct levd_node {
    struct levd_node *prev, *next;
    struct event_data *evd;
};

struct evd_list {
    struct levd_node *active;
    struct levd_node *sleep;
    struct levd_node *checkpos;
};
#define		ERR_FAULT						-1
#define		ERR_SUCCESS                                             0

int init_evd_list(struct evd_list *elist, struct event_data *evds, int events_count) {
    int index;
    struct levd_node *list_node;

    if (!elist || !evds) return ERR_FAULT;
    memset(elist, '\0', sizeof (struct evd_list));

    for (index = 0; index < events_count; index++) {
        list_node = (struct levd_node *) malloc(sizeof (struct levd_node));
        if (!list_node) {
            return ERR_FAULT;
        }
        memset(list_node, '\0', sizeof (struct levd_node));
        list_node->evd = &evds[index];
        CDL_APPEND(elist->sleep, list_node);
    }

    return ERR_SUCCESS;
}

struct levd_node *get_levd_node(struct evd_list *elist) {
    struct levd_node *list_node = elist->sleep;

    if (!elist->sleep) return NULL;
    CDL_DELETE(elist->sleep, list_node);
    CDL_APPEND(elist->active, list_node);
    if (!elist->checkpos)
        elist->checkpos = elist->active;

    return list_node;
}

struct levd_node *set_levd_node(struct evd_list *elist, struct event_data *evd) {
    struct levd_node *list_node;

    if (!elist->active) return NULL;
    // for(el=head; el; el=(el->next==head?0L:el->next)) 
    CDL_FOREACH(elist->active, list_node)
    if (list_node->evd == evd)
        break;
    if (!list_node) return NULL;

    if (list_node == elist->checkpos) {
        if (list_node->next == list_node) elist->checkpos = NULL;
        else elist->checkpos = list_node->next;
    }
    CDL_DELETE(elist->active, list_node);
    CDL_APPEND(elist->sleep, list_node);

    return list_node;
}

void delete_evd_list(struct evd_list *elist) {

}

#define CHECK_TIMEOUT(EPO_LIST, CHECK_COUNT, TIME_OUT) { \
    struct levd_node *startpos = NULL; \
    struct levd_node *levd_node, *next_node; \
    /* a simple timeout check here, every time 100, better to use a mini-heap, and add timer event */ \
    int index; \
    long now; \
printf("EPO_LIST->checkpos:%x\n", EPO_LIST->checkpos);\
    for (index = 0, levd_node = EPO_LIST->checkpos; levd_node && index<CHECK_COUNT; index++, levd_node = (startpos==next_node?0L:next_node)) { /* doesn't check listen fd */ \
printf("levd_node:%x\n", levd_node);\
        next_node = levd_node->next; \
        /* /////////////////////////////////////////////////// */ \
        if (index == 0) { /* 60s timeout */ \
    set_levd_node(EPO_LIST, levd_node->evd); \
            /* decrease auth cache refer */ \
        } else if (!startpos) { startpos = levd_node; printf("startpos:%x\n", startpos); }\
printf("startpos:%x next_node:%x\n", startpos, next_node);        \
    } \
}

int main(int argc, char** argv) {
    int max_events = 512;

    struct event_data *evds = (struct event_data *) malloc(sizeof (struct event_data) *(max_events)); // g_evds[max_events] is used by listen fd
    if (!evds) {
        return ERR_FAULT;
    }
    
    struct evd_list *elist = (struct evd_list *) malloc(sizeof (struct evd_list));
    if (!elist) {
        return ERR_FAULT;
    }
    memset(elist, '\0', sizeof (struct evd_list));
    if (init_evd_list(elist, evds, max_events)) {
        return ERR_FAULT;
    }
    ////////////////////////////////////////////////////////////////////////////
//    levd_node *n1 = get_levd_node(elist);
//printf("n1:%x\n", n1);
//    levd_node *n2 = get_levd_node(elist);
//printf("n2:%x\n", n2);
//    levd_node *n3 = get_levd_node(elist);
//printf("n3:%x\n", n3);
//    levd_node *n4 = get_levd_node(elist);
//printf("n4:%x\n", n4);
//    levd_node *n5 = get_levd_node(elist);
//printf("n5:%x\n", n5);
//    levd_node *n6 = get_levd_node(elist);  
//printf("n6:%x\n", n6);
     
    CHECK_TIMEOUT(elist, 7, 30)
    
//    set_levd_node(elist, n1->evd);
//    set_levd_node(elist, n6->evd);     
//    set_levd_node(elist, n5->evd);
//    set_levd_node(elist, n4->evd);   
//    set_levd_node(elist, n3->evd);
//    set_levd_node(elist, n2->evd);     
    
    return 0;
}

