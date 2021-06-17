/* 
 * File:   query_xml.h
 * Author: David
 *
 * Created on 2011
 */

#ifndef QUERYXML_H
#define	QUERYXML_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

#include "common_macro.h"
#include "string_utility.h"  
    
    struct xml_query {
        unsigned int uid;
        char req_valid[VALID_LENGTH];
        // int serv_locked; // 0:false 1:true
        unsigned int cached_anchor;
        serv_addr saddr;
        char seion_id[SESSION_LENGTH];
    };

#define NEW_QUERY_VALUE(xml_buff) xml_buff = malloc(XML_LENGTH);
#define DEL_QUERY_VALUE(xml_buff) if(xml_buff) free(xml_buff);

    int query_xml_value(char *xml_txt, struct xml_query *query_status);

#ifdef	__cplusplus
}
#endif

#endif	/* QUERYXML_H */

