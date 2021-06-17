#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>

#include "tiny_pool.h"

////////////////////////////////////////////////////////////////////////////////
#define MAX_ALLOC_FROM_POOL             (0x1000 - 1) // 4K-1
#define TINY_POOL_ALIGNMENT             16
#ifndef TINY_ALIGNMENT
#define TINY_ALIGNMENT   sizeof(unsigned long)    /* platform word */
#endif
#define TINY_INVALID_FILE         -1
#define TINY_FILE_ERROR           -1

#define tiny_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define tiny_align_ptr(p, a)                                                   \
	(unsigned char *) (((unsigned int) (p) + ((unsigned int) a - 1)) & ~((unsigned int) a - 1))

////////////////////////////////////////////////////////////////////////////////
#define tiny_memzero(buf, n)            (void) memset(buf, 0, n)
#define tiny_free                       free
#define tiny_alloc                      malloc
// #define tiny_memalign                   memalign
#define tiny_memalign(ALIGNMENT, size)  malloc(size)
//#define tiny_close                      close
#define tiny_close                      CloseHandle
#define tiny_delete(name)               _unlink((const char *) name)

////////////////////////////////////////////////////////////////////////////////
static void *tiny_palloc_block(struct tiny_pool *pool, size_t size);
static void *tiny_palloc_large(struct tiny_pool *pool, size_t size);

////////////////////////////////////////////////////////////////////////////////

struct tiny_pool *create_pool(size_t size) {
	struct tiny_pool *pool;

	pool = (struct tiny_pool *)tiny_memalign(TINY_POOL_ALIGNMENT, size);
	if (pool == NULL) return NULL;

	pool->data.last = (unsigned char *) pool + sizeof (struct tiny_pool);
	pool->data.end = (unsigned char *) pool + size;
	pool->data.next = NULL;
	pool->data.failed = 0;

	size = size - sizeof (struct tiny_pool);
	pool->max = (size < MAX_ALLOC_FROM_POOL) ? size : MAX_ALLOC_FROM_POOL;

	pool->current = pool;
	pool->large = NULL;
	pool->cleanup = NULL;

	return pool;
}

void destroy_pool(struct tiny_pool *pool) {
	struct tiny_pool *pool_ptr, *pool_next;
	struct pool_large *large;
	struct pool_cleanup *cleanup;

	for (cleanup = pool->cleanup; cleanup; cleanup = cleanup->next) {
		if (cleanup->handler) {
			// tiny_log_debug1(TINY_LOG_DEBUG_ALLOC, 0, "run cleanup: %p", cleanup);
			cleanup->handler(cleanup->data);
		}
	}

	for (large = pool->large; large; large = large->next) {
		// tiny_log_debug1(TINY_LOG_DEBUG_ALLOC, 0, "free: %p", large->alloc);
		if (large->alloc) tiny_free(large->alloc);
	}
#if (TINY_DEBUG)
	/* we could allocate the pool->log from this pool
	* so we cannot use this log while free()ing the pool*/
	for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
		tiny_log_debug2(TINY_LOG_DEBUG_ALLOC, 0, "free: %p, unused: %uz", p, p->d.end - p->d.last);
		if (n == NULL) break;
	}
#endif
	for (pool_ptr = pool, pool_next = pool->data.next; pool_ptr; pool_ptr = pool_next, pool_next = pool_next->data.next)
		tiny_free(pool_ptr);
}

void reset_pool(struct tiny_pool *pool) {
	struct tiny_pool *pool_ptr;
	struct pool_large *large;

	for (large = pool->large; large; large = large->next) {
		if (large->alloc) tiny_free(large->alloc);
	}

	pool->large = NULL;
	for (pool_ptr = pool; pool_ptr; pool_ptr = pool_ptr->data.next) {
		pool_ptr->data.last = (unsigned char *) pool_ptr + sizeof (struct tiny_pool);
	}
}

void *tiny_palloc(struct tiny_pool *pool, size_t size) {
	unsigned char *allocate;
	struct tiny_pool *pool_ptr;

	if (size <= pool->max) {
		pool_ptr = pool->current;
		do {
			allocate = tiny_align_ptr(pool_ptr->data.last, TINY_ALIGNMENT);
			if ((size_t) (pool_ptr->data.end - allocate) >= size) {
				pool_ptr->data.last = allocate + size;
				return allocate;
			}
			pool_ptr = pool_ptr->data.next;
		} while (pool_ptr);
		return tiny_palloc_block(pool, size);
	}

	return tiny_palloc_large(pool, size);
}

void *tiny_pnalloc(struct tiny_pool *pool, size_t nmemb, size_t size) {
	unsigned char *allocate;
	struct tiny_pool *pool_ptr;

	size_t alloc_size = size * nmemb;
	if (alloc_size <= pool->max) {
		pool_ptr = pool->current;
		do {
			allocate = pool_ptr->data.last;
			if ((size_t) (pool_ptr->data.end - allocate) >= alloc_size) {
				pool_ptr->data.last = allocate + alloc_size;
				return allocate;
			}
			pool_ptr = pool_ptr->data.next;
		} while (pool_ptr);
		return tiny_palloc_block(pool, alloc_size);
	}

	return tiny_palloc_large(pool, alloc_size);
}

