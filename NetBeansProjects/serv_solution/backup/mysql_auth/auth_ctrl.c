
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
    MYSQL mysql, *sql_sock;
    MYSQL_RES *respo;
    // MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

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

    while (row = mysql_fetch_row(respo)) {
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
                    strcpy(avalue->linked_status, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);

    return 0;
}

#define SELECT_UID_QUERY \
	"SELECT User_id, Location, Access_key FROM user WHERE User_id = %u;"

int get_cvalue_by_uid(struct cache_value *cvalue, struct auth_config *config, unsigned int uid) {
    MYSQL mysql, *sql_sock;
    MYSQL_RES *respo;
    // MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

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

    while (row = mysql_fetch_row(respo)) {
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

    return 0;
}

#define UPDATE_KEY_QUERY \
	"UPDATE user SET Access_key=\"%s\", Lease_time=%ld WHERE User_id = %u;"

int update_access_key(struct auth_config *config, unsigned int uid, char *access_key, long int lease_time) {
    MYSQL mysql, *sql_sock;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

    sprintf(query, UPDATE_KEY_QUERY, access_key, lease_time, uid);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    mysql_close(sql_sock);

    return 0;
}

#define UPDATE_LINKED_QUERY \
	"UPDATE user SET Linked_status=\"%s\" WHERE User_id = %u;"

int update_linked_status(struct auth_config *config, unsigned int uid, char *linked_status) {
    MYSQL mysql, *sql_sock;
    char query[LARGE_QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

    sprintf(query, UPDATE_LINKED_QUERY, linked_status, uid);
    // _LOG_INFO("UPDATE_LINKED_QUERY: %s", query);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
_LOG_WARN("query failed (%s)", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    mysql_close(sql_sock);

    return 0;
}

#define SELECT_PING_QUERY "SELECT * FROM user WHERE User_name = \"ping_auth\";"

int ping_auth_dbase(struct auth_config *config) {
    MYSQL mysql, *sql_sock;
    MYSQL_RES *respo;
    // MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
    
    mysql_init(&mysql);
    if (!(sql_sock == mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
printf("couldn't connect to engine!%d %s\n", mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

    strcpy(query, SELECT_PING_QUERY);
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
    while (row = mysql_fetch_row(respo)) {
printf("query auth data base ok!\n");
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);

    return 0;
}

#define SELECT_SETTING_QUERY \
	"SELECT User_id, Password, Salt, Pool_size, Version, Linked_status FROM user WHERE User_name = \"%s\";"

int get_svalue_by_uname(struct set_value *svalue, struct auth_config *config, const char *user_name) {
    MYSQL mysql, *sql_sock;
    MYSQL_RES *respo;
    // MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(&mysql), mysql_error(&mysql));
_LOG_WARN("sin_addr:%s user_name:%s password:%s database:%s sin_port:%d", 
        config->address.sin_addr, 
        config->user_name,
        config->password, 
        config->database, 
        config->address.sin_port);
        return -1;
    }

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

    while (row = mysql_fetch_row(respo)) {
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
                    strcpy(svalue->linked_status, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);

    return 0;
}

#define SELECT_LINK_QUERY \
	"SELECT User_id, Password, Salt, Linked_status FROM user WHERE User_name = \"%s\";"

int get_lvalue_by_uname(struct link_value *lvalue, struct auth_config *config, const char *user_name) {
    MYSQL mysql, *sql_sock;
    MYSQL_RES *respo;
    // MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }

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

    while (row = mysql_fetch_row(respo)) {
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
                    strcpy(lvalue->linked_status, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);

    return 0;
}

//
#define SELECT_SSEVCI_QUERY \
	"SELECT Serv_addr, Bind_port FROM ssev WHERE Cache_id = \"%s\";"

int get_ssev_by_auth(struct addr_list **alsth, struct ssev_config *config, const char *cache_id) {
    MYSQL mysql, *sql_sock;
    MYSQL_RES *respo;
    // MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(&mysql), mysql_error(&mysql));
_LOG_WARN("sin_addr:%s user_name:%s password:%s database:%s sin_port:%d", 
        config->address.sin_addr, 
        config->user_name,
        config->password, 
        config->database, 
        config->address.sin_port);
        return -1;
    }

    sprintf(query, SELECT_SSEVCI_QUERY, cache_id);
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
    serv_addr *saddr;
    while (row = mysql_fetch_row(respo)) {
        saddr = add_new_alist(alsth);
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    strcpy(saddr->sin_addr, row[inde]);
                    break;
                case 1:
                    saddr->sin_port = atoi(row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);

    return 0;
}

//
#define SELECT_SSEVSV_QUERY \
	"SELECT Cache_id FROM ssev WHERE Serv_addr = \"%s\" AND Bind_port = %d;"

int get_auth_by_ssev(char *cache_id, struct ssev_config *config, const char *serv_addr, int serv_port){
    MYSQL mysql, *sql_sock;
    MYSQL_RES *respo;
    // MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(&mysql), mysql_error(&mysql));
_LOG_WARN("sin_addr:%s user_name:%s password:%s database:%s sin_port:%d", 
        config->address.sin_addr, 
        config->user_name,
        config->password, 
        config->database, 
        config->address.sin_port);
        return -1;
    }

    sprintf(query, SELECT_SSEVSV_QUERY, serv_addr, serv_port);
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
    while (row = mysql_fetch_row(respo)) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    strcpy(cache_id, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);

    return 0;
}