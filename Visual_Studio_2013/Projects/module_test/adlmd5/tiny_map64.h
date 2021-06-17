/*
* tiny_map64.h todo: finish 64
*
*  Created on: 2010-3-9
*      Author: Administrator
*/

#ifndef TINY_MAP64_H_
#define TINY_MAP64_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "windows.h"

struct tiny_map64 {
	unsigned __int64 file_size;	/* File size (from status)		*/
	unsigned __int64 p_offset;		/* Window start				*/
	unsigned __int64 p_fd_offset;	/* offset of cursor in fd ala lseek	*/
	unsigned char *p;			/* Window pointer			*/
	int p_size;		/* Largest wind size we allocated	*/
	int p_len;		/* Latest (rounded) wind size		*/
	int def_wind_size;	/* Default wind size			*/
	HANDLE hfile;			/* File Descriptor			*/
	int status;			/* first errno from read errors		*/
};

struct tiny_map64 *map_file64(HANDLE hfile, unsigned __int64 len, int read_size, int blk_size);
unsigned char *map_ptr64(struct tiny_map64 *map, unsigned __int64 offset, int len);
int unmap_file64(struct tiny_map64 *map);

#endif /* TINY_MAP64_H_ */

