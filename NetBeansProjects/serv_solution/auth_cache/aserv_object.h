/* aserv_object.h
 * Created on: 2010-3-10
 * Author: David
 */

#ifndef ASERV_OBJECT_H_
#define ASERV_OBJECT_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "common_header.h"
#include "parse_conf.h"
#include "options.h"

//
int init_aserv_obj();
int final_aserv_obj();

//
extern struct server_list *_serv_address_;
extern hashmap *_serv_addr_hm_;
extern struct server_list *_light_server_;

#ifdef	__cplusplus
}
#endif

#endif /* ASERV_OBJECT_H_ */
