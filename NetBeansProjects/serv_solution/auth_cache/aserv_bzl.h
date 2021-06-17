/* aserv_bzl.h
 * Created on: 2010-3-10
 * Author: David
 */

#ifndef ASERVBZL_H_
#define ASERVBZL_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "aserv_object.h"

    int servbzl_create(struct cache_config **config, char *xml_config);
    int servbzl_destroy();

    /*Anch operator section*/



    /*Type operator section*/
    struct cache_config *load_cache_config(const char *config_path);
    inline void set_cache_config_file(const char *config_path);
    inline char *get_cache_config_file();
    inline void set_cache_config(struct cache_config *config);
    inline struct cache_config *get_cache_config();
//    inline unsigned int get_port_tatol();

    //
    int addi_server(char *address, int port);
    // #define clear_server_bzl(ADDRESS)        erase_serv_list(&_serv_address_, ADDRESS)
    int clear_server_bzl(char *address, int port); // -1:fail 0:ok 1:exception
    int valid_server(unsigned int sin_addr); // -1:error; 0:found; 1:not found 
    inline struct server_list *get_server_list();
    inline serv_server *get_light_server();

int slist_referin_next(char *saddr, int port);
int slist_referen_decre(char *saddr, int port);
#define SERVE_REFERIN_NEXT(SADDR, PORT) slist_referin_next(SADDR, PORT)
#define SERVE_REFEREN_DECREA(SADDR, PORT) slist_referen_decre(SADDR, PORT)

    //
    inline listen_admin *get_listen_admin();
    int addi_admin(char *address); // -1:fail 0:ok 1:exception
#define CLEAR_ADMIN_BZL(ADDRESS)         erase_addr_list(&_cache_config_.alist, ADDRESS) // 2:fail 0:ok 1:exception
    int valid_admin(char *address, int port);
    inline struct addr_list *get_admin_list();

    /*sync server aope*/

    /* file list aope */

#ifdef	__cplusplus
}
#endif

#endif /* ASERVBZL_H_ */
