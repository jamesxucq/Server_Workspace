#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>  // /usr/local/include/mysql
// GRANT ALL PRIVILEGES ON *.* TO root@'%' identified by '123456';
#include "logger.h"
#include "auth_ctrl.h"

#define QUERY_LENGTH    512

//
#define SELECT_LOCATION_QUERY \
        "SELECT Location FROM user WHERE User_name=\"%s\";"

int get_auth_value(char *location, struct auth_config *config, char *user_name) {
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

    sprintf(query, SELECT_LOCATION_QUERY, user_name);
// LOG_INFO("sql:%s\n", query);
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
    if (mysql_num_rows(response) < 1) {
LOG_INFO("mysql_num_rows:%d", mysql_num_rows(response));
        LOG_INFO("the result set error");
        mysql_free_result(response);
        mysql_close(sock);
        return -1;
    }

    while (row = mysql_fetch_row(response)) {
        strcpy(location, row[0]);
        LOG_INFO("query auth data base OK");
    }
    mysql_free_result(response);
    mysql_close(sock);
//
    return 0;
}


//
#define DELE_USER_QUERY \
	"DELETE FROM user WHERE User_name=\"%s\";"

int dele_user_query(struct auth_config *config, char *user_name) {
    MYSQL mysql, *sock;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
        LOG_INFO("couldn't connect to engine!\n%s", mysql_error(&mysql));
        return -1;
    }

    sprintf(query, DELE_USER_QUERY, user_name);
// LOG_INFO("sql:%s\n", query);
    if (mysql_real_query(sock, query, (unsigned int) strlen(query))) {
        LOG_INFO("query failed (%s)", mysql_error(sock));
        mysql_close(sock);
        return -1;
    }
    mysql_close(sock);
    //
    return 0;
}
