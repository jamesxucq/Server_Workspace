/* 
 * File:   dires_xml.h
 * Author: Divad
 */

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef DIRESXML_H
#define	DIRESXML_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recu_utility.h"

int flush_dlist_xmlfile(FILE *dlist_xml, char *location, char *list_file);

#ifdef	__cplusplus
}
#endif

#endif	/* DIRESXML_H */

