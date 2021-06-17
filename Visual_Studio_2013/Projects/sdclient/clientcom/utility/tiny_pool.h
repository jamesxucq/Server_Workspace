/* 
 * File:   tiny_pool.h
 * Author: Divad
 *
 * Created on 2012-1-17, PM 4:21
 */

#ifndef _TINY_POOL_H_
#define	_TINY_POOL_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    typedef void (*cleanup_handler)(void *data);

    struct pool_cleanup {
        cleanup_handler handler;
        void *data;
        struct pool_cleanup *next;
    };

    struct pool_large {
        struct pool_large *next;
        void *alloc;
    };

    typedef struct {
        unsigned char *last;
        unsigned char *end;
        struct tiny_pool *next;
        int failed;
    } pool_data;

    struct tiny_pool {
        pool_data data;
        size_t max;
        struct tiny_pool *current;
        struct pool_large *large;
        struct pool_cleanup *cleanup;
    };

    typedef struct {
        // int fd;
        HANDLE handle;
		unsigned char *name;
    } tiny_cleanup_file;

    struct tiny_pool *create_pool(size_t size);
    void destroy_pool(struct tiny_pool *pool);
    void reset_pool(struct tiny_pool *pool);

    void *tiny_palloc(struct tiny_pool *pool, size_t size);
    void *tiny_pnalloc(struct tiny_pool *pool, size_t nmemb, size_t size);
    void *tiny_pcalloc(struct tiny_pool *pool, size_t size);
    void *tiny_pmemalign(struct tiny_pool *pool, size_t size, size_t alignment);
    int tiny_pdelete(struct tiny_pool *pool, void *ptr);

    struct pool_cleanup *pool_cleanup_add(struct tiny_pool *pool, size_t size);
    void pool_run_cleanup_file(struct tiny_pool *pool, HANDLE handle);
    void pool_cleanup_file(void *data);
    void pool_delete_file(void *data);

#ifdef	__cplusplus
}
#endif

#endif	/* _TINY_POOL_H_ */

