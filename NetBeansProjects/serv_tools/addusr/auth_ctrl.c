#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>
#include "logger.h"
#include "auth_ctrl.h"

#define QUERY_LENGTH    512

//
#define ADDI_USER_QUERY \
	"INSERT INTO user(User_name, Password, Salt, Pool_size, Location, Auth_host, Access_key, Lease_time, Version) VALUES(\"%s\", \"%s\", \"%s\", \"%d\", \"%s\", \"%s\", \"MAXMAXMAX\", 1446908608, \"%s\");"

int addi_user_query(struct auth_config *config, struct user_info *uinfo) {
    MYSQL mysql, *sql_sock;
    char query[QUERY_LENGTH];

    mysql_init(&mysql);
    if (!(sql_sock = mysql_real_connect(&mysql, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0))) {
        LOG_INFO("couldn't connect to engine!\n%s", mysql_error(&mysql));
        return -1;
    }
//
    sprintf(query, ADDI_USER_QUERY, uinfo->user_name, uinfo->password, uinfo->salt, uinfo->pool_size, uinfo->location, uinfo->auth_host, uinfo->version);
// LOG_INFO("sql:%s\n", query);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
        LOG_INFO("query failed (%s)", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    mysql_close(sql_sock);
//
    return 0;
}

