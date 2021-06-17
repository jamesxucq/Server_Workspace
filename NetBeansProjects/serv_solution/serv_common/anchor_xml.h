/* 
 * File:   anchor_xml.h
 * Author: Divad
 *
 * Created on 2011
 */

#ifndef ANCHORXML_H
#define	ANCHORXML_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"    

    struct xml_anchor {
        unsigned int uid;
        unsigned int last_anchor;
    };

#define NEW_ANCHOR_VALUE(xml_buff) xml_buff = (BYTE *)malloc(XML_LENGTH)
#define DELE_ANCHOR_VALUE(xml_buff) if(xml_buff) free(xml_buff)

    int anchor_xml_value(char *xml_buffer, struct xml_anchor *anchor);

#ifdef	__cplusplus
}
#endif

#endif	/* ANCHORXML_H */

