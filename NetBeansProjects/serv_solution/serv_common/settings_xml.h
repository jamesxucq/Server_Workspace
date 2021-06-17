/* 
 * File:   settings_xml.h
 * Author: David
 *
 * Created on 2011
 */

#ifndef SETTINGSXML_H
#define	SETTINGSXML_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"    

    struct xml_settings {
        unsigned int uid;
        char client_version[VERSION_LENGTH];
        int pool_size;
        int data_bomb;
    };

#define NEW_SETTINGS_VALUE(xml_buff) xml_buff = malloc(XML_LENGTH);
#define DEL_SETTINGS_VALUE(xml_buff) if(xml_buff) free(xml_buff);  

    int settings_xml_value(char *xml_txt, struct xml_settings *settings);

#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGSXML_H */

