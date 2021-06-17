/* 
 * File:   status_xml.h
 * Author: David
 *
 * Created on 2011
 */

#ifndef STATUSXML_H
#define	STATUSXML_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>   

#include "common_macro.h"    

    struct xml_status {
        unsigned int uid;
        unsigned int cached_anchor;
        char location[MAX_PATH * 3];
        char cached_seion[SESSION_LENGTH];
    };

#define NEW_STATUS_VALUE(xml_buff) xml_buff = malloc(XML_LENGTH);
#define DEL_STATUS_VALUE(xml_buff) if(xml_buff) free(xml_buff);    

    int status_xml_value(char *xml_txt, struct xml_status *cached_status);
    int status_from_xmlfile(struct xml_status *cached_status, char *xml_txt);

#ifdef	__cplusplus
}
#endif

#endif	/* STATUSXML_H */

