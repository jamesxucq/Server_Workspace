/*
 * options.h
 *
 *  Created on: 2010-3-17
 *      Author: David
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "macro.h"

#define VERSION 	"1.0"
#define COPYRIGHT 	"Copyright (C) 2012 Pu Shan"
#define DISCLAIMER      "This is not free software.  "
#define FUNCTION	"author server" 
#define USAGE		"Usage: %s [options] <input>\n"

#define CLEAR_BACKUP_DEFAULT    "./ClearBackup.xml"

int parse_args(OUT char *xml_config, int argc, char *argv[]);

#endif /* OPTIONS_H_ */
