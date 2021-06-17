#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auth_ctrl.h"
#include "aserv_bzl.h"
#include "serve_bzl.h"

//
//0:error >0:number
static int split_port(int *bind_port, int max_port, char *port_txt) {
    char *toksp = NULL,  *tokep = NULL;
    int inde = 0x00;
    //
    if(!port_txt || '\0'==port_txt[0]) return 0x00;
    for( tokep = toksp = port_txt; tokep && max_port>inde; inde++) {
        if ((tokep = strchr(toksp, ';'))) {
            tokep[0] = '\0';
            bind_port[inde] = atoi(toksp);
            toksp = tokep + 0x01;
        } else bind_port[inde] = atoi(toksp);
    }
    //
    return inde;
}

//0:error >0:number

int build_addr_list(addr_listh alsth, struct ssev_addr *saddr, int serv_numbe) {
    int bind_port[MAX_BIND_PORT];
    int port_numbe;
    serv_addr *new_addr;
    //
    int ssev_inde, port_inde;
    for (ssev_inde = 0x00; serv_numbe > ssev_inde; ssev_inde++) {
        port_numbe = split_port(bind_port, MAX_BIND_PORT, saddr[ssev_inde].sin_port);
        for(port_inde = 0x00; port_numbe > port_inde; port_inde++) {
            new_addr = add_new_alist(alsth);
            strcpy(new_addr->sin_addr, saddr[ssev_inde].sin_addr);
            new_addr->sin_port = bind_port[port_inde];
        }
    }
//
    return 0x00;
}


int sserve_create(char *cache_address, int bind_port) {
    struct ssev_addr saddr[MAX_BIND_PORT];
    int serv_numbe = get_ssev_by_auth(saddr, &_auth_config_, cache_address, bind_port);
    //    
    struct addr_list *alist = NULL;
    build_addr_list(&alist, saddr, serv_numbe);
    //
    struct addr_list *alistmp;
    for(alistmp = alist; NULL != alistmp; alistmp = alistmp->next) {
_LOG_DEBUG("ssev addr:%s port:%d", alistmp->saddr.sin_addr, alistmp->saddr.sin_port);
        addi_server(alistmp->saddr.sin_addr, alistmp->saddr.sin_port);
    }
    //
    dele_addr_list(alist);
    return 0;
}
