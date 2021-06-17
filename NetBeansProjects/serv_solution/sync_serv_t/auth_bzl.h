#ifndef AUTHBZL_H
#define	AUTHBZL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_header.h"
#include "parse_conf.h"
#include "auth_cache.h"

    int authbzl_create(struct serv_config *config);
    int authbzl_destroy();

    /*auth_db operator section*/
    inline serv_addr *get_cache_saddr();
    struct list_data *get_adata_bzl(unsigned int uid, char *laddr, int port, char *req_valid);

    void auth_epoll_exception(unsigned int uid, unsigned int last_anchor, char *req_valid, int anchor_cached);
    void auth_reconn_exception(unsigned int uid);
    int erase_auth_cache_end(unsigned int uid);
    int erase_auth_cache_finish(unsigned int uid, unsigned int last_anchor, char *req_valid, int anchor_cached);
    int erase_auth_cache_success(unsigned int uid, unsigned int last_anchor, char *req_valid);

#define valid_auth_cache() ping_auth_cache(&_cache_saddr_)
#define keep_alive_cache_bzl(uid, seion_id) keep_alive_cache(uid, seion_id, _cache_sockfd_)
#define volid_ssev_dbase(_ssev_config_, addr_ping) ping_ssev_dbase(_ssev_config_, addr_ping)
//0:error >0:number
    int get_bind_port(short *bind_port, struct ssev_config *config, const char *serv_addr);


#ifdef	__cplusplus
}
#endif

#endif	/* AUTHBZL_H */

