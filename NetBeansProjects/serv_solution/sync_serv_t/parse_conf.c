/* parse_conf.c
 * Created on: 2010-3-8
 * Author: Divad
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
struct serv_config _serv_config_;
char _config_path_[MAX_PATH * 3];

//

int serv_config_parser(struct serv_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "User")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                strcpy(config->user_name, SSEV_USER_DEFAULT);
            } else strcpy(config->user_name, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, BAD_CAST "Group");
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                strcpy(config->group_name, SSEV_GROUP_DEFAULT);
            } else strcpy(config->group_name, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Address")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->serv_address, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "MaxConnecting")) {
            sValue = xmlNodeGetContent(curNode);
            config->max_connecting = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "TimeOut")) {
            sValue = xmlNodeGetContent(curNode);
            config->time_out = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "SessionTimeOut")) {
            sValue = xmlNodeGetContent(curNode);
            config->seion_timeout = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "LockFile")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->lock_file, SSEV_LOCK_DEFAULT)
            } else strcpy(config->lock_file, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "PidFile")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->pid_file, SSEV_PID_DEFAULT)
            } else strcpy(config->pid_file, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Anchor")) {
            sValue = xmlGetProp(curNode, BAD_CAST "Index");
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                strcpy(config->anchor.anch_idx, ANCH_INDE_DEFAULT);
            } else strcpy(config->anchor.anch_idx, (char *) sValue);
            xmlFree(sValue);
            //            
            sValue = xmlGetProp(curNode, BAD_CAST "Data");
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                strcpy(config->anchor.anch_acd, ANCH_DATA_DEFAULT);
            } else strcpy(config->anchor.anch_acd, (char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
    //
    return ERR_SUCCESS;
}

int ssev_dbase_parser(struct serv_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "Address")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->ssev_db.address.sin_addr, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "Port");
            config->ssev_db.address.sin_port = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "DBName")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->ssev_db.database, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "User")) {
            sValue = xmlGetProp(curNode, (const xmlChar *) "Name");
            strcpy(config->ssev_db.user_name, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "Password");
            strcpy(config->ssev_db.password, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Ping")) {
            sValue = xmlGetProp(curNode, (const xmlChar *) "CacheAddr");
            strcpy(config->ping_ssev.param_str, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "Port");
            config->ping_ssev.param_int = atoi((char *) sValue);
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

int log_config_parser(struct serv_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "RuntimeLog")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->runtime_log.logfile, SSEV_LOG_DEFAULT)
            } else strcpy(config->runtime_log.logfile, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "DebugLevel");
            config->runtime_log.debug_level = debug_level_value((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "AccessLog")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->access_log.logfile, SSEV_ACCESS_DEFAULT)
            } else strcpy(config->access_log.logfile, (char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
    //
    return ERR_SUCCESS;
}

int bakup_ctrl_parser(struct serv_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "MaxModify")) {
            sValue = xmlNodeGetContent(curNode);
            config->bakup_ctrl.max_modify = atoi((char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "IntervTime");
            config->bakup_ctrl.modi_interv = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "MaxDelete")) {
            sValue = xmlNodeGetContent(curNode);
            config->bakup_ctrl.max_delete = atoi((char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "IntervTime");
            config->bakup_ctrl.dele_interv = atoi((char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
    //
    return ERR_SUCCESS;
}

int parse_serv_config(struct serv_config *config, const char *xml_config) {
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
    if (xmlStrcmp(curNode->name, (const xmlChar *) "SyncServ")) {
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
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "ServeDB")) {
            if (ssev_dbase_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "BackupCtrl")) {
            if (bakup_ctrl_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "LogConfig")) {
            if (log_config_parser(config, docXml, curNode))
                return ERR_FAULT;
        }
        curNode = curNode->next;
    }
    //
    xmlFreeDoc(docXml);
    /* Clean up everything else before quitting. */
    xmlCleanupParser();
    return ERR_SUCCESS;
}

void serialize(char *sbuf, struct serv_config *config) {
    /* sprintf(sbuf, "serv_config is:\n"
                "\tmax_con:%d lock_file:[%s]\n"
                "\tpid_file:[%s]  tlog_file:[%s]\n"
                "\tauth_host:[%s]\tauth_port:%d\tuname:[%s]\n"
                "\tpasswd:[%s]\tdbname:[%s]\n",
                // Server info
                config->max_connecting,
                config->lock_file,
                config->pid_file,
                // config->sLogFile,
                // Auth DB
                config->cache_saddr);
     */
}

int serv_config_from_xml(struct serv_config *config, const char *xml_config) {
    if (!config || !xml_config) return -1;
    memset(config, '\0', sizeof (struct serv_config));
    if (parse_serv_config(config, xml_config)) return -1;
#ifdef _DEBUG
    //serialize(config);
#endif
    return 0;
}



