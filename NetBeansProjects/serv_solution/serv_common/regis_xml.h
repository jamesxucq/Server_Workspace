/* 
 * File:   regis_xml.h
 * Author: Divad
 *
 * Created on 2011年11月21日, 上午9:15
 */

#ifndef REGISXML_H
#define	REGISXML_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"
#include "string_utility.h"

    struct xml_register {
        unsigned int uid;
        serv_addr cache_addr;
        char access_key[KEY_LENGTH];
    };

#define NEW_REGIS_VALUE(xml_buff) xml_buff = malloc(XML_LENGTH);
#define DEL_REGIS_VALUE(xml_buff) if(xml_buff) free(xml_buff);

    int regis_xml_value(char *xml_txt, struct xml_register *regis);

#ifdef	__cplusplus
}
#endif

#endif	/* REGISXML_H */

