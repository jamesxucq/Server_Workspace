/*
 * auth_bzl.c
 *
 *  Created on: 2010-3-10
 *      Author: David
 */

#include "aserv_bzl.h"
#include "auth_bzl.h"
#include "auth_ctrl.h"
#include "auth_cache.h"


#define CREATE_AUTH_CONFIG(ACONFIG, CCONFIG) \
    strcpy((ACONFIG)->address.sin_addr, (CCONFIG)->auth_db.address.sin_addr); \
    (ACONFIG)->address.sin_port = (CCONFIG)->auth_db.address.sin_port; \
    strcpy((ACONFIG)->database, (CCONFIG)->auth_db.database); \
    strcpy((ACONFIG)->password, (CCONFIG)->auth_db.password); \
    strcpy((ACONFIG)->user_name, (CCONFIG)->auth_db.user_name);

int authbzl_create(struct cache_config *config) {
    int hashmap_size = config->max_connecting;
    hashmap_size <<= 2;
    _adata_hm_ = create_hashmap(free_adata, hashmap_size);
//
    CREATE_AUTH_CONFIG(&_auth_config_, config);
    if (volid_auth_dbase(config->ping_auth.param_str)) {
printf("connect auth_db failed!\n");
        return ERR_FAULT;
    }
printf("connect auth_db ok!\n");
//
    if (volid_ssev_dbase(&config->ssev_db, config->ping_ssev.param_str)) {
printf("connect ssev_db failed!\n");
        return ERR_FAULT;
    }
printf("connect ssev_db ok!\n");
//
    return ERR_SUCCESS; //succ
}

int authbzl_destroy() {
    if (_adata_hm_) {
        hashm_del(_adata_hm_);
        _adata_hm_ = NULL;
    }
//
    return ERR_SUCCESS; //succ
}

inline struct auth_config *get_auth_config_bzl() {
    return &_auth_config_;
}

inline int adata_clear_bzl() { // -1:fail 0:ok 1:exception
    if (!_adata_hm_) return -1;
    hashm_clear(_adata_hm_);
    return 0;
}

inline int adata_erase_bzl(unsigned int uid) { // -1:fail 0:ok 1:exception
    if (!_adata_hm_) return -1;
    struct auth_data *adata = hashm_value(uid, _adata_hm_);
    if (!adata) return 0x01;
    if (adata->serv_locked) return 0x01;
    hashm_remove(uid, _adata_hm_);
    return 0;
}

// delete the duplication port
int get_bind_port(short *cache_port, struct ssev_config *config, const char *cache_addr) {
    short bind_port[MAX_BIND_PORT];
    int aport_numbe, bport_numbe = 0x00;
    //
    aport_numbe = get_aport_by_auth(bind_port, config, cache_addr);
    int aind, cind;
    for(aind=0x00; aport_numbe > aind; aind++) {
        for(cind=0x00; bport_numbe > cind; cind++) {
            if(bind_port[aind] == cache_port[cind]) break;
        }
        if(bport_numbe == cind) {
            cache_port[cind] = bind_port[aind];
            bport_numbe++;
        }
    }
    //
    return bport_numbe;
}
    

//
struct auth_data *get_adata_bzl(unsigned int uid) {
    struct auth_data *adata = hashm_value(uid, _adata_hm_);
    if (adata) return adata;
//
    struct cache_value cvalue;
    if (get_cvalue_by_uid(&cvalue, &_auth_config_, uid)) return NULL;
    adata = creat_adata(&cvalue);
    if (!adata) return NULL;
//
    trans_auth_chks(adata->_author_chks_, cvalue.uid, cvalue.access_key);
    strcpy(adata->req_valid, random_valid());
_LOG_TRACE("init, new adata->req_valid:%s", adata->req_valid);
    strcpy(adata->seion_id, random_seion_id());
    adata->last_active = time(NULL);
    adata->serv_locked = FALSE;
//
    hashm_insert(uid, adata, _adata_hm_);
    return adata;
}

struct auth_data *acache_update_bzl(unsigned int uid) {
    struct auth_data *adata = hashm_value(uid, _adata_hm_);
    if (!adata) return NULL;
    struct cache_value cvalue;
    if (get_cvalue_by_uid(&cvalue, &_auth_config_, uid)) return NULL;
    trans_auth_chks(adata->_author_chks_, adata->uid, cvalue.access_key);
    return adata;
}

struct auth_data *creat_adata(struct cache_value *cvalue) {
    struct auth_data *adata = (struct auth_data *) malloc(sizeof (struct auth_data));
    if (!adata) return NULL;
    memset(adata, '\0', sizeof (struct auth_data));
    adata->uid = cvalue->uid;
    strcpy(adata->location, cvalue->location);
    return adata;
}
