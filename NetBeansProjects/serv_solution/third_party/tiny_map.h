#ifndef TINYMAP_H_
#define TINYMAP_H_

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

struct tiny_map {
    off_t file_size; /* File size (from stat) */
    off_t p_offset; /* Window start */
    off_t p_fd_offset; /* offset of cursor in fd ala lseek */
    unsigned char *p; /* Window pointer */
    int32 p_size; /* Largest wind size we allocated */
    int32 p_len; /* Latest (rounded) wind size */
    int32 def_wind_size; /* Default wind size */
    int fd; /* File Descriptor */
    int status; /* first errno from read errors	*/
};

struct tiny_map *map_file(int fd, off_t len, int32 read_size, int32 blk_size);
unsigned char *map_ptr(struct tiny_map *map, off_t offset, int32 len);
int unmap_file(struct tiny_map *map);

#define MAX_MAP_SIZE    0x80000 // 512K

#ifdef	__cplusplus
}
#endif

#endif /* TINYMAP_H_ */

