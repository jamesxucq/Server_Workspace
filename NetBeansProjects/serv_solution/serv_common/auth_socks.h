/* 
 * File:   auth_socks.h
 * Author: Divad
 *
 * Created on 2010
 */

#ifndef AUTHSOCKS_H
#define	AUTHSOCKS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"
#include "auth_reply.h"

    // 
    extern serv_addr _cache_saddr_;
    extern int _cache_sockfd_;

    //
    int auth_socks_create();
    int auth_socks_destroy();

    struct list_data *get_ldata_by_uid(struct list_data *ldata, unsigned int uid, char *saddr, int port, char *req_valid, int cache_sockfd);
    int set_cache_anchor(unsigned int uid, unsigned int last_anchor, char *req_valid, int cache_sockfd);
    int set_cache_unlocked(unsigned int uid, char *req_valid, int cache_sockfd);
    int keep_alive_cache(unsigned int uid, char *req_valid, int cache_sockfd);
    int ping_auth_cache(serv_addr *cache_saddr);

    //
    int init_cache_sockfd(serv_addr *cache_saddr);
    inline void close_cache_sockfd(int cache_sockfd);

    //
    int send_receive_cache(char *recv_buffer, char *send_buffer, int slen, int cache_sockfd);

#ifdef	__cplusplus
}
#endif

#endif	/* AUTHSOCKS_H */

