
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include "macro.h"
#include "config.h"

//

int user_config_parser(struct aupa_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    //    xmlChar *xml_name_str = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (curNode != NULL) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "PoolSize")) {
            sValue = xmlNodeGetContent(curNode);
            config->pool_size = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Location")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->location, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "AuthHost")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->auth_host, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Version")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->version, (char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }

    return ERR_SUCCESS;
}

int auth_db_parser(struct aupa_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    //    xmlChar *xml_name_str = NULL;

    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (curNode != NULL) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "Address")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->auth_db.address.sin_addr, (char *) sValue);
            xmlFree(sValue);
            ////////////////////////////////////////////////////////////
            sValue = xmlGetProp(curNode, "Port");
            config->auth_db.address.sin_port = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "DBName")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->auth_db.database, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "User")) {
            sValue = xmlGetProp(curNode, "Name");
            strcpy(config->auth_db.user_name, (char *) sValue);
            xmlFree(sValue);
            ////////////////////////////////////////////////////////////
            sValue = xmlGetProp(curNode, "Password");
            strcpy(config->auth_db.password, (char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }

    return ERR_SUCCESS;
}

int debug_level_value(char *level_txt) {
    int level_type;

    if (!strcmp("FATAL", level_txt)) level_type = 0;
    else if (!strcmp("ERROR", level_txt)) level_type = 1;
    else if (!strcmp("WARN", level_txt)) level_type = 2;
    else if (!strcmp("INFO", level_txt)) level_type = 3;
    else if (!strcmp("DEBUG", level_txt)) level_type = 4;
    else if (!strcmp("TRACE", level_txt)) level_type = 5;
    else if (!strcmp("MAX", level_txt)) level_type = 6;
    else level_type = 6;

    return level_type;
}

int log_config_parser(struct aupa_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    //    xmlChar *xml_name_str = NULL;

    /* We don't care what the top level element name is */
    // curNode = curNode->xmlChildrenNode;
    while (curNode != NULL) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "RuntimeLog")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->runtime_log.logfile, (char *) sValue);
            xmlFree(sValue);
            ////////////////////////////////////////////////////////////
            sValue = xmlGetProp(curNode, "DebugLevel");
            config->runtime_log.debug_level = debug_level_value((char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }

    return ERR_SUCCESS;
}

int parse_aupa_config(struct aupa_config *config, const char *xml_config) {
    xmlDocPtr docXml = NULL;
    xmlNodePtr curNode = NULL;
    //int done = 0;

    memset(config, '\0', sizeof (struct aupa_config));
    //
    docXml = xmlParseFile(xml_config);
    if (docXml == NULL) return ERR_FAULT;
    /** Check the document is of the right kind*/
    curNode = xmlDocGetRootElement(docXml);
    if (curNode == NULL) {
        //WriteErrorLog("empty document\n");
        xmlFreeDoc(docXml);
        return ERR_FAULT;
    }
    if (xmlStrcmp(curNode->name, (const xmlChar *) "AuthPath")) {
        //WriteErrorLog("document of the wrong type, root node != desktop-config\n");
        xmlFreeDoc(docXml);
        return ERR_FAULT;
    }

    /** Now, walk the tree.*/
    /* First level is a list of service*/
    curNode = curNode->xmlChildrenNode;
    while (curNode != NULL) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "UserConfig")) {
            if (user_config_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "AuthDB")) {
            if (auth_db_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "RuntimeLog")) {
            if (log_config_parser(config, docXml, curNode))
                return ERR_FAULT;
        }
        curNode = curNode->next;
    }

    xmlFreeDoc(docXml);
    /* Clean up everything else before quitting. */
    xmlCleanupParser();

    return ERR_SUCCESS;
}

int parse_create(struct aupa_config *config, char *xml_config) {
    if (!config || !xml_config) return -1;
    memset(config, '\0', sizeof (struct aupa_config));
    if (parse_aupa_config(config, xml_config)) return -1;
    return 0;
}

inline int parse_destroy() {
    return 0;
}
