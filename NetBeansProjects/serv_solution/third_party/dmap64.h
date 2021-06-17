/* 
 * File:   dmap64.h
 * Author: Administrator
 */

#ifndef TINY_MAP64_H
#define	TINY_MAP64_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "common.h"

struct dmap64 {
    uint64 file_size; /* File size (from stat) */
    uint64 p_offset; /* Window start */
    uint64 p_fd_offset; /* offset of cursor in fd ala lseek */
    unsigned char *p; /* Window pointer */
    int32 p_size; /* Largest wind size we allocated */
    int32 p_len; /* Latest (rounded) wind size */
    int32 def_wind_size; /* Default wind size */
    int fd; /* File Descriptor */
    int status; /* first errno from read errors	*/
};

struct dmap64 *dmap_file64(int fd, uint64 len, int32 read_size, int32 blk_size);
unsigned char *dmap_ptr64(struct dmap64 *map, uint64 offset, int32 len);
int undmap_file64(struct dmap64 *map);

#define MAX_MAP_SIZE    0x80000 // 512K

#ifdef	__cplusplus
}
#endif

#endif	/* TINY_MAP64_H */

