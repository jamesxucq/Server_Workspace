/* 
 * main.c
 * Author: Administrator
 * Created on 2013年1月23日, 下午3:17
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"
#include "logger.h"
#include "auth_ctrl.h"
#include "config.h"

#include "base64.h"

char *encode_base64_text(char *ciphertext, char *plaintext, char *salt_value);
char *random_salt(char *salt_value);

//
int initialize_env(struct aupa_config *aupconfig, char *xml_config);
int finalize_env();

//
int addi_user(struct aupa_config *aupconfig, char *user_name, char *password) {
    char salt_value[32];
    char ciphertext[128];
//
    random_salt(salt_value);
    encode_base64_text(ciphertext, password, salt_value);
//
    struct user_info uinfo;
    strcpy(uinfo.user_name, user_name);
    strcpy(uinfo.password, ciphertext);
    strcpy(uinfo.salt, salt_value);
    uinfo.pool_size = aupconfig->pool_size;
    USER_LOCATION(uinfo.location, aupconfig->location, user_name);
    strcpy(uinfo.auth_host, aupconfig->auth_host);
    strcpy(uinfo.version, aupconfig->version);    
    //
    if(addi_user_query(&aupconfig->auth_db, &uinfo)) {
        LOG_INFO("dele user auth failed!");
        return -1;
    }
    LOG_INFO("addi user to database ok.");
    //
    if(recu_mkdir(aupconfig->location, user_name)) {
        LOG_INFO("recu mkdir failed!");
        return -1;
    }
    LOG_INFO("addi user location ok.");
    //
    return 0;
}

void print_erase(struct aupa_config *aupconfig) {
/*
    struct erase_space *eraspa = &eraconfig->eraspa;
    LOG_INFO("start_uid: %d number: %d sleep: %d anchor_day: %d", eraspa->start_uid, eraspa->number, eraspa->sleep, eraspa->anchor_day);
    LOG_INFO("backup_day: %d max_length: %d delim_count: %d path: %s", eraspa->backup.backup_day, eraspa->backup.max_length, eraspa->local.delim_count, eraspa->local.path);
    struct erase_link *eralink = &eraconfig->eralink;
    LOG_INFO("unlink_day: %d bomb_day: %d", eralink->unlink_day, eralink->bomb_day);
    struct auth_config *aconfig = &eraconfig->auth_db;
    LOG_INFO("sin_addr: %s sin_port: %d database: %s  user_name: %s", aconfig->address.sin_addr, aconfig->address.sin_port, aconfig->database, aconfig->user_name);
    log_config *lconfig = &eraconfig->runtime_log;
    LOG_INFO("debug_level: %d logfile: %s", lconfig->debug_level, lconfig->logfile);
*/
}

//
int main(int argc, char** argv) {
    char user_name[MID_TEXT_LEN];
    char password[MID_TEXT_LEN];
    char xml_config[MAX_PATH * 3];
//
    //
    if (parse_args(user_name, password, xml_config, argc, argv)) return ERR_FAULT;
    if (FILE_EXISTS(xml_config)) {
        printf("%s configuration file not exists\n", xml_config);
        return EXIT_FAILURE;
    }
//
    struct aupa_config aupconfig;
    if (initialize_env(&aupconfig, xml_config)) {
        LOG_INFO("initialize_serv failed!");
        return ERR_FAULT;
    }
    LOG_INFO("initialize_serv OK");
    ////////////////////////////////////////////////////////////////////////////
    LOG_INFO("******************** configuration ********************");
    print_erase(&aupconfig);
    LOG_INFO("parse config ok.");
    LOG_INFO("******************** addi user ********************");
    if(addi_user(&aupconfig, user_name, password)) {
        LOG_INFO("addi user failed!");
    }
    LOG_INFO("addi user ok.");
    LOG_INFO("******************** finished ********************");
    ////////////////////////////////////////////////////////////////////////////
    if (finalize_env(xml_config)) {
        LOG_INFO("finalize_serv failed!");
        return ERR_FAULT;
    }
    LOG_INFO("finalize_serv OK");
//
    return (EXIT_SUCCESS);
}

int initialize_env(struct aupa_config *aupconfig, char *xml_config) {
    // init serv gui module
    parse_create(aupconfig, xml_config);
    // init log module
    log_init(aupconfig->runtime_log.logfile, aupconfig->runtime_log.debug_level);
//
    return ERR_SUCCESS;
}

int finalize_env() {
    log_fini();
    // uninit client gui module
    parse_destroy();
//
    return ERR_SUCCESS;
}


/*
int main(int argc, char** argv) {
    char salt_value[32];
    char ciphertext[128];

    if (argc < 3) printf("./add user password\n");
    else {
        random_salt(salt_value);
        encode_base64_text(ciphertext, argv[2], salt_value);
        printf("user:%s\n", argv[1]);
        printf("salt:%s\n", salt_value);
        printf("password:%s\n", ciphertext);
    }

    return 0;
}
*/

char *encode_base64_text(char *ciphertext, char *plaintext, char *salt_value) {
    char buffer[128];

    if (!ciphertext || !plaintext || !salt_value || *plaintext == '\0') return NULL;
    sprintf(buffer, "%s%s%s", salt_value, plaintext, salt_value);
    if (!base64_encode(ciphertext, 128, (const unsigned char *) buffer, strlen(buffer)))
        return NULL;

    return ciphertext;
}

char *random_salt(char *salt_value) {
    char rand_str[64];
    char code_array[62] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    unsigned int last_seed;

    if (!salt_value) return NULL;
    last_seed = time(NULL);
    int count;
    for (count = 0; count < 16; count++) {
        srand((unsigned int) last_seed);
        last_seed = rand();
        rand_str[count] = code_array[last_seed % 62];
    }
    rand_str[count] = '\0';
    strcpy(salt_value, rand_str);

    return salt_value;
}