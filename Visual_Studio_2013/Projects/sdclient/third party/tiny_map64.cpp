#include "stdafx.h"

#include "memory_utility.h"
#include "tiny_map64.h"
#include "Logger.h"

#ifndef ENODATA
#define ENODATA EAGAIN
#endif
/* This provides functionality somewhat similar to mmap() but using read().
* It gives sliding wind access to a file.  mmap() is not used because of
* the possibility of another program (such as a mailer) truncating the
* file thus giving us a SIGBUS. */
struct tiny_map64 *map_file64(HANDLE hfile, unsigned __int64 len, int read_size, int blk_size)
{
	struct tiny_map64 *map;

	if (!(map = (struct tiny_map64 *)malloc(sizeof (struct tiny_map64)))) {
		_LOG_FATAL(_T("map file malloc error\n"));
		return NULL;
	}
	memset(map, 0, sizeof (struct tiny_map64));

	if (blk_size && (read_size % blk_size))
		read_size += blk_size - (read_size % blk_size);

	map->hfile = hfile;
	SetFilePointer(map->hfile, 0, NULL, FILE_BEGIN);
	map->file_size = len;
	map->def_wind_size = read_size;

	return map;
}

/* slide the read wind in the file */
unsigned char *map_ptr64(struct tiny_map64 *map, unsigned __int64 offset,int len)
{
	// size_t rlen;
	DWORD rlen;
	unsigned __int64 wind_start, read_start;
	int wind_size, read_size, read_offset;

	if (len <= 0) {
		_LOG_WARN(_T("invalid len passed to map_ptr: %ld"), (long)len);
		return NULL;
	}

	/* in most cases the region will already be available */
	if (offset >= map->p_offset && offset+len <= map->p_offset+map->p_len)
		return map->p + (offset - map->p_offset);

	/* nope, we are going to have to do a read. Work out our desired wind */
	wind_start = offset;
	wind_size = map->def_wind_size;
	if (wind_start + wind_size > map->file_size)
		wind_size = (int)(map->file_size - wind_start);
	if (len > wind_size) wind_size = len;

	/* make sure we have allocated enough memory for the wind */
	if (wind_size > map->p_size) {
		map->p = (unsigned char *)realloc_array(map->p, sizeof(char), wind_size);
		if (!map->p) {
			_LOG_WARN(_T("map_ptr newc error"));
		}
		map->p_size = wind_size;
	}

	/* Now try to avoid re-reading any bytes by reusing any bytes
	* from the previous buffer. */
	if (wind_start >= map->p_offset &&
		wind_start < map->p_offset + map->p_len &&
		wind_start + wind_size >= map->p_offset + map->p_len) {
			read_start = map->p_offset + map->p_len;
			read_offset = (int)(read_start - wind_start);
			read_size = wind_size - read_offset;
			memmove(map->p, map->p + (map->p_len - read_offset), read_offset);
	} else {
		read_start = wind_start;
		read_size = wind_size;
		read_offset = 0;
	}

	if (read_size <= 0) {
		_LOG_WARN(_T("invalid read_size of %ld in map_ptr"), (long)read_size);
		return NULL;
	}

	if (map->p_fd_offset != read_start) {
		LONG move_high = read_start >> 32;
		DWORD dwResult = SetFilePointer(map->hfile, read_start&0xffffffff, &move_high, FILE_BEGIN);
		if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
			_LOG_WARN(_T("set file pointer error!"));
			return NULL;
		}
		map->p_fd_offset = read_start;
	}
	map->p_offset = wind_start;
	map->p_len = wind_size;

	while (read_size > 0) {
		if(!ReadFile(map->hfile, map->p + read_offset, read_size, &rlen, NULL)) {
			if(ERROR_HANDLE_EOF != GetLastError()) {
				_LOG_WARN(_T("read check sum file error, error no is:%d."), GetLastError());			
				return NULL;
			}
		}
		if (rlen <= 0) {
			if (!map->status) map->status = rlen ? errno : ENODATA;
			/* The best we can do is zero the buffer -- the file has changed mid transfer! */
			memset(map->p + read_offset, 0, read_size);
			break;
		}
		map->p_fd_offset += (off_t)rlen;
		read_offset += (int)rlen;
		read_size -= (int)rlen;
	}

	return map->p;
}

int unmap_file64(struct tiny_map64 *map)
{
	if (map->p) free(map->p);
	int	ret = map->status;
	free(map);
	return ret;
}



