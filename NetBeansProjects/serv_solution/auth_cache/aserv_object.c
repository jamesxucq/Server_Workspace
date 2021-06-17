/*
 * aserv_object.c
 *
 *  Created on: 2010-3-10
 *      Author: David
 */
#include "aserv_object.h"


//
struct server_list *_serv_address_;
hashmap *_serv_addr_hm_;
struct server_list *_light_server_;

#define SERVER_HASHMAP_SIZE     512

void func_null(void* ptr) {
}

int init_aserv_obj() {
    _serv_address_ = NULL;
    _light_server_ = NULL;
    _serv_addr_hm_ = create_hashmap(func_null, SERVER_HASHMAP_SIZE);
//
    return 0;
}

int final_aserv_obj() {
    hashm_del(_serv_addr_hm_);
    _serv_addr_hm_ = NULL;
    _light_server_ = NULL;
    DELETE_SLIST(_serv_address_);
    _serv_address_ = NULL;
//
    return 0;
}