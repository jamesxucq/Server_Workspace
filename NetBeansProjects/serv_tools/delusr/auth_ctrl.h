/* 
 * File:   auth_ctrl.h
 * Author: Administrator
 *
 * Created on 2015年12月19日, 上午8:52
 */

#ifndef AUTH_CTRL_H
#define	AUTH_CTRL_H

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
//
int get_auth_value(char *location, struct auth_config *config, char *user_name);
int dele_user_query(struct auth_config *config, char *user_name);


#ifdef	__cplusplus
}
#endif

#endif	/* AUTH_CTRL_H */

