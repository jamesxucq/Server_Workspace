/*
* FileIO.h
*
*  Created on: 2010-3-9
*      Author: Administrator
*/

#ifndef FILEIO_H_
#define FILEIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>


struct tiny_map {
	off_t file_size;	/* File size (from status)		*/
	off_t p_offset;		/* Window start				*/
	off_t p_fd_offset;	/* offset of cursor in fd ala lseek	*/
	char *p;			/* Window pointer			*/
	int p_size;		/* Largest window size we allocated	*/
	int p_len;		/* Latest (rounded) window size		*/
	int def_window_size;	/* Default window size			*/
	HANDLE hFile;			/* File Descriptor			*/
	int status;			/* first errno from read errors		*/
};

struct tiny_map *map_file(HANDLE hFile, off_t len, int read_size, int blk_size);
char *map_ptr(struct tiny_map *map, off_t offset, int len);
int unmap_file(struct tiny_map *map);

#endif /* FILEIO_H_ */

