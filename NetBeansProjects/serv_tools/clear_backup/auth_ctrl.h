/* 
 * File:   author.h
 * Author: Administrator
 *
 * Created on 2012年9月12日, 上午9:26
 */

#ifndef AUTH_H
#define	AUTH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "macro.h"

    typedef struct {
        char sin_addr[SERVNAME_LENGTH];
        int sin_port;
    } serv_addr;

    struct auth_config {
        serv_addr address;
        char database[DATABASE_LENGTH];
        char user_name[_USERNAME_LENGTH_];
        char password[PASSWORD_LENGTH];
    };

    struct auth_value {
        unsigned int uid;
        char location[MAX_PATH * 3];
        serv_addr saddr;
        char access_key[KEY_LENGTH];
    };
    int address_split(serv_addr *saddr, char *addr_txt);
    
    int get_auth_value(struct auth_value *avalue, struct auth_config *config, unsigned int uid, int count);
    int get_linked_status(char *linked_status, struct auth_config *config, unsigned int uid);
    int update_linked_status(struct auth_config *config, unsigned int uid, char *linked_status);

#define CONVERT_SQLTEXT(TEXT) \
    char SQL_TEXT[LARGE_TEXT_SIZE]; \
    char *token = TEXT, *sql = SQL_TEXT; \
    while('\0' != *token) { \
        if('\"' == *token) {*sql = '\\'; sql++;} \
        *sql = *token; \
        token++; sql++; \
    } \
    *sql = '\0';
    
#ifdef	__cplusplus
}
#endif

#endif	/* AUTH_H */

