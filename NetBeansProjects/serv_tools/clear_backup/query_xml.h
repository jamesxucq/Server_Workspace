/* status_xml.h
 * Author: David
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

#include "macro.h"    

    struct xml_query {
        unsigned int uid;
        char validation[VALID_LENGTH];
        unsigned int locked;
    };    

    int query_from_xmlfile(struct xml_query *cached_query, char *xml_txt);

#ifdef	__cplusplus
}
#endif

#endif	/* STATUSXML_H */

