/*
 * ParseConf.c
 *
 *  Created on: 2010-3-8
 *      Author: Divad
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include "common_header.h"

#include "parse_conf.h"

//
#define DEFAULT_VALUE    "Default"

//
struct regis_config _regis_config_;
char _config_path_[MAX_PATH * 3];

void bind_port_parser(short bind_port[], int max_port, char *server_port) {
    char *toksp, *tokep;
    char port_str[MIN_TEXT_LEN];
    int inde;
    // for (inde = 0; inde < max_port; port_array[inde] = 0, inde++);
    memset(bind_port, '\0', sizeof (short)*max_port);
    //
    tokep = toksp = server_port;
    for (inde = 0; tokep && (inde < max_port); toksp = tokep + 1, inde++) {
        if ((tokep = strchr(toksp, ','))) {
            strncpy(port_str, toksp, tokep - toksp);
            port_str[tokep - toksp] = '\0';
            bind_port[inde] = atoi(port_str);
        } else bind_port[inde] = atoi(toksp);
    }
}

int serv_config_parser(struct regis_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "UserName")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                strcpy(config->user_name, RSEV_USER_DEFAULT);
            } else strcpy(config->user_name, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, BAD_CAST "Group");
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                strcpy(config->group_name, RSEV_GROUP_DEFAULT);
            } else strcpy(config->group_name, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Port")) {
            sValue = xmlNodeGetContent(curNode);
            bind_port_parser(config->bind_port, MAX_BIND_PORT, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "MaxConnecting")) {
            sValue = xmlNodeGetContent(curNode);
            config->max_connecting = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "TimeOut")) {
            sValue = xmlNodeGetContent(curNode);
            config->time_out = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "LeaseTime")) {
            sValue = xmlNodeGetContent(curNode);
            config->lease_time = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "LockFile")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->lock_file, RSEV_LOCK_DEFAULT)
            } else strcpy(config->lock_file, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "PidFile")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->pid_file, RSEV_PID_DEFAULT)
            } else strcpy(config->pid_file, (char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
//
    return ERR_SUCCESS;
}

int auth_db_parser(struct regis_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "Address")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->auth_db.address.sin_addr, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "Port");
            config->auth_db.address.sin_port = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "DBName")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->auth_db.database, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "User")) {
            sValue = xmlGetProp(curNode, (const xmlChar *) "Name");
            strcpy(config->auth_db.user_name, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "Password");
            strcpy(config->auth_db.password, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Ping")) {
            sValue = xmlGetProp(curNode, (const xmlChar *) "Name");
            strcpy(config->ping_auth.param_str, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "UserId");
            config->ping_auth.param_int = atoi((char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
//
    return ERR_SUCCESS;
}

int debug_level_value(char *level_txt) {
    int level_type;
//
    if (!strcmp("FATAL", level_txt)) level_type = 0;
    else if (!strcmp("ERROR", level_txt)) level_type = 1;
    else if (!strcmp("WARN", level_txt)) level_type = 2;
    else if (!strcmp("INFO", level_txt)) level_type = 3;
    else if (!strcmp("DEBUG", level_txt)) level_type = 4;
    else if (!strcmp("TRACE", level_txt)) level_type = 5;
    else if (!strcmp("MAX", level_txt)) level_type = 6;
    else level_type = 6;
//
    return level_type;
}

int log_config_parser(struct regis_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "RuntimeLog")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->runtime_log.logfile, RSEV_LOG_DEFAULT)
            } else strcpy(config->runtime_log.logfile, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "DebugLevel");
            config->runtime_log.debug_level = debug_level_value((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "AccessLog")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->access_log.logfile, RSEV_ACCESS_DEFAULT)
            } else strcpy(config->access_log.logfile, (char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
//
    return ERR_SUCCESS;
}

int parse_regis_config(struct regis_config *config, const char *xml_config) {
    xmlDocPtr docXml = NULL;
    xmlNodePtr curNode = NULL;
    //
    docXml = xmlParseFile(xml_config);
    if (NULL == docXml) return ERR_FAULT;
    /** Check the document is of the right kind*/
    curNode = xmlDocGetRootElement(docXml);
    if (NULL == curNode) {
        //WriteErrorLog("empty document\n");
        xmlFreeDoc(docXml);
        return ERR_FAULT;
    }
    if (xmlStrcmp(curNode->name, (const xmlChar *) "RegisServ")) {
        //WriteErrorLog("document of the wrong type, root node != desktop-config\n");
        xmlFreeDoc(docXml);
        return ERR_FAULT;
    }
    /** Now, walk the tree.*/
    /* First level is a list of service*/
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "ServConfig")) {
            if (serv_config_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "AuthDB")) {
            if (auth_db_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "LogConfig")) {
            if (log_config_parser(config, docXml, curNode))
                return ERR_FAULT;
        }
        curNode = curNode->next;
    }
    xmlFreeDoc(docXml);
    /* Clean up everything else before quitting. */
    xmlCleanupParser();
//
    return ERR_SUCCESS;
}

void serialize(char *sbuf, struct regis_config *config) {
    /* sprintf(sbuf, "serv_config is:\n"
                "\tmax_con:%d lock_file:[%s]\n"
                "\tpid_file:[%s]  tlog_file:[%s]\n"
                "\tauth_host:[%s]\tauth_port:%d\tuname:[%s]\n"
                "\tpasswd:[%s]\tdbname:[%s]\n",
                //Server info
                config->max_connecting,
                config->lock_file,
                config->pid_file
                // config->sLogFile,
                //Auth DB
                // config->user_name,
                // config->sPassword,
                // config->sAuthDB
                );
     */
}

int regis_config_from_xml(struct regis_config *config, const char *xml_config) {
    if (!config || !xml_config) return -1;
    memset(config, '\0', sizeof (struct regis_config));
    if (parse_regis_config(config, xml_config)) return -1;
#ifdef _DEBUG
    //serialize(config);
#endif
    return 0;
}


