
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include "macro.h"
#include "config.h"
#include "clear.h"

int erase_config_parser(struct erase_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    //    xmlChar *xml_name_str = NULL;

    if (!xmlStrcmp(curNode->name, (const xmlChar *) "EraseSpace")) {
        ////////////////////////////////////////////////////////////
        sValue = xmlGetProp(curNode, "StartID");
        config->eraspa.start_uid = atoi((char *) sValue);
        xmlFree(sValue);
        ////////////////////////////////////////////////////////////
        sValue = xmlGetProp(curNode, "Number");
        config->eraspa.number = atoi((char *) sValue);
        xmlFree(sValue);
        ////////////////////////////////////////////////////////////
        sValue = xmlGetProp(curNode, "Sleep");
        config->eraspa.sleep = atoi((char *) sValue);
        xmlFree(sValue);
    }

    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (curNode != NULL) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "Backup")) {
            ////////////////////////////////////////////////////////////
            sValue = xmlGetProp(curNode, "BakDay");
            config->eraspa.backup.backup_day = atoi((char *) sValue);
            xmlFree(sValue);
            ////////////////////////////////////////////////////////////
            sValue = xmlGetProp(curNode, "MaxLength");
            config->eraspa.backup.max_length = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Anchor")) {
            sValue = xmlGetProp(curNode, "LastDay");
            config->eraspa.anchor_day = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Location")) {
            ////////////////////////////////////////////////////////////
            sValue = xmlGetProp(curNode, "Path");
            strcpy(config->eraspa.local.path, (char *) sValue);
            xmlFree(sValue);
            ////////////////////////////////////////////////////////////
            sValue = xmlGetProp(curNode, "DelimCount");
            config->eraspa.local.delim_count = atoi((char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }

    return ERR_SUCCESS;
}

int linked_config_parser(struct erase_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    //    xmlChar *xml_name_str = NULL;

    if (!xmlStrcmp(curNode->name, (const xmlChar *) "EraseLinked")) {
        ////////////////////////////////////////////////////////////
        sValue = xmlGetProp(curNode, "Unlink");
        config->eralink.unlink_day = atoi((char *) sValue);
        xmlFree(sValue);
        ////////////////////////////////////////////////////////////
        sValue = xmlGetProp(curNode, "DataBomb");
        config->eralink.bomb_day = atoi((char *) sValue);
        xmlFree(sValue);
    }

    return ERR_SUCCESS;
}

int auth_db_parser(struct erase_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
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

int log_config_parser(struct erase_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
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

int parse_erase_config(struct erase_config *config, const char *xml_config) {
    xmlDocPtr docXml = NULL;
    xmlNodePtr curNode = NULL;
    //int done = 0;

    memset(config, '\0', sizeof (struct erase_config));
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
    if (xmlStrcmp(curNode->name, (const xmlChar *) "EraseBackup")) {
        //WriteErrorLog("document of the wrong type, root node != desktop-config\n");
        xmlFreeDoc(docXml);
        return ERR_FAULT;
    }

    /** Now, walk the tree.*/
    /* First level is a list of service*/
    curNode = curNode->xmlChildrenNode;
    while (curNode != NULL) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "EraseSpace")) {
            if (erase_config_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "EraseLinked")) {
            if (linked_config_parser(config, docXml, curNode))
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

int parse_create(struct erase_config *config, char *xml_config) {
    if (!config || !xml_config) return -1;
    memset(config, '\0', sizeof (struct erase_config));
    if (parse_erase_config(config, xml_config)) return -1;
    return 0;
}

inline int parse_destroy() {
    return 0;
}