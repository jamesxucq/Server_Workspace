/* authbzl.c
 * Created on: 2010-3-10
 * Author: Divad
 */

// #include "sserv_bzl.h"
#include "opera_bzl.h"
#include "auth_bzl.h"
#include "auth_ctrl.h"
#include "auth_cache.h"

//
int authbzl_create(struct serv_config *config) {
    int hashmap_size = config->max_connecting;
    hashmap_size <<= 2;
    _ldata_hm_ = create_hashmap(free_ldata, hashmap_size);
//
    if (volid_ssev_dbase(&config->ssev_db, config->ping_ssev.param_str)) {
printf("connect ssev_db failed!\n");
        return ERR_FAULT;
    }
printf("connect ssev_db ok!\n");
//
    if(get_auth_by_ssev(&_cache_saddr_, &_auth_config_, config->serv_address))
        return ERR_FAULT;
printf("auth cache addr:%s port:%d\n", _cache_saddr_.sin_addr, _cache_saddr_.sin_port); 
    _cache_sockfd_ = init_cache_sockfd(&_cache_saddr_);
    if (-1 == _cache_sockfd_) {
        printf("connect auth cache failed!\n");
        return ERR_FAULT;
    }
    printf("connect auth cache ok!\n");
//
    if (valid_auth_cache()) {
        printf("check auth cache failed!\n");
        return ERR_FAULT;
    }
    printf("check auth cache ok!\n");
//
    return ERR_SUCCESS; //succ
}

int authbzl_destroy() {
    close_cache_sockfd(_cache_sockfd_);
    if (_ldata_hm_) {
        hashm_del(_ldata_hm_);
        _ldata_hm_ = NULL;
    }

    return ERR_SUCCESS; //succ
}

inline serv_addr *get_cache_saddr() {
    return &_cache_saddr_;
}

struct list_data *get_adata_bzl(unsigned int uid, char *laddr, int port, char *req_valid) {
    struct list_data *ldata;
    //
    if ((ldata = (struct list_data *) hashm_value(uid, _ldata_hm_))) ldata->refer_inde++;
    else if ((ldata = new_ldata())) {
        if (get_ldata_by_uid(ldata, uid, laddr, port, req_valid, _cache_sockfd_)) {
            if (!ldata->last_anchor) {
                ldata->anchor_cached = 0;
                ldata->last_anchor = GET_LAST_ANCHOR_BZL(ldata->location);
            } else ldata->anchor_cached = 1;
            ldata->refer_inde = 1;
            ldata->last_active = time(NULL);
            //
            hashm_insert(uid, ldata, _ldata_hm_);
        } else DELETE_LDATA(ldata);
    }
    // _LOG_DEBUG("increase auth cache refer inde:%d", ldata->refer_inde);
    //
    return ldata;
}

void auth_epoll_exception(unsigned int uid, unsigned int last_anchor, char *req_valid, int anchor_cached) {
    struct list_data *ldata;
    if ((ldata = (struct list_data *) hashm_value(uid, _ldata_hm_))) {
        ldata->refer_inde--;
        if (!ldata->refer_inde) {
            _LOG_INFO("timeout delete ldata refere inde:%d", ldata->refer_inde); // disable by james 20120410
            if (anchor_cached) set_cache_unlocked(uid, req_valid, _cache_sockfd_);
            else set_cache_anchor(uid, last_anchor, req_valid, _cache_sockfd_);
            hashm_remove(uid, _ldata_hm_);
        }
    }
    // _LOG_INFO("decrease auth cache refere inde:%d", ldata->refer_inde); // disable by james 20120410
}

void auth_reconn_exception(unsigned int uid) {
    struct list_data *ldata;
    if ((ldata = (struct list_data *) hashm_value(uid, _ldata_hm_))) {
        // _LOG_INFO("reconn delete ldata refere inde:%d", ldata->refer_inde); // disable by james 20120410
        hashm_remove(uid, _ldata_hm_);
    }
}

int erase_auth_cache_end(unsigned int uid) {
    struct list_data *ldata;
    //
    if ((ldata = (struct list_data *) hashm_value(uid, _ldata_hm_))) ldata->refer_inde--;
    else return ERR_FAULT;
    // _LOG_INFO("decrease auth cache refere refer inde:%d", ldata->refer_inde); // disable by james 20120410
    return ERR_SUCCESS;
}

int erase_auth_cache_finish(unsigned int uid, unsigned int last_anchor, char *req_valid, int anchor_cached) {
    struct list_data *ldata;
    //
    if ((ldata = (struct list_data *) hashm_value(uid, _ldata_hm_))) {
        if (anchor_cached) set_cache_unlocked(uid, req_valid, _cache_sockfd_);
        else set_cache_anchor(uid, last_anchor, req_valid, _cache_sockfd_);
        hashm_remove(uid, _ldata_hm_);
    } else return ERR_FAULT;
    //
    return ERR_SUCCESS;
}

int erase_auth_cache_success(unsigned int uid, unsigned int last_anchor, char *req_valid) {
    struct list_data *ldata;
    //
    if ((ldata = (struct list_data *) hashm_value(uid, _ldata_hm_))) {
        set_cache_anchor(uid, last_anchor, req_valid, _cache_sockfd_);
        hashm_remove(uid, _ldata_hm_);
    } else return ERR_FAULT;
    //
    return ERR_SUCCESS;
}

//
//0:error >0:number
static int split_port(short *bind_port, int max_port, char *port_txt) {
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
printf("parse bind port:%d\n", bind_port[inde]); 
    }
    //
    return inde;
}

//0:error >0:number
int get_bind_port(short *bind_port, struct ssev_config *config, const char *serv_addr) {    
    char port_txt[MAX_PORT_LEN];
    if(get_sport_by_ssev(port_txt, config, serv_addr))
        return ERR_FAULT;
    return split_port(bind_port, MAX_BIND_PORT, port_txt);
}
