/* 
 * File:   list_xml.h
 * Author: David
 * Created on 2012
 */

#ifndef LISTXML_H
#define	LISTXML_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"
#include "addr_list.h"
#include "server_list.h"    

#define NEW_LIST_VALUE(xml_buff) xml_buff = malloc(XML_LENGTH);
#define DEL_LIST_VALUE(xml_buff) if(xml_buff) free(xml_buff);       

    int admin_xml_value(char *xml_txt, struct addr_list *alist);
    int serv_xml_value(char *xml_txt, struct server_list *slist);
    
#ifdef	__cplusplus
}
#endif

#endif	/* LISTXML_H */

