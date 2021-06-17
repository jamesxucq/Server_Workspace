#include "common.h"
// #include "logger.h"
#include "memory_utility.h"

#include "dmap64.h"

/* This provides functionality somewhat similar to mmap() but using read().
 * It gives sliding wind access to a file.  mmap() is not used because of
 * the possibility of another program (such as a mailer) truncating the
 * file thus giving us a SIGBUS. */
struct dmap64 *dmap_file64(int fd, uint64 len, int32 read_size, int32 blk_size) {
    struct dmap64 *map;
//
    if (!(map = malloc(sizeof (struct dmap64)))) {
        fprintf(stderr, "map file malloc error\n");
        return NULL;
    }
    memset(map, 0, sizeof (struct dmap64));
//
    if (blk_size && (read_size % blk_size))
        read_size += blk_size - (read_size % blk_size);
//
    map->fd = fd;
    lseek64(map->fd, 0, SEEK_SET);
    map->file_size = len;
    map->def_wind_size = read_size;
//
    return map;
}

/* slide the read wind in the file */
unsigned char *dmap_ptr64(struct dmap64 *map, uint64 offset, int32 len) {
    int32 rlen;
    uint64 wind_start, read_start;
    int32 wind_size, read_size, read_offset;
//
    if (len <= 0) {
        fprintf(stderr, "invalid len passed to map_ptr: %ld\n", (long) len);
        return NULL;
    }
//
    /* in most cases the region will already be available */
    if (offset >= map->p_offset && offset + len <= map->p_offset + map->p_len)
        return map->p + (offset - map->p_offset);
//
    /* nope, we are going to have to do a read. Work out our desired wind */
    wind_start = offset;
    wind_size = map->def_wind_size;
    if (wind_start + wind_size > map->file_size)
        wind_size = (int32) (map->file_size - wind_start);
    if (len > wind_size) wind_size = len;
//
    /* make sure we have allocated enough memory for the wind */
    if (wind_size > map->p_size) {
        map->p = realloc_array(map->p, sizeof (char), wind_size);
        if (!map->p) fprintf(stderr, "map_ptr newc error\n");
        map->p_size = wind_size;
    }
//
    /* Now try to avoid re-reading any bytes by reusing any bytes
     * from the previous buffer. */
    if (wind_start >= map->p_offset &&
            wind_start < map->p_offset + map->p_len &&
            wind_start + wind_size >= map->p_offset + map->p_len) {
        read_start = map->p_offset + map->p_len;
        read_offset = (int32) (read_start - wind_start);
        read_size = wind_size - read_offset;
        memmove(map->p, map->p + (map->p_len - read_offset), read_offset);
    } else {
        read_start = wind_start;
        read_size = wind_size;
        read_offset = 0;
    }
//
    if (read_size <= 0) {
        fprintf(stderr, "invalid read_size of %ld in map_ptr\n", (long) read_size);
        return NULL;
    }
//
    if (map->p_fd_offset != read_start) {
        if (-1 == lseek64(map->fd, read_start, SEEK_SET)) {
            fprintf(stderr, "lseek return error!");
            return NULL;
        }
        map->p_fd_offset = read_start;
    }
    map->p_offset = wind_start;
    map->p_len = wind_size;
//
    while (read_size > 0) {
        rlen = read(map->fd, map->p + read_offset, read_size);
        if (rlen <= 0) {
            if (!map->status) map->status = rlen ? errno : ENODATA;
            /* The best we can do is zero the buffer -- the file has changed mid transfer! */
            memset(map->p + read_offset, 0, read_size);
            break;
        }
        map->p_fd_offset += rlen;
        read_offset += rlen;
        read_size -= rlen;
    }
//
    return map->p;
}

int undmap_file64(struct dmap64 *map) {
    if (map->p) free(map->p);
    int ret = map->status;
    free(map);
    return ret;
}

