
/*
 * AuthCtrl.c
 *
 *  Created on: 2010-3-10
 *      Author: David
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "logger.h"
#include "auth_ctrl.h"

#define QUERY_LENGTH    512
#define LARGE_QUERY_LENGTH    2048

struct auth_config _auth_config_;

#define SELECT_UNAME_QUERY \
	"SELECT User_id, Password, Salt, Auth_host, Access_key, Lease_time, Linked_status FROM user WHERE User_name = \"%s\";"
int get_avalue_by_uname(struct auth_value *avalue, struct auth_config *config, const char *user_name) {
    // MYSQL mysql;
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_UNAME_QUERY, user_name);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
_LOG_WARN("couldn't get result from %s", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo));
    if (0x01 != mysql_num_rows(respo)) {
_LOG_WARN("the result set error");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
//
    while ((row = mysql_fetch_row(respo))) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    avalue->uid = atol(row[inde]);
                    break;
                case 1:
                    strcpy(avalue->password, row[inde]);
                    break;
                case 2:
                    strcpy(avalue->_salt_, row[inde]);
                    break;
                case 3:
                    saddr_split(&avalue->saddr, row[inde]);
                    break;
                case 4:
                    strcpy(avalue->access_key, row[inde]);
                    break;
                case 5:
                    avalue->lease_time = atol(row[inde]);
                    break;
                case 6:
                    strcpy(avalue->link_con, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

#define SELECT_UID_QUERY \
	"SELECT User_id, Location, Access_key FROM user WHERE User_id = %u;"
int get_cvalue_by_uid(struct cache_value *cvalue, struct auth_config *config, unsigned int uid) {
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_UID_QUERY, uid);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
_LOG_WARN("couldn't get result from %s", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 != mysql_num_rows(respo)) {
_LOG_WARN("the result set error");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
//
    while ((row = mysql_fetch_row(respo))) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    cvalue->uid = atol(row[inde]);
                    break;
                case 1:
                    strcpy(cvalue->location, row[inde]);
                    break;
                case 2:
                    strcpy(cvalue->access_key, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

#define UPDATE_KEY_QUERY \
	"UPDATE user SET Access_key=\"%s\", Lease_time=%ld WHERE User_id = %u;"
int update_access_key(struct auth_config *config, unsigned int uid, char *access_key, long int lease_time) {
    MYSQL *sql_sock;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, UPDATE_KEY_QUERY, access_key, lease_time, uid);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    mysql_close(sql_sock);
//
    return 0;
}

//
#define SELECT_LINKED_QUERY \
	"SELECT Linked_status FROM user WHERE User_id = %u;"
int get_linked_status(struct auth_config *config, char *link_con, unsigned int uid) {
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_UID_QUERY, uid);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
_LOG_WARN("couldn't get result from %s", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 != mysql_num_rows(respo)) {
_LOG_WARN("the result set error");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
//
    while ((row = mysql_fetch_row(respo))) {
        strcpy(link_con, row[0]);
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

//
#define UPDATE_LINKED_QUERY \
	"UPDATE user SET Linked_status=\"%s\" WHERE User_id = %u;"
int update_linked_status(struct auth_config *config, unsigned int uid, char *link_con) {
    MYSQL *sql_sock;
    char query[LARGE_QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, UPDATE_LINKED_QUERY, link_con, uid);
    // _LOG_INFO("UPDATE_LINKED_QUERY: %s", query);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    mysql_close(sql_sock);
//
    return 0;
}

#define SELECT_PINGA_QUERY "SELECT * FROM user WHERE User_name = \"%s\";"
int ping_auth_dbase(struct auth_config *config, const char *ping_name) {
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
    //
printf("auth ping conn, addr:%s name:%s pwd:%s db:%s port:%d\n", config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port);
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
printf("couldn't connect to engine!%d %s\n", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_PINGA_QUERY, ping_name);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
printf("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
printf("couldn't get result from %s\n", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 != mysql_num_rows(respo)) {
printf("the result set error\n");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    while ((row = mysql_fetch_row(respo))) {
printf("query auth data base ok!\n");
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

;
#define SELECT_PINGS_QUERY "SELECT * FROM ssev WHERE Cache_addr = \"%s\";"
int ping_ssev_dbase(struct auth_config *config, const char *ping_addr) {
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
    //
printf("ssev ping conn, addr:%s name:%s pwd:%s db:%s port:%d\n", config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port);
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
printf("couldn't connect to engine!%d %s\n", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_PINGS_QUERY, ping_addr);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
printf("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
printf("couldn't get result from %s\n", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 != mysql_num_rows(respo)) {
printf("the result set error\n");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    while ((row = mysql_fetch_row(respo))) {
printf("query ssev data base ok!\n");
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}


#define SELECT_SETTING_QUERY \
	"SELECT User_id, Password, Salt, Pool_size, Version, Linked_status FROM user WHERE User_name = \"%s\";"
int get_svalue_by_uname(struct set_value *svalue, struct auth_config *config, const char *user_name) {
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
_LOG_WARN("sin_addr:%s user_name:%s password:%s database:%s sin_port:%d", 
        config->address.sin_addr, config->user_name, config->password, config->database, config->address.sin_port);
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_SETTING_QUERY, user_name);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
_LOG_WARN("couldn't get result from %s", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 != mysql_num_rows(respo)) {
_LOG_WARN("the result set error");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
//
    while ((row = mysql_fetch_row(respo))) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    svalue->uid = atol(row[inde]);
                    break;
                case 1:
                    strcpy(svalue->password, row[inde]);
                    break;
                case 2:
                    strcpy(svalue->_salt_, row[inde]);
                    break;
                case 3:
                    svalue->pool_size = atoi(row[inde]);
                    break;
                case 4:
                    strcpy(svalue->version, row[inde]);
                    break;
                case 5:
                    strcpy(svalue->link_con, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

#define SELECT_LINK_QUERY \
	"SELECT User_id, Password, Salt, Linked_status FROM user WHERE User_name = \"%s\";"
int get_lvalue_by_uname(struct link_value *lvalue, struct auth_config *config, const char *user_name) {
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_LINK_QUERY, user_name);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
_LOG_WARN("couldn't get result from %s", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 != mysql_num_rows(respo)) {
_LOG_WARN("the result set error");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
//
    while ((row = mysql_fetch_row(respo))) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    lvalue->uid = atol(row[inde]);
                    break;
                case 1:
                    strcpy(lvalue->password, row[inde]);
                    break;
                case 2:
                    strcpy(lvalue->_salt_, row[inde]);
                    break;
                case 3:
                    strcpy(lvalue->link_con, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

//
#define SELECT_SSEVCI_QUERY \
	"SELECT Serv_addr, Bind_port FROM ssev WHERE Cache_addr = \"%s\" AND Cache_port = %d;"
int get_ssev_by_auth(struct ssev_addr *saddr, struct ssev_config *config, const char *cache_addr, int cache_port) {
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
_LOG_WARN("sin_addr:%s user_name:%s password:%s database:%s sin_port:%d", 
        config->address.sin_addr, config->user_name, config->password, config->database, config->address.sin_port);
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_SSEVCI_QUERY, cache_addr, cache_port);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
_LOG_WARN("couldn't get result from %s", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 > mysql_num_rows(respo)) {
_LOG_WARN("the result set error");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    //
    int ssev_inde = 0x00;
    while ((row = mysql_fetch_row(respo))) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    strcpy(saddr[ssev_inde].sin_addr, row[inde]);
                    break;
                case 1:
                    strcpy(saddr[ssev_inde].sin_port, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
        ssev_inde++;
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return ssev_inde;
}

#define SELECT_SSEVAP_QUERY \
	"SELECT Cache_port FROM ssev WHERE Cache_addr = \"%s\";"
int get_aport_by_auth(short *bind_port, struct ssev_config *config, const char *cache_addr) {
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
_LOG_WARN("sin_addr:%s user_name:%s password:%s database:%s sin_port:%d", 
        config->address.sin_addr, config->user_name, config->password, config->database, config->address.sin_port);
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_SSEVAP_QUERY, cache_addr);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
_LOG_WARN("couldn't get result from %s", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 > mysql_num_rows(respo)) {
_LOG_WARN("the result set error");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    //
    int port_inde = 0x00;
    while ((row = mysql_fetch_row(respo))) {
        bind_port[port_inde++] = atoi(row[0x00]);
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return port_inde;
}

//
#define SELECT_SSEVAU_QUERY \
	"SELECT Cache_addr, Cache_port FROM ssev WHERE Serv_addr = \"%s\";"
int get_auth_by_ssev(serv_addr *caddr, struct ssev_config *config, const char *serv_addr){
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
_LOG_WARN("sin_addr:%s user_name:%s password:%s database:%s sin_port:%d", 
        config->address.sin_addr, config->user_name, config->password, config->database, config->address.sin_port);
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_SSEVAU_QUERY, serv_addr);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
_LOG_WARN("couldn't get result from %s", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 != mysql_num_rows(respo)) {
_LOG_WARN("the result set error");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    //
    while ((row = mysql_fetch_row(respo))) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    strcpy(caddr->sin_addr, row[inde]);
                    break;
                case 1:
                    caddr->sin_port = atoi(row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

//
#define SELECT_SSEVSP_QUERY \
	"SELECT Bind_port FROM ssev WHERE Serv_addr = \"%s\";"
int get_sport_by_ssev(char *bind_port, struct ssev_config *config, const char *serv_addr){
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
printf("couldn't connect to engine!%d %s\n", mysql_errno(sql_sock), mysql_error(sql_sock));
printf("sin_addr:%s user_name:%s password:%s database:%s sin_port:%d\n", 
        config->address.sin_addr, config->user_name, config->password, config->database, config->address.sin_port);
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_SSEVSP_QUERY, serv_addr);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
printf("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
printf("couldn't get result from %s\n", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo)); // disable by james 20130409
    if (0x01 != mysql_num_rows(respo)) {
printf("the result set error!\n");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    //
    while ((row = mysql_fetch_row(respo))) {
        strcpy(bind_port, row[0]);
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}