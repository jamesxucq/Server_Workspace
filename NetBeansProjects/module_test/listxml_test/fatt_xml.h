/* 
 * File:   fatt_xml.h
 * Author: Divad
 */

#ifndef FATT_XML_H
#define	FATT_XML_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "fatt_list.h"
#include "recu_utility.h"

int flush_elist_xmlfile(FILE *alist_xml, char *location, char *list_file);

#ifdef	__cplusplus
}
#endif

#endif	/* FATT_XML_H */

