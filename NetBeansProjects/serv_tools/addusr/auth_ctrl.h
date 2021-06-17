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
    
//
#include "macro.h"
    
//
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
    struct user_info {
        char user_name[_USERNAME_LENGTH_];
        char password[PASSWORD_LENGTH];
        char salt[SALT_LENGTH];
        int pool_size;
        char location[MAX_PATH * 3];
        char auth_host[MID_TEXT_LEN];
        char version[MID_TEXT_LEN];
    };
    
//
int addi_user_query(struct auth_config *config, struct user_info *uinfo);

#ifdef	__cplusplus
}
#endif

#endif	/* AUTH_CTRL_H */

