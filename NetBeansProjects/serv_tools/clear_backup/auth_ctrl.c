#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>  // /usr/local/include/mysql
// GRANT ALL PRIVILEGES ON *.* TO root@'%' identified by '123456';
#include "logger.h"
#include "auth_ctrl.h"

#define QUERY_LENGTH    512

#define SELECT_UNAME_QUERY \
        "SELECT User_id, Location, Auth_host, Access_key FROM user WHERE User_id >= %d AND User_id < %d;"

int get_auth_value(struct auth_value *avalue, struct auth_config *config, unsigned int uid, int count) {
    MYSQL mysql, *sock;
    MYSQL_RES *response;
    MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];

    if (!config) return 0;
    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
        LOG_INFO("couldn't connect to engine!\n%s", mysql_error(&mysql));
        return 0;
    }

    sprintf(query, SELECT_UNAME_QUERY, uid, uid + count);
    if (mysql_real_query(sock, query, (unsigned int) strlen(query))) {
        LOG_INFO("query failed (%s)\n", mysql_error(sock));
        mysql_close(sock);
        return 0;
    }
    if (!(response = mysql_store_result(sock))) {
        LOG_INFO("couldn't get result from %s", mysql_error(sock));
        mysql_free_result(response);
        mysql_close(sock);
        return 0;
    }

    // LOG_INFO("number of fields returned: %d", mysql_num_fields(response));
    int auth_num = mysql_num_rows(response);
    int row_idx = 0;
    while (row = mysql_fetch_row(response)) {
        int index;
        for (index = 0; index < mysql_num_fields(response); index++) {
            // LOG_INFO("%s", row[index]);
            switch (index) {
                case 0:
                    avalue[row_idx].uid = atol(row[index]);
                case 1:
                    strcpy(avalue[row_idx].location, row[index]);
                    break;
                case 2:
                    address_split(&avalue[row_idx].saddr, row[index]);
                    break;
                case 3:
                    strcpy(avalue[row_idx].access_key, row[index]);
                    break;
            }
        }
        row_idx++;
    }
    LOG_INFO("query auth data base OK");
    mysql_free_result(response);
    mysql_close(sock);

    return auth_num;
}

//
int address_split(serv_addr *saddr, char * addr_txt) {
    char *token = NULL;

    if (!saddr || !addr_txt) return -1;
    memset(saddr, '\0', sizeof (serv_addr));

    token = strchr(addr_txt, ':');
    if (token) {
        strncpy(saddr->sin_addr, addr_txt, token - addr_txt);
        saddr->sin_port = atoi(++token);
    } else strcpy(saddr->sin_addr, addr_txt);

    return 0;
}

#define SELECT_LINKED_QUERY \
        "SELECT Linked_status FROM user WHERE User_id = %d;"

int get_linked_status(char *linked_status, struct auth_config *config, unsigned int uid) {
    MYSQL mysql, *sock;
    MYSQL_RES *response;
    MYSQL_FIELD *fd;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
        LOG_INFO("couldn't connect to engine!\n%s", mysql_error(&mysql));
        return -1;
    }

    sprintf(query, SELECT_LINKED_QUERY, uid);
    if (mysql_real_query(sock, query, (unsigned int) strlen(query))) {
        LOG_INFO("query failed (%s)", mysql_error(sock));
        mysql_close(sock);
        return -1;
    }
    if (!(response = mysql_store_result(sock))) {
        LOG_INFO("couldn't get result from %s", mysql_error(sock));
        mysql_free_result(response);
        mysql_close(sock);
        return -1;
    }

    LOG_INFO("number of fields returned: %d", mysql_num_fields(response));
    if (mysql_num_rows(response) != 1) {
        LOG_INFO("the result set error");
        mysql_free_result(response);
        mysql_close(sock);
        return -1;
    }

    while (row = mysql_fetch_row(response)) {
        strcpy(linked_status, row[0]);
        LOG_INFO("query auth data base OK");
    }
    mysql_free_result(response);
    mysql_close(sock);

    return 0;
}


#define UPDATE_LINKED_QUERY \
	"UPDATE user SET Linked_status=\"%s\" WHERE User_id = %lu;"

int update_linked_status(struct auth_config *config, unsigned int uid, char *linked_status) {
    MYSQL mysql, *sock;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
        LOG_INFO("couldn't connect to engine!\n%s", mysql_error(&mysql));
        return -1;
    }

    sprintf(query, UPDATE_LINKED_QUERY, linked_status, uid);
    if (mysql_real_query(sock, query, (unsigned int) strlen(query))) {
        LOG_INFO("query failed (%s)", mysql_error(sock));
        mysql_close(sock);
        return -1;
    }
    mysql_close(sock);

    return 0;
}