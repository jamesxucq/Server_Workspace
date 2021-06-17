#include "stdafx.h"

#include "MemoryUtility.h"
#include "FileIO.h"
// #include "Logger.h"


#ifndef ENODATA
#define ENODATA EAGAIN
#endif
/* This provides functionality somewhat similar to mmap() but using read().
* It gives sliding window access to a file.  mmap() is not used because of
* the possibility of another program (such as a mailer) truncating the
* file thus giving us a SIGBUS. */
struct tiny_map *map_file(HANDLE hFile, off_t len, int read_size, int blk_size)
{
	struct tiny_map *map;

	if (!(map = newc(struct tiny_map))) {
		// LOG_DEBUG(_T("map_file newc error\n"));
	}

	if (blk_size && (read_size % blk_size))
		read_size += blk_size - (read_size % blk_size);

	map->hFile = hFile;
	map->file_size = len;
	map->def_window_size = read_size;

	return map;
}


/* slide the read window in the file */
char *map_ptr(struct tiny_map *map, off_t offset, int len)
{
	// size_t nread;
	DWORD nread;
	off_t window_start, read_start;
	int window_size, read_size, read_offset;

	if (len == 0) return NULL;
	if (len < 0) {
		// LOG_DEBUG(_T("invalid len passed to map_ptr: %ld\n"), (long)len);
		return NULL;
	}

	/* in most cases the region will already be available */
	if (offset >= map->p_offset && offset+len <= map->p_offset+map->p_len)
		return map->p + (offset - map->p_offset);

	/* nope, we are going to have to do a read. Work out our desired window */
	window_start = offset;
	window_size = map->def_window_size;
	if (window_start + window_size > map->file_size)
		window_size = (int)(map->file_size - window_start);
	if (len > window_size)
		window_size = len;

	/* make sure we have allocated enough memory for the window */
	if (window_size > map->p_size) {
		map->p = realloc_array(map->p, char, window_size);
		if (!map->p) {
			// LOG_DEBUG(_T("map_ptr newc error\n"));
		}
		map->p_size = window_size;
	}

	/* Now try to avoid re-reading any bytes by reusing any bytes
	* from the previous buffer. */
	if (window_start >= map->p_offset &&
		window_start < map->p_offset + map->p_len &&
		window_start + window_size >= map->p_offset + map->p_len) {
			read_start = map->p_offset + map->p_len;
			read_offset = (int)(read_start - window_start);
			read_size = window_size - read_offset;
			memmove(map->p, map->p + (map->p_len - read_offset), read_offset);
	} else {
		read_start = window_start;
		read_size = window_size;
		read_offset = 0;
	}

	if (read_size <= 0) {
		// LOG_DEBUG(_T("invalid read_size of %ld in map_ptr\n"), (long)read_size);
		return NULL;
	}

	if (map->p_fd_offset != read_start) {
		/*
		off_t ret = fseek(map->hFile, read_start, SEEK_SET);
		if (ret != read_start) {
		LOG_DEBUG(_T("lseek returned %.0f, not %.0f"), (double)ret, (double)read_start);
		LOG_DEBUG(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return NULL;
		}
		*/
		DWORD result = SetFilePointer(map->hFile, read_start, NULL, FILE_BEGIN);
		if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) {
			// LOG_DEBUG(_T("set file pointer error!"));
			return NULL;
		}
		map->p_fd_offset = read_start;
	}
	map->p_offset = window_start;
	map->p_len = window_size;

	while (read_size > 0) {
		// nread = fread(map->p + read_offset, 1, read_size, map->hFile);
		if(!ReadFile(map->hFile, map->p + read_offset, read_size, &nread, NULL)) {
			if(ERROR_HANDLE_EOF != GetLastError()) {
				// LOG_DEBUG(_T("read check sum file error, error no is:%d."), GetLastError());			
				return NULL;
			}
		}
		if (nread <= 0) {
			if (!map->status)
				map->status = nread ? errno : ENODATA;
			/* The best we can do is zero the buffer -- the file
			* has changed mid transfer! */
			memset(map->p + read_offset, 0, read_size);
			break;
		}
		map->p_fd_offset += (off_t)nread;
		read_offset += (int)nread;
		read_size -= (int)nread;
	}

	return map->p;
}


int unmap_file(struct tiny_map *map)
{
	int	ret;

	if (map->p) {
		free(map->p);
		map->p = NULL;
	}
	ret = map->status;
	memset(map, 0, sizeof map[0]);
	free(map);

	return ret;
}