static void *tiny_palloc_block(struct tiny_pool *pool, size_t size) {
	unsigned char *allocate;
	size_t psize;
	struct tiny_pool *pool_ptr, *new_pool, *current;

	psize = (size_t) (pool->data.end - (unsigned char *) pool);

	allocate = (unsigned char *)tiny_memalign(TINY_POOL_ALIGNMENT, psize);
	if (allocate == NULL) return NULL;

	new_pool = (struct tiny_pool *) allocate;

	new_pool->data.end = allocate + psize;
	new_pool->data.next = NULL;
	new_pool->data.failed = 0;

	allocate += sizeof (pool_data);
	allocate = tiny_align_ptr(allocate, TINY_ALIGNMENT);
	new_pool->data.last = allocate + size;

	current = pool->current;
	for (pool_ptr = current; pool_ptr->data.next; pool_ptr = pool_ptr->data.next) {
		if (pool_ptr->data.failed++ > 4) current = pool_ptr->data.next;
	}

	pool_ptr->data.next = new_pool;
	pool->current = current ? current : new_pool;

	return allocate;
}

static void *tiny_palloc_large(struct tiny_pool *pool, size_t size) {
	void *allocate;
	int times;
	struct pool_large *large;

	allocate = tiny_alloc(size);
	if (allocate == NULL) return NULL;

	times = 0;
	for (large = pool->large; large; large = large->next) {
		if (large->alloc == NULL) {
			large->alloc = allocate;
			return allocate;
		}
		if (times++ > 3) break;
	}

	large = (struct pool_large *)tiny_palloc(pool, sizeof (struct pool_large));
	if (large == NULL) {
		tiny_free(allocate);
		return NULL;
	}

	large->alloc = allocate;
	large->next = pool->large;
	pool->large = large;

	return allocate;
}

void *tiny_pmemalign(struct tiny_pool *pool, size_t size, size_t alignment) {
	void *allocate;
	struct pool_large *large;

	allocate = tiny_memalign(alignment, size);
	if (allocate == NULL) return NULL;

	large = (struct pool_large *)tiny_palloc(pool, sizeof (struct pool_large));
	if (large == NULL) {
		tiny_free(allocate);
		return NULL;
	}

	large->alloc = allocate;
	large->next = pool->large;
	pool->large = large;

	return allocate;
}

int tiny_pdelete(struct tiny_pool *pool, void *ptr) {
	struct pool_large *large;

	for (large = pool->large; large; large = large->next) {
		if (ptr == large->alloc) {
			// tiny_log_debug1(TINY_LOG_DEBUG_ALLOC, 0, "free: %p", large->alloc);
			tiny_free(large->alloc);
			large->alloc = NULL;
			return 0;
		}
	}

	return -1;
}

void *tiny_pcalloc(struct tiny_pool *pool, size_t size) {
	void *allocate;

	allocate = tiny_palloc(pool, size);
	if (allocate) tiny_memzero(allocate, size);

	return allocate;
}

struct pool_cleanup *pool_cleanup_add(struct tiny_pool *pool, size_t size) {
	struct pool_cleanup *cleanup;

	cleanup = (struct pool_cleanup *)tiny_palloc(pool, sizeof (struct pool_cleanup));
	if (cleanup == NULL) return NULL;

	if (size) {
		cleanup->data = tiny_palloc(pool, size);
		if (cleanup->data == NULL) return NULL;
	} else cleanup->data = NULL;

	cleanup->handler = NULL;
	cleanup->next = pool->cleanup;

	pool->cleanup = cleanup;
	// tiny_log_debug1(TINY_LOG_DEBUG_ALLOC, 0, "add cleanup: %p", cleanup);

	return cleanup;
}

void pool_run_cleanup_file(struct tiny_pool *pool, HANDLE handle) {
	struct pool_cleanup *cleanup;
	tiny_cleanup_file *cleanup_file;

	for (cleanup = pool->cleanup; cleanup; cleanup = cleanup->next) {
		if (cleanup->handler == pool_cleanup_file) {
			cleanup_file = (tiny_cleanup_file *)cleanup->data;
			if (cleanup_file->handle == handle) {
				cleanup->handler(cleanup_file);
				cleanup->handler = NULL;
				return;
			}
		}
	}
}

void pool_cleanup_file(void *data) {
	tiny_cleanup_file *cleanup_file = (tiny_cleanup_file *)data;

	// tiny_log_debug1(TINY_LOG_DEBUG_ALLOC, 0, "file cleanup: fd:%d", cleanup_file->fd);

	if (tiny_close(cleanup_file->handle) == TINY_FILE_ERROR) {
		// tiny_log_error(TINY_LOG_ALERT, tiny_errno, tiny_close_file_n " \"%s\" failed", cleanup_file->name);
	}
}

void pool_delete_file(void *data) {
	tiny_cleanup_file *cleanup_file = (tiny_cleanup_file *)data;

	// tiny_log_debug2(TINY_LOG_DEBUG_ALLOC, 0, "file cleanup: fd:%d %s", cleanup_file->fd, cleanup_file->name);
	if (tiny_delete(cleanup_file->name) == TINY_FILE_ERROR) {
		if (errno != ENOENT) {
			// tiny_log_error(TINY_LOG_CRIT, err, tiny_delete_file_n " \"%s\" failed", cleanup_file->name);
		}
	}

	if (tiny_close(cleanup_file->handle) == TINY_FILE_ERROR) {
		// tiny_log_error(TINY_LOG_ALERT, tiny_errno, tiny_close_file_n " \"%s\" failed", cleanup_file->name);
	}
}

#if 0

static void *get_cached_block(size_t size) {
	void *p;
	tiny_cached_block_slot_t *slot;
	if (tiny_cycle->cache == NULL) return NULL;
	slot = &tiny_cycle->cache[(size + tiny_pagesize - 1) / tiny_pagesize];
	slot->tries++;
	if (slot->number) {
		p = slot->block;
		slot->block = slot->block->next;
		slot->number--;
		return p;
	}
	return NULL;
}
#endif

