/* auth_bzl.h
 * Author: David
 * Created on: 2010-3-10
 */

#ifndef AUTHBZL_H
#define	AUTHBZL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "auth_cache.h"
#include "common_header.h"
#include "parse_conf.h"

    //    

    int authbzl_create(struct cache_config *config);
    int authbzl_destroy();

    //        
    /*auth_db operator section*/
    inline struct auth_config *get_auth_config_bzl();
    inline int adata_clear_bzl();
    inline int adata_erase_bzl(unsigned int uid);
    // inline int volid_auth_dbase();
#define volid_auth_dbase(user_ping) ping_auth_dbase(&_auth_config_, user_ping)
#define volid_ssev_dbase(_ssev_config_, addr_ping) ping_ssev_dbase(_ssev_config_, addr_ping)
    //0:error >0:number
int get_bind_port(short *cache_port, struct ssev_config *config, const char *cache_addr);

//
    struct auth_data *get_adata_bzl(unsigned int uid);
    struct auth_data *acache_update_bzl(unsigned int uid);

    //        
    /* tools */
    struct auth_data *creat_adata(struct cache_value *cvalue);

#ifdef	__cplusplus
}
#endif

#endif	/* AUTHBZL_H */

