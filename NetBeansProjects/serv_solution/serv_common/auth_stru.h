#ifdef	__cplusplus
extern "C" {
#endif

#ifndef AUTHSTRU_H_
#define AUTHSTRU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
#include "common_macro.h"
#include "string_utility.h"

//
struct auth_value {
    unsigned int uid;
    char password[PASSWORD_LENGTH];
    char _salt_[SALT_LENGTH];
    serv_addr saddr;
    char access_key[KEY_LENGTH];
    long int lease_time;
    char link_con[LARGE_TEXT_SIZE];
};

struct cache_value {
    unsigned int uid;
    char location[MAX_PATH * 3];
    char access_key[KEY_LENGTH];
};

struct set_value {
    unsigned int uid;
    char password[PASSWORD_LENGTH];
    char _salt_[SALT_LENGTH];
    char version[VERSION_LENGTH];
    int pool_size;
    char link_con[LARGE_TEXT_SIZE];
};

struct link_value {
    unsigned int uid;
    char password[PASSWORD_LENGTH];
    char _salt_[SALT_LENGTH];
    char link_con[LARGE_TEXT_SIZE];
};

struct auth_config {
    serv_addr address;
    char database[DATABASE_LENGTH];
    char user_name[_USERNAME_LENGTH_];
    char password[PASSWORD_LENGTH];
};
#define ssev_config auth_config

struct user_ping {
    char param_str[_USERNAME_LENGTH_];
    int param_int;
};
//
#ifdef	__cplusplus
}
#endif

#endif /* AUTHSTRU_H_ */
