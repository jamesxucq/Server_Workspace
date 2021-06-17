/*
 * aserv_bzl.c
 *
 *  Created on: 2010-3-10
 *      Author: David
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common_header.h"
#include "aserv_bzl.h"
#include "server_list.h"

int servbzl_create(struct cache_config **config, char *xml_config) {
    *config = load_cache_config(xml_config);
    if (!(*config)) {
fprintf(stderr, "load config failed!\n");
        return ERR_FAULT;
    }
fprintf(stderr, "load config ok!\n");
    if (init_aserv_obj()) {
fprintf(stderr, "init server object failed!\n");
    }
    //
    return ERR_SUCCESS; //succ
}

int servbzl_destroy() {
    if (final_aserv_obj()) { _LOG_WARN("destroy server object failed!"); }
    return ERR_SUCCESS; //succ
}

struct cache_config *load_cache_config(const char *config_path) {
    if (!config_path) return NULL;
    //
    strcpy(_config_path_, (char *) config_path);
    if (cache_config_from_xml(&_cache_config_, config_path)) return NULL;
    find_user_group(_cache_config_.user_name, _cache_config_.group_name, &_cache_config_._user_id_, &_cache_config_._group_id_);
    //
    return &_cache_config_;
}

/*
inline unsigned int get_port_tatol() {
    unsigned int port_tatol;
    for (port_tatol = 0; 0 != _cache_config_.bind_port[port_tatol]; port_tatol++);
    return port_tatol;
}
*/

inline void set_cache_config_file(const char *config_path) {
    strcpy(_config_path_, (char *) config_path);
}

inline char *get_cache_config_file() {
    return _config_path_;
}

inline void set_cache_config(struct cache_config *config) {
    memcpy(&_cache_config_, config, sizeof (struct cache_config));
}

inline struct cache_config *get_cache_config() {
    return &_cache_config_;
}

int addi_server(char *address, int port) {
    serv_server *saddr;
    //
    if (!address) return -1;
    saddr = add_new_slist(&_serv_address_);
    if (!saddr) return -1;
    if (!_light_server_) _light_server_ = _serv_address_;
    //
    strcpy(saddr->sin_addr, address);
    saddr->sin_port = port;
    //
    hashm_insert(inet_addr(address), saddr, _serv_addr_hm_);
    return 0;
}

int clear_server_bzl(char *address, int port) { // -1:fail 0:ok 1:exception
    if (!address) return -1;
    if (!_serv_address_) return 0x01;
    //
    struct server_list *prev_slist = NULL;
    struct server_list *find_slist = _serv_address_;
    while (find_slist) {
        if (!strcmp(address, find_slist->saddr.sin_addr) && (port == find_slist->saddr.sin_port))
            break;
        prev_slist = find_slist;
        find_slist = find_slist->next;
    }
    //
    int clear_value = 0;
    if (find_slist && (0 >= find_slist->saddr.refer_inde)) {
        if (_serv_address_ == find_slist) _serv_address_ = _serv_address_->next;
        else prev_slist->next = find_slist->next;
        _light_server_ = _serv_address_;
        free(find_slist);
    } else clear_value = 0x01;
    //
    if (!clear_value) hashm_remove(inet_addr(address), _serv_addr_hm_);
    return clear_value;
}

int valid_server(unsigned int sin_addr) { // -1:error; 0:found; 1:not found 
    int valid = -1;
// for test
struct in_addr saddr;
saddr.s_addr = sin_addr;
_LOG_DEBUG("sin_addr:%s", inet_ntoa(saddr));
    if (hashm_value(sin_addr, _serv_addr_hm_)) valid = 0;
    else valid = 1;
_LOG_DEBUG("-1:error; 0:found; 1:not found! valid:%d", valid);
    return valid;
}

inline struct server_list *get_server_list() {
    return _serv_address_;
}

inline serv_server *get_light_server() {
    static unsigned int requ_tatol;
    serv_server *serv_light = NULL;
    //
    if (_light_server_) {
        if (!(++requ_tatol & 0x0f)) { // mod 16
            if (_light_server_->next) _light_server_ = _light_server_->next;
            else _light_server_ = _serv_address_;
        }
        serv_light = &_light_server_->saddr;
    }
    //
    return serv_light;
}

int slist_referin_next(char *saddr, int port) {
    struct server_list *find_slist = NULL;
    serv_server *find_serv = NULL;
    //
    find_slist = _serv_address_;
    while (find_slist) {
        if (!strcmp(saddr, find_slist->saddr.sin_addr) && (port == find_slist->saddr.sin_port)) {
            find_serv = &find_slist->saddr;
            break;
        }
        find_slist = find_slist->next;
    }
    find_serv->refer_inde++;
    //
    return 0;
}

int slist_referen_decre(char *saddr, int port) {
    struct server_list *find_slist = NULL;
    serv_server *find_serv = NULL;
    //
    find_slist = _serv_address_;
    while (find_slist) {
        if (!strcmp(saddr, find_slist->saddr.sin_addr) && (port == find_slist->saddr.sin_port)) {
            find_serv = &find_slist->saddr;
            break;
        }
        find_slist = find_slist->next;
    }
    find_serv->refer_inde--;
    //
    return 0;
}

//

inline listen_admin *get_listen_admin() {
    return &_cache_config_.admin;
}

int addi_admin(char *address) { // -1:fail 0:ok 1:exception
    serv_addr *saddr;
    saddr = add_new_alist(&_cache_config_.alist);
    if (!saddr) return -1;
    strcpy(saddr->sin_addr, address);
    return 0;
}

int valid_admin(char *address, int port) {
    serv_addr * saddr;
    int valid = -1;
    //
    if (!address) return -1;
    FIND_ADDR_LIST(saddr, _cache_config_.alist, address)
    if (!saddr) valid = -1;
    else {
        if (!saddr->sin_port || (port == saddr->sin_port))
            valid = 0;
    }
    //
    return valid;
}

inline struct addr_list *get_admin_list() {
    return _cache_config_.alist;
}